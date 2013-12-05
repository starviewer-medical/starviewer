#include "autotest.h"
#include "leanbodymassformula.h"
#include <QtCore/qmath.h>
using namespace udg;

class test_LeanBodyMassFormula : public QObject {
Q_OBJECT

private slots:
    void computeMale_ShouldReturnExpectedResult_data();
    void computeMale_ShouldReturnExpectedResult();

    void computeFemale_ShouldReturnExpectedResult_data();
    void computeFemale_ShouldReturnExpectedResult();
};

void test_LeanBodyMassFormula::computeMale_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");

    QTest::newRow("normal values") << 165 << 70;
}

void test_LeanBodyMassFormula::computeMale_ShouldReturnExpectedResult()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);

    double expectedResult = 1.10 * patientsWeightInKg - 120 * qPow(patientsWeightInKg / (double)patientsHeightInCm, 2);

    LeanBodyMassFormula formula;

    QCOMPARE(formula.computeMale(patientsHeightInCm, patientsWeightInKg), expectedResult);
}

void test_LeanBodyMassFormula::computeFemale_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");

    QTest::newRow("Tipical values") << 175 << 82;
}

void test_LeanBodyMassFormula::computeFemale_ShouldReturnExpectedResult()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);

    double expectedResult = 1.07 * patientsWeightInKg - 148 * qPow(patientsWeightInKg / (double)patientsHeightInCm, 2);

    LeanBodyMassFormula formula;

    QCOMPARE(formula.computeFemale(patientsHeightInCm, patientsWeightInKg), expectedResult);
}


DECLARE_TEST(test_LeanBodyMassFormula)

#include "test_leanbodymassformula.moc"
