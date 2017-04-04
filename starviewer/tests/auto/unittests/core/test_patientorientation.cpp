#include "autotest.h"
#include "patientorientation.h"

#include <QVector3D>

#include "imageorientation.h"

using namespace udg;

class test_PatientOrientation : public QObject {
Q_OBJECT

private slots:
    void getOppositeOrientationLabel_ShouldReturnExpectedValues_data();
    void getOppositeOrientationLabel_ShouldReturnExpectedValues();

    void setLabels_ShouldReturnTrueAndOrientationStringIsSet_data();
    void setLabels_ShouldReturnTrueAndOrientationStringIsSet();

    void setLabels_2ParametersShouldReturnFalseAndSetEmptyOrientationString_data();
    void setLabels_2ParametersShouldReturnFalseAndSetEmptyOrientationString();

    void setLabels_3ParametersShouldReturnFalseAndSetEmptyOrientationString_data();
    void setLabels_3ParametersShouldReturnFalseAndSetEmptyOrientationString();

    void setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet_data();
    void setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet();

    void setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString_data();
    void setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString();

    void setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation_data();
    void setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation();

    void getRowDirectionLabel_ShouldReturnExpectedValues_data();
    void getRowDirectionLabel_ShouldReturnExpectedValues();

    void getColumnDirectionLabel_ShouldReturnExpectedValues_data();
    void getColumnDirectionLabel_ShouldReturnExpectedValues();

    void getNormalDirectionLabel_ShouldReturnExpectedValues_data();
    void getNormalDirectionLabel_ShouldReturnExpectedValues();
};

void test_PatientOrientation::getOppositeOrientationLabel_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("inputLabel");
    QTest::addColumn<QString>("resultingLabel");

    QTest::newRow("Empty string") << "" << "";
    QTest::newRow("R->L") << "R" << "L";
    QTest::newRow("L->R") << "L" << "R";
    QTest::newRow("A->P") << "A" << "P";
    QTest::newRow("P->A") << "P" << "A";
    QTest::newRow("H->F") << "H" << "F";
    QTest::newRow("F->H") << "F" << "H";
    QTest::newRow("I->?") << "I" << "?";
    QTest::newRow("S->?") << "S" << "?";
    QTest::newRow("\\->?") << "\\" << "?";

    // Cadenes de dos valors contemplats
    // Rx
    QTest::newRow("RA->LP") << "RA" << "LP";
    QTest::newRow("RP->LA") << "RP" << "LA";
    QTest::newRow("RH->LF") << "RH" << "LF";
    QTest::newRow("RF->LH") << "RF" << "LH";
    
    // Lx
    QTest::newRow("LP->RA") << "LP" << "RA";
    QTest::newRow("LA->RP") << "LA" << "RP";
    QTest::newRow("LF->RH") << "LF" << "RH";
    QTest::newRow("LH->RF") << "LH" << "RF";

    // Px
    QTest::newRow("PH->AF") << "PH" << "AF";
    QTest::newRow("PF->AH") << "PF" << "AH";
    QTest::newRow("PR->AL") << "PR" << "AL";
    QTest::newRow("PL->AR") << "PL" << "AR";
    
    // Ax
    QTest::newRow("AH->PF") << "AH" << "PF";
    QTest::newRow("AF->PH") << "AF" << "PH";
    QTest::newRow("AR->PL") << "AR" << "PL";
    QTest::newRow("AL->PR") << "AL" << "PR";
    
    // Hx
    QTest::newRow("HR->FL") << "HR" << "FL";
    QTest::newRow("HL->FR") << "HL" << "FR";
    QTest::newRow("HA->FP") << "HA" << "FP";
    QTest::newRow("HP->FA") << "HP" << "FA";
    
    // Fx
    QTest::newRow("FR->HL") << "FR" << "HL";
    QTest::newRow("FL->HR") << "FL" << "HR";
    QTest::newRow("FA->HP") << "FA" << "HP";
    QTest::newRow("FP->HA") << "FP" << "HA";

    // Cadenes de 3 valors contemplats
    // Rxx
    QTest::newRow("RAH->LPF") << "RAH" << "LPF";
    QTest::newRow("RAF->LPH") << "RAF" << "LPH";
    QTest::newRow("RPH->LAF") << "RPH" << "LAF";
    QTest::newRow("RPF->LAH") << "RPF" << "LAH";
    QTest::newRow("RHA->LFP") << "RHA" << "LFP";
    QTest::newRow("RHP->LFA") << "RHP" << "LFA";
    QTest::newRow("RFA->LHP") << "RFA" << "LHP";
    QTest::newRow("RFP->LHA") << "RFP" << "LHA";
    
    // Lxx
    QTest::newRow("LPH->RAF") << "LPH" << "RAF";
    QTest::newRow("LPF->RAH") << "LPF" << "RAH";
    QTest::newRow("LAH->RPF") << "LAH" << "RPF";
    QTest::newRow("LAF->RPH") << "LAF" << "RPH";
    QTest::newRow("LFA->RHP") << "LFA" << "RHP";
    QTest::newRow("LFP->RHA") << "LFP" << "RHA";
    QTest::newRow("LHA->RFP") << "LHA" << "RFP";
    QTest::newRow("LHP->RFA") << "LHP" << "RFA";
    
    // Pxx
    QTest::newRow("PHR->AFL") << "PHR" << "AFL";
    QTest::newRow("PHL->AFR") << "PHL" << "AFR";
    QTest::newRow("PFR->AHL") << "PFR" << "AHL";
    QTest::newRow("PFL->AHR") << "PFR" << "AHL";
    QTest::newRow("PRH->ALF") << "PRH" << "ALF";
    QTest::newRow("PRF->ALH") << "PRF" << "ALH";
    QTest::newRow("PLH->ARF") << "PLH" << "ARF";
    QTest::newRow("PLF->ARH") << "PLF" << "ARH";
    
    // Axx
    QTest::newRow("AHR->PFL") << "AHR" << "PFL";
    QTest::newRow("AHL->PFR") << "AHL" << "PFR";
    QTest::newRow("AFR->PHL") << "AFR" << "PHL";
    QTest::newRow("AFL->PHR") << "AFL" << "PHR";
    QTest::newRow("ARH->PLF") << "ARH" << "PLF";
    QTest::newRow("ARF->PLH") << "ARF" << "PLH";
    QTest::newRow("ALH->PRF") << "ALH" << "PRF";
    QTest::newRow("ALF->PRH") << "ALF" << "PRH";
    
    // Hxx
    QTest::newRow("HRA->FLP") << "HRA" << "FLP";
    QTest::newRow("HRP->FLA") << "HRP" << "FLA";
    QTest::newRow("HLA->FRP") << "HLA" << "FRP";
    QTest::newRow("HLP->FRA") << "HLP" << "FRA";
    QTest::newRow("HAR->FPL") << "HAR" << "FPL";
    QTest::newRow("HAL->FPR") << "HAL" << "FPR";
    QTest::newRow("HPR->FAL") << "HPR" << "FAL";
    QTest::newRow("HPL->FAR") << "HPL" << "FAR";

    // Fxx
    QTest::newRow("FRA->HLP") << "FRA" << "HLP";
    QTest::newRow("FRP->HLA") << "FRP" << "HLA";
    QTest::newRow("FLA->HRP") << "FLA" << "HRP";
    QTest::newRow("FLP->HRA") << "FLP" << "HRA";
    QTest::newRow("FAR->HPL") << "FAR" << "HPL";
    QTest::newRow("FAL->HPR") << "FAL" << "HPR";
    QTest::newRow("FPR->HAL") << "FPR" << "HAL";
    QTest::newRow("FPL->HAR") << "FPL" << "HAR";
    
    // Cadenes amb altres valors
    QTest::newRow("String with non-convertable values") << "abcdefghijklmnopqrstuvwxyz0123456789" << "????????????????????????????????????";
}

