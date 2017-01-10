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

#include "circletool.h"

#include "drawer.h"
#include "drawercrosshair.h"
#include "drawerpolygon.h"
#include "mathtools.h"
#include "q2dviewer.h"

#include <cmath>

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

CircleTool::CircleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "CircleTool";
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));

    initialize();
}

CircleTool::~CircleTool()
{
    initialize();
}

void CircleTool::handleEvent(unsigned long eventId)
{
    if (!m_2DViewer || !m_2DViewer->hasInput())
    {
        return;
    }

    switch (eventId)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (!m_isDrawing)
            {
                startDrawing();
            }
            break;
        case vtkCommand::MouseMoveEvent:
            if (m_isDrawing)
            {
                updateCircle();
            }
            break;
        case vtkCommand::LeftButtonReleaseEvent:
            if (m_isDrawing)
            {
                endDrawing();
            }
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27 && m_isDrawing)  // Esc
            {
                abortDrawing();
            }
            break;
    }
}

void CircleTool::startDrawing()
{
    Q_ASSERT(!m_isDrawing);

    m_isDrawing = true;
    m_startPoint = m_2DViewer->getEventWorldCoordinate();
}

void CircleTool::endDrawing()
{
    Q_ASSERT(m_isDrawing);

    // Cal comprovar si hi ha un objecte creat ja que podria ser que no s'hagués creat si s'hagués realitzat un doble clic,
    // per exemple, ja que no s'hauria passat per l'event de mouse move, que és quan es crea la primitiva.
    if (m_circle)
    {
        // Alliberem la primitiva perquè pugui ser esborrada
        m_circle->decreaseReferenceCount();
        // Pintem la primitiva al lloc corresponent
        m_2DViewer->getDrawer()->erasePrimitive(m_circle);
        equalizeDepth();
        m_2DViewer->getDrawer()->draw(m_circle, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Inicialitzem el punter a 0
        m_circle = 0;

        // Dibuixem la creu al centre
        Vector3 center = getCenter();
        DrawerCrossHair *cross = new DrawerCrossHair();
        cross->setPosition(center);
        m_2DViewer->getDrawer()->draw(cross, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    }

    m_isDrawing = false;
}

void CircleTool::abortDrawing()
{
    Q_ASSERT(m_isDrawing);

    initialize();
}

void CircleTool::updateCircle()
{
    Q_ASSERT(m_isDrawing);

    getEndPoint();

    // Si encara no havíem creat el polígon, ho fem
    if (!m_circle)
    {
        m_circle = new DrawerPolygon();
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_circle->increaseReferenceCount();
        m_2DViewer->getDrawer()->draw(m_circle);
    }

    // Actualitzem la forma i renderitzem
    updatePolygonPoints();
    m_2DViewer->render();
}

void CircleTool::getEndPoint()
{
    m_endPoint = m_2DViewer->getEventWorldCoordinate();

    auto startPointDisplay = m_2DViewer->computeWorldToDisplay(m_startPoint);
    auto endPointDisplay = m_2DViewer->computeWorldToDisplay(m_endPoint);

    double width = endPointDisplay.x - startPointDisplay.x;
    double height = endPointDisplay.y - startPointDisplay.y;
    
    // Ens quedem amb la mida més gran
    if (qAbs(width) > qAbs(height))
    {
        endPointDisplay.y = startPointDisplay.y + std::copysign(width, height);
    }
    else
    {
        endPointDisplay.x = startPointDisplay.x + std::copysign(height, width);
    }

    m_endPoint = m_2DViewer->computeDisplayToWorld(endPointDisplay);
}

void CircleTool::updatePolygonPoints()
{
    auto center = getCenter();
    auto centerDisplay = m_2DViewer->computeWorldToDisplay(center);
    double a = centerDisplay.x;
    double b = centerDisplay.y;
    auto startPointDisplay = m_2DViewer->computeWorldToDisplay(m_startPoint);
    double radius = qAbs(startPointDisplay.x - centerDisplay.x);

    m_circle->removeVertices();

    const int NumberOfPoints = 360;
    
    for (int i = 0; i < NumberOfPoints; i++)
    {
        double angle = static_cast<double>(i) / NumberOfPoints * 2.0 * MathTools::PiNumber;
        Vector3 pointDisplay(a + radius * cos(angle), b + radius * sin(angle), centerDisplay.z);
        auto point = m_2DViewer->computeDisplayToWorld(pointDisplay);
        m_circle->addVertex(point);
    }

    m_circle->update();
}

Vector3 CircleTool::getCenter() const
{
    return 0.5 * (m_startPoint + m_endPoint);
}

void CircleTool::equalizeDepth()
{
    auto startPointDisplay = m_2DViewer->computeWorldToDisplay(m_startPoint);
    startPointDisplay.z = 0;
    m_startPoint = m_2DViewer->computeDisplayToWorld(startPointDisplay);

    auto endPointDisplay = m_2DViewer->computeWorldToDisplay(m_endPoint);
    endPointDisplay.z = 0;
    m_endPoint = m_2DViewer->computeDisplayToWorld(endPointDisplay);

    updatePolygonPoints();
}

void CircleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (m_circle)
    {
        m_circle->decreaseReferenceCount();
        delete m_circle;
        m_2DViewer->render();
    }

    m_circle = 0;
    m_isDrawing = false;
}

}
