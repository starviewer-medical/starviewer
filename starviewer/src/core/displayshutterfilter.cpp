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

#include "displayshutterfilter.h"

#include "filteroutput.h"
#include "volume.h"

#include <vtkImageMask.h>

namespace udg {

DisplayShutterFilter::DisplayShutterFilter()
{
    m_imageMask = vtkImageMask::New();
    m_imageMask->SetMaskAlpha(1.0);
    m_imageMask->SetMaskedOutputValue(0);
    m_imageMask->NotMaskOn();
}

DisplayShutterFilter::~DisplayShutterFilter()
{
    m_imageMask->Delete();
}

void DisplayShutterFilter::setInput(Volume *input)
{
    m_imageMask->SetImageInputData(input->getVtkData());
}

void DisplayShutterFilter::setInput(vtkImageData *input)
{
    m_imageMask->SetImageInputData(input);
}

void DisplayShutterFilter::setInput(FilterOutput input)
{
    m_imageMask->SetInputConnection(input.getVtkAlgorithmOutput());
}

void DisplayShutterFilter::setDisplayShutter(vtkImageData *displayShutter)
{
    m_imageMask->SetMaskInputData(displayShutter);
}

void DisplayShutterFilter::setDisplayShutter(FilterOutput displayShutter)
{
    this->setDisplayShutter(displayShutter.getVtkImageData());
}

vtkAlgorithm* DisplayShutterFilter::getVtkAlgorithm() const
{
    return m_imageMask;
}

}
