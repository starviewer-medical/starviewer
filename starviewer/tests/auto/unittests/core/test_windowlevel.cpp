#include "autotest.h"
#include "windowlevel.h"

using namespace udg;

class test_WindowLevel : public QObject {
Q_OBJECT

private slots:
    void WindowLevel_InitializesDataAsExpected();
};

void test_WindowLevel::WindowLevel_InitializesDataAsExpected()
{
    WindowLevel windowLevel;

    QCOMPARE(windowLevel.getWidth(), 0.0);
    QCOMPARE(windowLevel.getLevel(), 0.0);
    QCOMPARE(windowLevel.getName(), QString());
}

DECLARE_TEST(test_WindowLevel)

#include "test_windowlevel.moc"
