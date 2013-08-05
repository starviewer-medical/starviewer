#ifndef UDGVIEWERSLAYOUTTOSYNCACTIONMANAGERADAPTER_H
#define UDGVIEWERSLAYOUTTOSYNCACTIONMANAGERADAPTER_H

#include <QObject>

namespace udg {

class ViewersLayout;
class SyncActionManager;
class Q2DViewerWidget;

/**
    Adapter class to adapt ViewersLayout signals to add, remove and set SyncActionManager viewers
 */
class ViewersLayoutToSyncActionManagerAdapter : public QObject {
Q_OBJECT
public:
    ViewersLayoutToSyncActionManagerAdapter(ViewersLayout *layout, SyncActionManager *manager, QObject *parent = 0);
    ~ViewersLayoutToSyncActionManagerAdapter();

private slots:
    /// Slots that will be called when a viewer has been added, removed or selected
    /// to apply the proper actions on the manager
    void addViewer(Q2DViewerWidget *viewerWidget);
    void updateMasterViewer(Q2DViewerWidget *viewerWidget);
    void removeViewer(Q2DViewerWidget *viewerWidget);

private:
    /// Creates the needed connection between the layout and the manager
    void createConnections();

private:
    /// The layout to adapt signals from and the manager
    ViewersLayout *m_layout;
    SyncActionManager *m_manager;
};

} // End namespace udg

#endif
