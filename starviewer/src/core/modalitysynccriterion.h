#ifndef UDGMODALITYSYNCCRITERION_H
#define UDGMODALITYSYNCCRITERION_H

#include "synccriterion.h"

class QStringList;

namespace udg {

class Q2DViewer;

/**
    Implements the SyncCriterion for the viewers input.
    The inputs have to be of the same modality to meet the criterion.
    The exception comes with the PT-CT case. In case we have PT and CT modalities we consider
    the criterion is met due to the case of PET-CT fusion.
 */
class ModalitySyncCriterion : public SyncCriterion {
public:
    ModalitySyncCriterion();
    ~ModalitySyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    /// Returns the modalities of the viewer inputs
    QStringList getModalities(Q2DViewer *viewer) const;

    /// Checks whether we are in the case of a combination of PET-CT
    bool isPETCTCase(const QStringList &modalities1, const QStringList &modalities2) const;
};

} // End namespace udg

#endif
