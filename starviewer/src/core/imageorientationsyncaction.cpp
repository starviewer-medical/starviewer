#include "imageorientationsyncaction.h"

#include "q2dviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "modalitysynccriterion.h"

namespace udg {

ImageOrientationSyncAction::ImageOrientationSyncAction()
 : SyncAction()
{
}

ImageOrientationSyncAction::~ImageOrientationSyncAction()
{
}

void ImageOrientationSyncAction::setImageOrientation(const PatientOrientation &orientation)
{
    m_orientation = orientation;
}

void ImageOrientationSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
    if (viewer2D)
    {
        viewer2D->setImageOrientation(m_orientation);
    }
}

void ImageOrientationSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ImageOrientationSyncAction", QObject::tr("image orientation"), "imageOrientation");
}

void ImageOrientationSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new ModalitySyncCriterion();
}

} // End namespace udg
