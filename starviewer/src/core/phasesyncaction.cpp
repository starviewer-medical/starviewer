#include "phasesyncaction.h"

#include "inputsynccriterion.h"
#include "q2dviewer.h"

namespace udg {

PhaseSyncAction::PhaseSyncAction()
 : SyncAction(), m_phase(0), m_volume(0)
{
}

PhaseSyncAction::~PhaseSyncAction()
{
}

void PhaseSyncAction::setPhase(int phase)
{
    m_phase = phase;
}

void PhaseSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void PhaseSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    // Limit action to 2D viewers and to secondary inputs
    if (viewer2D && viewer2D->getMainInput() != m_volume)
    {
        viewer2D->setPhaseInVolume(viewer2D->indexOfVolume(m_volume), m_phase);
    }
}

void PhaseSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("PhaseSyncAction", QObject::tr("phase"), "phase");
}

void PhaseSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new InputSyncCriterion();
}

}
