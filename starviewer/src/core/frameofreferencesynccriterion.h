#ifndef UDG_FRAMEOFREFERENCESYNCCRITERION_H
#define UDG_FRAMEOFREFERENCESYNCCRITERION_H

#include "synccriterion.h"

namespace udg {

class QViewer;

/**
    Implements the SyncCriterion for Frame Of References.
 */
class FrameOfReferenceSyncCriterion : public SyncCriterion
{
public:
    FrameOfReferenceSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);
};

} // namespace udg

#endif // UDG_FRAMEOFREFERENCESYNCCRITERION_H
