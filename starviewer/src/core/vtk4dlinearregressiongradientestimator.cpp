#include "vtk4dlinearregressiongradientestimator.h"

#include "logging.h"

#include "vtkDataArray.h"
#include "vtkDirectionEncoder.h"
#include "vtkImageData.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

namespace {

// Aquest és el mètode que calcula realment les normals i les magnituds del gradient.
template <class T>
void computeGradients(udg::Vtk4DLinearRegressionGradientEstimator *estimator, T *data, int threadId, int threadCount, int radius)
{
    int size[3];
    estimator->GetInputSize(size);

    int xStart, xLimit, yStart, yLimit, zStart, zLimit;
    float fThreadCount = threadCount;

    // Compute an offset based on the threadId. The volume will be broken into large slabs (threadCount slabs).
    // For this thread we need to access the correct slab.
    // Also compute the z plane that this slab starts on, and the z limit of this slab (one past the end of the slab).
    if (estimator->GetBoundsClip())
    {
        int bounds[6];
        estimator->GetBounds(bounds);
        xStart = bounds[0];
        xLimit = bounds[1] + 1;
        yStart = bounds[2];
        yLimit = bounds[3] + 1;
        zStart = static_cast<int>((threadId / fThreadCount) * (bounds[5] - bounds[4] + 1)) + bounds[4];
        zLimit = static_cast<int>(((threadId + 1) / fThreadCount) * (bounds[5] - bounds[4] + 1)) + bounds[4];
    }
    else
    {
        xStart = 0;
        xLimit = size[0];
        yStart = 0;
        yLimit = size[1];
        zStart = static_cast<int>((threadId / fThreadCount) * size[2]);
        zLimit = static_cast<int>(((threadId + 1) / fThreadCount) * size[2]);
    }

    // Do final error checking on limits - make sure they are all within bounds of the scalar input
    xStart = xStart < 0 ? 0 : xStart;
    yStart = yStart < 0 ? 0 : yStart;
    zStart = zStart < 0 ? 0 : zStart;
    xLimit = xLimit > size[0] ? size[0] : xLimit;
    yLimit = yLimit > size[1] ? size[1] : yLimit;
    zLimit = zLimit > size[2] ? size[2] : zLimit;

    bool useClip = estimator->GetUseCylinderClip() != 0;
    int *clip = estimator->GetCircleLimits();

    unsigned short *normalPointer = estimator->EncodedNormals;
    unsigned char *gradientPointer = estimator->GradientMagnitudes;

    // Compute steps through the volume in x, y, and z
    int xStep = 1;
    int yStep = size[0];
    int zStep = size[0] * size[1];

// Això és el que hi havia a l'original (vtkFiniteDifferenceGradientEstimator)
//     // Multiply by the spacing used for normal estimation
//     xStep *= estimator->SampleSpacingInVoxels;
//     yStep *= estimator->SampleSpacingInVoxels;
//     zStep *= estimator->SampleSpacingInVoxels;

    /// \todo El zeroPad s'hauria de fer servir. De moment actuem sempre com si fós cert.
//     bool zeroPad = estimator->GetZeroPad() != 0; // casting implícit des d'int

    // Precàlcul de distàncies euclidianes i offsets dins la màscara
    int diameter = 2 * radius + 1, diameter2 = diameter * diameter;
    int maskSize = diameter * diameter * diameter;
    float *w = new float[maskSize];
    //int maskOffset[maskSize];

    for (int ix = -radius, im = 0; ix <= radius; ix++)
    {
        for (int iy = -radius; iy <= radius; iy++)
        {
            for (int iz = -radius; iz <= radius; iz++, im++)
            {
                if (ix == 0 && iy == 0 && iz == 0)
                {
                    w[im] = 0.0f;
                }
                else
                {
                    w[im] = 1.0f / sqrt(static_cast<float>(ix * ix + iy * iy + iz * iz));
                }
                //maskOffset[im] = ix * xStep + iy * yStep + iz * zStep;    // amb això va un 11% més lent (???)
            }
        }
    }

    float aspect[3];
    estimator->GetInputAspect(aspect);

    /// \todo Això s'hauria de revisar per saber si està bé
    ///       SampleSpacingInVoxels és la distància entre les mostres agafades, i nosaltres considerem que és 1
// Això és el que hi havia a l'original (vtkFiniteDifferenceGradientEstimator)
//     // adjust the aspect
//     aspect[0] = aspect[0] * 2.0 * estimator->SampleSpacingInVoxels;
//     aspect[1] = aspect[1] * 2.0 * estimator->SampleSpacingInVoxels;
//     aspect[2] = aspect[2] * 2.0 * estimator->SampleSpacingInVoxels;

    // Adjust the aspect
    aspect[0] *= 2.0;
    aspect[1] *= 2.0;
    aspect[2] *= 2.0;

    bool computeGradientMagnitudes = estimator->GetComputeGradientMagnitudes() != 0;
    float scale = estimator->GetGradientMagnitudeScale();
    float bias = estimator->GetGradientMagnitudeBias();

    // Get the length at or below which normals are considered to be "zero"
    float zeroNormalThreshold = estimator->GetZeroNormalThreshold();

    vtkDirectionEncoder *directionEncoder = estimator->GetDirectionEncoder();

    // Loop through all the data and compute the encoded normal and gradient magnitude for each scalar location
    for (int z = zStart; z < zLimit; z++)
    {
        for (int y = yStart; y < yLimit; y++)
        {
            int xLow, xHigh;

            if (useClip)
            {
                xLow = clip[2 * y] > xStart ? clip[2 * y] : xStart;
                xHigh = clip[2 * y + 1] + 1 < xLimit ? clip[2 * y + 1] + 1 : xLimit;
            }
            else
            {
                xLow = xStart;
                xHigh = xLimit;
            }

            int offset = z * size[0] * size[1] + y * size[0] + xLow;

            // Set some pointers
            T *dPtr = data + offset;
            unsigned short *nPtr = normalPointer + offset;
            unsigned char *gPtr = gradientPointer + offset;

            for (int x = xLow; x < xHigh; x++)
            {
                /// \TODO Optimitzar treient les coses més enfora encara, precalculant i guardant en taules tot el que es pugui
                float A = 0.0, B = 0.0, C = 0.0, D = 0.0;

                for (int ix = -radius, im = 0; ix <= radius; ix++)
                {
                    int xPix = x + ix;
                    if (xPix < 0 || xPix >= size[0])
                    {
                        im += diameter2;
                        // v = 0
                        continue;
                    }
                    T *dPtrPixxStep = dPtr + ix * xStep;

                    for (int iy = -radius; iy <= radius; iy++)
                    {
                        int yPiy = y + iy;
                        if (yPiy < 0 || yPiy >= size[1])
                        {
                            im += diameter;
                            // v = 0
                            continue;
                        }
                        T *dPtrPixxStepPiyyStep = dPtrPixxStep + iy * yStep;

                        for (int iz = -radius; iz <= radius; iz++, im++)
                        {
                            int zPiz = z + iz;
                            if (zPiz < 0 || zPiz >= size[2])
                            {
                                // v = 0
                                continue;
                            }
                            // valor del vòxel (no pot ser 0 perquè ja hem fet les comprovacions abans)
                            float v = *(dPtrPixxStepPiyyStep + iz * zStep);
                            //float v = dPtr[maskOffset[im]];   // d'aquesta manera va més lent (???)
                            // w[im] = distància euclidiana
                            v *= w[im];
                            A += v * ix;
                            B += v * iy;
                            C += v * iz;
                            D += v;
                        }
                    }
                }

//                 for (int ix = -radius; ix <= radius; ix++)
//                 {
//                     for (int iy = -radius; iy <= radius; iy++)
//                     {
//                         for (int iz = -radius; iz <= radius; iz++)
//                         {
//                             // distància euclidiana
//                             float w = sqrt(ix * ix + iy * iy + iz * iz);
//                             // valor del vòxel (si és fora de rang considerem 0)
//                             float v = (x + ix < 0 || x + ix >= size[0] ||
//                                         y + iy < 0 || y + iy >= size[1] ||
//                                         z + iz < 0 || z + iz >= size[2]) ? 0.0
//                                     : *(dPtr + ix * xStep + iy * yStep + iz * zStep);
//                             v *= w;
//                             A += v * ix;
//                             B += v * iy;
//                             C += v * iz;
//                             D += v;
//                         }
//                     }
//                 }

/*
        // Compute the X component
        if (x < estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[0] = -((float)*(dPtr+xStep));
            }
          else
            {
            n[0] = 2.0*((float)*(dPtr) - (float)*(dPtr+xStep));
            }
          }
        else if (x >= size[0] - estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[0] =  ((float)*(dPtr-xStep));
            }
          else
            {
            n[0] = 2.0*((float)*(dPtr-xStep) - (float)*(dPtr));
            }
          }
        else
          {
          n[0] = (float)*(dPtr-xStep) - (float)*(dPtr+xStep);
          }

        // Compute the Y component
        if (y < estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[1] = -((float)*(dPtr+yStep));
            }
          else
            {
            n[1] = 2.0*((float)*(dPtr) - (float)*(dPtr+yStep));
            }
          }
        else if (y >= size[1] - estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[1] =  ((float)*(dPtr-yStep));
            }
          else
            {
            n[1] = 2.0*((float)*(dPtr-yStep) - (float)*(dPtr));
            }
          }
        else
          {
          n[1] = (float)*(dPtr-yStep) - (float)*(dPtr+yStep);
          }

        // Compute the Z component
        if (z < estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[2] = -((float)*(dPtr+zStep));
            }
          else
            {
            n[2] = 2.0*((float)*(dPtr) - (float)*(dPtr+zStep));
            }
          }
        else if (z >= size[2] - estimator->SampleSpacingInVoxels)
          {
          if (zeroPad)
            {
            n[2] =  ((float)*(dPtr-zStep));
            }
          else
            {
            n[2] = 2.0*((float)*(dPtr-zStep) - (float)*(dPtr));
            }
          }
        else
          {
          n[2] = (float)*(dPtr-zStep) - (float)*(dPtr+zStep);
          }
*/

                // Take care of the aspect ratio of the data.
                // Scaling in the vtkVolume is isotropic, so this is the only place we have to worry about non-isotropic scaling.
                float normal[3];
                normal[0] = A / aspect[0];
                normal[1] = B / aspect[1];
                normal[2] = C / aspect[2];

                // Compute the gradient magnitude
                float gradientMagnitude = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

                if (computeGradientMagnitudes)
                {
                    // Encode this into an 8 bit value
                    float gValue = (gradientMagnitude + bias) * scale;

                    if (gValue < 0.0)
                    {
                        *gPtr = 0;
                    }
                    else if (gValue > 255.0)
                    {
                        *gPtr = 255;
                    }
                    else
                    {
                        *gPtr = static_cast<unsigned char>(gValue);
                    }

                    gPtr++;
                }

                // Normalize the gradient direction
                if (gradientMagnitude > zeroNormalThreshold)
                {
                    normal[0] /= gradientMagnitude;
                    normal[1] /= gradientMagnitude;
                    normal[2] /= gradientMagnitude;
                }
                else
                {
                    normal[0] = normal[1] = normal[2] = 0.0;
                }

                // Convert the gradient direction into an encoded index value
                *nPtr = directionEncoder->GetEncodedDirection(normal);
                nPtr++;
                dPtr++;
            }
        }
    }

    delete[] w;
}

// Fa el casting de tipus de dades.
static VTK_THREAD_RETURN_TYPE switchOnDataType(void *arg)
{
    vtkMultiThreader::ThreadInfo *threadInfo = reinterpret_cast<vtkMultiThreader::ThreadInfo*>(arg);
    int threadId = threadInfo->ThreadID;
    int threadCount = threadInfo->NumberOfThreads;
    udg::Vtk4DLinearRegressionGradientEstimator *estimator = reinterpret_cast<udg::Vtk4DLinearRegressionGradientEstimator*>(threadInfo->UserData);
    vtkDataArray *scalars = estimator->Input->GetPointData()->GetScalars();

    if (!scalars)
    {
        return VTK_THREAD_RETURN_VALUE;
    }

    // Find the data type of the Input and call the correct templated function to actually compute the normals and magnitudes
    switch (scalars->GetDataType())
    {
            vtkTemplateMacro(computeGradients(estimator, static_cast<VTK_TT*>(scalars->GetVoidPointer(0)), threadId, threadCount, estimator->getRadius()));
        default:
            DEBUG_LOG("No es pot calcular el gradient per aquest tipus de dades.");
            WARN_LOG("No es pot calcular el gradient per aquest tipus de dades.");
    }

    return VTK_THREAD_RETURN_VALUE;
}

} // End namespace

namespace udg {

vtkCxxRevisionMacro(Vtk4DLinearRegressionGradientEstimator, "$Revision: 1.0 $");
vtkStandardNewMacro(Vtk4DLinearRegressionGradientEstimator);

unsigned int Vtk4DLinearRegressionGradientEstimator::getRadius() const
{
    return m_radius;
}

void Vtk4DLinearRegressionGradientEstimator::setRadius(unsigned int radius)
{
    m_radius = radius;
}

Vtk4DLinearRegressionGradientEstimator::Vtk4DLinearRegressionGradientEstimator()
    : m_radius(1)
{
}

Vtk4DLinearRegressionGradientEstimator::~Vtk4DLinearRegressionGradientEstimator()
{
}

void Vtk4DLinearRegressionGradientEstimator::UpdateNormals(void)
{
    DEBUG_LOG("S'estan actualitzant les normals");
    this->Threader->SetNumberOfThreads(this->NumberOfThreads);
    this->Threader->SetSingleMethod(switchOnDataType, this);
    this->Threader->SingleMethodExecute();
}

} // End namespace udg
