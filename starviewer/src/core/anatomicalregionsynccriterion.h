#ifndef UDG_ANATOMICALREGIONSYNCCRITERION_H
#define UDG_ANATOMICALREGIONSYNCCRITERION_H

#include "synccriterion.h"
#include "anatomicalplane.h"

namespace udg {

class Q2DViewer;


/// Implements the SyncCriterion for anatomical region restriction.

class AnatomicalRegionSyncCriterion : public SyncCriterion
{
public:
    AnatomicalRegionSyncCriterion();
    ~AnatomicalRegionSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    bool criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2);
    int getYIndex(AnatomicalPlane::AnatomicalPlaneType originalPlane, AnatomicalPlane::AnatomicalPlaneType currentPlane);
};

} // namespace udg

#endif // UDG_ANATOMICALREGIONSYNCCRITERION_H
