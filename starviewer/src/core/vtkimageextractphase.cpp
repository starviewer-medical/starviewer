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

#include "vtkimageextractphase.h"

#include "logging.h"

#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

namespace udg {

vtkStandardNewMacro(VtkImageExtractPhase)

void VtkImageExtractPhase::PrintSelf(std::ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Number of phases: " << m_numberOfPhases << "\n";
    os << indent << "Phase: " << m_phase << "\n";
}

int VtkImageExtractPhase::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

void VtkImageExtractPhase::setNumberOfPhases(int numberOfPhases)
{
    m_numberOfPhases = numberOfPhases;
    this->Modified();
}

int VtkImageExtractPhase::getPhase() const
{
    return m_phase;
}

void VtkImageExtractPhase::setPhase(int phase)
{
    m_phase = phase;
    this->Modified();
}

VtkImageExtractPhase::VtkImageExtractPhase()
 : m_numberOfPhases(1), m_phase(0)
{
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

VtkImageExtractPhase::~VtkImageExtractPhase()
{
}

int VtkImageExtractPhase::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    this->CopyInputArrayAttributesToOutput(request, inputVector, outputVector);

    if (canExtractPhase(inputVector[0]->GetInformationObject(0)))
    {
        vtkInformation *outInfo = outputVector->GetInformationObject(0);
        int extent[6];
        outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);

        int depth = extent[5] - extent[4] + 1;
        depth /= m_numberOfPhases;
        extent[5] = depth - 1;

        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);
    }

    return 1;
}

int VtkImageExtractPhase::RequestUpdateExtent(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    if (canExtractPhase(inInfo))
    {
        vtkInformation *outInfo = outputVector->GetInformationObject(0);
        int extent[6];
        outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), extent);

        computeInputExtentFromOutputExtent(extent, extent);

        inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), extent, 6);
    }

    return 1;
}

namespace {

// Executes the algorithm for a specific data type.
template <class T>
void execute(vtkImageData *inData, vtkImageData *outData, int inExtent[6], int outExtent[6], int numberOfPhases)
{
    T *outPtr = static_cast<T*>(outData->GetScalarPointerForExtent(outExtent));

    // Get increments to march through data
    vtkIdType inIncX, inIncY, inIncZ;
    inData->GetContinuousIncrements(inExtent, inIncX, inIncY, inIncZ);
    vtkIdType outIncX, outIncY, outIncZ;
    outData->GetContinuousIncrements(outExtent, outIncX, outIncY, outIncZ);

    for (int iz = inExtent[4], oz = outExtent[4]; oz <= outExtent[5]; iz += numberOfPhases, oz++)
    {
        // Extract each time the input pointer for the current slice
        inExtent[4] = iz;
        T *inPtr = static_cast<T*>(inData->GetScalarPointerForExtent(inExtent));

        for (int y = outExtent[2]; y <= outExtent[3]; y++)
        {
            for (int x = outExtent[0]; x <= outExtent[1]; x++)
            {
                for (int c = 0; c < outData->GetNumberOfScalarComponents(); c++)
                {
                    // TODO progress
                    *outPtr = *inPtr;
                    inPtr++;
                    outPtr++;
                }

//                inPtr += inIncX;      // incX is always 0 according to the documentation
//                outPtr += outIncX;    // incX is always 0 according to the documentation
            }

            inPtr += inIncY;
            outPtr += outIncY;
        }

        outPtr += outIncZ;
    }
}

}

void VtkImageExtractPhase::ThreadedRequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector,
                                               vtkInformationVector *vtkNotUsed(outputVector), vtkImageData ***inData, vtkImageData **outData, int outExtent[6],
                                               int vtkNotUsed(threadId))
{
    int inExtent[6];
    int numberOfPhases;

    if (canExtractPhase(inputVector[0]->GetInformationObject(0)))
    {
        computeInputExtentFromOutputExtent(inExtent, outExtent);
        numberOfPhases = m_numberOfPhases;
    }
    else
    {
        memcpy(inExtent, outExtent, sizeof(inExtent));
        numberOfPhases = 1;
        WARN_LOG(QString("Can't extract the desired phase. Will copy the whole image instead. (number of phases = %1, phase = %2")
                 .arg(m_numberOfPhases).arg(m_phase));
    }

    switch (inData[0][0]->GetScalarType())
    {
        vtkTemplateMacro(execute<VTK_TT>(inData[0][0], outData[0], inExtent, outExtent, numberOfPhases));

        default:
            ERROR_LOG("Unknown scalar type");
    }
}

bool VtkImageExtractPhase::canExtractPhase(vtkInformation *inInfo) const
{
    int extent[6];
    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
    int depth = extent[5] - extent[4] + 1;

    return m_numberOfPhases > 0 && m_phase >= 0 && m_phase < m_numberOfPhases && depth % m_numberOfPhases == 0;
}

void VtkImageExtractPhase::computeInputExtentFromOutputExtent(int inExtent[6], const int outExtent[6]) const
{
    for (int i = 0; i < 4; i++)
    {
        inExtent[i] = outExtent[i];
    }

    inExtent[4] = outExtent[4] * m_numberOfPhases + m_phase;
    inExtent[5] = outExtent[5] * m_numberOfPhases + m_phase;
}

} // namespace udg
