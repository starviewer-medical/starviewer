#include "autotest.h"
#include "windowlevel.h"

using namespace udg;

class test_WindowLevel : public QObject {
Q_OBJECT

private slots:
    void WindowLevel_InitializesDataAsExpected();

    void isValid_ReturnsExpectedValues_data();
    void isValid_ReturnsExpectedValues();

    void valuesAreEqual_returnsExpectedResult_data();
    void valuesAreEqual_returnsExpectedResult();

    void operatorEqual_returnsExpectedResult_data();
    void operatorEqual_returnsExpectedResult();
};

Q_DECLARE_METATYPE(WindowLevel)

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

void test_WindowLevel::valuesAreEqual_returnsExpectedResult_data()
{
    QTest::addColumn<WindowLevel>("wl1");
    QTest::addColumn<WindowLevel>("wl2");
    QTest::addColumn<bool>("expectedResult");

    const QString name1("WINDOW 1");
    const QString name2("WINDOW 2");
    
    WindowLevel wl1(1.0, 1.0, name1);
    WindowLevel wl2(1.0, 1.0, name2);
    WindowLevel wl3(3.0, 3.0, name1);
    WindowLevel wl4(4.0, 4.0, name2);
    
    QTest::newRow("Same values, same names") << wl1 << wl1 << true;
    QTest::newRow("Same values, different names") << wl1 << wl2 << true;
    QTest::newRow("Different values, same names") << wl1 << wl3 << false;
    QTest::newRow("Different values, different names") << wl3 << wl4 << false;
}

void test_WindowLevel::valuesAreEqual_returnsExpectedResult()
{
    QFETCH(WindowLevel, wl1);
    QFETCH(WindowLevel, wl2);
    QFETCH(bool, expectedResult);

    QCOMPARE(wl1.valuesAreEqual(wl2), expectedResult);
}

void test_WindowLevel::operatorEqual_returnsExpectedResult_data()
{
    QTest::addColumn<WindowLevel>("wl1");
    QTest::addColumn<WindowLevel>("wl2");
    QTest::addColumn<bool>("expectedResult");

    const QString name1("WINDOW 1");
    const QString name2("WINDOW 2");
    
    WindowLevel wl1(1.0, 1.0, name1);
    WindowLevel wl2(1.0, 1.0, name2);
    WindowLevel wl3(3.0, 3.0, name1);
    WindowLevel wl4(4.0, 4.0, name2);
    
    QTest::newRow("Same values, same names") << wl1 << wl1 << true;
    QTest::newRow("Same values, different names") << wl1 << wl2 << false;
    QTest::newRow("Different values, same names") << wl1 << wl3 << false;
    QTest::newRow("Different values, different names") << wl3 << wl4 << false;
}

void test_WindowLevel::operatorEqual_returnsExpectedResult()
{
    QFETCH(WindowLevel, wl1);
    QFETCH(WindowLevel, wl2);
    QFETCH(bool, expectedResult);

    QCOMPARE(wl1 == wl2, expectedResult);
}

DECLARE_TEST(test_WindowLevel)

#include "test_windowlevel.moc"
