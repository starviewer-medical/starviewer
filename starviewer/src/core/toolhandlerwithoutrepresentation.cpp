/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolhandlerwithoutrepresentation.h"
#include "logging.h"
#include "drawerprimitive.h"

#include <vtkCommand.h>

namespace udg {

ToolHandlerWithoutRepresentation::ToolHandlerWithoutRepresentation(QList<DrawerPrimitive *> primitives, QObject *parent)
 : ToolHandler(parent)
{
    m_primitives = primitives;
}

ToolHandlerWithoutRepresentation::~ToolHandlerWithoutRepresentation()
{
}

void ToolHandlerWithoutRepresentation::update()
{
    // Nothing to do
}

void ToolHandlerWithoutRepresentation::handleEvents(long unsigned eventID, double *point)
{
    switch(eventID)
    {
        case vtkCommand::MouseMoveEvent:
            // HACK!!
            transformPointsDepth(point);
            // Fi HACK
            double movement[3];
            this->getMovement(point, movement);
            emit moveAllPoints(movement);
            break;
    }
}

void ToolHandlerWithoutRepresentation::isClickNearMe(double *point)
{
    foreach(DrawerPrimitive *primitive, m_primitives)
    {
        double distance = primitive->getDistanceToPoint(point);

        if(distance <=  m_maxDistance)
        {
            m_startPoint[0] = point[0]; 
            m_startPoint[1] = point[1];
            m_startPoint[2] = point[2];
            select();
            emit clicked(this);
        }
    }
}

void ToolHandlerWithoutRepresentation::getMovement(double *finalPoint, double *movementVector)
{
    movementVector[0] = finalPoint[0] - m_startPoint[0];
    movementVector[1] = finalPoint[1] - m_startPoint[1];
    movementVector[2] = finalPoint[2] - m_startPoint[2];

    m_startPoint[0] = finalPoint[0]; 
    m_startPoint[1] = finalPoint[1];
    m_startPoint[2] = finalPoint[2];
}

}
