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

#include "ellipticalroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "series.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "mathtools.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

EllipticalROITool::EllipticalROITool(QViewer *viewer, QObject *parent)
 : ROITool(viewer, parent), m_state(Ready)
{
    m_toolName = "EllipticalROITool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

EllipticalROITool::~EllipticalROITool()
{
    deleteTemporalRepresentation();
}

void EllipticalROITool::deleteTemporalRepresentation()
{
    if (!m_roiPolygon.isNull() && m_state == FirstPointFixed)
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = 0;
    m_state = Ready;
}

void EllipticalROITool::handleEvent(long unsigned eventID)
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateEllipse();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            closeForm();
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27) // ESC
            {
                deleteTemporalRepresentation();
            }
            break;
    }
}

void EllipticalROITool::setTextPosition(DrawerText *text)
{
    auto firstPointDisplay = m_2DViewer->computeWorldToDisplay(m_firstPoint);
    auto secondPointDisplay = m_2DViewer->computeWorldToDisplay(m_secondPoint);
    auto attachmentPointDisplay = secondPointDisplay;
    attachmentPointDisplay.x = (firstPointDisplay.x + secondPointDisplay.x) / 2.0;

    const double Padding = 5.0;

    if (secondPointDisplay.y <= firstPointDisplay.y)
    {
        attachmentPointDisplay.y -= Padding;
        text->setVerticalJustification("Top");
    }
    else
    {
        attachmentPointDisplay.y += Padding;
        text->setVerticalJustification("Bottom");
    }

    auto attachmentPoint = m_2DViewer->computeDisplayToWorld(attachmentPointDisplay);
    text->setAttachmentPoint(attachmentPoint);
}

void EllipticalROITool::handlePointAddition()
{
    if (m_state == Ready)
    {
        m_firstPoint = m_2DViewer->getEventWorldCoordinate();
        m_firstPoint = m_2DViewer->putCoordinateInCurrentImagePlane(m_firstPoint);

        m_secondPoint = m_firstPoint;

        m_state = FirstPointFixed;
    }
}

void EllipticalROITool::simulateEllipse()
{
    if (m_state == FirstPointFixed)
    {
        // Obtenim el segon punt
        m_secondPoint = m_2DViewer->getEventWorldCoordinate();
        m_secondPoint = m_2DViewer->putCoordinateInCurrentImagePlane(m_secondPoint);

        // Si encara no havíem creat el polígon, ho fem
        if (!m_roiPolygon)
        {
            m_roiPolygon = new DrawerPolygon;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_roiPolygon->increaseReferenceCount();
            m_2DViewer->getDrawer()->draw(m_roiPolygon);
        }

        // Actualitzem la forma i renderitzem
        updatePolygonPoints();
        m_2DViewer->render();
    }
}

void EllipticalROITool::updatePolygonPoints()
{
    auto firstPointDisplay = m_2DViewer->computeWorldToDisplay(m_firstPoint);
    auto secondPointDisplay = m_2DViewer->computeWorldToDisplay(m_secondPoint);
    auto centreDisplay = (firstPointDisplay + secondPointDisplay) * 0.5;

    // Algorisme pel càlcul de l'el·lipse, extret de http://en.wikipedia.org/wiki/Ellipse#Ellipses_in_computer_graphics
    double xRadius = (secondPointDisplay.x - firstPointDisplay.x) * 0.5;
    double yRadius = (secondPointDisplay.y - firstPointDisplay.y) * 0.5;
    double depthValue = centreDisplay.z;

    const int PolygonPoints = 50;
    int vertexIndex = 0;

    for (double i = 0; i < 360.0; i += 360.0 / PolygonPoints)
    {
        double alpha = MathTools::degreesToRadians(i);
        double sinusAlpha = sin(alpha);
        double cosinusAlpha = cos(alpha);
        Vector3 polygonPointDisplay(centreDisplay.x + xRadius * cosinusAlpha, centreDisplay.y + yRadius * sinusAlpha, depthValue);
        m_roiPolygon->setVertex(vertexIndex++, m_2DViewer->computeDisplayToWorld(polygonPointDisplay));
    }

    m_roiPolygon->update();
}

void EllipticalROITool::closeForm()
{
    // Cal comprovar si hi ha un objecte creat ja que podria ser que no s'hagués creat si s'hagués realitzat un doble clic,
    // per exemple, ja que no s'hauria passat per l'event de mouse move, que és quan es crea la primitiva.
    if (m_roiPolygon)
    {
        equalizeDepth();
        printData();
        // Alliberem la primitiva perquè pugui ser esborrada
        m_roiPolygon->decreaseReferenceCount();
        // Pintem la primitiva al lloc corresponent
        m_2DViewer->getDrawer()->erasePrimitive(m_roiPolygon);
        m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Inicialitzem el punter a 0
        m_roiPolygon = 0;
    }

    m_state = Ready;
}

void EllipticalROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = 0;
    m_state = Ready;
}

void EllipticalROITool::equalizeDepth()
{
    double z = m_2DViewer->getCurrentDisplayedImageDepth();
    auto pointDisplay = m_2DViewer->computeWorldToDisplay(m_firstPoint);
    pointDisplay.z = z;
    m_firstPoint = m_2DViewer->computeDisplayToWorld(pointDisplay);
    pointDisplay = m_2DViewer->computeWorldToDisplay(m_secondPoint);
    pointDisplay.z = z;
    m_secondPoint = m_2DViewer->computeDisplayToWorld(pointDisplay);
    updatePolygonPoints();
}

}
