#include "synccriterion.h"

#include "qviewer.h"

namespace udg {

SyncCriterion::SyncCriterion()
{
}

SyncCriterion::~SyncCriterion()
{
}

bool SyncCriterion::isCriterionSatisfied(QViewer *viewer1, QViewer *viewer2)
{
    if (viewer1 && viewer2)
    {
        if (!viewer1->hasInput() || !viewer2->hasInput())
        {
            return false;
        }
        else
        {
            return criterionIsMet(viewer1, viewer2);
        }
    }
    else
    {
        return false;
    }
}

} // End namespace udg
