#include "phasesignaltosyncactionmapper.h"

#include "q2dviewer.h"

namespace udg {

PhaseSignalToSyncActionMapper::PhaseSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
    m_mappedSyncAction = new PhaseSyncAction();
}

PhaseSignalToSyncActionMapper::~PhaseSignalToSyncActionMapper()
{
}

void PhaseSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        mapToSyncAction(viewer2D->getCurrentPhase());
    }
}

void PhaseSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        connect(viewer2D, SIGNAL(phaseChanged(int)), SLOT(mapToSyncAction(int)));
    }
}

void PhaseSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        disconnect(viewer2D, SIGNAL(phaseChanged(int)), this, SLOT(mapToSyncAction(int)));
    }
}

void PhaseSignalToSyncActionMapper::mapToSyncAction(int phase)
{
    PhaseSyncAction *action = static_cast<PhaseSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setPhase(phase);

    emit actionMapped(m_mappedSyncAction);
}

}
