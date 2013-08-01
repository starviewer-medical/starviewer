#include "testingmammographyimagehelper.h"

namespace testing {

TestingMammographyImageHelper::TestingMammographyImageHelper()
{
}

TestingMammographyImageHelper::~TestingMammographyImageHelper()
{
}

const QStringList TestingMammographyImageHelper::getMammographyAutoOrientationExceptions()
{
    return m_testingMammographyAutoOrientationExceptions;
}

} // namespace udg
