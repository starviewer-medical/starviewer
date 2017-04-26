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

#include "windowlevellefttool.h"

#include <vtkCommand.h>

namespace udg {

WindowLevelLeftTool::WindowLevelLeftTool(QViewer *viewer, QObject *parent)
    : WindowLevelTool(viewer, parent)
{
    m_toolName = "WindowLevelLeftTool";
}

void WindowLevelLeftTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            this->startWindowLevel();
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state != None)
            {
                this->doWindowLevel();
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            this->endWindowLevel();
            break;

        default:
            break;
    }
}

} // namespace udg
