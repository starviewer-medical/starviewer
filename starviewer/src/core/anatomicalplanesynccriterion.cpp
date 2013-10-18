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
        return viewer1->getCurrentAnatomicalPlaneLabel() == viewer2->getCurrentAnatomicalPlaneLabel();
    }
}

} // End namespace udg
