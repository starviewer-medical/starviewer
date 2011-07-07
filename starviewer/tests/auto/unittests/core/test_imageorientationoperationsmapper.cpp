#include "autotest.h"

#include "imageorientationoperationsmapper.h"

using namespace udg;

class test_ImageOrientationOperationsMapper : public QObject {
Q_OBJECT

private slots:
    void getNumberOfClockwiseTurnsToApply_ShouldReturnExpectedValue_data();
    void getNumberOfClockwiseTurnsToApply_ShouldReturnExpectedValue();

    void requiresHorizontalFlip_ShouldReturnExpectedValue_data();
    void requiresHorizontalFlip_ShouldReturnExpectedValue();

private:
    void setupClockWiseAndFlipData();
};

void test_ImageOrientationOperationsMapper::getNumberOfClockwiseTurnsToApply_ShouldReturnExpectedValue_data()
{
    this->setupClockWiseAndFlipData();
}

void test_ImageOrientationOperationsMapper::getNumberOfClockwiseTurnsToApply_ShouldReturnExpectedValue()
{
    QFETCH(QString, initialRowOrientationLabel);
    QFETCH(QString, initialColumnOrientationLabel);
    QFETCH(QString, desiredRowOrientationLabel);
    QFETCH(QString, desiredColumnOrientationLabel);
    QFETCH(int, turns);

    PatientOrientation initialOrientation;
    initialOrientation.setLabels(initialRowOrientationLabel, initialColumnOrientationLabel);
    
    PatientOrientation desiredOrientation;
    desiredOrientation.setLabels(desiredRowOrientationLabel, desiredColumnOrientationLabel);
    
    ImageOrientationOperationsMapper mapper;
    mapper.setInitialOrientation(initialOrientation);
    mapper.setDesiredOrientation(desiredOrientation);

    QCOMPARE(turns, mapper.getNumberOfClockwiseTurnsToApply());
}

void test_ImageOrientationOperationsMapper::requiresHorizontalFlip_ShouldReturnExpectedValue_data()
{
    this->setupClockWiseAndFlipData();
}

void test_ImageOrientationOperationsMapper::requiresHorizontalFlip_ShouldReturnExpectedValue()
{
    QFETCH(QString, initialRowOrientationLabel);
    QFETCH(QString, initialColumnOrientationLabel);
    QFETCH(QString, desiredRowOrientationLabel);
    QFETCH(QString, desiredColumnOrientationLabel);
    QFETCH(bool, hasToFlip);

    PatientOrientation initialOrientation;
    initialOrientation.setLabels(initialRowOrientationLabel, initialColumnOrientationLabel);
    
    PatientOrientation desiredOrientation;
    desiredOrientation.setLabels(desiredRowOrientationLabel, desiredColumnOrientationLabel);
    
    ImageOrientationOperationsMapper mapper;
    mapper.setInitialOrientation(initialOrientation);
    mapper.setDesiredOrientation(desiredOrientation);

    QCOMPARE(hasToFlip, mapper.requiresHorizontalFlip());
}

