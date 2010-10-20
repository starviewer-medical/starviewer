#include "ellipticroitoolrepresentation.h"

#include "q2dviewer.h"
#include "drawerpolygon.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"

namespace udg {

EllipticROIToolRepresentation::EllipticROIToolRepresentation(Drawer *drawer, QObject *parent)
: ROIToolRepresentation(drawer, parent)
{
}

EllipticROIToolRepresentation::~EllipticROIToolRepresentation()
{
}

void EllipticROIToolRepresentation::createHandlers()
{
    // Aquest cas és més complicat ja que el moviment d'un handler no es correspon amb
    // moure un punt i prou, sinó que cal un comportament més complexe. Depenent del punt escollit
    // allargarem un dels radis o tots dos fent que l'el·lipse es faci més petita o gran
    // De moment deixem el mateix comportament que amb PolylineROIToolRepresentation
    // TODO Implementar-ho correctament perquè l'edició de l'el·lipse sigui com cal
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
