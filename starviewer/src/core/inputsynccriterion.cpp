#include "inputsynccriterion.h"

#include "volume.h"
#include "qviewer.h"

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
    return viewer1->getInput() == viewer2->getInput();
}

} // End namespace udg
