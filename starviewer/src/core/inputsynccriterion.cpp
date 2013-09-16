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
        bool found = false;
        QList<Volume*> volumes = viewer2D->getInputs();
        int i = 0;
        while(!found && i < volumes.size())
        {
            found = viewer1->getMainInput() == volumes.at(i);
            i++;
        }

        if (volumes.size() > 1)
        {
            found = found;
        }
        return found;
    }
    else
    {
        return viewer1->getMainInput() == viewer2->getMainInput();
    }
}

} // End namespace udg