void test_PatientOrientation::getOppositeOrientationLabel_ShouldReturnExpectedValues()
{
    QFETCH(QString, inputLabel);
    QFETCH(QString, resultingLabel);

    QCOMPARE(PatientOrientation::getOppositeOrientationLabel(inputLabel), resultingLabel);
}

void test_PatientOrientation::setLabels_ShouldReturnTrueAndOrientationStringIsSet_data()
{
    QTest::addColumn<QString>("rowLabel");
    QTest::addColumn<QString>("columnLabel");
    QTest::addColumn<QString>("normalLabel");

    QTest::newRow("row and column with value, normal empty") << "R" << "A" << "";
    QTest::newRow("row and column with value, normal empty (multiple chars)") << "RLHFAPRLHFAP" << "AAARLHFAPAAARRRRRFFFF" << "";
    QTest::newRow("3 items") << "R" << "A" << "F";
    QTest::newRow("3 items (multiple chars)") << "RRRRLHFAPRRR" << "AAAAARLHFAPAAAAA" << "FFFFRLHFAPFFFFFF";
}

void test_PatientOrientation::setLabels_ShouldReturnTrueAndOrientationStringIsSet()
{
    QFETCH(QString, rowLabel);
    QFETCH(QString, columnLabel);
    QFETCH(QString, normalLabel);

    PatientOrientation patientOrientationInitializedWith3Parameters;

    QCOMPARE(patientOrientationInitializedWith3Parameters.setLabels(rowLabel, columnLabel, normalLabel), true);
    QCOMPARE(patientOrientationInitializedWith3Parameters.getRowDirectionLabel(), rowLabel);
    QCOMPARE(patientOrientationInitializedWith3Parameters.getColumnDirectionLabel(), columnLabel);
    QCOMPARE(patientOrientationInitializedWith3Parameters.getNormalDirectionLabel(), normalLabel);

    PatientOrientation patientOrientationInitializedWith2Parameters;

    QCOMPARE(patientOrientationInitializedWith2Parameters.setLabels(rowLabel, columnLabel), true);
    QCOMPARE(patientOrientationInitializedWith2Parameters.getRowDirectionLabel(), rowLabel);
    QCOMPARE(patientOrientationInitializedWith2Parameters.getColumnDirectionLabel(), columnLabel);
    QCOMPARE(patientOrientationInitializedWith2Parameters.getNormalDirectionLabel(), QString());
}

void test_PatientOrientation::setLabels_2ParametersShouldReturnFalseAndSetEmptyOrientationString_data()
{
    QTest::addColumn<QString>("rowLabel");
    QTest::addColumn<QString>("columnLabel");
    
    QTest::newRow("1 item") << "R" << "";
    QTest::newRow("1 item (invalid char)") << "r" << "";
    QTest::newRow("1 item (multiple valid chars)") << "RLHFAPRLHAAAARLHFA" << "";
    QTest::newRow("1 item (multiple invalid chars)") << "rlhfaprlhaaaarlhfa" << "";
    
    QTest::newRow("row label is Empty (others have valid values)") << "" << "A";
    QTest::newRow("column label is Empty (others have valid values)") << "R" << "";
    QTest::newRow("2 items (one invalid char)") << "Q" << "R";
    
    QTest::newRow("2 items (multiple invalid chars)") << "rlhfa67832biusn9QWTY" << "aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("2 items (multiple invalid chars mixed with valid chars)") << "rlhfaPRLHFAP" << "AAArlhfapaaaRRRqqqq826735RRFFFF";
}

void test_PatientOrientation::setLabels_2ParametersShouldReturnFalseAndSetEmptyOrientationString()
{
    QFETCH(QString, rowLabel);
    QFETCH(QString, columnLabel);

    PatientOrientation patientOrientationInitializedWith2Parameters;

    QCOMPARE(patientOrientationInitializedWith2Parameters.setLabels(rowLabel, columnLabel), false);
    QCOMPARE(patientOrientationInitializedWith2Parameters.getRowDirectionLabel(), QString());
    QCOMPARE(patientOrientationInitializedWith2Parameters.getColumnDirectionLabel(), QString());
    QCOMPARE(patientOrientationInitializedWith2Parameters.getNormalDirectionLabel(), QString());
}

