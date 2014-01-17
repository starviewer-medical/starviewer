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
    QTest::addColumn<QString>("units");
    QTest::addColumn<double>("philipsActivityConcentrationScaleFactor");
    QTest::addColumn<double>("expectedResult");

    double notUsedDouble = 0.39383;

    QTest::newRow("valid values") << true << 34.54 << 7373.22 << 32.987 << 78 << "not CNTS" << notUsedDouble << 32.987 / (7373.22 * 34.54) * (78 * 1000);
    QTest::newRow("valid values") << true << 34.54 << 7373.22 << 12345.0 << 78 << "CNTS" << 0.0 << 12345.0 / (7373.22 * 34.54) * (78 * 1000);
    QTest::newRow("valid values") << true << 34.54 << 7373.22 << 12345.0 << 78 << "CNTS" << 1.232 << (12345.0 * 1.232) / (7373.22 * 34.54) * (78 * 1000);
}

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(bool, canComputeDecayFactor);
    QFETCH(double, decayFactor);
    QFETCH(double, radionuclideTotalDose);
    QFETCH(double, activityConcentrationValue);
    QFETCH(int, patientWeight);
    QFETCH(QString, units);
    QFETCH(double, philipsActivityConcentrationScaleFactor);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMRadionuclideTotalDose, radionuclideTotalDose);
    tagReader.addTag(DICOMPatientWeight, patientWeight);
    tagReader.addTag(DICOMUnits, units);
    if (philipsActivityConcentrationScaleFactor != 0.0)
    {
        tagReader.addTag(DICOMTag(0x7053, 0x1009), philipsActivityConcentrationScaleFactor);
    }

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
    formulaCalculator.setActivityConcentrationInImageUnits(activityConcentrationValue);
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
    QTest::addColumn<QString>("pixelUnits");
    QTest::addColumn<double>("philipsActivityConcentrationScaleFactor");
    QTest::addColumn<bool>("expectedResult");

    double notUsedDouble = 1.2928;

    QTest::newRow("valid values") <<  true << 34.54 << 7373.22 << notUsedDouble << 89 << "BQML" << notUsedDouble << true;
    QTest::newRow("units in counts but present philips scale factor") <<  true << 34.54 << 7373.22 << notUsedDouble << 89 << "CNTS" << 0.393 << true;
    QTest::newRow("valid values, negative injectedDoseInBq") <<  true << 34.54 << -348.43 << notUsedDouble << 23 << "BQML" << notUsedDouble << true;
    QTest::newRow("valid values, negative decayFactor") <<  true << -0.54 << 87.3 << notUsedDouble << 32 << "BQML" << notUsedDouble << true;
    QTest::newRow("cannot compute decay factor") <<  false << notUsedDouble << 7373.22 << notUsedDouble << 71 << "BQML" << notUsedDouble << false;
    QTest::newRow("invalid injectedDoseInBq") <<  true << 34.54 << 0.0 << notUsedDouble << 34 << "BQML" << notUsedDouble << false;
    QTest::newRow("invalid decayFactor") <<  true << 0.0 << 32.11 << notUsedDouble << 54 << "BQML" << notUsedDouble << false;
    QTest::newRow("invalid patientWeight") <<  true << 2.0 << 32.11 << notUsedDouble << 0 << "BQML" << notUsedDouble << false;
    QTest::newRow("invalid units") <<  true << 34.54 << 7373.22 << notUsedDouble << 89 << "CNTS" << 0.0 << false;
    QTest::newRow("all invalid values") <<  true << 0.0 << 0.0 << notUsedDouble << -30 << "diferent de BQML" << notUsedDouble << false;
}

void test_StandardizedUptakeValueBodyWeightFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(bool, canComputeDecayFactor);
    QFETCH(double, decayFactor);
    QFETCH(double, radionuclideTotalDose);
    QFETCH(double, activityConcentrationValue);
    QFETCH(int, patientWeight);
    QFETCH(QString, pixelUnits);
    QFETCH(double, philipsActivityConcentrationScaleFactor);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMRadionuclideTotalDose, radionuclideTotalDose);
    tagReader.addTag(DICOMPatientWeight, patientWeight);
    tagReader.addTag(DICOMUnits, pixelUnits);

    if (philipsActivityConcentrationScaleFactor != 0.0)
    {
        tagReader.addTag(DICOMTag(0x7053, 0x1009), philipsActivityConcentrationScaleFactor);
    }

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
    formulaCalculator.setActivityConcentrationInImageUnits(activityConcentrationValue);
    formulaCalculator.setDecayCorrectionCalculator(decayCorrectionCalculator);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}


DECLARE_TEST(test_StandardizedUptakeValueBodyWeightFormulaCalculator)

#include "test_standardizeduptakevaluebodyweightformulacalculator.moc"
