/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroitoolrepresentation.h"

#include "q2dviewer.h"
#include "drawerpolygon.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"

namespace udg {

PolylineROIToolRepresentation::PolylineROIToolRepresentation(Drawer *drawer, QObject *parent)
 : ROIToolRepresentation(drawer, parent)
{
}

PolylineROIToolRepresentation::~PolylineROIToolRepresentation()
{
}

void PolylineROIToolRepresentation::createHandlers()
{
    int zCoordinate = Q2DViewer::getZIndexForView(m_view);
    double depth = m_roiPolygon->getVertix(0)[zCoordinate];

    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    for (int i = 0; i < m_roiPolygon->getNumberOfPoints(); ++i)
    {
        ToolHandlerWithRepresentation *handler = new ToolHandlerWithRepresentation(m_drawer, (double *)m_roiPolygon->getVertix(i));
        m_toolHandlers << handler;
        m_toolHandlersMap.insert(i, handler);
        connect(handler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
        connect(handler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
        connect(this, SIGNAL(hideToolHandlers()), handler, SLOT(hide()));
        connect(this, SIGNAL(showToolHandlers()), handler, SLOT(show()));

        connect(toolHandler, SIGNAL(moveAllPoints(double *)), handler, SLOT(move(double *)));

        handler->setParams(zCoordinate, depth);
    }

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << toolHandler;
}

}
