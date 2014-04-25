#include "zoomfactorsyncaction.h"

#include "qviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "modalitysynccriterion.h"

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
        viewer->absoluteZoom(m_zoomFactor);
    }
}

void ZoomFactorSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ZoomFactorSyncAction", QObject::tr("Zoom"), "zoom");
}

void ZoomFactorSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new ModalitySyncCriterion();
}

} // End namespace udg