void test_ImageOrientationOperationsMapper::setupClockWiseAndFlipData()
{
    QTest::addColumn<QString>("initialRowOrientationLabel");
    QTest::addColumn<QString>("initialColumnOrientationLabel");
    QTest::addColumn<QString>("desiredRowOrientationLabel");
    QTest::addColumn<QString>("desiredColumnOrientationLabel");
    QTest::addColumn<int>("turns");
    QTest::addColumn<bool>("hasToFlip");

    // Casos en que no s'ha de fer res
    // La orientació inicial i la desitjada són la mateixa
    QTest::newRow("AF->AF") << "A" << "F" << "A" << "F" << 0 << false;
    QTest::newRow("AH->AH") << "A" << "H" << "A" << "H" << 0 << false;
    QTest::newRow("AR->AR") << "A" << "R" << "A" << "R" << 0 << false;
    QTest::newRow("AL->AL") << "A" << "L" << "A" << "L" << 0 << false;

    QTest::newRow("PF->PF") << "P" << "F" << "P" << "F" << 0 << false;
    QTest::newRow("PH->PH") << "P" << "H" << "P" << "H" << 0 << false;
    QTest::newRow("PR->PR") << "P" << "R" << "P" << "R" << 0 << false;
    QTest::newRow("PL->PL") << "P" << "L" << "P" << "L" << 0 << false;

    QTest::newRow("HA->HA") << "H" << "A" << "H" << "A" << 0 << false;
    QTest::newRow("HP->HP") << "H" << "P" << "H" << "P" << 0 << false;
    QTest::newRow("HR->HR") << "H" << "R" << "H" << "R" << 0 << false;
    QTest::newRow("HL->HL") << "H" << "L" << "H" << "L" << 0 << false;

    QTest::newRow("FA->FA") << "F" << "A" << "F" << "A" << 0 << false;
    QTest::newRow("FP->FP") << "F" << "P" << "F" << "P" << 0 << false;
    QTest::newRow("FR->FR") << "F" << "R" << "F" << "R" << 0 << false;
    QTest::newRow("FL->FL") << "F" << "L" << "F" << "L" << 0 << false;

    QTest::newRow("RA->RA") << "R" << "A" << "R" << "A" << 0 << false;
    QTest::newRow("RP->RP") << "R" << "P" << "R" << "P" << 0 << false;
    QTest::newRow("RF->RF") << "R" << "F" << "R" << "F" << 0 << false;
    QTest::newRow("RH->RH") << "R" << "H" << "R" << "H" << 0 << false;

    QTest::newRow("LA->LA") << "L" << "A" << "L" << "A" << 0 << false;
    QTest::newRow("LP->LP") << "L" << "P" << "L" << "P" << 0 << false;
    QTest::newRow("LF->LF") << "L" << "F" << "L" << "F" << 0 << false;
    QTest::newRow("LH->LH") << "L" << "H" << "L" << "H" << 0 << false;
    
    // Casos en que hem d'aplicar alguna transformació
    // Casos d'orientació sagital
    QTest::newRow("AF->AH") << "A" << "F" << "A" << "H" << 2 << true;
    QTest::newRow("AF->PF") << "A" << "F" << "P" << "F" << 0 << true;
    QTest::newRow("AF->PH") << "A" << "F" << "P" << "H" << 2 << false;
    QTest::newRow("AF->HA") << "A" << "F" << "H" << "A" << 1 << false;
    QTest::newRow("AF->FA") << "A" << "F" << "F" << "A" << 1 << true;
    QTest::newRow("AF->FP") << "A" << "F" << "F" << "P" << 3 << false;
    QTest::newRow("AF->HP") << "A" << "F" << "H" << "P" << 3 << true;

    QTest::newRow("AH->AF") << "A" << "H" << "A" << "F" << 2 << true;
    QTest::newRow("AH->PF") << "A" << "H" << "P" << "F" << 2 << false;
    QTest::newRow("AH->PH") << "A" << "H" << "P" << "H" << 0 << true;
    QTest::newRow("AH->FA") << "A" << "H" << "F" << "A" << 1 << false;
    QTest::newRow("AH->HA") << "A" << "H" << "H" << "A" << 1 << true;
    QTest::newRow("AH->FP") << "A" << "H" << "F" << "P" << 3 << true;
    QTest::newRow("AH->HP") << "A" << "H" << "H" << "P" << 3 << false;

    QTest::newRow("PF->AF") << "P" << "F" << "A" << "F" << 0 << true;
    QTest::newRow("PF->AH") << "P" << "F" << "A" << "H" << 2 << false;
    QTest::newRow("PF->PH") << "P" << "F" << "P" << "H" << 2 << true;
    QTest::newRow("PF->FA") << "P" << "F" << "F" << "A" << 3 << false;
    QTest::newRow("PF->HA") << "P" << "F" << "H" << "A" << 3 << true;
    QTest::newRow("PF->FP") << "P" << "F" << "F" << "P" << 1 << true;
    QTest::newRow("PF->HP") << "P" << "F" << "H" << "P" << 1 << false;

    QTest::newRow("PH->AF") << "P" << "H" << "A" << "F" << 2 << false;
    QTest::newRow("PH->AH") << "P" << "H" << "A" << "H" << 0 << true;
    QTest::newRow("PH->PF") << "P" << "H" << "P" << "F" << 2 << true;
    QTest::newRow("PH->FA") << "P" << "H" << "F" << "A" << 3 << true;
    QTest::newRow("PH->HA") << "P" << "H" << "H" << "A" << 3 << false;
    QTest::newRow("PH->FP") << "P" << "H" << "F" << "P" << 1 << false;
    QTest::newRow("PH->HP") << "P" << "H" << "H" << "P" << 1 << true;

    QTest::newRow("FA->AF") << "F" << "A" << "A" << "F" << 1 << true;
    QTest::newRow("FA->AH") << "F" << "A" << "A" << "H" << 3 << false;
    QTest::newRow("FA->PF") << "F" << "A" << "P" << "F" << 1 << false;
    QTest::newRow("FA->PH") << "F" << "A" << "P" << "H" << 3 << true;
    QTest::newRow("FA->HA") << "F" << "A" << "H" << "A" << 0 << true;
    QTest::newRow("FA->FP") << "F" << "A" << "F" << "P" << 2 << true;
    QTest::newRow("FA->HP") << "F" << "A" << "H" << "P" << 2 << false;

    QTest::newRow("FP->AF") << "F" << "P" << "A" << "F" << 1 << false;
    QTest::newRow("FP->AH") << "F" << "P" << "A" << "H" << 3 << true;
    QTest::newRow("FP->PF") << "F" << "P" << "P" << "F" << 1 << true;
    QTest::newRow("FP->PH") << "F" << "P" << "P" << "H" << 3 << false;
    QTest::newRow("FP->FA") << "F" << "P" << "F" << "A" << 2 << true;
    QTest::newRow("FP->HA") << "F" << "P" << "H" << "A" << 2 << false;
    QTest::newRow("FP->HP") << "F" << "P" << "H" << "P" << 0 << true;

    QTest::newRow("HA->AF") << "H" << "A" << "A" << "F" << 3 << false;
    QTest::newRow("HA->AH") << "H" << "A" << "A" << "H" << 1 << true;
    QTest::newRow("HA->PF") << "H" << "A" << "P" << "F" << 3 << true;
    QTest::newRow("HA->PH") << "H" << "A" << "P" << "H" << 1 << false;
    QTest::newRow("HA->FA") << "H" << "A" << "F" << "A" << 0 << true;
    QTest::newRow("HA->FP") << "H" << "A" << "F" << "P" << 2 << false;
    QTest::newRow("HA->HP") << "H" << "A" << "H" << "P" << 2 << true;

    QTest::newRow("HP->AF") << "H" << "P" << "A" << "F" << 1 << true;
    QTest::newRow("HP->AH") << "H" << "P" << "A" << "H" << 1 << false;
    QTest::newRow("HP->PF") << "H" << "P" << "P" << "F" << 3 << false;
    QTest::newRow("HP->PH") << "H" << "P" << "P" << "H" << 3 << true;
    QTest::newRow("HP->FA") << "H" << "P" << "F" << "A" << 2 << false;
    QTest::newRow("HP->HA") << "H" << "P" << "H" << "A" << 2 << true;
    QTest::newRow("HP->FP") << "H" << "P" << "F" << "P" << 0 << true;

    // Casos d'orientació axial
    QTest::newRow("AR->AL") << "A" << "R" << "A" << "L" << 2 << true;
    QTest::newRow("AR->PR") << "A" << "R" << "P" << "R" << 0 << true;
    QTest::newRow("AR->PL") << "A" << "R" << "P" << "L" << 2 << false;
    QTest::newRow("AR->RA") << "A" << "R" << "R" << "A" << 1 << true;
    QTest::newRow("AR->LA") << "A" << "R" << "L" << "A" << 1 << false;
    QTest::newRow("AR->RP") << "A" << "R" << "R" << "P" << 3 << false;
    QTest::newRow("AR->LP") << "A" << "R" << "L" << "P" << 3 << true;

    QTest::newRow("AL->AR") << "A" << "L" << "A" << "R" << 2 << true;
    QTest::newRow("AL->PR") << "A" << "L" << "P" << "R" << 2 << false;
    QTest::newRow("AL->PL") << "A" << "L" << "P" << "L" << 0 << true;
    QTest::newRow("AL->RA") << "A" << "L" << "R" << "A" << 1 << false;
    QTest::newRow("AL->LA") << "A" << "L" << "L" << "A" << 1 << true;
    QTest::newRow("AL->RP") << "A" << "L" << "R" << "P" << 3 << true;
    QTest::newRow("AL->LP") << "A" << "L" << "L" << "P" << 3 << false;

    QTest::newRow("PR->AR") << "P" << "R" << "A" << "R" << 0 << true;
    QTest::newRow("PR->AL") << "P" << "R" << "A" << "L" << 2 << false;
    QTest::newRow("PR->PL") << "P" << "R" << "P" << "L" << 2 << true;
    QTest::newRow("PR->RA") << "P" << "R" << "R" << "A" << 3 << false;
    QTest::newRow("PR->LA") << "P" << "R" << "L" << "A" << 3 << true;
    QTest::newRow("PR->RP") << "P" << "R" << "R" << "P" << 1 << true;
    QTest::newRow("PR->LP") << "P" << "R" << "L" << "P" << 1 << false;

    QTest::newRow("PL->AR") << "P" << "L" << "A" << "R" << 2 << false;
    QTest::newRow("PL->AL") << "P" << "L" << "A" << "L" << 0 << true;
    QTest::newRow("PL->PR") << "P" << "L" << "P" << "R" << 2 << true;
    QTest::newRow("PL->RA") << "P" << "L" << "R" << "A" << 3 << true;
    QTest::newRow("PL->LA") << "P" << "L" << "L" << "A" << 3 << false;
    QTest::newRow("PL->RP") << "P" << "L" << "R" << "P" << 1 << false;
    QTest::newRow("PL->LP") << "P" << "L" << "L" << "P" << 1 << true;

    QTest::newRow("RA->AR") << "R" << "A" << "A" << "R" << 1 << true;
    QTest::newRow("RA->AL") << "R" << "A" << "A" << "L" << 3 << false;
    QTest::newRow("RA->PR") << "R" << "A" << "P" << "R" << 1 << false;
    QTest::newRow("RA->PL") << "R" << "A" << "P" << "L" << 3 << true;
    QTest::newRow("RA->LA") << "R" << "A" << "L" << "A" << 0 << true;
    QTest::newRow("RA->RP") << "R" << "A" << "R" << "P" << 2 << true;
    QTest::newRow("RA->LP") << "R" << "A" << "L" << "P" << 2 << false;

    QTest::newRow("RP->AR") << "R" << "P" << "A" << "R" << 1 << false;
    QTest::newRow("RP->AL") << "R" << "P" << "A" << "L" << 3 << true;
    QTest::newRow("RP->PR") << "R" << "P" << "P" << "R" << 1 << true;
    QTest::newRow("RP->PL") << "R" << "P" << "P" << "L" << 3 << false;
    QTest::newRow("RP->RA") << "R" << "P" << "R" << "A" << 2 << true;
    QTest::newRow("RP->LA") << "R" << "P" << "L" << "A" << 2 << false;
    QTest::newRow("RP->LP") << "R" << "P" << "L" << "P" << 0 << true;

    QTest::newRow("LA->AR") << "L" << "A" << "A" << "R" << 3 << false;
    QTest::newRow("LA->AL") << "L" << "A" << "A" << "L" << 1 << true;
    QTest::newRow("LA->PR") << "L" << "A" << "P" << "R" << 3 << true;
    QTest::newRow("LA->PL") << "L" << "A" << "P" << "L" << 1 << false;
    QTest::newRow("LA->RA") << "L" << "A" << "R" << "A" << 0 << true;
    QTest::newRow("LA->RP") << "L" << "A" << "R" << "P" << 2 << false;
    QTest::newRow("LA->LP") << "L" << "A" << "L" << "P" << 2 << true;

    QTest::newRow("LP->AR") << "L" << "P" << "A" << "R" << 3 << true;
    QTest::newRow("LP->AL") << "L" << "P" << "A" << "L" << 1 << false;
    QTest::newRow("LP->PR") << "L" << "P" << "P" << "R" << 3 << false;
    QTest::newRow("LP->PL") << "L" << "P" << "P" << "L" << 1 << true;
    QTest::newRow("LP->RA") << "L" << "P" << "R" << "A" << 2 << false;
    QTest::newRow("LP->LA") << "L" << "P" << "L" << "A" << 2 << true;
    QTest::newRow("LP->RP") << "L" << "P" << "R" << "P" << 0 << true;
}

DECLARE_TEST(test_ImageOrientationOperationsMapper)

#include "test_imageorientationoperationsmapper.moc"
