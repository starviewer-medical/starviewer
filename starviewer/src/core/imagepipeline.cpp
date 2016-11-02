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

#include "imagepipeline.h"

#include "phasefilter.h"
#include "voilut.h"
#include "vtkRunThroughFilter.h"
#include "windowlevelfilter.h"

#include <vtkImageData.h>

namespace udg {

ImagePipeline::ImagePipeline()
 : m_input(nullptr), m_enableColorMapping(false), m_hasTransferFunction(false)
{
    m_phaseFilter = new PhaseFilter();
    m_windowLevelLUTFilter = new WindowLevelFilter();
    m_outputFilter = vtkRunThroughFilter::New();
}

ImagePipeline::~ImagePipeline()
{
    delete m_phaseFilter;
    delete m_windowLevelLUTFilter;
    m_outputFilter->Delete();
}

void ImagePipeline::setInput(vtkImageData *input)
{
    m_input = input;
    rebuild();
}

void ImagePipeline::setInput(FilterOutput input)
{
    setInput(input.getVtkImageData());
}

void ImagePipeline::setNumberOfPhases(int numberOfPhases)
{
    m_phaseFilter->setNumberOfPhases(numberOfPhases);
    rebuild();
}

void ImagePipeline::setPhase(int phase)
{
    m_phaseFilter->setPhase(phase);
}

vtkImageData* ImagePipeline::getPhaseOutput()
{
    return m_phaseFilter->getOutput().getVtkImageData();
}

void ImagePipeline::enableColorMapping(bool enable)
{
    m_enableColorMapping = enable;
    rebuild();
}

void ImagePipeline::setVoiLut(const VoiLut &voiLut)
{
    m_windowLevelLUTFilter->setWindowLevel(voiLut.getWindowLevel());

    if (!m_hasTransferFunction)
    {
        if (voiLut.isLut())
        {
            m_windowLevelLUTFilter->setTransferFunction(voiLut.getLut());
        }
        else
        {
            m_windowLevelLUTFilter->clearTransferFunction();
        }
    }
}

void ImagePipeline::setTransferFunction(const TransferFunction &transferFunction)
{
    m_windowLevelLUTFilter->setTransferFunction(transferFunction);
    m_hasTransferFunction = true;
}

void ImagePipeline::clearTransferFunction()
{
    m_windowLevelLUTFilter->clearTransferFunction();
    m_hasTransferFunction = false;
}

vtkAlgorithm* ImagePipeline::getVtkAlgorithm() const
{
    return m_outputFilter;
}

void ImagePipeline::rebuild()
{
    if (m_phaseFilter->getNumberOfPhases() > 1 && m_enableColorMapping)
    {
        m_phaseFilter->setInput(m_input);
        m_windowLevelLUTFilter->setInput(m_phaseFilter->getOutput());
        m_outputFilter->SetInputConnection(m_windowLevelLUTFilter->getOutput().getVtkAlgorithmOutput());
    }
    else if (m_phaseFilter->getNumberOfPhases() > 1)
    {
        m_phaseFilter->setInput(m_input);
        m_outputFilter->SetInputConnection(m_phaseFilter->getOutput().getVtkAlgorithmOutput());
    }
    else if (m_enableColorMapping)
    {
        m_windowLevelLUTFilter->setInput(m_input);
        m_outputFilter->SetInputConnection(m_windowLevelLUTFilter->getOutput().getVtkAlgorithmOutput());
    }
    else
    {
        m_outputFilter->SetInputData(m_input);
    }
}

}
