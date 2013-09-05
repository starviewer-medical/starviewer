#include "autotest.h"

#include "transferfunction.h"

using namespace udg;

class test_TransferFunction : public QObject {

    Q_OBJECT

private slots:

    void toNewRange_ShouldReturnExpectedTransferFunction_data();
    void toNewRange_ShouldReturnExpectedTransferFunction();

};

Q_DECLARE_METATYPE(TransferFunction)

void test_TransferFunction::toNewRange_ShouldReturnExpectedTransferFunction_data()
{
    QTest::addColumn<TransferFunction>("transferFunction");
    QTest::addColumn<double>("oldX1");
    QTest::addColumn<double>("oldX2");
    QTest::addColumn<double>("newX1");
    QTest::addColumn<double>("newX2");
    QTest::addColumn<TransferFunction>("expectedTransferFunction");

    QTest::newRow("empty") << TransferFunction() << 0.0 << 1.0 << 0.0 << 100.0 << TransferFunction();

    TransferFunction transferFunction1;
    transferFunction1.set(0.0, 1.0, 0.0, 0.0, 1.0);
    TransferFunction transferFunction1b;
    transferFunction1b.set(50.0, 1.0, 0.0, 0.0, 1.0);
    QTest::newRow("1 point") << transferFunction1 << -1.0 << 1.0 << 0.0 << 100.0 << transferFunction1b;

    TransferFunction transferFunction2;
    transferFunction2.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction2.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction2b;
    transferFunction2b.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction2b.set(1.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("2 points coincident") << transferFunction2 << 0.0 << 100.0 << 0.0 << 1.0 << transferFunction2b;

    TransferFunction transferFunction3;
    transferFunction3.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction3.set(50.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction3.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction3b;
    transferFunction3b.set(10.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction3b.set(60.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction3b.set(110.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("n points positive shift") << transferFunction3 << 5.0 << 15.0 << 15.0 << 25.0 << transferFunction3b;

    TransferFunction transferFunction4;
    transferFunction4.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction4.set(50.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction4.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction4b;
    transferFunction4b.set(-10.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction4b.set(40.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction4b.set(90.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("n points negative shift") << transferFunction4 << 5.0 << 15.0 << -5.0 << 5.0 << transferFunction4b;

    TransferFunction transferFunction5;
    transferFunction5.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction5.set(50.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction5.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction5b;
    transferFunction5b.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction5b.set(100.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction5b.set(200.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("n points upscale") << transferFunction5 << 5.0 << 15.0 << 10.0 << 30.0 << transferFunction5b;

    TransferFunction transferFunction6;
    transferFunction6.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction6.set(50.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction6.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction6b;
    transferFunction6b.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction6b.set(25.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction6b.set(50.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("n points downscale") << transferFunction6 << 5.0 << 15.0 << 2.5 << 7.5 << transferFunction6b;

    TransferFunction transferFunction7;
    transferFunction7.set(0.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction7.set(50.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction7.set(100.0, 0.0, 0.0, 1.0, 1.0);
    TransferFunction transferFunction7b;
    transferFunction7b.set(101.0, 1.0, 0.0, 0.0, 1.0);
    transferFunction7b.set(201.0, 0.0, 1.0, 0.0, 0.5);
    transferFunction7b.set(301.0, 0.0, 0.0, 1.0, 1.0);
    QTest::newRow("n points random range change") << transferFunction7 << -44.0 << -8.0 << 13.0 << 85.0 << transferFunction7b;
}

void test_TransferFunction::toNewRange_ShouldReturnExpectedTransferFunction()
{
    QFETCH(TransferFunction, transferFunction);
    QFETCH(double, oldX1);
    QFETCH(double, oldX2);
    QFETCH(double, newX1);
    QFETCH(double, newX2);
    QFETCH(TransferFunction, expectedTransferFunction);

    QCOMPARE(transferFunction.toNewRange(oldX1, oldX2, newX1, newX2), expectedTransferFunction);
}

DECLARE_TEST(test_TransferFunction)

#include "test_transferfunction.moc"
