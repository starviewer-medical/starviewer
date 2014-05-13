#ifndef UDGANGLEBETWEENPLANESYNCCRITERIA_H
#define UDGANGLEBETWEENPLANESYNCCRITERIA_H

#include "synccriterion.h"

namespace udg {

class Q2DViewer;

/**
    Implements the SyncCriterion for the maximum angle between planes.
 */
class AngleBetweenPlanesSyncCriterion : public SyncCriterion {
public:
    AngleBetweenPlanesSyncCriterion();
    ~AngleBetweenPlanesSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    bool criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2);
};

} // End namespace udg

#endif
