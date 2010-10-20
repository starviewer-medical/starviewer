/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetool.h"

#include "q2dviewer.h"
#include "logging.h"
#include "drawerline.h"
#include "image.h"
#include "volume.h"
#include "lineoutliner.h"
#include "distancetoolrepresentation.h"
#include "representationslayer.h"

namespace udg {

DistanceTool::DistanceTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "DistanceTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_lineOutliner = new LineOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_line = NULL;

    // Default compatibility
    this->setEditionCompatibility(false);
}

DistanceTool::~DistanceTool()
{
}

void DistanceTool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void DistanceTool::outlinerFinished(DrawerPrimitive *primitive)
{
    disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
    disconnect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    delete m_lineOutliner;

    m_line = qobject_cast<DrawerLine *>(primitive);
    if (!m_line)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerLine!!!-> ") + primitive->metaObject()->className());
    }

    m_distanceToolRepresentation = new DistanceToolRepresentation(m_2DViewer->getDrawer());
    m_distanceToolRepresentation->setLine(m_line);
    m_distanceToolRepresentation->setParams(m_2DViewer->getView(), m_2DViewer->getInput()->getImage(0)->getPixelSpacing(), m_2DViewer->getInput()->getSpacing());
    m_distanceToolRepresentation->calculate();
    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_distanceToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    emit finished();
}

}
