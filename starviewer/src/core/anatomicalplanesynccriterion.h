#ifndef UDGANATOMICALPLANESYNCCRITERIA_H
#define UDGANATOMICALPLANESYNCCRITERIA_H

#include "synccriterion.h"

namespace udg {

class Q2DViewer;

/**
    Implements the SyncCriterion for AnatomicalPlanes.
 */
class AnatomicalPlaneSyncCriterion : public SyncCriterion {
public:
    AnatomicalPlaneSyncCriterion();
    ~AnatomicalPlaneSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    bool criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2);
};

} // End namespace udg

#endif
