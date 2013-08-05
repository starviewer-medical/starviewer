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
        m_manager = new SyncActionManager(this);
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
