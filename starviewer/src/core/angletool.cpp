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

#include "angletool.h"
#include "q2dviewer.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"
// Per la funció swap (si passem a C++11 s'haurà de canviar per <utility>)
#include <algorithm>
// VTK
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

#include <QVector2D>

namespace udg {

AngleTool::AngleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_mainPolyline(NULL), m_circlePolyline(NULL), m_state(None)
{
    m_toolName = "AngleTool";
    m_hasSharedData = false;

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

AngleTool::~AngleTool()
{
    deleteTemporalRepresentation();
}

void AngleTool::deleteTemporalRepresentation()
{
    if (m_state != None)
    {
        bool hasToRefresh = false;
        // Cal decrementar el reference count perquè
        // l'annotació s'esborri si "matem" l'eina
        if (m_mainPolyline)
        {
            m_mainPolyline->decreaseReferenceCount();
            delete m_mainPolyline;
            hasToRefresh = true;
        }

        if (m_circlePolyline)
        {
            m_circlePolyline->decreaseReferenceCount();
            delete m_circlePolyline;
            hasToRefresh = true;
        }

        if (hasToRefresh)
        {
            m_2DViewer->render();
        }

        m_state = None;
    }
}

void AngleTool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateCorrespondingSegmentOfAngle();
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

void AngleTool::annotateFirstPoint()
{
    m_mainPolyline = new DrawerPolyline;

    // Obtenim el punt clickat
    Vector3 clickedWorldPoint = m_2DViewer->getEventWorldCoordinate();
    // Afegim el punt a la polilínia
    m_mainPolyline->addPoint(clickedWorldPoint);
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_mainPolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw(m_mainPolyline);

    // Actualitzem l'estat de la tool
    m_state = FirstPointFixed;
}

void AngleTool::fixFirstSegment()
{
    // Posem l'estat de la tool a CenterFixed, així haurà agafat l'últim valor.
    m_state = CenterFixed;

    // Creem la polilínia per a dibuixar l'arc de circumferència i l'afegim al drawer
    m_circlePolyline = new DrawerPolyline;
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_circlePolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw(m_circlePolyline);
}

void AngleTool::drawCircle()
{
    auto firstPoint = m_mainPolyline->getPoint(0);
    auto circleCentre = m_mainPolyline->getPoint(1);
    auto lastPoint = m_mainPolyline->getPoint(2);

    auto firstPointDisplay = m_2DViewer->computeWorldToDisplay(firstPoint);
    auto circleCentreDisplay = m_2DViewer->computeWorldToDisplay(circleCentre);
    auto lastPointDisplay = m_2DViewer->computeWorldToDisplay(lastPoint);
    firstPointDisplay.z = circleCentreDisplay.z = lastPointDisplay.z = 0.0;

    // Calculem l'angle que formen els dos segments
    Vector3 firstSegment = MathTools::directorVector(circleCentreDisplay, firstPointDisplay);
    Vector3 secondSegment = MathTools::directorVector(circleCentreDisplay, lastPointDisplay);
    m_currentAngle = MathTools::angleInDegrees(firstSegment, secondSegment);

    // Calculem el radi de l'arc de circumferència que mesurarà un quart del segment més curt dels dos que formen l'angle
    double distance1 = firstSegment.length();
    double distance2 = secondSegment.length();
    double radius = qMin(distance1, distance2) / 4.0;

    // Calculem el rang de les iteracions per pintar l'angle correctament
    double initialAngle = MathTools::angleInRadians(QVector2D(firstSegment.x, firstSegment.y));
    if (initialAngle < 0.0)
    {
        initialAngle += 2.0 * MathTools::PiNumber;
    }
    double finalAngle = MathTools::angleInRadians(QVector2D(secondSegment.x, secondSegment.y));
    if (finalAngle < 0.0)
    {
        finalAngle += 2.0 * MathTools::PiNumber;
    }
    // Tenim els dos angles al rang [0,2pi)
    // Assegurem que girem en sentit horari -> tindrem angle positiu
    if (finalAngle < initialAngle)
    {
        std::swap(initialAngle, finalAngle);
    }
    double angle = finalAngle - initialAngle;
    // Assegurem que tenim l'angle al rang [0, pi]
    if (angle > MathTools::PiNumber)
    {
        angle = 2.0 * MathTools::PiNumber - angle;
        std::swap(initialAngle, finalAngle);
    }
    int degrees = qRound(angle * MathTools::RadiansToDegreesAsDouble);
    double increment = angle / degrees;

    // Reconstruim l'arc de circumferència
    m_circlePolyline->deleteAllPoints();
    for (int i = 0; i <= degrees; i++)
    {
        angle = initialAngle + i * increment;
        Vector3 newPointDisplay(cos(angle) * radius + circleCentreDisplay.x, sin(angle) * radius + circleCentreDisplay.y, 0.0);
        auto newPoint = m_2DViewer->computeDisplayToWorld(newPointDisplay);
        m_circlePolyline->addPoint(newPoint);
    }

    m_circlePolyline->update();
}

void AngleTool::handlePointAddition()
{
    if (m_2DViewer->hasInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            if (m_state == None)
            {
                this->annotateFirstPoint();
            }
            else if (m_state == FirstPointFixed)
            {
                this->fixFirstSegment();
            }
            else
            {
                // Voldrem enregistrar l'últim punt, pertant posem l'estat a none
                m_state = None;
                finishDrawing();
            }
        }
    }
}

void AngleTool::simulateCorrespondingSegmentOfAngle()
{
    if (!m_mainPolyline)
    {
        return;
    }

    if (m_state != None)
    {
        // Agafem la coordenada de pantalla
        Vector3 clickedWorldPoint = m_2DViewer->getEventWorldCoordinate();

        int pointIndex;
        if (m_state == FirstPointFixed)
        {
            pointIndex = 1;
        }
        else if (m_state == CenterFixed)
        {
            pointIndex = 2;
        }

        // Assignem el segon o tercer punt de l'angle segons l'estat
        m_mainPolyline->setPoint(pointIndex, clickedWorldPoint);
        m_mainPolyline->update();

        if (m_state == CenterFixed)
        {
            drawCircle();
        }

        m_2DViewer->render();
    }
}

void AngleTool::finishDrawing()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_mainPolyline->decreaseReferenceCount();
    m_circlePolyline->decreaseReferenceCount();
    // Eliminem l'arc de circumferència (s'esborra automàticament del drawer)
    delete m_circlePolyline;

