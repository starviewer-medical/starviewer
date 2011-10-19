#ifndef FUZZYCOMPARETESTHELPER_H
#define FUZZYCOMPARETESTHELPER_H

#include <QVector>

class QVector3D;

namespace testing {

/// Classe que contindrà les funcions de comparació fuzzy que necessitem
class FuzzyCompareTestHelper {
public:
    /// Fa una comparació fuzzy de dos valors double. Tenim un tercer paràmetre opcional epsilon que ens 
    /// permetrà fixar la precisió que volem en la comparació.
    static bool fuzzyCompare(const double &v1, const double &v2, const double &epsilon = 0.000000001);

    /// Fa una comparació fuzzy de dos valors QVector<double>. Tots dos hauran de ser de la mateixa longitud.
    /// Tenim un tercer paràmetre opcional epsilon que ens permetrà fixar la precisió que volem en la comparació.
    static bool fuzzyCompare(const QVector<double> &v1, const QVector<double> &v2, const double &epsilon = 0.000000001);

    /// Fa una comparació fuzzy de dos valors QVector3D.
    /// Tenim un tercer paràmetre opcional epsilon que ens permetrà fixar la precisió que volem en la comparació.
    static bool fuzzyCompare(const QVector3D &v1, const QVector3D &v2, const double &epsilon = 0.000000001);
};

}

#endif // FUZZYCOMPARETESTHELPER_H
