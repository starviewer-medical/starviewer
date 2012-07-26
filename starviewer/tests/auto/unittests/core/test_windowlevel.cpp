#include "autotest.h"
#include "windowlevel.h"

using namespace udg;

class test_WindowLevel : public QObject {
Q_OBJECT

private slots:
    void WindowLevel_InitializesDataAsExpected();

    void isValid_ReturnsExpectedValues_data();
    void isValid_ReturnsExpectedValues();
};

void test_WindowLevel::WindowLevel_InitializesDataAsExpected()
{
    WindowLevel windowLevel;

    QCOMPARE(windowLevel.getWidth(), 0.0);
    QCOMPARE(windowLevel.getLevel(), 0.0);
    QCOMPARE(windowLevel.getName(), QString());
}

void test_WindowLevel::isValid_ReturnsExpectedValues_data()
{
    QTest::addColumn<double>("width");
    QTest::addColumn<double>("level");
    QTest::addColumn<bool>("expectedReturnValue");

    QTest::newRow("invalid values") << 0.0 << 12.0 << false; 
    QTest::newRow("valid values") << 128.7 << 0.0 << true;
}

void test_WindowLevel::isValid_ReturnsExpectedValues()
{
    QFETCH(double, width);
    QFETCH(double, level);
    QFETCH(bool, expectedReturnValue);

    WindowLevel windowLevel;

    windowLevel.setWidth(width);
    windowLevel.setLevel(level);

    QCOMPARE(windowLevel.isValid(), expectedReturnValue);

}

DECLARE_TEST(test_WindowLevel)

#include "test_windowlevel.moc"
