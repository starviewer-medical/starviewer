#include "autotest.h"
#include "decaycorrectionfactorformula.h"
#include <QtCore/qmath.h>

using namespace udg;

class test_DecayCorrectionFactorFormula : public QObject {
Q_OBJECT

private slots:
    void compute_ShouldReturnExpectedResult_data();
    void compute_ShouldReturnExpectedResult();
};

void test_DecayCorrectionFactorFormula::compute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<int>("timeLapseInSeconds");

    QTest::newRow("normal values") << 43 << 25;
    QTest::newRow("time lapse is 0") << 175 << 0;
    QTest::newRow("radionuclide is 0") << 0 << 5;
}

void test_DecayCorrectionFactorFormula::compute_ShouldReturnExpectedResult()
{
    QFETCH(int, radionuclideHalfLifeInSeconds);
    QFETCH(int, timeLapseInSeconds);

    double expectedResult = qPow(2, -timeLapseInSeconds / (double)radionuclideHalfLifeInSeconds);

    DecayCorrectionFactorFormula formula;

    QCOMPARE(formula.compute(radionuclideHalfLifeInSeconds, timeLapseInSeconds), expectedResult);
}


DECLARE_TEST(test_DecayCorrectionFactorFormula)

#include "test_decaycorrectionfactorformula.moc"
