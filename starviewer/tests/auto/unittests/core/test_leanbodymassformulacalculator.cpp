#include "autotest.h"
#include "leanbodymassformulacalculator.h"
#include "testingdicomtagreader.h"
#include "imagetesthelper.h"


#include <QString>
#include <QtCore/qmath.h>

using namespace udg;
using namespace testing;

class test_LeanBodyMassFormulaCalculator : public QObject {
    Q_OBJECT

private slots:
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data();
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource();

    void compute_ShouldReturnExpectedResultUsingImageAsDataSource_data();
    void compute_ShouldReturnExpectedResultUsingImageAsDataSource();

    void canCompute_ShouldReturnExpectedResult_data();
    void canCompute_ShouldReturnExpectedResult();

private:
    void prepareComputeData();
};

void test_LeanBodyMassFormulaCalculator::prepareComputeData()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");
    QTest::addColumn<QString>("patientsSex");
    QTest::addColumn<double>("expectedResult");

    int patientsWeightInKg = 70;
    int patientsHeightInCm = 184;

    double maleExpectedResult = 1.10 * patientsWeightInKg - 120 * qPow(patientsWeightInKg / (double)patientsHeightInCm, 2);
    double femaleExpectedResult = 1.07 * patientsWeightInKg - 148 * qPow(patientsWeightInKg / (double)patientsHeightInCm, 2);

    QTest::newRow("male") <<  patientsHeightInCm << patientsWeightInKg << "M" << maleExpectedResult;
    QTest::newRow("female") << patientsHeightInCm << patientsWeightInKg << "F" << femaleExpectedResult;
    QTest::newRow("invalid sex") << patientsHeightInCm << patientsWeightInKg << "invalid" << 0.0;
}

void test_LeanBodyMassFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data()
{
    prepareComputeData();
}

void test_LeanBodyMassFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(QString, patientsSex);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMPatientSex, patientsSex);
    tagReader.addTag(DICOMPatientSize, patientsHeightInCm / 100.0);
    tagReader.addTag(DICOMPatientWeight, patientsWeightInKg);

    LeanBodyMassFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.compute(), expectedResult);
}

void test_LeanBodyMassFormulaCalculator::compute_ShouldReturnExpectedResultUsingImageAsDataSource_data()
{
    prepareComputeData();
}

void test_LeanBodyMassFormulaCalculator::compute_ShouldReturnExpectedResultUsingImageAsDataSource()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(QString, patientsSex);
    QFETCH(double, expectedResult);

    Image *image = ImageTestHelper::createImageWithSexAndHeightAndWeight(patientsSex, patientsHeightInCm / 100.0, patientsWeightInKg);

    LeanBodyMassFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(image);

    QCOMPARE(formulaCalculator.compute(), expectedResult);

    ImageTestHelper::cleanUp(image);
}

void test_LeanBodyMassFormulaCalculator::canCompute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");
    QTest::addColumn<QString>("patientsSex");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("valid male values") << 123 << 108 << "M" << true;
    QTest::newRow("valid female values") << 187 << 35 << "F" << true;
    QTest::newRow("invalid weight") << 0 << 1 << "M" << false;
    QTest::newRow("invalid height") << 1 << -1 << "M" << false;
    QTest::newRow("invalid sex") << 134 << 10 << "MF" << false;
    QTest::newRow("invalid weight and height") << -5 << 0 << "M" << false;
    QTest::newRow("invalid weight and sex") << -5 << 70 << "m" << false;
    QTest::newRow("invalid height and sex") << 191 << -1 << "f" << false;
    QTest::newRow("invalid weigth and height and sex") << -10 << -1 << "" << false;
}

void test_LeanBodyMassFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(QString, patientsSex);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMPatientSex, patientsSex);
    tagReader.addTag(DICOMPatientSize, patientsHeightInCm / 100.0);
    tagReader.addTag(DICOMPatientWeight, patientsWeightInKg);

    LeanBodyMassFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}


DECLARE_TEST(test_LeanBodyMassFormulaCalculator)

#include "test_leanbodymassformulacalculator.moc"
