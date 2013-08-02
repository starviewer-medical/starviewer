#ifndef UDGSYNCCRITERION_H
#define UDGSYNCCRITERION_H

namespace udg {

class QViewer;

/**
    Interface to define a criterion for synchronization or propagation purposes.
    Each subclass must implement criterionIsMet() method which tells whether 
    the criterion is met between two viewers or not
 */
class SyncCriterion {
public:
    SyncCriterion();
    virtual ~SyncCriterion();

    /// Returns true if the given viewers meet the specified criteria
    bool isCriterionSatisfied(QViewer *viewer1, QViewer *viewer2);

protected:
    /// To be implemented by each su
    virtual bool criterionIsMet(QViewer *viewer1, QViewer *viewer2) = 0;
};

} // End namespace udg

#endif
