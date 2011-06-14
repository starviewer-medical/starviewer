#include "autotest.h"
#include "patientorientation.h"

using namespace udg;

class test_PatientOrientation : public QObject {
Q_OBJECT

private slots:
    void getOppositeOrientationLabel_ShouldReturnExpectedValues_data();
    void getOppositeOrientationLabel_ShouldReturnExpectedValues();
};

void test_PatientOrientation::getOppositeOrientationLabel_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("inputLabel");
    QTest::addColumn<QString>("resultingLabel");

    QTest::newRow("Empty string")
        << "" << "";
    
    QTest::newRow("R->L")
        << "R" << "L";

    QTest::newRow("L->R")
        << "L" << "R";

    QTest::newRow("A->P")
        << "A" << "P";
    
    QTest::newRow("P->A")
        << "P" << "A";
    
    QTest::newRow("H->F")
        << "H" << "F";
    
    QTest::newRow("F->H")
        << "F" << "H";
    
    QTest::newRow("I->?")
        << "I" << "?";

    QTest::newRow("S->?")
        << "S" << "?";

    QTest::newRow("\\->?")
        << "\\" << "?";

    // Cadenes de dos valors contemplats
    // Rx
    QTest::newRow("RA->LP")
        << "RA" << "LP";
    
    QTest::newRow("RP->LA")
        << "RP" << "LA";
    
    QTest::newRow("RH->LF")
        << "RH" << "LF";

    QTest::newRow("RF->LH")
        << "RF" << "LH";
    
    // Lx
    QTest::newRow("LP->RA")
        << "LP" << "RA";
    
    QTest::newRow("LA->RP")
        << "LA" << "RP";

    QTest::newRow("LF->RH")
        << "LF" << "RH";

    QTest::newRow("LH->RF")
        << "LH" << "RF";

    // Px
    QTest::newRow("PH->AF")
        << "PH" << "AF";

    QTest::newRow("PF->AH")
        << "PF" << "AH";

    QTest::newRow("PR->AL")
        << "PR" << "AL";

    QTest::newRow("PL->AR")
        << "PL" << "AR";
    
    // Ax
    QTest::newRow("AH->PF")
        << "AH" << "PF";

    QTest::newRow("AF->PH")
        << "AF" << "PH";

    QTest::newRow("AR->PL")
        << "AR" << "PL";

    QTest::newRow("AL->PR")
        << "AL" << "PR";
    
    // Hx
    QTest::newRow("HR->FL")
        << "HR" << "FL";

    QTest::newRow("HL->FR")
        << "HL" << "FR";

    QTest::newRow("HA->FP")
        << "HA" << "FP";

    QTest::newRow("HP->FA")
        << "HP" << "FA";
    
    // Fx
    QTest::newRow("FR->HL")
        << "FR" << "HL";

    QTest::newRow("FL->HR")
        << "FL" << "HR";

    QTest::newRow("FA->HP")
        << "FA" << "HP";

    QTest::newRow("FP->HA")
        << "FP" << "HA";

    // Cadenes de 3 valors contemplats
    // Rxx
    QTest::newRow("RAH->LPF")
        << "RAH" << "LPF";
    QTest::newRow("RAF->LPH")
        << "RAF" << "LPH";
    
    QTest::newRow("RPH->LAF")
        << "RPH" << "LAF";
    QTest::newRow("RPF->LAH")
        << "RPF" << "LAH";
    
    QTest::newRow("RHA->LFP")
        << "RHA" << "LFP";
    QTest::newRow("RHP->LFA")
        << "RHP" << "LFA";

    QTest::newRow("RFA->LHP")
        << "RFA" << "LHP";
    QTest::newRow("RFP->LHA")
        << "RFP" << "LHA";
    
    // Lxx
    QTest::newRow("LPH->RAF")
        << "LPH" << "RAF";
    QTest::newRow("LPF->RAH")
        << "LPF" << "RAH";

    QTest::newRow("LAH->RPF")
        << "LAH" << "RPF";
    QTest::newRow("LAF->RPH")
        << "LAF" << "RPH";

    QTest::newRow("LFA->RHP")
        << "LFA" << "RHP";
    QTest::newRow("LFP->RHA")
        << "LFP" << "RHA";

    QTest::newRow("LHA->RFP")
        << "LHA" << "RFP";
    QTest::newRow("LHP->RFA")
        << "LHP" << "RFA";
    
    // Pxx
    QTest::newRow("PHR->AFL")
        << "PHR" << "AFL";
    QTest::newRow("PHL->AFR")
        << "PHL" << "AFR";

    QTest::newRow("PFR->AHL")
        << "PFR" << "AHL";
    QTest::newRow("PFL->AHR")
        << "PFR" << "AHL";

    QTest::newRow("PRH->ALF")
        << "PRH" << "ALF";
    QTest::newRow("PRF->ALH")
        << "PRF" << "ALH";

    QTest::newRow("PLH->ARF")
        << "PLH" << "ARF";
    QTest::newRow("PLF->ARH")
        << "PLF" << "ARH";
    
    // Axx
    QTest::newRow("AHR->PFL")
        << "AHR" << "PFL";
    QTest::newRow("AHL->PFR")
        << "AHL" << "PFR";

    QTest::newRow("AFR->PHL")
        << "AFR" << "PHL";
    QTest::newRow("AFL->PHR")
        << "AFL" << "PHR";

    QTest::newRow("ARH->PLF")
        << "ARH" << "PLF";
    QTest::newRow("ARF->PLH")
        << "ARF" << "PLH";
    
    QTest::newRow("ALH->PRF")
        << "ALH" << "PRF";
    QTest::newRow("ALF->PRH")
        << "ALF" << "PRH";
    
    // Hxx
    QTest::newRow("HRA->FLP")
        << "HRA" << "FLP";
    QTest::newRow("HRP->FLA")
        << "HRP" << "FLA";

    QTest::newRow("HLA->FRP")
        << "HLA" << "FRP";
    QTest::newRow("HLP->FRA")
        << "HLP" << "FRA";
    
    QTest::newRow("HAR->FPL")
        << "HAR" << "FPL";
    QTest::newRow("HAL->FPR")
        << "HAL" << "FPR";
    
    QTest::newRow("HPR->FAL")
        << "HPR" << "FAL";
    QTest::newRow("HPL->FAR")
        << "HPL" << "FAR";

    // Fxx
    QTest::newRow("FRA->HLP")
        << "FRA" << "HLP";
    QTest::newRow("FRP->HLA")
        << "FRP" << "HLA";
    
    QTest::newRow("FLA->HRP")
        << "FLA" << "HRP";
    QTest::newRow("FLP->HRA")
        << "FLP" << "HRA";
    
    QTest::newRow("FAR->HPL")
        << "FAR" << "HPL";
    QTest::newRow("FAL->HPR")
        << "FAL" << "HPR";
    
    QTest::newRow("FPR->HAL")
        << "FPR" << "HAL";
    QTest::newRow("FPL->HAR")
        << "FPL" << "HAR";
    
    // Cadenes amb altres valors
    QTest::newRow("String with non-convertable values")
        << "abcdefghijklmnopqrstuvwxyz0123456789" 
        << "????????????????????????????????????";
}

void test_PatientOrientation::getOppositeOrientationLabel_ShouldReturnExpectedValues()
{
    QFETCH(QString, inputLabel);
    QFETCH(QString, resultingLabel);

    QCOMPARE(PatientOrientation::getOppositeOrientationLabel(inputLabel), resultingLabel);
}

DECLARE_TEST(test_PatientOrientation)

#include "test_patientorientation.moc"