    // Col·loquem l'angle en el pla corresponent
    m_2DViewer->getDrawer()->erasePrimitive(m_mainPolyline);
    equalizeDepth();
    m_2DViewer->getDrawer()->draw(m_mainPolyline, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Afegim l'annotació textual
    DrawerText *text = new DrawerText;
    text->setText(tr("%1 degrees").arg(m_currentAngle, 0, 'f', 1));
    placeText(text);
    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void AngleTool::placeText(DrawerText *angleText)
{
    // Padding de 5 pixels
    const double Padding = 5.0;

    auto point1Display = m_2DViewer->computeWorldToDisplay(m_mainPolyline->getPoint(0));
    auto point2Display = m_2DViewer->computeWorldToDisplay(m_mainPolyline->getPoint(1));
    auto point3Display = m_2DViewer->computeWorldToDisplay(m_mainPolyline->getPoint(2));

    // Mirem on estan horitzontalment els punts point1 i point3 respecte del point2
    if (point1Display.x <= point2Display.x)
    {
        angleText->setHorizontalJustification("Left");
    }
    else
    {
        angleText->setHorizontalJustification("Right");
    }

    if (point3Display.x <= point2Display.x)
    {
        auto point2 = m_mainPolyline->getPoint(1);
        angleText->setAttachmentPoint(point2);
    }
    else
    {
        // Apliquem el padding
        if (point2Display.y <= point3Display.y)
        {
            point2Display.y -= Padding;
        }
        else
        {
            point2Display.y += Padding;
        }
        // Tornem a coordenades de món
        Vector3 position = m_2DViewer->computeDisplayToWorld(point2Display);

        // Ara position és l'attachment point que volem
        angleText->setAttachmentPoint(position);
    }
}

void AngleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (m_mainPolyline)
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }

    if (m_circlePolyline)
    {
        m_circlePolyline->decreaseReferenceCount();
        delete m_circlePolyline;
    }

    m_mainPolyline = NULL;
    m_circlePolyline = NULL;
    m_state = None;
}

void AngleTool::equalizeDepth()
{
    for (int i = 0; i < 2; i++)
    {
        auto point = m_mainPolyline->getPoint(i);
        auto pointDisplay = m_2DViewer->computeWorldToDisplay(point);
        pointDisplay.z = 0.0;
        point = m_2DViewer->computeDisplayToWorld(pointDisplay);
        m_mainPolyline->setPoint(i, point);
    }
    m_mainPolyline->update();
}

}
