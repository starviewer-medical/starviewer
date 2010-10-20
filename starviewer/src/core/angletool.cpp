/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "angletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "representationslayer.h"
#include "polylineangleoutliner.h"
#include "angletoolrepresentation.h"

namespace udg {

AngleTool::AngleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "AngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_polylineAngleOutliner = new PolylineAngleOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineAngleOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_polylineAngleOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_polyline = NULL;

    // Default compatibility
    this->setEditionCompatibility(false);
}

AngleTool::~AngleTool()
{
}

void AngleTool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void AngleTool::outlinerFinished(DrawerPrimitive *primitive)
{
    disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineAngleOutliner, SLOT(handleEvent(long unsigned)));
    disconnect(m_polylineAngleOutliner, SIGNAL(finished(DrawerPrimitive*)), this, SLOT(outlinerFinished(DrawerPrimitive*)));

    delete m_polylineAngleOutliner;

    m_polyline = qobject_cast<DrawerPolyline *>(primitive);
    if (!m_polyline)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerPolyline!!!-> ") + primitive->metaObject()->className());
    }

    m_angleToolRepresentation = new AngleToolRepresentation(m_2DViewer->getDrawer());
    m_angleToolRepresentation->setPolyline(m_polyline);
    m_angleToolRepresentation->setParams(m_2DViewer->getView(), m_2DViewer->getCurrentSlice(), m_polylineAngleOutliner->getAngleDegrees());
    m_angleToolRepresentation->calculate();
    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_angleToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    emit finished();
}

}
