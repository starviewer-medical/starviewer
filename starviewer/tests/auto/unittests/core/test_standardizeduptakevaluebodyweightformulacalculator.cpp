#include "autotest.h"
#include "standardizeduptakevaluebodyweightformulacalculator.h"
#include "testingdecaycorrectionfactorformulacalculator.h"

#include "testingdicomtagreader.h"
#include "imagetesthelper.h"
#include "dicomsequenceattribute.h"
#include "dicomsequenceitem.h"
#include "dicomvalueattribute.h"
#include "mathtools.h"

#include <QString>
#include <QtCore/qmath.h>

using namespace udg;
using namespace testing;


class TestingStandardizedUptakeValueBodyWeightFormulaCalculator : public StandardizedUptakeValueBodyWeightFormulaCalculator {

public:
    void setDecayCorrectionCalculator(DecayCorrectionFactorFormulaCalculator *calculator)
    {
        m_decayCorrectionCalculator = calculator;
    }

};

class test_StandardizedUptakeValueBodyWeightFormulaCalculator : public QObject {
    Q_OBJECT

private slots:
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data();
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource();

    void canCompute_ShouldReturnExpectedResult_data();
    void canCompute_ShouldReturnExpectedResult();
};

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data()
{
    QTest::addColumn<bool>("canComputeDecayFactor");
    QTest::addColumn<double>("decayFactor");
    QTest::addColumn<double>("radionuclideTotalDose");
    QTest::addColumn<double>("activityConcentrationValue");
    QTest::addColumn<int>("patientWeight");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("valid values") << true << 34.54 << 7373.22 << 32.987 << 78 << 32.987 / (7373.22 * 34.54) * (78 * 1000);
}

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(bool, canComputeDecayFactor);
    QFETCH(double, decayFactor);
    QFETCH(double, radionuclideTotalDose);
    QFETCH(double, activityConcentrationValue);
    QFETCH(int, patientWeight);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMRadionuclideTotalDose, radionuclideTotalDose);
    tagReader.addTag(DICOMPatientWeight, patientWeight);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideTotalDoseAttribute = new DICOMValueAttribute();
    radionuclideTotalDoseAttribute->setTag(DICOMRadionuclideTotalDose);
    radionuclideTotalDoseAttribute->setValue(radionuclideTotalDose);
    item->addAttribute(radionuclideTotalDoseAttribute);

    TestingDecayCorrectionFactorFormulaCalculator *decayCorrectionCalculator = new TestingDecayCorrectionFactorFormulaCalculator();
    decayCorrectionCalculator->m_canCompute = canComputeDecayFactor;
    decayCorrectionCalculator->m_computeValue = decayFactor;

    TestingStandardizedUptakeValueBodyWeightFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);
    formulaCalculator.setActivityConcentrationInBqMl(activityConcentrationValue);
    formulaCalculator.setDecayCorrectionCalculator(decayCorrectionCalculator);

    double computedValue = formulaCalculator.compute();

    if (MathTools::isNaN(computedValue))
    {
        QVERIFY2(MathTools::isNaN(computedValue) == MathTools::isNaN(expectedResult), "No both values are NaN");
    }
    else
    {
        QCOMPARE(computedValue, expectedResult);
    }

    delete sequence;
}

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::canCompute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<bool>("canComputeDecayFactor");
    QTest::addColumn<double>("decayFactor");
    QTest::addColumn<double>("radionuclideTotalDose");
    QTest::addColumn<double>("activityConcentrationValue");
    QTest::addColumn<int>("patientWeight");
    QTest::addColumn<bool>("expectedResult");

    double notUsedDouble;

    QTest::newRow("valid values") <<  true << 34.54 << 7373.22 << notUsedDouble << 89 << true;
    QTest::newRow("valid values, negative injectedDoseInBq") <<  true << 34.54 << -348.43 << notUsedDouble << 23 << true;
    QTest::newRow("valid values, negative decayFactor") <<  true << -0.54 << 87.3 << notUsedDouble << 32 << true;
    QTest::newRow("cannot compute decay factor") <<  false << notUsedDouble << 7373.22 << notUsedDouble << 71 << false;
    QTest::newRow("invalid injectedDoseInBq") <<  true << 34.54 << 0.0 << notUsedDouble << 34 << false;
    QTest::newRow("invalid decayFactor") <<  true << 0.0 << 32.11 << notUsedDouble << 54 << false;
    QTest::newRow("invalid patientWeight") <<  true << 2.0 << 32.11 << notUsedDouble << 0 << false;
    QTest::newRow("all invalid values") <<  true << 0.0 << 0.0 << notUsedDouble << -30 << false;
}

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(bool, canComputeDecayFactor);
    QFETCH(double, decayFactor);
    QFETCH(double, radionuclideTotalDose);
    QFETCH(double, activityConcentrationValue);
    QFETCH(int, patientWeight);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMRadionuclideTotalDose, radionuclideTotalDose);
    tagReader.addTag(DICOMPatientWeight, patientWeight);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideTotalDoseAttribute = new DICOMValueAttribute();
    radionuclideTotalDoseAttribute->setTag(DICOMRadionuclideTotalDose);
    radionuclideTotalDoseAttribute->setValue(radionuclideTotalDose);
    item->addAttribute(radionuclideTotalDoseAttribute);

    TestingDecayCorrectionFactorFormulaCalculator *decayCorrectionCalculator = new TestingDecayCorrectionFactorFormulaCalculator();
    decayCorrectionCalculator->m_canCompute = canComputeDecayFactor;
    decayCorrectionCalculator->m_computeValue = decayFactor;

    TestingStandardizedUptakeValueBodyWeightFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);
    formulaCalculator.setActivityConcentrationInBqMl(activityConcentrationValue);
    formulaCalculator.setDecayCorrectionCalculator(decayCorrectionCalculator);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}


DECLARE_TEST(test_StandardizedUptakeValueBodyWeightFormulaCalculator)

#include "test_standardizeduptakevaluebodyweightformulacalculator.moc"
