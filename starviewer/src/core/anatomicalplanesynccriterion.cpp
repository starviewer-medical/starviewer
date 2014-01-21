#include "anatomicalplanesynccriterion.h"

#include "q2dviewer.h"

namespace udg {

AnatomicalPlaneSyncCriterion::AnatomicalPlaneSyncCriterion()
 : SyncCriterion()
{
}

AnatomicalPlaneSyncCriterion::~AnatomicalPlaneSyncCriterion()
{
}

bool AnatomicalPlaneSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    return criterionIsMet(Q2DViewer::castFromQViewer(viewer1), Q2DViewer::castFromQViewer(viewer2));
}

bool AnatomicalPlaneSyncCriterion::criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    else
    {
        AnatomicalPlane::AnatomicalPlaneType anatomicalPlane = viewer1->getCurrentAnatomicalPlane();
        if (anatomicalPlane != AnatomicalPlane::Axial && anatomicalPlane != AnatomicalPlane::Sagittal && anatomicalPlane != AnatomicalPlane::Coronal)
        {
            return false;
        }
        else
        {
            return anatomicalPlane == viewer2->getCurrentAnatomicalPlane();
        }
    }
}

} // End namespace udg
