/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "polylineroioutliner.h"
#include "polylineroitoolrepresentation.h"
#include "representationslayer.h"

namespace udg {

PolylineROITool::PolylineROITool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "PolylineROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_polylineROIOutliner = new PolylineROIOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineROIOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_polylineROIOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_roiPolygon = NULL;

    // Default compatibility
    this->setEditionCompatibility(false);
}

PolylineROITool::~PolylineROITool()
{
}

void PolylineROITool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void PolylineROITool::outlinerFinished(DrawerPrimitive *primitive)
{
    disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineROIOutliner, SLOT(handleEvent(long unsigned)));
    disconnect(m_polylineROIOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    delete m_polylineROIOutliner;

    m_roiPolygon = qobject_cast<DrawerPolygon *>(primitive);
    if (!m_roiPolygon)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerPolyline!!!-> ") + primitive->metaObject()->className());
    }

    m_polylineROIToolRepresentation = new PolylineROIToolRepresentation(m_2DViewer->getDrawer());
    m_polylineROIToolRepresentation->setPolygon(m_roiPolygon);
    m_polylineROIToolRepresentation->setParameters(m_2DViewer->getView(), m_2DViewer->getCurrentSlice(), m_2DViewer->getInput(), m_2DViewer->getCurrentSlabProjection(), m_2DViewer->isThickSlabActive());
    m_polylineROIToolRepresentation->calculate();
    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_polylineROIToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    emit finished();
}

}
