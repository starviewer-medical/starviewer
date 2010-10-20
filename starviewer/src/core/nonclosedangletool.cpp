/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "nonclosedangletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerline.h"
#include "lineoutliner.h"
#include "nonclosedangletoolrepresentation.h"
#include "representationslayer.h"

namespace udg {

NonClosedAngleTool::NonClosedAngleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_firstLine(0), m_secondLine(0)
{
    m_toolName = "NonClosedAngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_lineOutliner = new LineOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_firstLine = NULL;
    m_secondLine = NULL;

    // Default compatibility
    this->setEditionCompatibility(false);
}

NonClosedAngleTool::~NonClosedAngleTool()
{
}

void NonClosedAngleTool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void NonClosedAngleTool::outlinerFinished(DrawerPrimitive *primitive)
{
    if (m_firstLine == NULL)
    {
        disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
        disconnect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

        m_firstLine = qobject_cast<DrawerLine *>(primitive);
        if (!m_firstLine)
        {
            DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerLine!!!-> ")+ primitive->metaObject()->className());
        }
        
        delete m_lineOutliner;

        m_lineOutliner = new LineOutliner(m_2DViewer);

        connect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
        connect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));
    }
    else // Both lines drawn
    {
        disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_lineOutliner, SLOT(handleEvent(long unsigned)));
        disconnect(m_lineOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

        delete m_lineOutliner;

        m_secondLine = qobject_cast<DrawerLine *>(primitive);
        if (!m_secondLine)
        {
            DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerLine!!!-> ")+ primitive->metaObject()->className());
        }

        m_nonClosedToolRepresentation = new NonClosedAngleToolRepresentation(m_2DViewer->getDrawer());
        m_nonClosedToolRepresentation->setLines(m_firstLine, m_secondLine);
        m_nonClosedToolRepresentation->setParams(m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        m_nonClosedToolRepresentation->calculate();
        m_2DViewer->getRepresentationsLayer()->addRepresentation(m_nonClosedToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        emit finished();
    }
}

}
