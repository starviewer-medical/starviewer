#include "autotest.h"

#include "opacitytransferfunction.h"

using namespace udg;

class test_OpacityTransferFunction : public QObject {

    Q_OBJECT

private slots:

    void set_ShouldSetOpacityForX_data();
    void set_ShouldSetOpacityForX();

    void get_ShouldReturnExpectedValue_data();
    void get_ShouldReturnExpectedValue();

};

typedef QPair<double, double> XOpacityPair;
Q_DECLARE_METATYPE(QList<XOpacityPair>)
Q_DECLARE_METATYPE(OpacityTransferFunction)

void test_OpacityTransferFunction::set_ShouldSetOpacityForX_data()
{
    QTest::addColumn< QList<XOpacityPair> >("input");
    QTest::addColumn<double>("getX");
    QTest::addColumn<double>("getOpacity");

    QList<XOpacityPair> inputOneTime;
    inputOneTime << XOpacityPair(5.0, 1.0);
    QList<XOpacityPair> inputMultipleTimes;
    inputMultipleTimes << XOpacityPair(5.0, 1.0) << XOpacityPair(5.0, 0.1234);
    QTest::newRow("set one time") << inputOneTime << 5.0 << 1.0;
    QTest::newRow("set multiple times") << inputMultipleTimes << 5.0 << 0.1234;
}

void test_OpacityTransferFunction::set_ShouldSetOpacityForX()
{
    QFETCH(QList<XOpacityPair>, input);
    QFETCH(double, getX);
    QFETCH(double, getOpacity);

    OpacityTransferFunction opacityTransferFunction;
    
    foreach (const XOpacityPair &pair, input)
    {
        opacityTransferFunction.set(pair.first, pair.second);
    }

    QVERIFY(opacityTransferFunction.isSet(getX));
    QCOMPARE(opacityTransferFunction.get(getX), getOpacity);
}

void test_OpacityTransferFunction::get_ShouldReturnExpectedValue_data()
{
    OpacityTransferFunction emptyOpacityTransferFunction;
    OpacityTransferFunction opacityTransferFunction;
    opacityTransferFunction.set(-1000.0, 0.35);
    opacityTransferFunction.set(0.0, 0.99);
    opacityTransferFunction.set(0.01, 0.01);
    opacityTransferFunction.set(0.02, 0.99);
    opacityTransferFunction.set(95.0, 0.99);
    opacityTransferFunction.set(4095.0, 0.7);
    
    QTest::addColumn<OpacityTransferFunction>("opacityTransferFunction");
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("opacity");

    QTest::newRow("get from empty function") << emptyOpacityTransferFunction << 42.0 << double();
    QTest::newRow("get at left of leftmost x") << opacityTransferFunction << -1200.0 << 0.35;
    QTest::newRow("get at defined x") << opacityTransferFunction << 0.0 << 0.99;
    QTest::newRow("get at right of rightmost x") << opacityTransferFunction << 4400.0 << 0.7;
    QTest::newRow("interpolate uphill") << opacityTransferFunction << -500.0 << 0.67;
    QTest::newRow("interpolate plain") << opacityTransferFunction << 50.0 << 0.99;
    QTest::newRow("interpolate downhill") << opacityTransferFunction << 2095.0 << 0.845;
    QTest::newRow("interpolate between close values") << opacityTransferFunction << 0.005 << 0.5;
}

void test_OpacityTransferFunction::get_ShouldReturnExpectedValue()
{
    QFETCH(OpacityTransferFunction, opacityTransferFunction);
    QFETCH(double, x);
    QFETCH(double, opacity);

    QCOMPARE(opacityTransferFunction.get(x), opacity);
}

DECLARE_TEST(test_OpacityTransferFunction)

#include "test_opacitytransferfunction.moc"
