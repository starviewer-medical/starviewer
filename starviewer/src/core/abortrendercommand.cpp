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

#include "abortrendercommand.h"
#include <vtkRenderWindow.h>
#include "logging.h"

namespace udg {

AbortRenderCommand* AbortRenderCommand::New()
{
    return new AbortRenderCommand();
}

void AbortRenderCommand::Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData))
{
    Q_ASSERT(eventId == AbortCheckEvent);

    vtkRenderWindow *renderWindow = vtkRenderWindow::SafeDownCast(caller);

    if (!renderWindow)
    {
        DEBUG_LOG("El caller no és una vtkRenderWindow!");
        return;
    }

    if (renderWindow->GetEventPending())
    {
        renderWindow->SetAbortRender(1);
    }
}

AbortRenderCommand::AbortRenderCommand()
 : vtkCommand()
{
}

AbortRenderCommand::~AbortRenderCommand()
{
}

}
