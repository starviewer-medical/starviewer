#ifndef TESTINGMAMMOGRAPHYIMAGEHELPER_H
#define TESTINGMAMMOGRAPHYIMAGEHELPER_H

#include "mammographyimagehelper.h"

#include <QStringList>

namespace testing {

class TestingMammographyImageHelper : public udg::MammographyImageHelper
{
public:
    TestingMammographyImageHelper();
    virtual ~TestingMammographyImageHelper();

    QStringList m_testingMammographyAutoOrientationExceptions;

protected:
    virtual const QStringList getMammographyAutoOrientationExceptions();
};

} // namespace testing

#endif // TESTINGMAMMOGRAPHYIMAGEHELPER_H
