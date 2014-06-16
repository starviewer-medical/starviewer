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

#include "windowlevelfilter.h"

#include "transferfunction.h"
#include "filteroutput.h"
#include "windowlevel.h"

#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include "vtkImageMapToWindowLevelColors3.h"
#include <vtkLookupTable.h>

namespace udg {

WindowLevelFilter::WindowLevelFilter()
{
    m_filter = vtkImageMapToWindowLevelColors3::New();
}

WindowLevelFilter::~WindowLevelFilter()
{
    m_filter->Delete();
}

void WindowLevelFilter::setInput(vtkImageData *input)
{
    m_filter->SetInputData(input);
}

void WindowLevelFilter::setInput(FilterOutput input)
{
    m_filter->SetInputConnection(input.getVtkAlgorithmOutput());
}

void WindowLevelFilter::setWindowLevel(const WindowLevel &windowLevel)
{
    setWindowLevel(windowLevel.getWidth(), windowLevel.getCenter());
}

void WindowLevelFilter::setWindowLevel(double window, double level)
{
    m_filter->SetWindow(window);
    m_filter->SetLevel(level);
}

void WindowLevelFilter::setWindow(double window)
{
    m_filter->SetWindow(window);
}

double WindowLevelFilter::getWindow() const
{
    return m_filter->GetWindow();
}

void WindowLevelFilter::setLevel(double level)
{
    m_filter->SetLevel(level);
}

double WindowLevelFilter::getLevel() const
{
    return m_filter->GetLevel();
}

void WindowLevelFilter::setTransferFunction(const TransferFunction &transferFunction)
{
    m_filter->SetLookupTable(transferFunction.toVtkLookupTable());
}

void WindowLevelFilter::clearTransferFunction()
{
    m_filter->SetLookupTable(0);
}

vtkAlgorithm* WindowLevelFilter::getVtkAlgorithm() const
{
    return m_filter;
}

}
