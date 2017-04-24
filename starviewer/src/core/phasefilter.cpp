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

#include "phasefilter.h"

#include "vtkimageextractphase.h"

#include <vtkImageData.h>

namespace udg {

PhaseFilter::PhaseFilter()
{
    m_filter = VtkImageExtractPhase::New();
}

PhaseFilter::~PhaseFilter()
{
    m_filter->Delete();
}

void PhaseFilter::setInput(vtkImageData *input)
{
    m_filter->SetInputData(input);
}

void PhaseFilter::setInput(FilterOutput input)
{
    m_filter->SetInputConnection(input.getVtkAlgorithmOutput());
}

int PhaseFilter::getNumberOfPhases() const
{
    return m_filter->getNumberOfPhases();
}

void PhaseFilter::setNumberOfPhases(int numberOfPhases)
{
    m_filter->setNumberOfPhases(numberOfPhases);
}

int PhaseFilter::getPhase() const
{
    return m_filter->getPhase();
}

void PhaseFilter::setPhase(int phase)
{
    m_filter->setPhase(phase);
}

vtkAlgorithm* PhaseFilter::getVtkAlgorithm() const
{
    return m_filter;
}

} // namespace udg
