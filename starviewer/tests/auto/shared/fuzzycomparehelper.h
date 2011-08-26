#ifndef FUZZYCOMPAREHELPER_H
#define FUZZYCOMPAREHELPER_H

#include <QString>

namespace testing {

/// Classe que contindrà les funcions de comparació fuzzy que necessitem
class FuzzyCompareHelper {
public:
    /// Fa una comparació fuzzy de dos valors double. Tenim un tercer paràmetre opcional epsilon que ens 
    /// permetrà fixar la precisió que volem en la comparació.
    static bool fuzzyCompare(const double &v1, const double &v2, const double &epsilon = 0.000000001);
};

}

#endif // FUZZYCOMPAREHELPER_H
