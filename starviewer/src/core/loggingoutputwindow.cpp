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

#include "loggingoutputwindow.h"

#include "logging.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>

namespace udg {

vtkStandardNewMacro(LoggingOutputWindow);

void LoggingOutputWindow::DisplayText(const char *text)
{
    INFO_LOG(text);
}

void LoggingOutputWindow::DisplayErrorText(const char *text)
{
    // HACK: ignore error thrown when using the magnifying glass tool
    QString string(text);
    if (string.contains("vtkImageMapper3D.cxx") && string.contains("Cannot follow camera"))
    {
        return;
    }

    ERROR_LOG(text);
    this->InvokeEvent(vtkCommand::ErrorEvent, const_cast<char*>(text));
}

void LoggingOutputWindow::DisplayWarningText(const char *text)
{
    WARN_LOG(text);
    this->InvokeEvent(vtkCommand::WarningEvent, const_cast<char*>(text));
}

void LoggingOutputWindow::DisplayGenericWarningText(const char *text)
{
    WARN_LOG(text);
}

void LoggingOutputWindow::DisplayDebugText(const char *text)
{
    DEBUG_LOG(text);
}

LoggingOutputWindow::LoggingOutputWindow()
{
}

LoggingOutputWindow::~LoggingOutputWindow()
{
}

}
