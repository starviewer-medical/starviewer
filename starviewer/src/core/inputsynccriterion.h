#ifndef UDGINPUTSYNCCRITERIA_H
#define UDGINPUTSYNCCRITERIA_H

#include "synccriterion.h"

namespace udg {

/**
    Implements the SyncCriterion for the viewers input.
    The inputs have to be the same to meet the criterion.
 */
class InputSyncCriterion : public SyncCriterion {
public:
    InputSyncCriterion();
    ~InputSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);
};

} // End namespace udg

#endif
