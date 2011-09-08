#include "fuzzycomparetesthelper.h"

namespace testing {

bool FuzzyCompareTestHelper::fuzzyCompare(const double &v1, const double &v2, const double &epsilon)
{
    const double diff = v1 - v2;
    return (diff < epsilon) && (diff > -epsilon);
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

}
