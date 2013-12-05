#include "autotest.h"
#include "bodysurfaceareaformula.h"
#include <QtCore/qmath.h>

using namespace udg;

class test_BodySurfaceAreaFormula : public QObject {
Q_OBJECT

private slots:
    void compute_ShouldReturnExpectedResult_data();
    void compute_ShouldReturnExpectedResult();
};

void test_BodySurfaceAreaFormula::compute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");

    QTest::newRow("normal values") << 175 << 82;
}

void test_BodySurfaceAreaFormula::compute_ShouldReturnExpectedResult()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);

    double expectedResult = 71.84 * qPow(patientsHeightInCm, 0.725) * qPow(patientsWeightInKg, 0.425);

    BodySurfaceAreaFormula formula;

    QCOMPARE(formula.compute(patientsHeightInCm, patientsWeightInKg), expectedResult);
}


DECLARE_TEST(test_BodySurfaceAreaFormula)

#include "test_bodysurfaceareaformula.moc"
