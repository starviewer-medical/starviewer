#include "singlesliceorvolumetricsynccriterion.h"

#include "qviewer.h"
#include "volume.h"

namespace udg {

SingleSliceOrVolumetricSyncCriterion::SingleSliceOrVolumetricSyncCriterion()
{
}

SingleSliceOrVolumetricSyncCriterion::~SingleSliceOrVolumetricSyncCriterion()
{
}

bool SingleSliceOrVolumetricSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    return viewer1->getMainInput()->areAllImagesInTheSameAnatomicalPlane() && viewer2->getMainInput()->areAllImagesInTheSameAnatomicalPlane();
}

} // End namespace udg
