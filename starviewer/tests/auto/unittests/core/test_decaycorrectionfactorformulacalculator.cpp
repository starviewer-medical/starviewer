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
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data();
    void compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource();

    void canCompute_ShouldReturnExpectedResult_data();
    void canCompute_ShouldReturnExpectedResult();

private:
    void prepareComputeData();
};

void test_DecayCorrectionFactorFormulaCalculator::prepareComputeData()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<int>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<double>("expectedResult");

    QString notUsedString;

    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << 23 << qPow(2, 0/(double)23);
    QTest::newRow("decayCorrection START") <<  "START" << "20131212" << "120200" << "120000" << 156 << qPow(2, -120/(double)156);
    double zeroDouble = 0.0;

    QTest::newRow("decayCorrection ADMIN, radionuclideHalfLifeInSeconds is 0") <<  "ADMIN" << notUsedString << notUsedString << notUsedString << 0 << qPow(2, 0/zeroDouble);
    QTest::newRow("decayCorrection START, invalid SeriesDate") <<  "START" << "20131312" << "121000" << "120000" << 156 << qPow(2, -(-1)/(double)156);
    QTest::newRow("decayCorrection START, invalid SeriesTime") <<  "START" << "20131212" << "no valid" << "120000" << 156 << qPow(2, -(-1)/(double)156);
    QTest::newRow("decayCorrection START, invalid radiopharmaceuticalStartTime") <<  "START" << "20131212" << "121000" << "120090" << 156 << qPow(2, -(-1)/(double)156);
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data()
{
    prepareComputeData();
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(int, radionuclideHalfLifeInSeconds);
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

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<QString>("decayCorrection");
    QTest::addColumn<QString>("seriesDate");
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<int>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<bool>("expectedResult");

    QString notUsedString;
    int notUsedInt = 1;

    QTest::newRow("decayCorrection NONE") <<  "NONE" << notUsedString << notUsedString << notUsedString << notUsedInt << false;
    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << "20100101" << notUsedString << notUsedString << 1 << true;
    QTest::newRow("decayCorrection START") <<  "START" << "20100101" << "221200" << "201200" << 2 << true;
    QTest::newRow("invalid decayCorrection") <<  "no none, no start, no admin" << notUsedString << notUsedString << notUsedString << notUsedInt << false;
    QTest::newRow("invalid time lapse") <<  "START" << "20100101" << "000000" << "000001" << 1 << false;
    QTest::newRow("invalid radionuclide half life") <<  "START" << "20100101" << "000001" << "000000" << -1 << false;
    QTest::newRow("valid values") <<  "START" << "20100101" << "000005" << "000000" << 1 << true;
}

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesDate);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(int, radionuclideHalfLifeInSeconds);
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


DECLARE_TEST(test_DecayCorrectionFactorFormulaCalculator)

#include "test_decaycorrectionfactorformulacalculator.moc"
