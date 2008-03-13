/*=========================================================================

  Module:    $RCSfile: vtk4DLinearRegressionGradientEstimator.h,v $

  Copyright (c) Marc Ruiz Altisent

=========================================================================*/
#include "vtk4DLinearRegressionGradientEstimator.h"

#include "vtkCharArray.h"
#include "vtkDirectionEncoder.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkUnsignedShortArray.h"

#include <cmath>

vtkCxxRevisionMacro(vtk4DLinearRegressionGradientEstimator, "$Revision: 1.0 $");
vtkStandardNewMacro(vtk4DLinearRegressionGradientEstimator);

// This is the templated function that actually computes the EncodedNormal
// and the GradientMagnitude
template <class T>
void vtkComputeGradients( 
  vtk4DLinearRegressionGradientEstimator *estimator, T *data_ptr,
  int thread_id, int thread_count )
{
  int                 xstep, ystep, zstep;
  int                 x, y, z;
  int                 offset;
  int                 x_start, x_limit;
  int                 y_start, y_limit;
  int                 z_start, z_limit;
  int                 useClip;
  int                 *clip;
  T                   *dptr;
  unsigned char       *gptr;
  unsigned short      *nptr;
  float               n[3], t;
  float               gvalue;
  float               zeroNormalThreshold;
  int                 useBounds;
  int                 bounds[6];
  int                 size[3];
  float               aspect[3];
  int                 xlow, xhigh;
  float               scale, bias;
  int                 computeGradientMagnitudes;
  vtkDirectionEncoder *direction_encoder;
  int                 zeroPad;
  
  estimator->GetInputSize( size );
  estimator->GetInputAspect( aspect );
  computeGradientMagnitudes = estimator->GetComputeGradientMagnitudes();
  scale = estimator->GetGradientMagnitudeScale();
  bias = estimator->GetGradientMagnitudeBias();
  zeroPad = estimator->GetZeroPad();
  
  // adjust the aspect
  aspect[0] = aspect[0] * 2.0;// * estimator->SampleSpacingInVoxels;
  aspect[1] = aspect[1] * 2.0;// * estimator->SampleSpacingInVoxels;
  aspect[2] = aspect[2] * 2.0;// * estimator->SampleSpacingInVoxels;
  
  // Compute steps through the volume in x, y, and z
  xstep = 1;
  ystep = size[0];
  zstep = size[0] * size[1];

  // Multiply by the spacing used for normal estimation
//   xstep *= estimator->SampleSpacingInVoxels;
//   ystep *= estimator->SampleSpacingInVoxels;
//   zstep *= estimator->SampleSpacingInVoxels;

  // Get the length at or below which normals are considered to
  // be "zero"
  zeroNormalThreshold = estimator->GetZeroNormalThreshold();
  
  useBounds = estimator->GetBoundsClip();
  
  // Compute an offset based on the thread_id. The volume will
  // be broken into large slabs (thread_count slabs). For this thread
  // we need to access the correct slab. Also compute the z plane that
  // this slab starts on, and the z limit of this slab (one past the
  // end of the slab)
  if ( useBounds )
    {
    estimator->GetBounds( bounds );
    x_start = bounds[0];
    x_limit = bounds[1]+1;
    y_start = bounds[2];
    y_limit = bounds[3]+1;
    z_start = (int)(( (float)thread_id / (float)thread_count ) *
                    (float)(bounds[5]-bounds[4]+1) ) + bounds[4];
    z_limit = (int)(( (float)(thread_id + 1) / (float)thread_count ) *
                    (float)(bounds[5]-bounds[4]+1) ) + bounds[4];
    }
  else
    {
    x_start = 0;
    x_limit = size[0];
    y_start = 0;
    y_limit = size[1];
    z_start = (int)(( (float)thread_id / (float)thread_count ) *
                    size[2] );
    z_limit = (int)(( (float)(thread_id + 1) / (float)thread_count ) *
                    size[2] );
    }

  // Do final error checking on limits - make sure they are all within bounds
  // of the scalar input
  
  x_start = (x_start<0)?(0):(x_start);
  y_start = (y_start<0)?(0):(y_start);
  z_start = (z_start<0)?(0):(z_start);
  
  x_limit = (x_limit>size[0])?(size[0]):(x_limit);
  y_limit = (y_limit>size[1])?(size[1]):(y_limit);
  z_limit = (z_limit>size[2])?(size[2]):(z_limit);


  direction_encoder = estimator->GetDirectionEncoder();

  useClip = estimator->GetUseCylinderClip();
  clip = estimator->GetCircleLimits();

  // Loop through all the data and compute the encoded normal and
  // gradient magnitude for each scalar location
  for ( z = z_start; z < z_limit; z++ )
    {
    for ( y = y_start; y < y_limit; y++ )
      {
      if ( useClip )
        {
        xlow = ((clip[2*y])>x_start)?(clip[2*y]):(x_start);
        xhigh = ((clip[2*y+1]+1)<x_limit)?(clip[2*y+1]+1):(x_limit);
        }
      else
        {
        xlow = x_start;
        xhigh = x_limit;
        }
      offset = z * size[0] * size[1] + y * size[0] + xlow;
      
      // Set some pointers
      dptr = data_ptr + offset;
      nptr = estimator->EncodedNormals + offset;
      gptr = estimator->GradientMagnitudes + offset;

      for ( x = xlow; x < xhigh; x++ )
        {
        /// \TODO Optimitzar treient les coses mës enfora encara, precalculant i guardant en taules tot el que es pugui
        float A = 0.0;
        float B = 0.0;
        float C = 0.0;
        float D = 0.0;
        for ( int ix = -2; ix <= 2; ix++ )
        {
            int xPix = x + ix;
            if ( xPix < 0 || xPix >= size[0] ) continue;    // v = 0
            int ix2 = ix * ix;
            T * dptrPixxstep = dptr + ix * xstep;
            for ( int iy = -2; iy <= 2; iy++ )
            {
                int yPiy = y + iy;
                if ( yPiy < 0 || yPiy >= size[1] ) continue;    // v = 0
                int ix2Piy2 = ix2 + iy * iy;
                T * dptrPixxstepPiyystep = dptrPixxstep + iy * ystep;
                for ( int iz = -2; iz <= 2; iz++ )
                {
                    int zPiz = z + iz;
                    if ( zPiz < 0 || zPiz >= size[2] ) continue;    // v = 0
                    // distància euclidiana
                    float w = sqrt( (double)(ix2Piy2 + iz * iz) );
                    // valor del vòxel (no pot ser 0 perquê ja hem fet les comprovacions abans)
                    float v = *( dptrPixxstepPiyystep + iz * zstep );
                    v *= w;
                    A += v * ix;
                    B += v * iy;
                    C += v * iz;
                    D += v;
                }
            }
        }
//         for ( int ix = -2; ix <= 2; ix++ )
//         {
//             for ( int iy = -2; iy <= 2; iy++ )
//             {
//                 for ( int iz = -2; iz <= 2; iz++ )
//                 {
//                     // distància euclidiana
//                     float w = sqrt( ix * ix + iy * iy + iz * iz );
//                     // valor del vòxel (si és fora de rang considerem 0)
//                     float v = ( x + ix < 0 || x + ix >= size[0] ||
//                                 y + iy < 0 || y + iy >= size[1] ||
//                                 z + iz < 0 || z + iz >= size[2] ) ? 0.0
//                             : *( dptr + ix * xstep + iy * ystep + iz * zstep );
//                     v *= w;
//                     A += v * ix;
//                     B += v * iy;
//                     C += v * iz;
//                     D += v;
//                 }
//             }
//         }
/*
        // Compute the X component
        if ( x < estimator->SampleSpacingInVoxels ) 
          {
          if ( zeroPad )
            {
            n[0] = -((float)*(dptr+xstep));
            }
          else
            {
            n[0] = 2.0*((float)*(dptr) - (float)*(dptr+xstep));
            }
          }
        else if ( x >= size[0] - estimator->SampleSpacingInVoxels )
          {
          if ( zeroPad )
            {
            n[0] =  ((float)*(dptr-xstep));
            }
          else
            {
            n[0] = 2.0*((float)*(dptr-xstep) - (float)*(dptr));
            }
          }
        else
          {
          n[0] = (float)*(dptr-xstep) - (float)*(dptr+xstep); 
          }
        
        // Compute the Y component
        if ( y < estimator->SampleSpacingInVoxels )
          {
          if ( zeroPad )
            {
            n[1] = -((float)*(dptr+ystep));
            }
          else
            {
            n[1] = 2.0*((float)*(dptr) - (float)*(dptr+ystep)); 
            }
          }
        else if ( y >= size[1] - estimator->SampleSpacingInVoxels )
          {
          if ( zeroPad )
            {
            n[1] =  ((float)*(dptr-ystep));
            }
          else
            {
            n[1] = 2.0*((float)*(dptr-ystep) - (float)*(dptr)); 
            }
          }
        else
          {
          n[1] = (float)*(dptr-ystep) - (float)*(dptr+ystep); 
          }
        
        // Compute the Z component
        if ( z < estimator->SampleSpacingInVoxels )
          {
          if ( zeroPad )
            {
            n[2] = -((float)*(dptr+zstep));
            }
          else
            {
            n[2] = 2.0*((float)*(dptr) - (float)*(dptr+zstep)); 
            }
          }
        else if ( z >= size[2] - estimator->SampleSpacingInVoxels )
          {
          if ( zeroPad )
            {
            n[2] =  ((float)*(dptr-zstep));
            }
          else
            {
            n[2] = 2.0*((float)*(dptr-zstep) - (float)*(dptr)); 
            }
          }
        else
          {
          n[2] = (float)*(dptr-zstep) - (float)*(dptr+zstep); 
          }
*/
        n[0] = A;
        n[1] = B;
        n[2] = C;

        // Take care of the aspect ratio of the data
        // Scaling in the vtkVolume is isotropic, so this is the
        // only place we have to worry about non-isotropic scaling.
        n[0] /= aspect[0];
        n[1] /= aspect[1];
        n[2] /= aspect[2];
        
        // Compute the gradient magnitude
        t = sqrt( (double)( n[0]*n[0] + 
                            n[1]*n[1] + 
                            n[2]*n[2] ) );
        
        if ( computeGradientMagnitudes )
          {
          // Encode this into an 8 bit value 
          gvalue = (t + bias) * scale; 
          
          if ( gvalue < 0.0 )
            {
            *gptr = 0;
            }
          else if ( gvalue > 255.0 )
            {
            *gptr = 255;
            }
          else 
            {
            *gptr = (unsigned char) gvalue;
            }
          gptr++;
          }

        // Normalize the gradient direction
        if ( t > zeroNormalThreshold )
          {
          n[0] /= t;
          n[1] /= t;
          n[2] /= t;
          }
        else
          {
          n[0] = n[1] = n[2] = 0.0;
          }

        // Convert the gradient direction into an encoded index value
        *nptr = direction_encoder->GetEncodedDirection( n );
        nptr++;
        dptr++;

        }
      }
    }
}