void test_PatientOrientation::setLabels_3ParametersShouldReturnFalseAndSetEmptyOrientationString_data()
{
    QTest::addColumn<QString>("rowLabel");
    QTest::addColumn<QString>("columnLabel");
    QTest::addColumn<QString>("normalLabel");
    
    QTest::newRow("1 item") << "R" << "" << "";
    QTest::newRow("1 item (invalid char)") << "r" << "" << "";
    QTest::newRow("1 item (multiple valid chars)") << "RLHFAPRLHAAAARLHFA" << "" << "";
    QTest::newRow("1 item (multiple invalid chars)") << "rlhfaprlhaaaarlhfa" << "" << "";
    
    QTest::newRow("row label is Empty (others have valid values)") << "" << "A" << "";
    QTest::newRow("column label is Empty (others have valid values)") << "R" << "" << "";
    QTest::newRow("normal label is Empty (row is wrong)") << "Q" << "R" << "";
    QTest::newRow("normal label is Empty (column is wrong)") << "A" << "W" << "";
    QTest::newRow("row and column are empty") << "" << "" << "A";
    
    QTest::newRow("2 items (multiple invalid chars)(1)") << "rlhfa67832biusn9QWTY" << "aaarlhfa1!paaarrrqqqq826735rrffff" << "";
    QTest::newRow("2 items (multiple invalid chars)(2)") << "rlhfa67832biusn9QWTY" << "" << "aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("2 items (multiple invalid chars)(3)") << "" << "rlhfa67832biusn9QWTY" << "aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("2 items (multiple invalid chars mixed with valid chars) (1)") << "rlhfaPRLHFAP" << "AAArlhfapaaaRRRqqqq826735RRFFFF" << "";
    QTest::newRow("2 items (multiple invalid chars mixed with valid chars) (2)") << "rlhfaPRLHFAP" << "" << "AAArlhfapaaaRRRqqqq826735RRFFFF";
    QTest::newRow("2 items (multiple invalid chars mixed with valid chars) (3)") << "" << "rlhfaPRLHFAP" << "AAArlhfapaaaRRRqqqq826735RRFFFF";
    
    QTest::newRow("3 items (one invalid char)") << "R" << "A" << "f";
    QTest::newRow("3 items (multiple invalid chars)") << "rrrrlhfaprrr" << "rlhfa67832biusn9QWTY" << "aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("3 items (multiple invalid chars mixed with valid chars)") << "rlhfaPRLHFAP" << "AAArlhfapaaaRRRqqqq826735RRFFFF" << "RLHFAPRLHAAQWSMOXE";
    QTest::newRow("3 empty values") << "" << "" << "";
}

void test_PatientOrientation::setLabels_3ParametersShouldReturnFalseAndSetEmptyOrientationString()
{
    QFETCH(QString, rowLabel);
    QFETCH(QString, columnLabel);
    QFETCH(QString, normalLabel);

    PatientOrientation patientOrientationInitializedWith3Parameters;

    QCOMPARE(patientOrientationInitializedWith3Parameters.setLabels(rowLabel, columnLabel, normalLabel), false);
    QCOMPARE(patientOrientationInitializedWith3Parameters.getRowDirectionLabel(), QString());
    QCOMPARE(patientOrientationInitializedWith3Parameters.getColumnDirectionLabel(), QString());
    QCOMPARE(patientOrientationInitializedWith3Parameters.getNormalDirectionLabel(), QString());
}
void test_PatientOrientation::setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet_data()
{
    QTest::addColumn<QString>("orientation");

    QTest::newRow("Empty string") << "";
    QTest::newRow("2 items") << "R\\A";
    QTest::newRow("2 items (multiple chars)") << "RLHFAPRLHFAP\\AAARLHFAPAAARRRRRFFFF";
    QTest::newRow("3 items") << "R\\A\\F";
    QTest::newRow("3 items (multiple chars)") << "RRRRLHFAPRRR\\AAAAARLHFAPAAAAA\\FFFFRLHFAPFFFFFF";
}

void test_PatientOrientation::setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet()
{
    QFETCH(QString, orientation);

    PatientOrientation patientOrientation;
    QCOMPARE(patientOrientation.setDICOMFormattedPatientOrientation(orientation), true);
    QCOMPARE(patientOrientation.getDICOMFormattedPatientOrientation(), orientation);
}

void test_PatientOrientation::setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString_data()
{
    QTest::addColumn<QString>("orientation");

    QTest::newRow("1 item") << "R";
    QTest::newRow("1 item (invalid char)") << "r";
    QTest::newRow("1 item (multiple valid chars)") << "RLHFAPRLHAAAARLHFA";
    QTest::newRow("1 item (multiple invalid chars)") << "rlhfaprlhaaaarlhfa";
    
    QTest::newRow("2 items (one invalid char)") << "Q\\R";
    QTest::newRow("2 items (multiple invalid chars)") << "rlhfa67832biusn9QWTY\\aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("2 items (multiple invalid chars mixed with valid chars)") << "rlhfaPRLHFAP\\AAArlhfapaaaRRRqqqq826735RRFFFF";
    
    QTest::newRow("3 items (one invalid char)") << "R\\A\\f";
    QTest::newRow("3 items (multiple invalid chars)") << "rrrrlhfaprrr\\rlhfa67832biusn9QWTY\\aaarlhfa1!paaarrrqqqq826735rrffff";
    QTest::newRow("3 items (multiple invalid chars mixed with valid chars)") << "rlhfaPRLHFAP\\AAArlhfapaaaRRRqqqq826735RRFFFF\\RLHFAPRLHAAQWSMOXE";

    QTest::newRow("1 sepatator alone") << "\\";
    QTest::newRow("2 sepatators alone") << "\\\\";
    QTest::newRow("3 sepatators alone") << "\\\\\\";
    QTest::newRow("Extra separator at the beginning with 2 valid labels") << "\\A\\R";
    QTest::newRow("Extra separator at the end with 2 valid labels") << "A\\R\\";
    QTest::newRow("Extra separators at the beggining and the end with 2 valid labels") << "\\A\\R\\";
    QTest::newRow("Extra separator at the beginning with 3 valid labels") << "\\A\\R\\F";
    QTest::newRow("Extra separator at the end with 3 valid labels") << "A\\R\\H\\";
    QTest::newRow("Extra separators at the beggining and the end with 3 valid labels") << "\\A\\R\\L\\";

    QTest::newRow("4 items (valid chars)") << "R\\A\\F\\H";
    QTest::newRow("4 items (mixed with valid chars)") << "aR\\Q\\RAHF\\doqndJOEOM=·eH";
}

