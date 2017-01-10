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

#include "polylineroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"

// Vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

PolylineROITool::PolylineROITool(QViewer *viewer, QObject *parent)
 : ROITool(viewer, parent), m_mainPolyline(0), m_closingPolyline(0)
{
    m_toolName = "PolylineROITool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

PolylineROITool::~PolylineROITool()
{
    deleteTemporalRepresentation();
}

void PolylineROITool::deleteTemporalRepresentation()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè
    // l'annotació s'esborri si "matem" l'eina
    if (!m_mainPolyline.isNull())
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
        hasToRefresh = true;
    }

    if (!m_closingPolyline.isNull())
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
        hasToRefresh = true;
    }

    if (!m_roiPolygon.isNull())
    {
        delete m_roiPolygon;
    }

    if (hasToRefresh)
    {
        m_2DViewer->render();
    }
}

void PolylineROITool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateClosingPolyline();
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

void PolylineROITool::handlePointAddition()
{
    if (m_2DViewer->hasInput())
    {
        switch (m_2DViewer->getInteractor()->GetRepeatCount())
        {
            // Single-click o primer click d'un doble click. Afegim un nou punt a la ROI
            case 0:
                annotateNewPoint();
                break;
            // Doble-click, si tenim més de 2 punts, llavors tanquem la ROI
            case 1:
                if (m_mainPolyline->getNumberOfPoints() > 2)
                {
                    closeForm();
                }
                break;
        }
    }
}

void PolylineROITool::annotateNewPoint()
{
    Vector3 pickedPoint = m_2DViewer->getEventWorldCoordinate();
    pickedPoint = m_2DViewer->putCoordinateInCurrentImagePlane(pickedPoint);

    bool firstPoint = false;
    if (!m_mainPolyline)
    {
        firstPoint = true;
        m_mainPolyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_mainPolyline->increaseReferenceCount();
    }
    // Afegim el punt de la nova polilínia
    m_mainPolyline->addPoint(pickedPoint);

    // L'afegim a l'escena
    if (firstPoint)
    {
        m_2DViewer->getDrawer()->draw(m_mainPolyline);
    }
    // Actualitzem l'estructura interna
    else
    {
        m_mainPolyline->update();
    }

    if (!m_roiPolygon)
    {
        m_roiPolygon = new DrawerPolygon;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_roiPolygon->increaseReferenceCount();
    }
    // Afegim el punt al polígon de la ROI
    m_roiPolygon->addVertex(pickedPoint);
}

void PolylineROITool::simulateClosingPolyline()
{
    if (m_mainPolyline && (m_mainPolyline->getNumberOfPoints() >= 1))
    {
        Vector3 pickedPoint = m_2DViewer->getEventWorldCoordinate();
        pickedPoint = m_2DViewer->putCoordinateInCurrentImagePlane(pickedPoint);

        if (!m_closingPolyline)
        {
            m_closingPolyline = new DrawerPolyline;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_closingPolyline->increaseReferenceCount();
            m_closingPolyline->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);
            m_2DViewer->getDrawer()->draw(m_closingPolyline);

            // Afegim els punts que simulen aquesta polilínia
            m_closingPolyline->addPoint(m_mainPolyline->getPoint(0));
            m_closingPolyline->addPoint(pickedPoint);
        }
        else
        {
            // Modifiquem els punts que han canviat
            m_closingPolyline->setPoint(1, pickedPoint);
            if (m_mainPolyline->getNumberOfPoints() >= 2)
            {
                m_closingPolyline->setPoint(2, m_mainPolyline->getPoint(m_mainPolyline->getNumberOfPoints() - 1));
            }
            // Actualitzem els atributs de la polilínia
            m_closingPolyline->update();
        }
        m_2DViewer->render();
    }
}

void PolylineROITool::closeForm()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_closingPolyline->decreaseReferenceCount();
    m_mainPolyline->decreaseReferenceCount();
    m_roiPolygon->decreaseReferenceCount();
    // Eliminem les polilínies amb les que hem simulat el dibuix de la ROI
    delete m_closingPolyline;
    delete m_mainPolyline;

    equalizeDepth();

    // Dibuixem el polígon resultant
    m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Indiquem que hem finalitzat les tasques de dibuix
    printData();

    // Un cop fets els càlculs, fem el punter nul per poder controlar si podem fer una nova roi o no
    // No fem delete, perquè sinó això faria que s'esborrés del drawer
    m_roiPolygon = NULL;
}

void PolylineROITool::setTextPosition(DrawerText *text)
{
    Vector3 lastPoint = m_2DViewer->getEventWorldCoordinate();
    lastPoint = m_2DViewer->putCoordinateInCurrentImagePlane(lastPoint);
    auto lastPointDisplay = m_2DViewer->computeWorldToDisplay(lastPoint);

    auto displayBoundingBoxPoints = getDisplayBoundingBoxPoints();

    Vector3 closestPointDisplay;
    int closestEdgeDisplay;
    MathTools::getPointToClosestEdgeDistance(lastPointDisplay, displayBoundingBoxPoints, true, closestPointDisplay, closestEdgeDisplay);

    const double Padding = 5.0;

    //     2
    //    ____
    // 3 |    | 1
    //   |____|
    //     0
    switch (closestEdgeDisplay)
    {
        case 0:
            closestPointDisplay.y -= Padding;
            text->setVerticalJustification("Top");
            text->setHorizontalJustification("Centered");
            break;
        case 1:
            closestPointDisplay.x += Padding;
            text->setVerticalJustification("Centered");
            text->setHorizontalJustification("Left");
            break;
        case 2:    
            closestPointDisplay.y += Padding;
            text->setVerticalJustification("Bottom");
            text->setHorizontalJustification("Centered");
            break;
        case 3:
            closestPointDisplay.x -= Padding;
            text->setVerticalJustification("Centered");
            text->setHorizontalJustification("Right");
            break;
    }

    auto closestPoint = m_2DViewer->computeDisplayToWorld(closestPointDisplay);
    text->setAttachmentPoint(closestPoint);
}

QList<Vector3> PolylineROITool::getDisplayBoundingBoxPoints()
{
    auto displayBounds = getDisplayBounds();

    QList<Vector3> pointsList;
    pointsList << Vector3(displayBounds[0], displayBounds[2], 0.0)
               << Vector3(displayBounds[1], displayBounds[2], 0.0)
               << Vector3(displayBounds[1], displayBounds[3], 0.0)
               << Vector3(displayBounds[0], displayBounds[3], 0.0);

    return pointsList;
}

void PolylineROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_mainPolyline.isNull())
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }

    if (!m_closingPolyline.isNull())
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
    }

    if (!m_roiPolygon.isNull())
    {
        delete m_roiPolygon;
    }

    m_closingPolyline = NULL;
    m_mainPolyline = NULL;
    m_roiPolygon = NULL;
}

void PolylineROITool::equalizeDepth()
{
    double z = m_2DViewer->getCurrentDisplayedImageDepth();

    for (int i = 0; i < m_roiPolygon->getNumberOfPoints(); i++)
    {
        auto point = m_roiPolygon->getVertex(i);
        auto pointDisplay = m_2DViewer->computeWorldToDisplay(point);
        pointDisplay.z = z;
        point = m_2DViewer->computeDisplayToWorld(pointDisplay);
        m_roiPolygon->setVertex(i, point);
    }

    m_roiPolygon->update();
}

}
