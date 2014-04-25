#include "pansyncaction.h"

#include "q2dviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "frameofreferencesynccriterion.h"
#include "anglebetweenplanessynccriterion.h"
#include "anatomicalregionsynccriterion.h"

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
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
    if (viewer2D)
    {
        viewer2D->absolutePan(m_motionVector);
    }
}

void PanSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("PanSyncAction", QObject::tr("Pan"), "pan");
}

void PanSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new FrameOfReferenceSyncCriterion() << new AngleBetweenPlanesSyncCriterion() << new AnatomicalRegionSyncCriterion();
}

} // End namespace udg