void test_PatientOrientation::setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString()
{
    QFETCH(QString, orientation);

    PatientOrientation patientOrientation;
    QCOMPARE(patientOrientation.setDICOMFormattedPatientOrientation(orientation), false);
    QCOMPARE(patientOrientation.getDICOMFormattedPatientOrientation(), QString(""));
}

void test_PatientOrientation::setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation_data()
{
    QTest::addColumn<QVector3D>("rowVector");
    QTest::addColumn<QVector3D>("columnVector");
    QTest::addColumn<QString>("patientOrientationString");

    QVector3D zeroVector(0, 0, 0);
    QVector3D pxVector( 1,  0,  0);
    QVector3D nxVector(-1,  0,  0);
    QVector3D pyVector( 0,  1,  0);
    QVector3D nyVector( 0, -1,  0);
    QVector3D pzVector( 0,  0,  1);
    QVector3D nzVector( 0,  0, -1);

    QTest::newRow("Zero, Zero Vectors") << zeroVector << zeroVector << "";
    QTest::newRow("Zero, X Vectors") << zeroVector << pxVector << "";
    QTest::newRow("X, Zero, Vectors") << pxVector << zeroVector << "";
    QTest::newRow("X, Y Vectors") << pxVector << pyVector << "L\\P\\H";
    QTest::newRow("X, Z Vectors") << pxVector << pzVector << "L\\H\\A";
    QTest::newRow("Y, X Vectors") << pyVector << pxVector << "P\\L\\F";
    QTest::newRow("Y, Z Vectors") << pyVector << pzVector << "P\\H\\L";
    QTest::newRow("Z, X Vectors") << pzVector << pxVector << "H\\L\\P";
    QTest::newRow("Z, Y Vectors") << pzVector << pyVector << "H\\P\\R";
    QTest::newRow("X, -Y Vectors") << pxVector << nyVector << "L\\A\\F";
    QTest::newRow("-X, -Y Vectors") << nxVector << nyVector << "R\\A\\H";
    QTest::newRow("-X, Y Vectors") << nxVector << pyVector << "R\\P\\F";
    QTest::newRow("X, -Z Vectors") << pxVector << nzVector << "L\\F\\P";
    QTest::newRow("-X, -Z Vectors") << nxVector << nzVector << "R\\F\\A";
    QTest::newRow("-X, Z Vectors") << nxVector << pzVector << "R\\H\\P";
    QTest::newRow("Y, -Z Vectors") << pyVector << nzVector << "P\\F\\R";
    QTest::newRow("-Y, -Z Vectors") << nyVector << nzVector << "A\\F\\L";
    QTest::newRow("-Y, Z Vectors") << nyVector << pzVector << "A\\H\\R";
    QTest::newRow("Y, -X Vectors") << pyVector << nxVector << "P\\R\\H";
    QTest::newRow("-Y, -X Vectors") << nyVector << nxVector << "A\\R\\F";
    QTest::newRow("-Y, X Vectors") << nyVector << pxVector << "A\\L\\H";

    QVector3D pxpyVector(QVector3D( 1,  1,  0).normalized());
    QVector3D nxpyVector(QVector3D(-1,  1,  0).normalized());
    QVector3D pxnyVector(QVector3D( 1, -1,  0).normalized());
    QVector3D nxnyVector(QVector3D(-1, -1,  0).normalized());
    QVector3D pxpzVector(QVector3D( 1,  0,  1).normalized());
    QVector3D nxpzVector(QVector3D(-1,  0,  1).normalized());
    QVector3D pxnzVector(QVector3D( 1,  0, -1).normalized());
    QVector3D nxnzVector(QVector3D(-1,  0, -1).normalized());
    QVector3D pypzVector(QVector3D( 0,  1,  1).normalized());
    QVector3D nypzVector(QVector3D( 0, -1,  1).normalized());
    QVector3D pynzVector(QVector3D( 0,  1, -1).normalized());
    QVector3D nynzVector(QVector3D( 0, -1, -1).normalized());

    // XY
    {
        QTest::newRow("+x+y, -x+y") << pxpyVector << nxpyVector << "LP\\RP\\H";
        QTest::newRow("+x+y, +x-y") << pxpyVector << pxnyVector << "LP\\LA\\F";
        QTest::newRow("+x+y, +x+z") << pxpyVector << pxpzVector << "LP\\LH\\LAF";
        QTest::newRow("+x+y, -x+z") << pxpyVector << nxpzVector << "LP\\RH\\LAH";
        QTest::newRow("+x+y, +x-z") << pxpyVector << pxnzVector << "LP\\LF\\RPF";
        QTest::newRow("+x+y, -x-z") << pxpyVector << nxnzVector << "LP\\RF\\RPH";
        QTest::newRow("+x+y, +y+z") << pxpyVector << pypzVector << "LP\\PH\\LAH";
        QTest::newRow("+x+y, -y+z") << pxpyVector << nypzVector << "LP\\AH\\LAF";
        QTest::newRow("+x+y, +y-z") << pxpyVector << pynzVector << "LP\\PF\\RPH";
        QTest::newRow("+x+y, -y-z") << pxpyVector << nynzVector << "LP\\AF\\RPF";

        QTest::newRow("-x+y, +x+y") << nxpyVector << pxpyVector << "RP\\LP\\F";
        QTest::newRow("-x+y, -x-y") << nxpyVector << nxnyVector << "RP\\RA\\H";
        QTest::newRow("-x+y, +x+z") << nxpyVector << pxpzVector << "RP\\LH\\LPF";
        QTest::newRow("-x+y, -x+z") << nxpyVector << nxpzVector << "RP\\RH\\LPH";
        QTest::newRow("-x+y, +x-z") << nxpyVector << pxnzVector << "RP\\LF\\RAF";
        QTest::newRow("-x+y, -x-z") << nxpyVector << nxnzVector << "RP\\RF\\RAH";
        QTest::newRow("-x+y, +y+z") << nxpyVector << pypzVector << "RP\\PH\\LPF";
        QTest::newRow("-x+y, -y+z") << nxpyVector << nypzVector << "RP\\AH\\LPH";
        QTest::newRow("-x+y, +y-z") << nxpyVector << pynzVector << "RP\\PF\\RAF";
        QTest::newRow("-x+y, -y-z") << nxpyVector << nynzVector << "RP\\AF\\RAH";

        QTest::newRow("+x-y, +x+y") << pxnyVector << pxpyVector << "LA\\LP\\H";
        QTest::newRow("+x-y, -x-y") << pxnyVector << nxnyVector << "LA\\RA\\F";
        QTest::newRow("+x-y, +x+z") << pxnyVector << pxpzVector << "LA\\LH\\RAH";
        QTest::newRow("+x-y, -x+z") << pxnyVector << nxpzVector << "LA\\RH\\RAF";
        QTest::newRow("+x-y, +x-z") << pxnyVector << pxnzVector << "LA\\LF\\LPH";
        QTest::newRow("+x-y, -x-z") << pxnyVector << nxnzVector << "LA\\RF\\LPF";
        QTest::newRow("+x-y, +y+z") << pxnyVector << pypzVector << "LA\\PH\\RAH";
        QTest::newRow("+x-y, -y+z") << pxnyVector << nypzVector << "LA\\AH\\RAF";
        QTest::newRow("+x-y, +y-z") << pxnyVector << pynzVector << "LA\\PF\\LPH";
        QTest::newRow("+x-y, -y-z") << pxnyVector << nynzVector << "LA\\AF\\LPF";

        QTest::newRow("-x-y, -x+y") << nxnyVector << nxpyVector << "RA\\RP\\F";
        QTest::newRow("-x-y, +x-y") << nxnyVector << pxnyVector << "RA\\LA\\H";
        QTest::newRow("-x-y, +x+z") << nxnyVector << pxpzVector << "RA\\LH\\RPH";
        QTest::newRow("-x-y, -x+z") << nxnyVector << nxpzVector << "RA\\RH\\RPF";
        QTest::newRow("-x-y, +x-z") << nxnyVector << pxnzVector << "RA\\LF\\LAH";
        QTest::newRow("-x-y, -x-z") << nxnyVector << nxnzVector << "RA\\RF\\LAF";
        QTest::newRow("-x-y, +y+z") << nxnyVector << pypzVector << "RA\\PH\\RPF";
        QTest::newRow("-x-y, -y+z") << nxnyVector << nypzVector << "RA\\AH\\RPH";
        QTest::newRow("-x-y, +y-z") << nxnyVector << pynzVector << "RA\\PF\\LAF";
        QTest::newRow("-x-y, -y-z") << nxnyVector << nynzVector << "RA\\AF\\LAH";
    }

    // XZ
    {
        QTest::newRow("+x+z, +x+y") << pxpzVector << pxpyVector << "LH\\LP\\RPH";
        QTest::newRow("+x+z, -x+y") << pxpzVector << nxpyVector << "LH\\RP\\RAH";
        QTest::newRow("+x+z, +x-y") << pxpzVector << pxnyVector << "LH\\LA\\LPF";
        QTest::newRow("+x+z, -x-y") << pxpzVector << nxnyVector << "LH\\RA\\LAF";
        QTest::newRow("+x+z, -x+z") << pxpzVector << nxpzVector << "LH\\RH\\A";
        QTest::newRow("+x+z, +x-z") << pxpzVector << pxnzVector << "LH\\LF\\P";
        QTest::newRow("+x+z, +y+z") << pxpzVector << pypzVector << "LH\\PH\\RAH";
        QTest::newRow("+x+z, -y+z") << pxpzVector << nypzVector << "LH\\AH\\LAF";
        QTest::newRow("+x+z, +y-z") << pxpzVector << pynzVector << "LH\\PF\\RPH";
        QTest::newRow("+x+z, -y-z") << pxpzVector << nynzVector << "LH\\AF\\LPF";

        QTest::newRow("-x+z, +x+y") << nxpzVector << pxpyVector << "RH\\LP\\RPF";
        QTest::newRow("-x+z, -x+y") << nxpzVector << nxpyVector << "RH\\RP\\RAF";
        QTest::newRow("-x+z, +x-y") << nxpzVector << pxnyVector << "RH\\LA\\LPH";
        QTest::newRow("-x+z, -x-y") << nxpzVector << nxnyVector << "RH\\RA\\LAH";
        QTest::newRow("-x+z, +x+z") << nxpzVector << pxpzVector << "RH\\LH\\P";
        QTest::newRow("-x+z, -x-z") << nxpzVector << nxnzVector << "RH\\RF\\A";
        QTest::newRow("-x+z, +y+z") << nxpzVector << pypzVector << "RH\\PH\\RPF";
        QTest::newRow("-x+z, -y+z") << nxpzVector << nypzVector << "RH\\AH\\LPH";
        QTest::newRow("-x+z, +y-z") << nxpzVector << pynzVector << "RH\\PF\\RAF";
        QTest::newRow("-x+z, -y-z") << nxpzVector << nynzVector << "RH\\AF\\LAH";

        QTest::newRow("+x-z, +x+y") << pxnzVector << pxpyVector << "LF\\LP\\LAH";
        QTest::newRow("+x-z, -x+y") << pxnzVector << nxpyVector << "LF\\RP\\LPH";
        QTest::newRow("+x-z, +x-y") << pxnzVector << pxnyVector << "LF\\LA\\RAF";
        QTest::newRow("+x-z, -x-y") << pxnzVector << nxnyVector << "LF\\RA\\RPF";
        QTest::newRow("+x-z, +x+z") << pxnzVector << pxpzVector << "LF\\LH\\A";
        QTest::newRow("+x-z, -x-z") << pxnzVector << nxnzVector << "LF\\RF\\P";
        QTest::newRow("+x-z, +y+z") << pxnzVector << pypzVector << "LF\\PH\\LAH";
        QTest::newRow("+x-z, -y+z") << pxnzVector << nypzVector << "LF\\AH\\RAF";
        QTest::newRow("+x-z, +y-z") << pxnzVector << pynzVector << "LF\\PF\\LPH";
        QTest::newRow("+x-z, -y-z") << pxnzVector << nynzVector << "LF\\AF\\RPF";

        QTest::newRow("-x-z, +x+y") << nxnzVector << pxpyVector << "RF\\LP\\LAF";
        QTest::newRow("-x-z, -x+y") << nxnzVector << nxpyVector << "RF\\RP\\LPF";
        QTest::newRow("-x-z, +x-y") << nxnzVector << pxnyVector << "RF\\LA\\RAH";
        QTest::newRow("-x-z, -x-y") << nxnzVector << nxnyVector << "RF\\RA\\RPH";
        QTest::newRow("-x-z, -x+z") << nxnzVector << nxpzVector << "RF\\RH\\P";
        QTest::newRow("-x-z, +x-z") << nxnzVector << pxnzVector << "RF\\LF\\A";
        QTest::newRow("-x-z, +y+z") << nxnzVector << pypzVector << "RF\\PH\\LPF";
        QTest::newRow("-x-z, -y+z") << nxnzVector << nypzVector << "RF\\AH\\RPH";
        QTest::newRow("-x-z, +y-z") << nxnzVector << pynzVector << "RF\\PF\\LAF";
        QTest::newRow("-x-z, -y-z") << nxnzVector << nynzVector << "RF\\AF\\RAH";
    }

    // YZ
    {
        QTest::newRow("+y+z, +x+y") << pypzVector << pxpyVector << "PH\\LP\\RPF";
        QTest::newRow("+y+z, -x+y") << pypzVector << nxpyVector << "PH\\RP\\RAH";
        QTest::newRow("+y+z, +x-y") << pypzVector << pxnyVector << "PH\\LA\\LPF";
        QTest::newRow("+y+z, -x-y") << pypzVector << nxnyVector << "PH\\RA\\LAH";
        QTest::newRow("+y+z, +x+z") << pypzVector << pxpzVector << "PH\\LH\\LPF";
        QTest::newRow("+y+z, -x+z") << pypzVector << nxpzVector << "PH\\RH\\LAH";
        QTest::newRow("+y+z, +x-z") << pypzVector << pxnzVector << "PH\\LF\\RPF";
        QTest::newRow("+y+z, -x-z") << pypzVector << nxnzVector << "PH\\RF\\RAH";
        QTest::newRow("+y+z, -y+z") << pypzVector << nypzVector << "PH\\AH\\L";
        QTest::newRow("+y+z, +y-z") << pypzVector << pynzVector << "PH\\PF\\R";

        QTest::newRow("-y+z, +x+y") << nypzVector << pxpyVector << "AH\\LP\\RPH";
        QTest::newRow("-y+z, -x+y") << nypzVector << nxpyVector << "AH\\RP\\RAF";
        QTest::newRow("-y+z, +x-y") << nypzVector << pxnyVector << "AH\\LA\\LPH";
        QTest::newRow("-y+z, -x-y") << nypzVector << nxnyVector << "AH\\RA\\LAF";
        QTest::newRow("-y+z, +x+z") << nypzVector << pxpzVector << "AH\\LH\\RPH";
        QTest::newRow("-y+z, -x+z") << nypzVector << nxpzVector << "AH\\RH\\RAF";
        QTest::newRow("-y+z, +x-z") << nypzVector << pxnzVector << "AH\\LF\\LPH";
        QTest::newRow("-y+z, -x-z") << nypzVector << nxnzVector << "AH\\RF\\LAF";
        QTest::newRow("-y+z, +y+z") << nypzVector << pypzVector << "AH\\PH\\R";
        QTest::newRow("-y+z, -y-z") << nypzVector << nynzVector << "AH\\AF\\L";

        QTest::newRow("+y-z, +x+y") << pynzVector << pxpyVector << "PF\\LP\\LAF";
        QTest::newRow("+y-z, -x+y") << pynzVector << nxpyVector << "PF\\RP\\LPH";
        QTest::newRow("+y-z, +x-y") << pynzVector << pxnyVector << "PF\\LA\\RAF";
        QTest::newRow("+y-z, -x-y") << pynzVector << nxnyVector << "PF\\RA\\RPH";
        QTest::newRow("+y-z, +x+z") << pynzVector << pxpzVector << "PF\\LH\\LAF";
        QTest::newRow("+y-z, -x+z") << pynzVector << nxpzVector << "PF\\RH\\LPH";
        QTest::newRow("+y-z, +x-z") << pynzVector << pxnzVector << "PF\\LF\\RAF";
        QTest::newRow("+y-z, -x-z") << pynzVector << nxnzVector << "PF\\RF\\RPH";
        QTest::newRow("+y-z, +y+z") << pynzVector << pypzVector << "PF\\PH\\L";
        QTest::newRow("+y-z, -y-z") << pynzVector << nynzVector << "PF\\AF\\R";

        QTest::newRow("-y-z, +x+y") << nynzVector << pxpyVector << "AF\\LP\\LAH";
        QTest::newRow("-y-z, -x+y") << nynzVector << nxpyVector << "AF\\RP\\LPF";
        QTest::newRow("-y-z, +x-y") << nynzVector << pxnyVector << "AF\\LA\\RAH";
        QTest::newRow("-y-z, -x-y") << nynzVector << nxnyVector << "AF\\RA\\RPF";
        QTest::newRow("-y-z, +x+z") << nynzVector << pxpzVector << "AF\\LH\\RAH";
        QTest::newRow("-y-z, -x+z") << nynzVector << nxpzVector << "AF\\RH\\RPF";
        QTest::newRow("-y-z, +x-z") << nynzVector << pxnzVector << "AF\\LF\\LAH";
        QTest::newRow("-y-z, -x-z") << nynzVector << nxnzVector << "AF\\RF\\LPF";
        QTest::newRow("-y-z, -y+z") << nynzVector << nypzVector << "AF\\AH\\R";
        QTest::newRow("-y-z, +y-z") << nynzVector << pynzVector << "AF\\PF\\L";
    }

    QVector3D pxpypzVector(QVector3D( 1,  1,  1).normalized());
    QVector3D nxpypzVector(QVector3D(-1,  1,  1).normalized());
    QVector3D pxnypzVector(QVector3D( 1, -1,  1).normalized());
    QVector3D nxnypzVector(QVector3D(-1, -1,  1).normalized());
    QVector3D pxpynzVector(QVector3D( 1,  1, -1).normalized());
    QVector3D nxpynzVector(QVector3D(-1,  1, -1).normalized());
    QVector3D pxnynzVector(QVector3D( 1, -1, -1).normalized());
    QVector3D nxnynzVector(QVector3D(-1, -1, -1).normalized());

    // Below are all the orthogonal combinations of 3 and 2 components != 0 (there is none of 3 and 3)

    QTest::newRow("+x+y+z, -x+y") << pxpypzVector << nxpyVector << "LPH\\RP\\HRA";
    QTest::newRow("+x+y+z, +x-y") << pxpypzVector << pxnyVector << "LPH\\LA\\FLP";
    QTest::newRow("+x+y+z, -x+z") << pxpypzVector << nxpzVector << "LPH\\RH\\ALH";
    QTest::newRow("+x+y+z, +x-z") << pxpypzVector << pxnzVector << "LPH\\LF\\PRF";
    QTest::newRow("+x+y+z, -y+z") << pxpypzVector << nypzVector << "LPH\\AH\\LAF";
    QTest::newRow("+x+y+z, +y-z") << pxpypzVector << pynzVector << "LPH\\PF\\RPH";

    QTest::newRow("-x+y+z, +x+y") << nxpypzVector << pxpyVector << "RPH\\LP\\FRP";
    QTest::newRow("-x+y+z, -x-y") << nxpypzVector << nxnyVector << "RPH\\RA\\HLA";
    QTest::newRow("-x+y+z, +x+z") << nxpypzVector << pxpzVector << "RPH\\LH\\PLF";
    QTest::newRow("-x+y+z, -x-z") << nxpypzVector << nxnzVector << "RPH\\RF\\ARH";
    QTest::newRow("-x+y+z, -y+z") << nxpypzVector << nypzVector << "RPH\\AH\\LPH";
    QTest::newRow("-x+y+z, +y-z") << nxpypzVector << pynzVector << "RPH\\PF\\RAF";

    QTest::newRow("+x-y+z, +x+y") << pxnypzVector << pxpyVector << "LAH\\LP\\HRP";
    QTest::newRow("+x-y+z, -x-y") << pxnypzVector << nxnyVector << "LAH\\RA\\FLA";
    QTest::newRow("+x-y+z, -x+z") << pxnypzVector << nxpzVector << "LAH\\RH\\ARF";
    QTest::newRow("+x-y+z, +x-z") << pxnypzVector << pxnzVector << "LAH\\LF\\PLH";
    QTest::newRow("+x-y+z, +y+z") << pxnypzVector << pypzVector << "LAH\\PH\\RAH";
    QTest::newRow("+x-y+z, -y-z") << pxnypzVector << nynzVector << "LAH\\AF\\LPF";

    QTest::newRow("-x-y+z, -x+y") << nxnypzVector << nxpyVector << "RAH\\RP\\FRA";
    QTest::newRow("-x-y+z, +x-y") << nxnypzVector << pxnyVector << "RAH\\LA\\HLP";
    QTest::newRow("-x-y+z, +x+z") << nxnypzVector << pxpzVector << "RAH\\LH\\PRH";
    QTest::newRow("-x-y+z, -x-z") << nxnypzVector << nxnzVector << "RAH\\RF\\ALF";
    QTest::newRow("-x-y+z, +y+z") << nxnypzVector << pypzVector << "RAH\\PH\\RPF";
    QTest::newRow("-x-y+z, -y-z") << nxnypzVector << nynzVector << "RAH\\AF\\LAH";

    QTest::newRow("+x+y-z, -x+y") << pxpynzVector << nxpyVector << "LPF\\RP\\HLP";
    QTest::newRow("+x+y-z, +x-y") << pxpynzVector << pxnyVector << "LPF\\LA\\FRA";
    QTest::newRow("+x+y-z, +x+z") << pxpynzVector << pxpzVector << "LPF\\LH\\ALF";
    QTest::newRow("+x+y-z, -x-z") << pxpynzVector << nxnzVector << "LPF\\RF\\PRH";
    QTest::newRow("+x+y-z, +y+z") << pxpynzVector << pypzVector << "LPF\\PH\\LAH";
    QTest::newRow("+x+y-z, -y-z") << pxpynzVector << nynzVector << "LPF\\AF\\RPF";

    QTest::newRow("-x+y-z, +x+y") << nxpynzVector << pxpyVector << "RPF\\LP\\FLA";
    QTest::newRow("-x+y-z, -x-y") << nxpynzVector << nxnyVector << "RPF\\RA\\HRP";
    QTest::newRow("-x+y-z, -x+z") << nxpynzVector << nxpzVector << "RPF\\RH\\PLH";
    QTest::newRow("-x+y-z, +x-z") << nxpynzVector << pxnzVector << "RPF\\LF\\ARF";
    QTest::newRow("-x+y-z, +y+z") << nxpynzVector << pypzVector << "RPF\\PH\\LPF";
    QTest::newRow("-x+y-z, -y-z") << nxpynzVector << nynzVector << "RPF\\AF\\RAH";

    QTest::newRow("+x-y-z, +x+y") << pxnynzVector << pxpyVector << "LAF\\LP\\HLA";
    QTest::newRow("+x-y-z, -x-y") << pxnynzVector << nxnyVector << "LAF\\RA\\FRP";
    QTest::newRow("+x-y-z, +x+z") << pxnynzVector << pxpzVector << "LAF\\LH\\ARH";
    QTest::newRow("+x-y-z, -x-z") << pxnynzVector << nxnzVector << "LAF\\RF\\PLF";
    QTest::newRow("+x-y-z, -y+z") << pxnynzVector << nypzVector << "LAF\\AH\\RAF";
    QTest::newRow("+x-y-z, +y-z") << pxnynzVector << pynzVector << "LAF\\PF\\LPH";

    QTest::newRow("-x-y-z, -x+y") << nxnynzVector << nxpyVector << "RAF\\RP\\FLP";
    QTest::newRow("-x-y-z, +x-y") << nxnynzVector << pxnyVector << "RAF\\LA\\HRA";
    QTest::newRow("-x-y-z, -x+z") << nxnynzVector << nxpzVector << "RAF\\RH\\PRF";
    QTest::newRow("-x-y-z, +x-z") << nxnynzVector << pxnzVector << "RAF\\LF\\ALH";
    QTest::newRow("-x-y-z, -y+z") << nxnynzVector << nypzVector << "RAF\\AH\\RPH";
    QTest::newRow("-x-y-z, +y-z") << nxnynzVector << pynzVector << "RAF\\PF\\LAF";

    QTest::newRow("Sagittal Lumbar MR example") << QVector3D(-0.0488199964165, 0.99880760908126, 1.7226173612e-11) 
        << QVector3D(0.01777809672057, 0.00086896272841, -0.9998415708541) << "PR\\FLP\\RAF";

    QTest::newRow("Axial Lumbar MR example") << QVector3D(0.99864935874938, 0.04881225898861, 0.01779931783676)
        << QVector3D(-0.0516529306769, 0.89577716588974, 0.44149202108383) << "LPH\\PHR\\HAL";

    QTest::newRow("Axial Lumbar MR example (2)") << QVector3D(0.99962580204010, -0.0025094528682, 0.02723966166377)
        << QVector3D(-0.0061289877630, 0.94992113113403, 0.31242957711219) << "LHA\\PHR\\HAR";

    QTest::newRow("Axial Lumbar MR example (3)") << QVector3D(0.99928122758865, 0.00065188325243, 0.03790331259369)
        << QVector3D(0.00063067907467, 0.99942785501480, -0.0338159352540) << "LHP\\PFL\\HRP";

    QTest::newRow("Axial Diffusion Brain MR example") << QVector3D(0.99786239862442, 0.06464176625013, -0.0095974076539)
        << QVector3D(-0.0645796582102, 0.95291411876678, -0.2962839603424) << "LPF\\PFR\\HPR";

    QTest::newRow("Sagittal Diffusion Brain MR example") << QVector3D(-0.0524810999631, 0.98934137821197, -0.1358285695314)
        << QVector3D(0.00632020272314, -0.1356842368841, -0.9907319545745) << "PFR\\FAL\\RAH";

    QTest::newRow("Coronal Diffusion Brain MR example") << QVector3D(0.99754029512405, 0.06982568651437, -0.0061442572623)
        << QVector3D(0.00340184569358, -0.1357780992984, -0.9907334446907) << "LPF\\FAL\\PFR";

    QTest::newRow("Axial Brain MR example") << QVector3D(1, 0, 0) << QVector3D(0, 1, 0) << "L\\P\\H";
    QTest::newRow("Sagittal Brain MR example") << QVector3D(0, 1, 0) << QVector3D(0, 0, -1) << "P\\F\\R";
    QTest::newRow("Coronal Brain MR example") << QVector3D(1, 0, 0) << QVector3D(0, 0, -1) << "L\\F\\P";
    QTest::newRow("Axial CT example") << QVector3D(1, 0, 0) << QVector3D(0, 0.956304756, -0.292371705) << "L\\PF\\HP";
    QTest::newRow("Sagittal CT example") << QVector3D(0, 1, 0) << QVector3D(-0.0428148313, 0, -0.999083025) << "P\\FR\\RH";
}