// Construct a vtk4DLinearRegressionGradientEstimator
vtk4DLinearRegressionGradientEstimator::vtk4DLinearRegressionGradientEstimator()
{
}

// Destruct a vtk4DLinearRegressionGradientEstimator - free up any memory used
vtk4DLinearRegressionGradientEstimator::~vtk4DLinearRegressionGradientEstimator()
{
}

static VTK_THREAD_RETURN_TYPE vtkSwitchOnDataType( void *arg )
{
  vtk4DLinearRegressionGradientEstimator   *estimator;
  int                                    thread_count;
  int                                    thread_id;
  vtkDataArray                           *scalars;

  thread_id = ((vtkMultiThreader::ThreadInfo *)(arg))->ThreadID;
  thread_count = ((vtkMultiThreader::ThreadInfo *)(arg))->NumberOfThreads;
  estimator = (vtk4DLinearRegressionGradientEstimator *)
    (((vtkMultiThreader::ThreadInfo *)(arg))->UserData);
  scalars = estimator->Input->GetPointData()->GetScalars();

  if (scalars == NULL)
    {
    return VTK_THREAD_RETURN_VALUE;
    }
  
  // Find the data type of the Input and call the correct 
  // templated function to actually compute the normals and magnitudes
  
  switch ( scalars->GetDataType() )
    {
    vtkTemplateMacro(
      vtkComputeGradients(estimator,
                          static_cast<VTK_TT*>(scalars->GetVoidPointer(0)),
                          thread_id, thread_count)
      );
    default:
      vtkGenericWarningMacro("unable to encode scalar type!");
    }
  
  return VTK_THREAD_RETURN_VALUE;
}


// This method is used to compute the encoded normal and the
// magnitude of the gradient for each voxel location in the 
// Input.
void vtk4DLinearRegressionGradientEstimator::UpdateNormals( )
{
  vtkDebugMacro( << "Updating Normals!" );
  this->Threader->SetNumberOfThreads( this->NumberOfThreads );
  
  this->Threader->SetSingleMethod( vtkSwitchOnDataType,
                                  (vtkObject *)this );
  
  this->Threader->SingleMethodExecute();
}

// Print the vtk4DLinearRegressionGradientEstimator
void vtk4DLinearRegressionGradientEstimator::PrintSelf(ostream& os,
                                                     vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
