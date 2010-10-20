/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolhandler.h"
#include "logging.h"

#include <vtkCommand.h>

namespace udg {

ToolHandler::ToolHandler(QObject *parent)
 : QObject(parent)
{
    m_maxDistance = 5.0;
	m_sphereRadius = 1.0;
}

ToolHandler::~ToolHandler()
{
}

bool ToolHandler::isSelected()
{
    return m_isSelected;
}

void ToolHandler::select()
{
    m_isSelected = true;
    emit selectRepresentation();
}

void ToolHandler::deselect()
{
    m_isSelected = false;
}

void ToolHandler::setColor(QColor color)
{
    m_color = color;
}

QColor ToolHandler::getColor() const
{
    return m_color;
}

// Start HACK
void ToolHandler::setParams(int zCoordinate, double depth)
{
    m_zCoordinate = zCoordinate;
    m_depth = depth;
}

void ToolHandler::transformPointsDepth(double *point)
{
    if (m_zCoordinate != -1)
    {
        point[m_zCoordinate] = m_depth;
    }
}
// End HACK

}