void test_PatientOrientation::setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation()
{
    QFETCH(QVector3D, rowVector);
    QFETCH(QVector3D, columnVector);
    QFETCH(QString, patientOrientationString);

    ImageOrientation imageOrientation(rowVector, columnVector);
    PatientOrientation patientOrientation;
    patientOrientation.setPatientOrientationFromImageOrientation(imageOrientation);
    
    QCOMPARE(patientOrientation.getDICOMFormattedPatientOrientation(), patientOrientationString);
}

void test_PatientOrientation::getRowDirectionLabel_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("orientationString");
    QTest::addColumn<QString>("expectedLabel");

    QTest::newRow("empty string") << "" << "";
    QTest::newRow("2 multi-valued items") << "LPF\\HPR" << "LPF";
    QTest::newRow("2 single valued items") << "P\\H" << "P";
    QTest::newRow("3 multi-valued items") << "LPF\\PFR\\HPR" << "LPF";
    QTest::newRow("3 single valued items") << "L\\P\\H" << "L";
}

void test_PatientOrientation::getRowDirectionLabel_ShouldReturnExpectedValues()
{
    QFETCH(QString, orientationString);
    QFETCH(QString, expectedLabel);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(orientationString);

    QCOMPARE(patientOrientation.getRowDirectionLabel(), expectedLabel);
}

