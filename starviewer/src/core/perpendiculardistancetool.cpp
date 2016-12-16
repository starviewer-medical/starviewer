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

#include "perpendiculardistancetool.h"

#include "drawer.h"
#include "drawerline.h"
#include "mathtools.h"
#include "q2dviewer.h"

#include <vtkRenderWindowInteractor.h>

namespace udg {

PerpendicularDistanceTool::PerpendicularDistanceTool(QViewer *viewer, QObject *parent)
 : GenericDistanceTool(viewer, parent)
{
    m_toolName = "PerpendicularDistanceTool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));

    reset();
}

PerpendicularDistanceTool::~PerpendicularDistanceTool()
{
    reset();
}

void PerpendicularDistanceTool::handleLeftButtonPress()
{
    // Si hi ha doble clic només tenim en compte el primer
    if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
    {
        handleClick();
    }
}

void PerpendicularDistanceTool::handleMouseMove()
{
    if (m_state == DrawingReferenceLine)
    {
        updateReferenceLineAndRender();
    }
    else if (m_state == DrawingFirstPerpendicularLine)
    {
        updateFirstPerpendicularLineAndRender();
    }
    else if (m_state == DrawingSecondPerpendicularLine)
    {
        updateSecondPerpendicularLineAndRender();
    }
}

void PerpendicularDistanceTool::handleClick()
{
    switch (m_state)
    {
        case NotDrawing:
            addFirstPoint();
            break;
        case DrawingReferenceLine:
            addSecondPoint();
            break;
        case DrawingFirstPerpendicularLine:
            addThirdPoint();
            break;
        case DrawingSecondPerpendicularLine:
            addFourthPoint();
    }
}

void PerpendicularDistanceTool::addFirstPoint()
{
    Q_ASSERT(m_state == NotDrawing);

    if (!m_referenceLine)
    {
        m_referenceLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_referenceLine->increaseReferenceCount();
    }

    Vector3 clickedWorldPoint;
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint.data());

    // Afegim el punt
    m_referenceLine->setFirstPoint(clickedWorldPoint);
    m_referenceLine->setSecondPoint(clickedWorldPoint);
    m_referenceLine->update();

    m_2DViewer->getDrawer()->draw(m_referenceLine);

    m_state = DrawingReferenceLine;
}

void PerpendicularDistanceTool::addSecondPoint()
{
    Q_ASSERT(m_state == DrawingReferenceLine);

    updateReferenceLine();

    if (!m_firstPerpendicularLine)
    {
        m_firstPerpendicularLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_firstPerpendicularLine->increaseReferenceCount();
    }

    Vector3 clickedWorldPoint;
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint.data());
    m_firstPerpendicularLine->setFirstPoint(clickedWorldPoint);
    m_firstPerpendicularLine->setSecondPoint(clickedWorldPoint);
    m_firstPerpendicularLine->update();

    m_2DViewer->getDrawer()->draw(m_firstPerpendicularLine);

    m_state = DrawingFirstPerpendicularLine;
}

void PerpendicularDistanceTool::addThirdPoint()
{
    Q_ASSERT(m_state == DrawingFirstPerpendicularLine);

    updateFirstPerpendicularLine();

    if (!m_secondPerpendicularLine)
    {
        m_secondPerpendicularLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_secondPerpendicularLine->increaseReferenceCount();
    }

    Vector3 clickedWorldPoint;
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint.data());
    m_secondPerpendicularLine->setFirstPoint(clickedWorldPoint);
    m_secondPerpendicularLine->setSecondPoint(clickedWorldPoint);
    m_secondPerpendicularLine->update();

    m_2DViewer->getDrawer()->draw(m_secondPerpendicularLine);

    m_state = DrawingSecondPerpendicularLine;
}

