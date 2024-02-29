/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include <QDebug>

#include <vtkImageReslice.h>
#include <vtkImageAppendComponents.h>
#include <vtkMatrix4x4.h>

#include "deeplearningsegmentation.h"

namespace udg {

DeepLearningSegmentation::DeepLearningSegmentation()
{
    m_inputVtkImage = nullptr;
    m_axes = {0, 1, 2};
    m_cosines = {1,0,0, 0,1,0, 0,0,1};
    m_modelDims = {1, 1, 1};
    m_numChannels = 1;
    m_isNormalised = true;
    m_numLabels = 2; // at least two labels for segmentation
    m_activationFunction = ActivationFunction::SIGMOID;
    m_sliceRange = {0, 0};
    m_currentSlice = 0;
    m_modelPath = QString("");
    m_outputMask = nullptr;
}

QVector<int> DeepLearningSegmentation::getAxes() const
{
    return m_axes;
}

void DeepLearningSegmentation::setInput(vtkImageData *image)
{
    m_inputVtkImage = image;
}

void DeepLearningSegmentation::setAxes(int x, int y, int z)
{
    m_axes[0] = x;
    m_axes[1] = y;
    m_axes[2] = z;
}

void DeepLearningSegmentation::setAxes(const int* axes)
{
    setAxes(axes[0], axes[1], axes[2]);
}

void DeepLearningSegmentation::setCosines(double x0, double x1, double x2,
                                        double y0, double y1, double y2,
                                        double z0, double z1, double z2)
{
    m_cosines[0] = x0; m_cosines[1] = x1; m_cosines[2] = x2;
    m_cosines[3] = y0; m_cosines[4] = y1; m_cosines[5] = y2;
    m_cosines[6] = z0; m_cosines[7] = z1; m_cosines[8] = z2;
}

void DeepLearningSegmentation::setCosines(const double *cosines)
{
    setCosines(cosines[0], cosines[1], cosines[2],
               cosines[3], cosines[4], cosines[5],
               cosines[6], cosines[7], cosines[8]);
}

void DeepLearningSegmentation::setModelInputDimensions(int x, int y, int z)
{
    m_modelDims[0] = x;
    m_modelDims[1] = y;
    m_modelDims[2] = z;
}

void DeepLearningSegmentation::setModelInputDimensions(const int *dims)
{
    setModelInputDimensions(dims[0], dims[1], dims[2]);
}

void DeepLearningSegmentation::setNormalisation(bool isNormalised)
{
    m_isNormalised = isNormalised;
}

void DeepLearningSegmentation::setNumberOfChannels(int channels)
{
    if (channels > 0) {
        m_numChannels = channels;
    }
}

void DeepLearningSegmentation::setNumberOfLabels(int labels)
{
    if (labels > 1) {
        m_numLabels = labels;
    }
}

void DeepLearningSegmentation::setActivationFunction(ActivationFunction function)
{
    m_activationFunction = function;
}

void DeepLearningSegmentation::setSliceRange(int first, int last)
{
    if (m_inputVtkImage) {
        // Ensure that slice range is in bounds
        int minZ = m_inputVtkImage->GetExtent()[4];
        int maxZ = m_inputVtkImage->GetExtent()[5];

        // If first slice is not in bounds, it is not changed
        if (first >= minZ && first<= maxZ) {
            m_sliceRange[0] = first;
        }

        // If last slice is not after the first slice or it is not in bounds,
        // then last = first
        if (last > first && last >= minZ && last <= maxZ) {
            m_sliceRange[1] = last;
        }
        else {
            m_sliceRange[1] = first;
        }
    }
}

void DeepLearningSegmentation::setSliceRange(const int* range)
{
    setSliceRange(range[0], range[1]);
}

void DeepLearningSegmentation::setModelPath(const QString& path)
{
    m_modelPath = path;
}

void DeepLearningSegmentation::predict()
{
    // Prepare the reslicer to extract input images, and allocate mask
    vtkNew<vtkImageReslice> reslicer;
    setupInputOutput(reslicer);

    // Load deep-learning model
    loadModelParameters();

    // Predict each slice of the range and store result into the mask
    for (m_currentSlice = m_sliceRange[0]; m_currentSlice <= m_sliceRange[1]; m_currentSlice++) {
        // Extract current slice
        vtkNew<vtkImageData> extract;
        extractSlice(reslicer, extract);

        // Run the model for that slice and save result
        run((float*)extract->GetScalarPointer());

        // Interpret the result and fill the mask
        fillMask();
    }

    // Restore input parameters to the mask
    resetOutput(reslicer);
}

void DeepLearningSegmentation::setupInputOutput(vtkImageReslice* reslicer)
{
    // Get input dimensions and spacing
    int* inDims = m_inputVtkImage->GetDimensions();
    double* inSpacing = m_inputVtkImage->GetSpacing();

    // Set output spacing from output dimensions (in column-major order: rows,
    // columns, slices)
    double outSpacing[3] = {inSpacing[m_axes[0]]*(inDims[m_axes[0]]-1)/(m_modelDims[0]-1),
                            inSpacing[m_axes[1]]*(inDims[m_axes[1]]-1)/(m_modelDims[1]-1),
                            inSpacing[m_axes[2]]}; // Z axis is not reshaped

    // Set output spacing = input spacing for dimensions divided by 0
    for (int i = 0; i < 3; i++) {
        if (m_modelDims[i] < 2) outSpacing[i] = inSpacing[m_axes[i]];
    }


    // **
    // * Prepare the reslicer to extract input images
    // **

    // Set image to be segmented as input data
    reslicer->SetInputData(m_inputVtkImage);

    // Change axes using previously defined cosines
    reslicer->SetResliceAxesDirectionCosines(m_cosines.data());

    // Remove Z axis to keep a single slice
    reslicer->SetOutputDimensionality(2);

    // Set scalar type to float for linear interpolation
    reslicer->SetOutputScalarType(VTK_FLOAT);

    // A different spacing between voxels forces image to resize
    reslicer->SetOutputSpacing(outSpacing);

    // Set VTK linear interpolation
    // (careful: not typical linear interpolation)
    reslicer->SetInterpolationModeToLinear();


    // **
    // * Allocate data for 3D segmentation mask
    // **

    m_outputMask = vtkSmartPointer<vtkImageData>::New();
    int maskDimZ = m_sliceRange[1] - m_sliceRange[0] + 1;
    m_outputMask->SetDimensions(m_modelDims[0], m_modelDims[1], maskDimZ + 1); // + 1 = dummy slice
    m_outputMask->SetSpacing(outSpacing);
    m_outputMask->AllocateScalars(VTK_SHORT, 1);

    // Get pointer to first voxel of last slice of output mask (dummy slice)
    short* maskData = static_cast<short*>(m_outputMask->GetScalarPointer(0, 0, maskDimZ));

    // Compute number of pixels of a single slice of output mask
    int numPixels = m_modelDims[0] * m_modelDims[1];

    // Initialise scalar values to 0 for last slice of output mask
    for (int i = 0; i < numPixels; i++) {
        *maskData++ = 0;
    }
}

void DeepLearningSegmentation::resetOutput(vtkImageReslice* reslicer)
{
    // **
    // * Update reslicer to upscale mask and restore input axes
    // **

    // Change reslicer parameters to upscale the segmentation mask
    reslicer->SetInputData(m_outputMask);

    // Dimensionality restored to 3 (Z axis restored)
    reslicer->SetOutputDimensionality(3);

    // Reslice axes are inverted (opposite behaviour)
    reslicer->GetResliceAxes()->Invert();

    // Input spacing is assigned to restore input dimensions
    reslicer->SetOutputSpacing(m_inputVtkImage->GetSpacing());

    // Mask values are not floats
    reslicer->SetOutputScalarType(VTK_SHORT);

    // Mask values should not be linearly interpolated
    reslicer->SetInterpolationModeToNearestNeighbor();

    // Get upscaled mask and set origin to first predicted slice
    reslicer->Update();
    vtkImageData* upscaledMask = reslicer->GetOutput();
    int ijk[3] = {m_inputVtkImage->GetExtent()[m_axes[0]*2],
                  m_inputVtkImage->GetExtent()[m_axes[1]*2],
                  m_sliceRange[0]};
    upscaledMask->SetOrigin(m_inputVtkImage->GetPoint(m_inputVtkImage->ComputePointId(ijk)));

    // Set final output mask
    m_outputMask = upscaledMask;
}

void DeepLearningSegmentation::extractSlice(vtkImageReslice* reslicer, vtkImageData* extract) const
{
    // Voxel coordinates for a point (any) in current slice
    int ijk[3];
    ijk[m_axes[0]] = m_inputVtkImage->GetExtent()[m_axes[0]*2];
    ijk[m_axes[1]] = m_inputVtkImage->GetExtent()[m_axes[1]*2];
    ijk[m_axes[2]] = m_currentSlice;

    // Set reslicer origin to extract slice at that point
    reslicer->SetResliceAxesOrigin(m_inputVtkImage->GetPoint(m_inputVtkImage->ComputePointId(ijk)));

    if (m_isNormalised) {
        // Normalise image (scalar values in [0, 1], where 0 = min and 1 = max)
        // (reslicer must have been updated to get scalar range)
        reslicer->Update();
        double* scalarRange = reslicer->GetOutput()->GetScalarRange();
        reslicer->SetScalarShift(-scalarRange[0]);
        reslicer->SetScalarScale(1 / (scalarRange[1] - scalarRange[0]));
    }

    // Duplicate image components to match number of channels
    // (single channel assumed as input)
    vtkNew<vtkImageAppendComponents> append;
    for (int i = 0; i < m_numChannels; i++) {
        append->AddInputConnection(reslicer->GetOutputPort());
    }
    append->Update();

    // Store result to the output parameter
    extract->DeepCopy(append->GetOutput());

    // Set scalar shift and scalar scale to default values (avoid erroneous
    // normalisation in subsequent iterations when obtaining scalar range)
    reslicer->SetScalarShift(0);
    reslicer->SetScalarScale(1);
}

vtkSmartPointer<vtkImageData> DeepLearningSegmentation::getOutputMask() const
{
    return m_outputMask;
}

} // namespace udg
