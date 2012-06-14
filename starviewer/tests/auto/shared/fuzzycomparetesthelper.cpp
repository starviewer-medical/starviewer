#include "fuzzycomparetesthelper.h"

#include <QVector3D>

namespace testing {

bool FuzzyCompareTestHelper::fuzzyCompare(const double &v1, const double &v2, const double &epsilon)
{
    const double diff = v1 - v2;
    // La primera comprovació d'igualtat és imprescindible per comparar infinits, sinó surt NaN
    return (v1 == v2) || ((diff < epsilon) && (diff > -epsilon));
}

bool FuzzyCompareTestHelper::fuzzyCompare(const QVector<double> &v1, const QVector<double> &v2, const double &epsilon)
{
    if (v1.size() != v2.size())
    {
        return false;
    }

    int i = 0;
    bool equal = true;
    while (i < v1.size() && equal)
    {
        equal = fuzzyCompare(v1.at(i), v2.at(i), epsilon);
        ++i;
    }

    return equal;
}

bool FuzzyCompareTestHelper::fuzzyCompare(const QVector3D &v1, const QVector3D &v2, const double &epsilon)
{
    return fuzzyCompare(v1.x(), v2.x(), epsilon)
        && fuzzyCompare(v1.y(), v2.y(), epsilon)
        && fuzzyCompare(v1.z(), v2.z(), epsilon);
}

bool FuzzyCompareTestHelper::fuzzyCompare(const Vector3Float &v1, const Vector3Float &v2, const double &epsilon)
{
    return fuzzyCompare(v1.x, v2.x, epsilon)
        && fuzzyCompare(v1.y, v2.y, epsilon)
        && fuzzyCompare(v1.z, v2.z, epsilon);
}

bool FuzzyCompareTestHelper::fuzzyCompare(const Vector3 &v1, const Vector3 &v2, const double &epsilon)
{
    return fuzzyCompare(v1.x, v2.x, epsilon)
        && fuzzyCompare(v1.y, v2.y, epsilon)
        && fuzzyCompare(v1.z, v2.z, epsilon);
}

}
