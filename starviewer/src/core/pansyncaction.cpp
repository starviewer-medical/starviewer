#include "pansyncaction.h"

#include "qviewer.h"
#include "anatomicalplanesynccriterion.h"

namespace udg {

PanSyncAction::PanSyncAction()
 : SyncAction()
{
    for (int i = 0; i < 3; ++i)
    {
        m_motionVector[i] = 0.0;
    }
}

PanSyncAction::~PanSyncAction()
{
}

void PanSyncAction::setMotionVector(double vector[3])
{
    for (int i = 0; i < 3; ++i)
    {
        m_motionVector[i] = vector[i];
    }
}

void PanSyncAction::run(QViewer *viewer)
{
    if (viewer)
    {
        viewer->absolutePan(m_motionVector);
    }
}

void PanSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("PanSyncAction", QObject::tr("pan"), "pan");
}

void PanSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion();
}

} // End namespace udg

