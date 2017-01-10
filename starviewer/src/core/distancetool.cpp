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

#include "distancetool.h"

#include "q2dviewer.h"
#include "drawer.h"
#include "drawerline.h"

// Vtk's
#include <vtkRenderWindowInteractor.h>

namespace udg {

DistanceTool::DistanceTool(QViewer *viewer, QObject *parent)
 : GenericDistanceTool(viewer, parent)
{
    m_toolName = "DistanceTool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
    initialize();
}

DistanceTool::~DistanceTool()
{
    deleteTemporalRepresentation();
}

void DistanceTool::deleteTemporalRepresentation()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè
    // l'annotació s'esborri si "matem" l'eina
    if (m_distanceLine)
    {
        m_distanceLine->decreaseReferenceCount();
        delete m_distanceLine;
        hasToRefresh = true;
    }

    if (hasToRefresh)
    {
        m_2DViewer->render();
    }

    m_lineState = NoPointFixed;
}

void DistanceTool::handleLeftButtonPress()
{
    handlePointAddition();
}

void DistanceTool::handleMouseMove()
{
    simulateLine();
}

void DistanceTool::abortDrawing()
{
    deleteTemporalRepresentation();
}

void DistanceTool::handlePointAddition()
{
    if (m_2DViewer->hasInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            this->annotateNewPoint();
        }
    }
}

void DistanceTool::annotateNewPoint()
{
    if (!m_distanceLine)
    {
        m_distanceLine = new DrawerLine;
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
        m_distanceLine->increaseReferenceCount();
    }

    auto clickedWorldPoint = m_2DViewer->getEventWorldCoordinate();

    // Afegim el punt
    if (m_lineState == NoPointFixed)
    {
        m_distanceLine->setFirstPoint(clickedWorldPoint);
        m_distanceLine->setSecondPoint(clickedWorldPoint);
        m_lineState = FirstPointFixed;

        m_2DViewer->getDrawer()->draw(m_distanceLine);
    }
    else
    {
        m_distanceLine->setSecondPoint(clickedWorldPoint);
        m_distanceLine->update();
        equalizeDepth();

        // Posem el text
        drawMeasurement();
        
        // Alliberem la primitiva perquè pugui ser esborrada
        m_distanceLine->decreaseReferenceCount();
        m_2DViewer->getDrawer()->erasePrimitive(m_distanceLine);
        // Coloquem la primitiva en el pla corresponent
        m_2DViewer->getDrawer()->draw(m_distanceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Reiniciem l'estat de la tool
        m_lineState = NoPointFixed;
        m_distanceLine = NULL;
    }
}

void DistanceTool::simulateLine()
{
    if (m_distanceLine)
    {
        auto clickedWorldPoint = m_2DViewer->getEventWorldCoordinate();

        m_distanceLine->setSecondPoint(clickedWorldPoint);
        m_distanceLine->update();
        m_2DViewer->render();
    }
}

void DistanceTool::initialize()
{
    if (m_distanceLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_distanceLine->decreaseReferenceCount();
        delete m_distanceLine;
    }
    m_lineState = NoPointFixed;
    m_distanceLine = NULL;
}

void DistanceTool::equalizeDepth()
{
    auto firstPoint = m_distanceLine->getFirstPoint();
    auto firstPointDisplay = m_2DViewer->computeWorldToDisplay(firstPoint);
    firstPointDisplay.z = 0;
    firstPoint = m_2DViewer->computeDisplayToWorld(firstPointDisplay);
    m_distanceLine->setFirstPoint(firstPoint);

    auto secondPoint = m_distanceLine->getSecondPoint();
    auto secondPointDisplay = m_2DViewer->computeWorldToDisplay(secondPoint);
    secondPointDisplay.z = 0;
    secondPoint = m_2DViewer->computeDisplayToWorld(secondPointDisplay);
    m_distanceLine->setSecondPoint(secondPoint);

    m_distanceLine->update();
}

}
