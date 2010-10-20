/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolhandlerwithrepresentation.h"
#include "logging.h"
#include "drawer.h"
#include "drawerprimitive.h"
#include "drawerpoint.h"

#include <vtkCommand.h>

namespace udg {

ToolHandlerWithRepresentation::ToolHandlerWithRepresentation(Drawer *drawer, double *point, QColor color,  QObject *parent)
 : ToolHandler(parent)
{
    m_drawer = drawer;
    m_point = new DrawerPoint;
    m_point->setPosition(point);
	m_point->setRadius(m_sphereRadius);
    m_point->setOpacity(0.8);
    m_color = color;
    m_point->setColor(m_color);
    m_point->update();
    m_drawer->drawWorkInProgress(m_point);
}

ToolHandlerWithRepresentation::~ToolHandlerWithRepresentation()
{
    delete m_point;
}

void ToolHandlerWithRepresentation::update()
{
    m_point->setColor(m_color);
    m_point->update();
}

void ToolHandlerWithRepresentation::setPosition(double *point)
{
    m_point->setPosition(point);
    m_point->update();
}

void ToolHandlerWithRepresentation::handleEvents(long unsigned eventID, double *point)
{
    switch(eventID)
    {
        case vtkCommand::MouseMoveEvent:
            // HACK!!
            transformPointsDepth(point);
            // Fi HACK
            setPosition(point);
            emit movePoint(this, point);
            break;
    }
}

void ToolHandlerWithRepresentation::hide()
{
    m_point->setVisibility(false);
    m_point->update();
}

void ToolHandlerWithRepresentation::show()
{
    m_point->setVisibility(true);
    m_point->update();
}

void ToolHandlerWithRepresentation::isClickNearMe(double *point)
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

void ToolHandlerWithRepresentation::move(double *movement)
{
    double *origin = m_point->getPosition();
    double newPoint[3];

    newPoint[0] = origin[0] + movement[0];
    newPoint[1] = origin[1] + movement[1];
    newPoint[2] = origin[2] + movement[2];

    setPosition(newPoint);
}

void ToolHandlerWithRepresentation::repaintHandler()
{
    m_drawer->updateRenderer();
}

}
