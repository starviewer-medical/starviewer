#ifndef UDGSIGNALTOSYNCACTIONMAPPER_H
#define UDGSIGNALTOSYNCACTIONMAPPER_H

#include <QObject>

namespace udg {

class QViewer;
class SyncAction;

/**
    This is an abstract class to map viewer signals to an specific SyncAction.
    The inherited classes have to map a signal from the given viewer in setViewer() method into a SyncAction.
    mapSignal() should establish the connection between the viewer signal and the specific slot that creates the intended SyncAction and 
    and eventually emit it through actionMapped() signal
    unmapSignal() should undo the connection created on mapSignal()
 */
class SignalToSyncActionMapper : public QObject {
Q_OBJECT
public:
    SignalToSyncActionMapper(QObject *parent = 0);
    ~SignalToSyncActionMapper();

    /// Viewer to map signals from
    void setViewer(QViewer *viewer);

signals:
    /// This signal should be emitted when the correspoding viewer's signal had been mapped to the corresponding SyncAction
    void actionMapped(SyncAction*);

protected:
    /// Abstract method responsible of mapping the proper viewer signal to obtain the desired parameters to sync
    virtual void mapSignal() = 0;

    /// Abstract method responsible of unmapping the previously mapped viewer signal on mapSignal() method
    virtual void unmapSignal() = 0;

protected:
    /// The source viewer whose signals are mapped from
    QViewer *m_viewer;

    /// Mapped SyncAction from the received signals
    SyncAction *m_mappedSyncAction;
};

} // End namespace udg

#endif