void PerpendicularDistanceTool::addFourthPoint()
{
    Q_ASSERT(m_state == DrawingSecondPerpendicularLine);

    updateSecondPerpendicularLine();

    // Movem les línies al pla corresponent
    m_referenceLine->decreaseReferenceCount();
    m_firstPerpendicularLine->decreaseReferenceCount();
    m_secondPerpendicularLine->decreaseReferenceCount();
    m_2DViewer->getDrawer()->erasePrimitive(m_referenceLine);
    m_2DViewer->getDrawer()->erasePrimitive(m_firstPerpendicularLine);
    m_2DViewer->getDrawer()->erasePrimitive(m_secondPerpendicularLine);
    m_2DViewer->getDrawer()->draw(m_firstPerpendicularLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    m_2DViewer->getDrawer()->draw(m_secondPerpendicularLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    m_2DViewer->getDrawer()->draw(m_referenceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Calculem i dibuxem la distància
    drawDistance();

    m_referenceLine = 0;
    m_firstPerpendicularLine = 0;
    m_secondPerpendicularLine = 0;

    m_state = NotDrawing;
}

void PerpendicularDistanceTool::updateReferenceLine()
{
    Q_ASSERT(m_state == DrawingReferenceLine);

    Vector3 mouseWorldPoint;
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint.data());
    m_referenceLine->setSecondPoint(mouseWorldPoint);
    m_referenceLine->update();
}

void PerpendicularDistanceTool::updateFirstPerpendicularLine()
{
    Q_ASSERT(m_state == DrawingFirstPerpendicularLine);

    equalizeDepth();

    // Obtenim la posició del ratolí
    Vector3 mouseWorldPoint;
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint.data());

    // Obtenim el punt de la línia de referència més proper a la posició del ratolí i també la distància
    // Serà el primer punt de la primera línia perpendicular
    Vector3 firstPerpendicularLineFirstPoint;
    double distance;
    distance = MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_referenceLine->getFirstPoint(), m_referenceLine->getSecondPoint(),
                                                       firstPerpendicularLineFirstPoint);

    // Calculem una línia perpendicular a la línia de referència que passi pel punt obtingut al pas anterior
    auto referenceLineFirstPoint = m_referenceLine->getFirstPoint();
    auto referenceLineSecondPoint = m_referenceLine->getSecondPoint();
    auto referenceLineDirectorVector = MathTools::directorVector(referenceLineFirstPoint, referenceLineSecondPoint);
    int xIndex, yIndex, zIndex;
    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);
    Vector3 auxiliarLineDirectorVector;
    auxiliarLineDirectorVector[xIndex] = referenceLineDirectorVector[yIndex];
    auxiliarLineDirectorVector[yIndex] = -referenceLineDirectorVector[xIndex];
    auxiliarLineDirectorVector[zIndex] = referenceLineDirectorVector[zIndex];
    auto auxiliarLineFirstPoint = firstPerpendicularLineFirstPoint + distance * auxiliarLineDirectorVector;
    auto auxiliarLineSecondPoint = firstPerpendicularLineFirstPoint - distance * auxiliarLineDirectorVector;

    // Obtenim el punt de la línia auxiliar més proper a la posició del ratolí
    // Serà el segon punt de la primera línia perpendicular
    Vector3 firstPerpendicularLineSecondPoint;
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, auxiliarLineFirstPoint, auxiliarLineSecondPoint, firstPerpendicularLineSecondPoint);

    // Assignem els punts calculats a la primera línia perpendicular
    m_firstPerpendicularLine->setFirstPoint(firstPerpendicularLineFirstPoint);
    m_firstPerpendicularLine->setSecondPoint(firstPerpendicularLineSecondPoint);
    m_firstPerpendicularLine->update();
}

void PerpendicularDistanceTool::updateSecondPerpendicularLine()
{
    Q_ASSERT(m_state == DrawingSecondPerpendicularLine);

    equalizeDepth();

    // Obtenim la posició del ratolí
    Vector3 mouseWorldPoint;
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint.data());

    // Obtenim el punt de la línia de referència més proper a la posició del ratolí
    // Serà el primer punt de la segona línia perpendicular
    Vector3 secondPerpendicularLineFirstPoint;
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_referenceLine->getFirstPoint(), m_referenceLine->getSecondPoint(),
                                            secondPerpendicularLineFirstPoint);

    // Calculem la longitud i el vector director de la primera línia perpendicular per fer que la segona sigui igual
    auto firstPerpendicularLineFirstPoint = m_firstPerpendicularLine->getFirstPoint();
    auto firstPerpendicularLineSecondPoint = m_firstPerpendicularLine->getSecondPoint();
    // No fem servir el mètode de MathTools perquè volem el vector que va del primer punt al segon, i MathTools no assegura que sigui sempre així.
    auto firstPerpendicularLineDirectorVector = firstPerpendicularLineSecondPoint - firstPerpendicularLineFirstPoint;
    double length = firstPerpendicularLineDirectorVector.length();
    firstPerpendicularLineDirectorVector.normalize();

    // Calculem el segon punt de la segona línia perpendicular
    auto secondPerpendicularLineSecondPoint = secondPerpendicularLineFirstPoint + length * firstPerpendicularLineDirectorVector;

    // Assignem els punts calculats a la segona línia perpendicular
    m_secondPerpendicularLine->setFirstPoint(secondPerpendicularLineFirstPoint);
    m_secondPerpendicularLine->setSecondPoint(secondPerpendicularLineSecondPoint);
    m_secondPerpendicularLine->update();
}

