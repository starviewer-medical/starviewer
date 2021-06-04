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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
    All rights reserved.
    See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

       This software is distributed WITHOUT ANY WARRANTY; without even
       the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE.  See the above copyright notice for more information.
 *************************************************************************************/

#include "vtkcorrectimageblend.h"

#include <vtkImageData.h>
#include <vtkImageIterator.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

namespace {

// Computes the input update extent as the intersection of the output update extent and the input whole extent.
void computeInputUpdateExtent(int inputUpdateExtent[6], const int outputUpdateExtent[6], const int inputWholeExtent[6])
{
    memcpy(inputUpdateExtent, outputUpdateExtent, sizeof(int) * 6);

    // clip with the whole extent
    for (int i = 0; i < 3; i++)
    {
        if (inputUpdateExtent[2*i] < inputWholeExtent[2*i])
        {
            inputUpdateExtent[2*i] = inputWholeExtent[2*i];
        }

        if (inputUpdateExtent[2*i+1] > inputWholeExtent[2*i+1])
        {
            inputUpdateExtent[2*i+1] = inputWholeExtent[2*i+1];
        }
    }
}

// Returns true if all the given inputs are supported, i.e. they are 32-bit RGBA images.
bool allInputsAreSupported(vtkImageData **inputData, int numberOfInputs)
{
    for (int i = 0; i < numberOfInputs; i++)
    {
        if (inputData[i]->GetNumberOfScalarComponents() != 4 || inputData[i]->GetScalarType() != VTK_UNSIGNED_CHAR)
        {
            return false;
        }
    }

    return true;
}

// Returns true if the given extents have a non-empty intersection.
bool overlappingExtents(const int extent1[6], const int extent2[6])
{
    for (int i = 0; i < 3; i++)
    {
        if (extent1[2*i+1] < extent2[2*i] || extent1[2*i] > extent2[2*i+1])
        {
            return false;
        }
    }

    return true;
}

// Copies the values from the input data to the output data in the region inside the given extent.
void copyData(vtkImageData *inputData, vtkImageData *outputData, int extent[6])
{
    unsigned char *inputPointer = static_cast<unsigned char*>(inputData->GetScalarPointerForExtent(extent));
    unsigned char *outputPointer = static_cast<unsigned char*>(outputData->GetScalarPointerForExtent(extent));

    // Get increments to march through inputData
    vtkIdType incX, incY, incZ;
    inputData->GetIncrements(incX, incY, incZ);

    // find the region to loop over
    int rowLength = (extent[1] - extent[0] + 1) * incX * inputData->GetScalarSize();
    int maxY = extent[3] - extent[2];
    int maxZ = extent[5] - extent[4];

    incY *= inputData->GetScalarSize();
    incZ *= inputData->GetScalarSize();

    // Loop through outputData pixels
    for (int z = 0; z <= maxZ; z++)
    {
        unsigned char *sourcePointer = inputPointer + z*incZ;

        for (int y = 0; y <= maxY; y++)
        {
            memcpy(outputPointer, sourcePointer, rowLength);
            sourcePointer += incY;
            outputPointer += rowLength;
        }
    }
}

// Blends the input data over the output data in the region inside the given extent.
void blend(int extent[6], vtkImageData *inputData, vtkImageData *outputData)
{
    vtkImageIterator<unsigned char> inputIterator(inputData, extent);
    vtkImageIterator<unsigned char> outputIterator(outputData, extent);

    unsigned char *inputPointer = inputIterator.BeginSpan();
    unsigned char *inputSpanEndPointer = inputIterator.EndSpan();

    while (!outputIterator.IsAtEnd())
    {
        unsigned char* outputPointer = outputIterator.BeginSpan();
        unsigned char* outputSpanEndPointer = outputIterator.EndSpan();

        while (outputPointer != outputSpanEndPointer)
        {
            // Blending equations from: http://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
            double srcAlpha = inputPointer[3] / 255.0;
            double dstAlpha = outputPointer[3] / 255.0;
            double outAlpha = srcAlpha + dstAlpha * (1.0 - srcAlpha);
            unsigned char *src = inputPointer;
            unsigned char *dst = outputPointer;

            for (int i = 0; i < 3; i++)
            {
                if (outAlpha > 0.0)
                {
                    outputPointer[i] = static_cast<unsigned char>((src[i] * srcAlpha + dst[i] * dstAlpha * (1.0 - srcAlpha)) / outAlpha);
                }
                else
                {
                    outputPointer[i] = 0;
                }
            }

            outputPointer[3] = static_cast<unsigned char>(outAlpha * 255.0);

            outputPointer += 4;
            inputPointer += 4;
        }

        // go to the next span
        outputIterator.NextSpan();

        if (inputPointer == inputSpanEndPointer)
        {
            inputIterator.NextSpan();
            inputPointer = inputIterator.BeginSpan();
            inputSpanEndPointer = inputIterator.EndSpan();
        }
    }
}

}

namespace udg {

vtkStandardNewMacro(VtkCorrectImageBlend)

VtkCorrectImageBlend::VtkCorrectImageBlend()
{
}

VtkCorrectImageBlend::~VtkCorrectImageBlend()
{
}

int VtkCorrectImageBlend::FillInputPortInformation(int port, vtkInformation *info)
{
    if (port == 0)
    {
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
        info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
    }

    return 1;
}

int VtkCorrectImageBlend::RequestUpdateExtent(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation* outputInfo = outputVector->GetInformationObject(0);

    // default input extent will be that of output extent
    int *outputUpdateExtent = outputInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());

    for (int inputIndex = 0; inputIndex < this->GetNumberOfInputConnections(0); inputIndex++)
    {
        vtkInformation *inputInfo = inputVector[0]->GetInformationObject(inputIndex);
        int *inputWholeExtent = inputInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
        int inputUpdateExtent[6];
        computeInputUpdateExtent(inputUpdateExtent, outputUpdateExtent, inputWholeExtent);
        inputInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inputUpdateExtent, 6);
    }

    return 1;
}

void VtkCorrectImageBlend::ThreadedRequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector,
                                               vtkInformationVector *vtkNotUsed(outputVector), vtkImageData ***inputData, vtkImageData **outputData,
                                               int outputExtent[6], int vtkNotUsed(threadId))
{
    // check
    if (!allInputsAreSupported(inputData[0], this->GetNumberOfInputConnections(0)))
    {
        vtkErrorMacro("Only RGBA images are supported.");
        return;
    }

    // init
    copyData(inputData[0][0], outputData[0], outputExtent);

    // process each input
    for (int i = 1; i < this->GetNumberOfInputConnections(0); i++)
    {
        if (inputData[0][i] != 0)
        {
            // input extents
            vtkInformation *inputInfo = inputVector[0]->GetInformationObject(i);
            int *inputWholeExtent = inputInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
            int inputUpdateExtent[6];
            computeInputUpdateExtent(inputUpdateExtent, outputExtent, inputWholeExtent);

            if (!overlappingExtents(inputUpdateExtent, outputExtent))
            {
                // extents don't overlap, skip this input
                continue;
            }

            blend(inputUpdateExtent, inputData[0][i], outputData[0]);
        }
    }
}

} // namespace udg
