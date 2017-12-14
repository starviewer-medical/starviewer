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

#include "qexampleextension.h"

#include "patient.h"
#include "toolmanager.h"

namespace udg {

QExampleExtension::QExampleExtension(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    ToolManager *toolManager2D = new ToolManager(this);
    toolManager2D->registerTool("ZoomTool");            // left button
    toolManager2D->registerTool("TranslateTool");       // middle button
    toolManager2D->registerTool("WindowLevelTool");     // right button
    toolManager2D->registerTool("SlicingKeyboardTool"); // keyboard
    toolManager2D->registerTool("SlicingWheelTool");    // wheel
    toolManager2D->setupRegisteredTools(m_2DViewer->getViewer());
    toolManager2D->triggerTools({"ZoomTool", "TranslateTool", "WindowLevelTool", "SlicingKeyboardTool", "SlicingWheelTool"});

    ToolManager *toolManager3D = new ToolManager(this);
    toolManager3D->registerTool("ZoomTool");        // left button
    toolManager3D->registerTool("TranslateTool");   // middle button
    toolManager3D->registerTool("Rotate3DTool");    // right button
    toolManager3D->setupRegisteredTools(m_3DViewer);
    toolManager3D->triggerTools({"ZoomTool", "TranslateTool", "Rotate3DTool"});
}

void QExampleExtension::setPatient(Patient *patient)
{
    if (patient->getNumberOfVolumes() == 0)
    {
        return;
    }

    m_2DViewer->setInputAsynchronously(patient->getVolumesList().first());
    m_3DViewer->setInput(patient->getVolumesList().first());
}

} // namespace udg
