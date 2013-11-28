#ifndef UDGBODYSURFACEAREAFORMULA_H
#define UDGBODYSURFACEAREAFORMULA_H

#include "formula.h"

namespace udg {

/**
    Computes Body Surface Area using Du Bois formula[1]
    
    [1] Du Bois, Delafield and Du Bois, Eugene F. A formula to estimate the approximate surface area if height and weight be known.
        Archives of Internal Medicine. 1916, Vol. 17, 6-2, pp. 863-871.
 */
class BodySurfaceAreaFormula : public Formula {
public:
    BodySurfaceAreaFormula();
    ~BodySurfaceAreaFormula();

    static QString getUnits();

    double compute(int patientsHeightInCm, int patientsWeightInKg);
};

} // End namespace udg

#endif
