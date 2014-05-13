/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "viewerslayouttosyncactionmanageradapter.h"

#include "viewerslayout.h"
#include "syncactionmanager.h"
#include "q2dviewerwidget.h"

namespace udg {

ViewersLayoutToSyncActionManagerAdapter::ViewersLayoutToSyncActionManagerAdapter(ViewersLayout *layout, SyncActionManager *manager, QObject *parent)
 : QObject(parent)
{
    m_layout = layout;
    m_manager = manager;

    if (!m_manager)
    {
        m_manager = new SyncActionManager(0, this);
    }

    createConnections();
}

ViewersLayoutToSyncActionManagerAdapter::~ViewersLayoutToSyncActionManagerAdapter()
{
}

void ViewersLayoutToSyncActionManagerAdapter::addViewer(Q2DViewerWidget *viewerWidget)
{
    if (!viewerWidget)
    {
        return;
    }

    m_manager->addSyncedViewer(viewerWidget->getViewer());
}

void ViewersLayoutToSyncActionManagerAdapter::updateMasterViewer(Q2DViewerWidget *viewerWidget)
{
    if (!viewerWidget)
    {
        return;
    }

    m_manager->setMasterViewer(viewerWidget->getViewer());
}

void ViewersLayoutToSyncActionManagerAdapter::removeViewer(Q2DViewerWidget *viewerWidget)
{
    if (!viewerWidget)
    {
        return;
    }

    m_manager->removeSyncedViewer(viewerWidget->getViewer());
}

void ViewersLayoutToSyncActionManagerAdapter::createConnections()
{
    connect(m_layout, SIGNAL(selectedViewerChanged(Q2DViewerWidget*)), SLOT(updateMasterViewer(Q2DViewerWidget*)));
    
    connect(m_layout, SIGNAL(viewerAdded(Q2DViewerWidget*)), SLOT(addViewer(Q2DViewerWidget*)));
    connect(m_layout, SIGNAL(viewerShown(Q2DViewerWidget*)), SLOT(addViewer(Q2DViewerWidget*)));
    
    connect(m_layout, SIGNAL(viewerRemoved(Q2DViewerWidget*)), SLOT(removeViewer(Q2DViewerWidget*)));
    connect(m_layout, SIGNAL(viewerHidden(Q2DViewerWidget*)), SLOT(removeViewer(Q2DViewerWidget*)));
}

} // End namespace udg
