/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolhandlerrotation.h"
#include "logging.h"
#include "mathtools.h"
#include "drawer.h"
#include "drawerprimitive.h"
#include "drawerpoint.h"

#include <vtkCommand.h>

namespace udg {

ToolHandlerRotation::ToolHandlerRotation(Drawer *drawer, double *point, QObject *parent)
 : ToolHandler(parent)
{
    m_drawer = drawer;
    m_point = new DrawerPoint;
    m_point->setPosition(point);
	m_point->setRadius(m_sphereRadius);
    m_point->setOpacity(0.8);
    m_point->setColor(m_color);
    m_point->update();
    m_drawer->drawWorkInProgress(m_point);
    m_drawer->updateRenderer();
}

ToolHandlerRotation::~ToolHandlerRotation()
{
    delete m_point;
}

void ToolHandlerRotation::update()
{
    m_point->setColor(m_color);
    m_point->update();
    m_drawer->updateRenderer();
}

void ToolHandlerRotation::setRotationCenter(double *center)
{
    m_rotationCenter[0] = center[0];
    m_rotationCenter[1] = center[1];
    m_rotationCenter[2] = center[2];
}

void ToolHandlerRotation::setPosition(double *point)
{
    m_point->setPosition(point);
    m_point->update();
}

void ToolHandlerRotation::handleEvents(long unsigned eventID, double *point)
{
    switch (eventID)
    {
        case vtkCommand::MouseMoveEvent:
            // HACK!!
            transformPointsDepth(point);
            // Fi HACK
            double angle = calculateRotationAngle(point);
            emit rotate(angle);
            break;
    }
}

void ToolHandlerRotation::hide()
{
    m_point->setVisibility(false);
    m_point->update();
}

void ToolHandlerRotation::show()
{
    m_point->setVisibility(true);
    m_point->update();
}

void ToolHandlerRotation::isClickNearMe(double *point)
{
    // HACK!!
    transformPointsDepth(point);
    // Fi HACK

    double distance = m_point->getDistanceToPoint(point);

    if (distance <= m_maxDistance)
    {
        select();
        emit clicked(this);
    }
}

void ToolHandlerRotation::move(double *movement)
{
    double *origin = m_point->getPosition();
    double newPoint[3];

    newPoint[0] = origin[0] + movement[0];
    newPoint[1] = origin[1] + movement[1];
    newPoint[2] = origin[2] + movement[2];

    setPosition(newPoint);
    repaintHandler();
}

void ToolHandlerRotation::repaintHandler()
{
    m_drawer->updateRenderer();
}

double ToolHandlerRotation::calculateRotationAngle(double *point)
{
    double *dv1 = MathTools::directorVector(m_point->getPosition(), m_rotationCenter);
    double *dv2 = MathTools::directorVector(point, m_rotationCenter);

    double *cross;
    MathTools::crossProduct(dv1, dv2, cross);

    double angle = (cross[m_zCoordinate] > 0) ? MathTools::angleInDegrees(dv1, dv2) : -1 * MathTools::angleInDegrees(dv1, dv2);

    return angle;
}

}
