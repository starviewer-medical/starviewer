#include "frameofreferencesynccriterion.h"

#include "qviewer.h"
#include "volume.h"
#include "series.h"

namespace udg {

FrameOfReferenceSyncCriterion::FrameOfReferenceSyncCriterion()
{
}

bool FrameOfReferenceSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    else
    {
        QString frameOfReferenceViewer1 = viewer1->getMainInput()->getSeries()->getFrameOfReferenceUID();
        return !frameOfReferenceViewer1.isEmpty() && (frameOfReferenceViewer1 == viewer2->getMainInput()->getSeries()->getFrameOfReferenceUID());
    }
}

} // namespace udg