void test_PatientOrientation::getColumnDirectionLabel_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("orientationString");
    QTest::addColumn<QString>("expectedLabel");

    QTest::newRow("empty string") << "" << "";
    QTest::newRow("2 multi-valued items") << "LPF\\HPR" << "HPR";
    QTest::newRow("2 single valued items") << "P\\H" << "H";
    QTest::newRow("3 multi-valued items") << "LPF\\PFR\\HPR" << "PFR";
    QTest::newRow("3 single valued items") << "L\\P\\H" << "P";
}

void test_PatientOrientation::getColumnDirectionLabel_ShouldReturnExpectedValues()
{
    QFETCH(QString, orientationString);
    QFETCH(QString, expectedLabel);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(orientationString);

    QCOMPARE(patientOrientation.getColumnDirectionLabel(), expectedLabel);
}

void test_PatientOrientation::getNormalDirectionLabel_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("orientationString");
    QTest::addColumn<QString>("expectedLabel");

    QTest::newRow("empty string") << "" << "";
    QTest::newRow("2 multi-valued items") << "LPF\\HPR" << "";
    QTest::newRow("2 single valued items") << "P\\H" << "";
    QTest::newRow("3 multi-valued items") << "LPF\\PFR\\HPR" << "HPR";
    QTest::newRow("3 single valued items") << "L\\P\\H" << "H";
}

void test_PatientOrientation::getNormalDirectionLabel_ShouldReturnExpectedValues()
{
    QFETCH(QString, orientationString);
    QFETCH(QString, expectedLabel);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(orientationString);

    QCOMPARE(patientOrientation.getNormalDirectionLabel(), expectedLabel);
}

DECLARE_TEST(test_PatientOrientation)

#include "test_patientorientation.moc"
