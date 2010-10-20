/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ellipticroitool.h"
#include "q2dviewer.h"
#include "ellipseoutliner.h"
#include "logging.h"
#include "drawerpolygon.h"
#include "ellipticroitoolrepresentation.h"
#include "representationslayer.h"

namespace udg {

EllipticROITool::EllipticROITool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_ellipsePolygon(0)
{
    m_toolName = "EllipticROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_ellipseOutliner = new EllipseOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_ellipseOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_ellipseOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    // Default compatibility
    this->setEditionCompatibility(false);
}

EllipticROITool::~EllipticROITool()
{
}

void EllipticROITool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void EllipticROITool::outlinerFinished(DrawerPrimitive *primitive)
{
    disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_ellipseOutliner, SLOT(handleEvent(long unsigned)));
    disconnect(m_ellipseOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_ellipsePolygon = qobject_cast<DrawerPolygon *>(primitive);
    if (!m_ellipsePolygon)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerPolygon!!!-> ") + primitive->metaObject()->className());
    }

    m_ellipticROIToolRepresentation = new EllipticROIToolRepresentation(m_2DViewer->getDrawer());
    m_ellipticROIToolRepresentation->setPolygon(m_ellipsePolygon);
    
    m_ellipticROIToolRepresentation->setParameters(m_2DViewer->getView(), m_2DViewer->getCurrentSlice(), m_2DViewer->getInput(), m_2DViewer->getCurrentSlabProjection(), m_2DViewer->isThickSlabActive());
    m_ellipticROIToolRepresentation->calculate();
    
    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_ellipticROIToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    delete m_ellipseOutliner;
    emit finished();
}

}
