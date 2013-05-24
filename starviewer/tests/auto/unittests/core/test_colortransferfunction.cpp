#include "autotest.h"

#include "colortransferfunction.h"

using namespace udg;

class test_ColorTransferFunction : public QObject {

    Q_OBJECT

private slots:

    void set_ShouldSetColorForX_data();
    void set_ShouldSetColorForX();

    void get_ShouldReturnExpectedColor_data();
    void get_ShouldReturnExpectedColor();

};

typedef QPair<double, QColor> XColorPair;
Q_DECLARE_METATYPE(QList<XColorPair>)
Q_DECLARE_METATYPE(ColorTransferFunction)

void test_ColorTransferFunction::set_ShouldSetColorForX_data()
{
    QTest::addColumn< QList<XColorPair> >("input");
    QTest::addColumn<double>("getX");
    QTest::addColumn<QColor>("getColor");

    QList<XColorPair> inputOneTime;
    inputOneTime << XColorPair(5.0, QColor(12, 34, 56));
    QList<XColorPair> inputMultipleTimes;
    inputMultipleTimes << XColorPair(5.0, QColor(193, 91, 9)) << XColorPair(5.0, QColor(48, 88, 20));
    QTest::newRow("set one time") << inputOneTime << 5.0 << QColor(12, 34, 56);
    QTest::newRow("set multiple times") << inputMultipleTimes << 5.0 << QColor(48, 88, 20);
}

void test_ColorTransferFunction::set_ShouldSetColorForX()
{
    QFETCH(QList<XColorPair>, input);
    QFETCH(double, getX);
    QFETCH(QColor, getColor);

    ColorTransferFunction ColorTransferFunction;
    
    foreach (const XColorPair &pair, input)
    {
        ColorTransferFunction.set(pair.first, pair.second);
    }

    QVERIFY(ColorTransferFunction.isSet(getX));
    QCOMPARE(ColorTransferFunction.get(getX), getColor);
}

void test_ColorTransferFunction::get_ShouldReturnExpectedColor_data()
{
    ColorTransferFunction emptyColorTransferFunction;
    ColorTransferFunction colorTransferFunction;
    colorTransferFunction.set(-1000.0, QColor(100, 100, 100));
    colorTransferFunction.set(0.0, QColor(150, 180, 200));
    colorTransferFunction.set(0.01, QColor(170, 140, 100));
    colorTransferFunction.set(0.02, QColor(250, 255, 0));
    colorTransferFunction.set(95.0, QColor(250, 255, 0));
    colorTransferFunction.set(4095.0, QColor(0, 1, 0));
    
    QTest::addColumn<ColorTransferFunction>("colorTransferFunction");
    QTest::addColumn<double>("x");
    QTest::addColumn<QColor>("color");

    QTest::newRow("get from empty function") << emptyColorTransferFunction << 42.0 << QColor(Qt::black);
    QTest::newRow("get at left of leftmost x") << colorTransferFunction << -1200.0 << QColor(100, 100, 100);
    QTest::newRow("get at defined x") << colorTransferFunction << 0.0 << QColor(150, 180, 200);
    QTest::newRow("get at right of rightmost x") << colorTransferFunction << 4400.0 << QColor(0, 1, 0);
    QTest::newRow("interpolate uphill") << colorTransferFunction << -500.0 << QColor(125, 140, 150);
    QTest::newRow("interpolate plain") << colorTransferFunction << 50.0 << QColor(250, 255, 0);
    QTest::newRow("interpolate downhill") << colorTransferFunction << 2095.0 << QColor(125, 128, 0);
    QTest::newRow("interpolate between close values") << colorTransferFunction << 0.005 << QColor(160, 160, 150);
}

void test_ColorTransferFunction::get_ShouldReturnExpectedColor()
{
    QFETCH(ColorTransferFunction, colorTransferFunction);
    QFETCH(double, x);
    QFETCH(QColor, color);

    QCOMPARE(colorTransferFunction.get(x), color);
}

DECLARE_TEST(test_ColorTransferFunction)

#include "test_colortransferfunction.moc"
