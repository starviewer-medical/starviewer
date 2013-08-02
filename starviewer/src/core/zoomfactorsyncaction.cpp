#include "zoomfactorsyncaction.h"

#include "qviewer.h"
#include "anatomicalplanesynccriterion.h"

namespace udg {

ZoomFactorSyncAction::ZoomFactorSyncAction()
 : SyncAction()
{
    m_zoomFactor = 1.0;
}

ZoomFactorSyncAction::~ZoomFactorSyncAction()
{
}

void ZoomFactorSyncAction::setZoomFactor(double factor)
{
    m_zoomFactor = factor;
}

void ZoomFactorSyncAction::run(QViewer *viewer)
{
    if (viewer)
    {
        viewer->zoom(m_zoomFactor);
    }
}

void ZoomFactorSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ZoomFactorSyncAction", QObject::tr("zoom"), "zoom");
}

void ZoomFactorSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion();
}

} // End namespace udg

