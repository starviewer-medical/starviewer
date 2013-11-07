#include "inputsynccriterion.h"

#include "volume.h"
#include "qviewer.h"
#include "q2dviewer.h"

namespace udg {

InputSyncCriterion::InputSyncCriterion()
 : SyncCriterion()
{
}

InputSyncCriterion::~InputSyncCriterion()
{
}

bool InputSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    // TODO Should also check if some reconstruction is applied, i.e. thick slab?
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer2);

    if (viewer2D)
    {
        return viewer2D->containsVolume(viewer1->getMainInput());
    }
    else
    {
        return viewer1->getMainInput() == viewer2->getMainInput();
    }
}

} // End namespace udg
