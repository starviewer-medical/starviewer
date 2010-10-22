/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "toolrepresentation.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerprimitive.h"
#include "toolhandler.cpp"

#include <QColor>
#include <QMultiMap>

namespace udg {

ToolRepresentation::ToolRepresentation(Drawer *drawer, QObject *parent)
 : QObject(parent), m_drawer(drawer)
{
    m_representationColor = QColor(255, 165, 0);
    m_selectedRepresentationColor = QColor(Qt::darkMagenta);
    m_handlerColor = QColor(255, 165, 0);
    m_selectedHandlerColor = QColor(Qt::darkMagenta);
}

ToolRepresentation::~ToolRepresentation()
{
}

bool ToolRepresentation::isVisible()
{
    return m_isVisible;
}

bool ToolRepresentation::isSelected()
{
    return m_isSelected;
}

void ToolRepresentation::select()
{
    m_isSelected = true;

    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        primitive->setColor(m_selectedRepresentationColor);
        primitive->update();
    }
}

void ToolRepresentation::deselect()
{
    // Deselecting all representation's handlers
    foreach (ToolHandler *toolHandler, m_toolHandlers)
    {
        if (toolHandler->isSelected())
        {
            toolHandler->deselect();
        }
    }

    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        primitive->setColor(m_representationColor);
        primitive->update();
    }
    foreach (ToolHandler *handler, m_toolHandlers)
    {
        handler->setColor(m_handlerColor);
        handler->update();
    }

    m_isSelected = false;
}

void ToolRepresentation::hide()
{
    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        primitive->visibilityOff();
        primitive->update();
    }

    emit hideToolHandlers();
}

void ToolRepresentation::show()
{
    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        primitive->visibilityOn();
        primitive->update();
    }

    emit showToolHandlers();
}

QList<ToolHandler *>& ToolRepresentation::getToolHandlers()
{
    return m_toolHandlers;
}

void ToolRepresentation::refresh()
{
    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        primitive->update();
    }
    m_drawer->updateRenderer();
}

void ToolRepresentation::selectRepresentation()
{
    select();
    emit selected(this);
}

void ToolRepresentation::movePoint(ToolHandler *handler, double *point)
{
    QList<double *> affectedPoints = mapHandlerToPoint(handler);

    foreach (double *affectedPoint, affectedPoints)
    {
        affectedPoint[0] = point[0];
        affectedPoint[1] = point[1];
        affectedPoint[2] = point[2];
    }

    this->refresh();
	m_drawer->updateRenderer();
}

bool ToolRepresentation::isInsideOfBounds(double bounds[6], int view)
{
    bool isInside = true;
    int position = 0;
    double primitiveBounds[6];

    int xIndex = Q2DViewer::getXIndexForView(view);
    int yIndex = Q2DViewer::getYIndexForView(view);

    while (isInside && position < m_primitivesList.size())
    {
        m_primitivesList.at(position)->getBounds(primitiveBounds);
        
        if (!(bounds[xIndex * 2] <= primitiveBounds[xIndex * 2] && 
            bounds[xIndex * 2 + 1] >= primitiveBounds[xIndex * 2 + 1] && 
            bounds[yIndex * 2] <= primitiveBounds[yIndex * 2] && 
            bounds[yIndex * 2 + 1] >= primitiveBounds[yIndex * 2 + 1]))
        {
            isInside = false;
        }

        position++;
    }

    return isInside;
}

}
