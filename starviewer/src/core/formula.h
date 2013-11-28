#ifndef UDGFORMULA_H
#define UDGFORMULA_H

#include <QString>

namespace udg {

/**
    Base class to define formulas
 */
class Formula {
public:
    Formula();
    ~Formula();

    /// Returns the units of the formula. Should be re-implemented on each inherited class.
    /// Default will be units-less (empty string).
    static QString getUnits();
};

} // End namespace udg

#endif
