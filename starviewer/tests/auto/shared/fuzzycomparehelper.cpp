#include "fuzzycomparehelper.h"

namespace testing {

bool FuzzyCompareHelper::fuzzyCompare(const double &v1, const double &v2, const double &epsilon)
{
    const double diff = v1 - v2;
    return (diff < epsilon) && (diff > -epsilon);
}

}
