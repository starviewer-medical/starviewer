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
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<int>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<double>("expectedResult");

    QString notUsedString;
    int notUsedInt = 1;

    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << notUsedString << notUsedString << 23 << qPow(2, 0/(double)23);
    QTest::newRow("decayCorrection START") <<  "START" << "120000" << "120200" << 156 << qPow(2, 120/(double)156);
    double zeroDouble = 0.0;

    QTest::newRow("decayCorrection ADMIN, radionuclideHalfLifeInSeconds is 0") <<  "ADMIN" << notUsedString << notUsedString << 0 << qPow(2, 0/zeroDouble);
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource_data()
{
    prepareComputeData();
}

void test_DecayCorrectionFactorFormulaCalculator::compute_ShouldReturnExpectedResultUsingTagReaderAsDataSource()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(int, radionuclideHalfLifeInSeconds);
    QFETCH(double, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
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
    QTest::addColumn<QString>("seriesTime");
    QTest::addColumn<QString>("radiopharmaceuticalStartTime");
    QTest::addColumn<int>("radionuclideHalfLifeInSeconds");
    QTest::addColumn<bool>("expectedResult");

    QString notUsedString;
    int notUsedInt = 1;

    QTest::newRow("decayCorrection NONE") <<  "NONE" << notUsedString << notUsedString << notUsedInt << false;
    QTest::newRow("decayCorrection ADMIN") <<  "ADMIN" << notUsedString << notUsedString << notUsedInt << true;
    QTest::newRow("decayCorrection START") <<  "START" << notUsedString << notUsedString << notUsedInt << true;
    QTest::newRow("invalid decayCorrection") <<  "no none, no start, no admin" << notUsedString << notUsedString << notUsedInt << false;
    QTest::newRow("invalid time lapse") <<  "START" << "000000" << "000001" << 1 << false;
    QTest::newRow("invalid radionuclide half life") <<  "START" << "000001" << "000000" << -1 << false;
    QTest::newRow("valid values") <<  "START" << "000005" << "000000" << 1 << true;
}

void test_DecayCorrectionFactorFormulaCalculator::canCompute_ShouldReturnExpectedResult()
{
    QFETCH(QString, decayCorrection);
    QFETCH(QString, seriesTime);
    QFETCH(QString, radiopharmaceuticalStartTime);
    QFETCH(int, radionuclideHalfLifeInSeconds);
    QFETCH(bool, expectedResult);

    TestingDICOMTagReader tagReader;
    tagReader.addTag(DICOMDecayCorrection, decayCorrection);
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
