#include "autotest.h"
#include "decaycorrectionfactorformulacalculator.h"
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

class test_DecayCorrectionFactorFormulaCalculator : public QObject {
    Q_OBJECT

private slots:
    void compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTimeUsingTagReaderAsDataSource_data();
    void compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTimeUsingTagReaderAsDataSource();

    void compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTimeUsingTagReaderAsDataSource_data();
    void compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTimeUsingTagReaderAsDataSource();

    void canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTime_data();
    void canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTime();

    void canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTime_data();
    void canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTime();
};

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTimeUsingTagReaderAsDataSource_data()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartDateTime");
    QTest::addColumn<double>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<double>("expectedResult");

    QString notUsedString;
    double zeroDouble = 0.0;

    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << 23.2 << qPow(2, 0 / 23.2);
    QTest::newRow("decayCorrection START") <<  "START" << "20131212" << "120000.400" << "20131210120001.300" << 156.0 << qPow(2, -172799.1 / 156.0);
    QTest::newRow("decayCorrection ADMIN, radionuclideHalfLifeInSeconds is 0") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << zeroDouble << qPow(2, 0/zeroDouble);
    QTest::newRow("decayCorrection START, invalid SeriesDate") <<  "START" << "20131312" << "121000" << "20131211120000" << 156.004 << qPow(2, -(-1) / 156.004);
    QTest::newRow("decayCorrection START, invalid SeriesTime") <<  "START" << "20131212" << "no valid" << "20131211120000" << 156.005 << qPow(2, -(-1) / 156.005);
    QTest::newRow("decayCorrection START, invalid radiopharmaceuticalStartDateTime") <<  "START" << "20131212" << "121000" << "120000" << 156.293 << qPow(2, -(-1) / 156.293);
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTimeUsingTagReaderAsDataSource()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartDateTime);
    QFETCH(double, radionuclideHalfLifeInSeconds);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
    tagReader.addTag(DICOMSeriesDate, seriesDate);
    tagReader.addTag(DICOMSeriesTime, seriesTime);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideHalfLifeAttribute = new DICOMValueAttribute();
    radionuclideHalfLifeAttribute->setTag(DICOMRadionuclideHalfLife);
    radionuclideHalfLifeAttribute->setValue(radionuclideHalfLifeInSeconds);
    item->addAttribute(radionuclideHalfLifeAttribute);
    DICOMValueAttribute *radiopharmaceuticalStartTimeAttribute = new DICOMValueAttribute();
    radiopharmaceuticalStartTimeAttribute->setTag(DICOMRadiopharmaceuticalStartDateTime);
    radiopharmaceuticalStartTimeAttribute->setValue(radiopharmaceuticalStartDateTime);
    item->addAttribute(radiopharmaceuticalStartTimeAttribute);

    DecayCorrectionFactorFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

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

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTimeUsingTagReaderAsDataSource_data()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<double>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<double>("expectedResult");

    QString notUsedString;
    double zeroDouble = 0.0;

    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << 23.0 << qPow(2, 0 / 23.0);
    QTest::newRow("decayCorrection START") <<  "START" << "20131212" << "120200" << "120000.100" << 156.092 << qPow(2, -119.9 / 156.092);
    QTest::newRow("decayCorrection ADMIN, radionuclideHalfLifeInSeconds is 0") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << zeroDouble << qPow(2, 0 / zeroDouble);
    QTest::newRow("decayCorrection START, invalid SeriesDate") <<  "START" << "20131312" << "121000" << "120000" << 156.34 << qPow(2, -(-1) / 156.34);
    QTest::newRow("decayCorrection START, invalid SeriesTime") <<  "START" << "20131212" << "no valid" << "120000" << 156.023 << qPow(2, -(-1) / 156.023);
    QTest::newRow("decayCorrection START, invalid radiopharmaceuticalStartTime") <<  "START" << "20131212" << "121000" << "120090" << 156.998 << qPow(2, -(-1) / 156.998);
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTimeUsingTagReaderAsDataSource()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(double, radionuclideHalfLifeInSeconds);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
    tagReader.addTag(DICOMSeriesDate, seriesDate);
    tagReader.addTag(DICOMSeriesTime, seriesTime);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideHalfLifeAttribute = new DICOMValueAttribute();
    radionuclideHalfLifeAttribute->setTag(DICOMRadionuclideHalfLife);
    radionuclideHalfLifeAttribute->setValue(radionuclideHalfLifeInSeconds);
    item->addAttribute(radionuclideHalfLifeAttribute);
    DICOMValueAttribute *radiopharmaceuticalStartTimeAttribute = new DICOMValueAttribute();
    radiopharmaceuticalStartTimeAttribute->setTag(DICOMRadiopharmaceuticalStartTime);
    radiopharmaceuticalStartTimeAttribute->setValue(radiopharmaceuticalStartTime);
    item->addAttribute(radiopharmaceuticalStartTimeAttribute);

    DecayCorrectionFactorFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

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

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTime_data()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<double>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<bool>("expectedResult");

    QString notUsedString;
    double notUsedDouble = 1.0;

    QTest::newRow("decayCorrection NONE") <<  "NONE" << notUsedString << notUsedString << notUsedString << notUsedDouble << false;
    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << "20100101" << notUsedString << notUsedString << 1.3 << true;
    QTest::newRow("decayCorrection START") <<  "START" << "20100101" << "221200" << "201200" << 2.1 << true;
    QTest::newRow("invalid decayCorrection") <<  "no none, no start, no admin" << notUsedString << notUsedString << notUsedString << notUsedDouble << false;
    QTest::newRow("invalid time lapse") <<  "START" << "20100101" << "000000" << "000001" << 1.0 << false;
    QTest::newRow("invalid radionuclide half life") <<  "START" << "20100101" << "000001" << "000000" << -1.9 << false;
    QTest::newRow("valid values") <<  "START" << "20100101" << "000005" << "000000" << 1.003 << true;
}

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartTime()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(double, radionuclideHalfLifeInSeconds);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
    tagReader.addTag(DICOMSeriesDate, seriesDate);
    tagReader.addTag(DICOMSeriesTime, seriesTime);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideHalfLifeAttribute = new DICOMValueAttribute();
    radionuclideHalfLifeAttribute->setTag(DICOMRadionuclideHalfLife);
    radionuclideHalfLifeAttribute->setValue(radionuclideHalfLifeInSeconds);
    item->addAttribute(radionuclideHalfLifeAttribute);
    DICOMValueAttribute *radiopharmaceuticalStartTimeAttribute = new DICOMValueAttribute();
    radiopharmaceuticalStartTimeAttribute->setTag(DICOMRadiopharmaceuticalStartTime);
    radiopharmaceuticalStartTimeAttribute->setValue(radiopharmaceuticalStartTime);
    item->addAttribute(radiopharmaceuticalStartTimeAttribute);

    DecayCorrectionFactorFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTime_data()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartDateTime");
    QTest::addColumn<double>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<bool>("expectedResult");

    QString notUsedString;
    double notUsedDouble = 1.029;

    QTest::newRow("decayCorrection NONE") <<  "NONE" << notUsedString << notUsedString << notUsedString << notUsedDouble << false;
    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << "20100101" << notUsedString << notUsedString << 1.043 << true;
    QTest::newRow("decayCorrection START") <<  "START" << "20100101" << "221200" << "20100101201200" << 2.000 << true;
    QTest::newRow("invalid decayCorrection") <<  "no none, no start, no admin" << notUsedString << notUsedString << notUsedString << notUsedDouble << false;
    QTest::newRow("invalid radiopharmaceuticalStartDateTime") <<  "START" << "20100101" << "000000" << "" << 1.83 << false;
    QTest::newRow("invalid time lapse") <<  "START" << "20100101" << "000000" << "20100102000001" << 1.0 << false;
    QTest::newRow("invalid radionuclide half life") <<  "START" << "20100101" << "000001" << "20100101000000" << -1.980 << false;
    QTest::newRow("valid values") <<  "START" << "20100101" << "000005" << "20000101000000" << 1.0 << true;
}

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResultGivenRadiopharmaceuticalStartDateTime()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartDateTime);
    QFETCH(double, radionuclideHalfLifeInSeconds);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
    tagReader.addTag(DICOMSeriesDate, seriesDate);
    tagReader.addTag(DICOMSeriesTime, seriesTime);

    DICOMSequenceAttribute *sequence = new DICOMSequenceAttribute();
    sequence->setTag(DICOMRadiopharmaceuticalInformationSequence);
    tagReader.addSequence(sequence);
    DICOMSequenceItem *item = new DICOMSequenceItem();
    sequence->addItem(item);

    DICOMValueAttribute *radionuclideHalfLifeAttribute = new DICOMValueAttribute();
    radionuclideHalfLifeAttribute->setTag(DICOMRadionuclideHalfLife);
    radionuclideHalfLifeAttribute->setValue(radionuclideHalfLifeInSeconds);
    item->addAttribute(radionuclideHalfLifeAttribute);
    DICOMValueAttribute *radiopharmaceuticalStartTimeAttribute = new DICOMValueAttribute();
    radiopharmaceuticalStartTimeAttribute->setTag(DICOMRadiopharmaceuticalStartDateTime);
    radiopharmaceuticalStartTimeAttribute->setValue(radiopharmaceuticalStartDateTime);
    item->addAttribute(radiopharmaceuticalStartTimeAttribute);

    DecayCorrectionFactorFormulaCalculator formulaCalculator;
    formulaCalculator.setDataSource(&tagReader);

    QCOMPARE(formulaCalculator.canCompute(), expectedResult);
}

DECLARE_TEST(test_DecayCorrectionFactorFormulaCalculator)

#include "test_decaycorrectionfactorformulacalculator.moc"
