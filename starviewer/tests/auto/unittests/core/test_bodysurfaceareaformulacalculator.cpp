#include "autotest.h"
#include "bodysurfaceareaformulacalculator.h"
#include "testingdicomtagreader.h"
#include "imagetesthelper.h"


#include <QString>
#include <QtCore/qmath.h>

using namespace udg;
using namespace testing;

class test_BodySurfaceAreaFormulaCalculator : public QObject {
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

Q_DECLARE_METATYPE(TestingDICOMTagReader)

void test_BodySurfaceAreaFormulaCalculator::prepareComputeData()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");
    QTest::addColumn<double>("expectedResult");

    int patientsWeightInKg = 70;
    int patientsHeightInCm = 184;

    double expectedResult = 71.84 * pow(patientsHeightInCm, 0.725) * qPow(patientsWeightInKg, 0.425);

    QTest::newRow("valid values") <<  patientsHeightInCm << patientsWeightInKg << expectedResult;
}

void test_BodySurfaceAreaFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data()
{
    prepareComputeData();
}

void test_BodySurfaceAreaFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMPatientSize, patientsHeightInCm / 100.0);
    tagReader.addTag(DICOMPatientWeight, patientsWeightInKg);

    BodySurfaceAreaFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.compute(), expectedResult);
}

void test_BodySurfaceAreaFormulaCalculator::compute_ShouldReturnExpectedResultUsingImageAsDataSource_data()
{
    prepareComputeData();
}

void test_BodySurfaceAreaFormulaCalculator::compute_ShouldReturnExpectedResultUsingImageAsDataSource()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(double, expectedResult);

    Image *image = ImageTestHelper::createImageWithHeightAndWeight(patientsHeightInCm / 100.0, patientsWeightInKg);

    BodySurfaceAreaFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(image);

    QCOMPARE(formulaCalculator.compute(), expectedResult);

    ImageTestHelper::cleanUp(image);
}

void test_BodySurfaceAreaFormulaCalculator::canCompute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<int>("patientsHeightInCm");
    QTest::addColumn<int>("patientsWeightInKg");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("valid values") << 123 << 108 << true;
    QTest::newRow("invalid weight") << 0 << 1 << false;
    QTest::newRow("invalid height") << 1 << -1 << false;
    QTest::newRow("invalid weight and height") << -5 << 0 << false;
}

void test_BodySurfaceAreaFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(int, patientsHeightInCm);
    QFETCH(int, patientsWeightInKg);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMPatientSize, patientsHeightInCm / 100.0);
    tagReader.addTag(DICOMPatientWeight, patientsWeightInKg);

    BodySurfaceAreaFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}


DECLARE_TEST(test_BodySurfaceAreaFormulaCalculator)

#include "test_bodysurfaceareaformulacalculator.moc"