void PerpendicularDistanceTool::updateReferenceLineAndRender()
{
    updateReferenceLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::updateFirstPerpendicularLineAndRender()
{
    updateFirstPerpendicularLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::updateSecondPerpendicularLineAndRender()
{
    updateSecondPerpendicularLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::drawDistance()
{
    drawDistanceLine();
    drawMeasurement();
}

void PerpendicularDistanceTool::drawDistanceLine()
{
    m_distanceLine = new DrawerLine();

    // Obtenim la posició del ratolí
    Vector3 mouseWorldPoint;
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint.data());

    // Obtenim el punt de la primera línia perpendicular més proper a la posició del ratolí
    // Serà el primer punt de la línia de distància
    Vector3 distanceLineFirstPoint;
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_firstPerpendicularLine->getFirstPoint(), m_firstPerpendicularLine->getSecondPoint(),
                                            distanceLineFirstPoint);

    // Obtenim el punt de la segona línia perpendicular més proper a la posició del ratolí
    // Serà el segon punt de la línia de distància
    Vector3 distanceLineSecondPoint;
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_secondPerpendicularLine->getFirstPoint(), m_secondPerpendicularLine->getSecondPoint(),
                                            distanceLineSecondPoint);

    // La línia de distància coincidirà amb la de referència; la posem a l'altre extrem de les línies perpendiculars
    if (distanceLineFirstPoint == m_firstPerpendicularLine->getFirstPoint())
    {
        distanceLineFirstPoint = m_firstPerpendicularLine->getSecondPoint();
        distanceLineSecondPoint = m_secondPerpendicularLine->getSecondPoint();
    }

    // Assignem els punts calculats a la línia de distància
    m_distanceLine->setFirstPoint(distanceLineFirstPoint);
    m_distanceLine->setSecondPoint(distanceLineSecondPoint);
    m_distanceLine->update();

    // Pintem la línia de distància
    m_2DViewer->getDrawer()->draw(m_distanceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void PerpendicularDistanceTool::abortDrawing()
{
    reset();
}

void PerpendicularDistanceTool::equalizeDepth()
{
    // Ens quedem amb la z de la llesca actual
    Vector3 currentPoint;
    m_2DViewer->getEventWorldCoordinate(currentPoint.data());
    int zIndex = m_2DViewer->getView().getZIndex();
    double z = currentPoint[zIndex];
    auto point = m_referenceLine->getFirstPoint();
    point[zIndex] = z;
    m_referenceLine->setFirstPoint(point);
    point = m_referenceLine->getSecondPoint();
    point[zIndex] = z;
    m_referenceLine->setSecondPoint(point);
    m_referenceLine->update();
    point = m_firstPerpendicularLine->getFirstPoint();
    point[zIndex] = z;
    m_firstPerpendicularLine->setFirstPoint(point);
    point = m_firstPerpendicularLine->getSecondPoint();
    point[zIndex] = z;
    m_firstPerpendicularLine->setSecondPoint(point);
    m_firstPerpendicularLine->update();
    if (m_secondPerpendicularLine)
    {
        point = m_secondPerpendicularLine->getFirstPoint();
        point[zIndex] = z;
        m_secondPerpendicularLine->setFirstPoint(point);
        point = m_secondPerpendicularLine->getSecondPoint();
        point[zIndex] = z;
        m_secondPerpendicularLine->setSecondPoint(point);
        m_secondPerpendicularLine->update();
    }
}

void PerpendicularDistanceTool::reset()
{
    bool hasToRender = false;

    if (m_referenceLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_referenceLine->decreaseReferenceCount();
        delete m_referenceLine;
        hasToRender &= true;
    }

    if (m_firstPerpendicularLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_firstPerpendicularLine->decreaseReferenceCount();
        delete m_firstPerpendicularLine;
        hasToRender &= true;
    }

    if (m_secondPerpendicularLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_secondPerpendicularLine->decreaseReferenceCount();
        delete m_secondPerpendicularLine;
        hasToRender &= true;
    }

    if (hasToRender)
    {
        m_2DViewer->render();
    }

    m_state = NotDrawing;
}

}
