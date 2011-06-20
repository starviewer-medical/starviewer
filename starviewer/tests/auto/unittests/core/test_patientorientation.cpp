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

    void setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet_data();
    void setDICOMFormattedPatientOrientation_ShouldReturnTrueAndOrientationStringIsSet();

    void setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString_data();
    void setDICOMFormattedPatientOrientation_ShouldReturnFalseAndSetEmptyOrientationString();

    void setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation_data();
    void setPatientOrientationFromImageOrientation_ShouldMakeRightPatientOrientation();
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
    QVector3D xVector(1, 0, 0);
    QVector3D yVector(0, 1, 0);
    QVector3D zVector(0, 0, 1);
    QVector3D xMinusVector(-1, 0, 0);
    QVector3D yMinusVector(0, -1, 0);
    QVector3D zMinusVector(0, 0, -1);
    
    QTest::newRow("Zero, Zero Vectors")
        << zeroVector << zeroVector << "";
    
    QTest::newRow("Zero, X Vectors")
        << zeroVector << xVector << "";

    QTest::newRow("X, Zero, Vectors")
        << xVector << zeroVector << "";

    QTest::newRow("X, Y Vectors")
        << xVector << yVector << "L\\P\\H";

    QTest::newRow("X, Z Vectors")
        << xVector << zVector << "L\\H\\A";
    
    QTest::newRow("Y, X Vectors")
        << yVector << xVector << "P\\L\\F";

    QTest::newRow("Y, Z Vectors")
        << yVector << zVector << "P\\H\\L";

    QTest::newRow("Z, X Vectors")
        << zVector << xVector << "H\\L\\P";

    QTest::newRow("Z, Y Vectors")
        << zVector << yVector << "H\\P\\R";

    QTest::newRow("X, -Y Vectors")
        << xVector << yMinusVector << "L\\A\\F";

    QTest::newRow("-X, -Y Vectors")
        << xMinusVector << yMinusVector << "R\\A\\H";

    QTest::newRow("-X, Y Vectors")
        << xMinusVector << yVector << "R\\P\\F";
    
    QTest::newRow("X, -Z Vectors")
        << xVector << zMinusVector << "L\\F\\P";

    QTest::newRow("-X, -Z Vectors")
        << xMinusVector << zMinusVector << "R\\F\\A";

    QTest::newRow("-X, Z Vectors")
        << xMinusVector << zVector << "R\\H\\P";

    QTest::newRow("Y, -Z Vectors")
        << yVector << zMinusVector << "P\\F\\R";

    QTest::newRow("-Y, -Z Vectors")
        << yMinusVector << zMinusVector << "A\\F\\L";

    QTest::newRow("-Y, Z Vectors")
        << yMinusVector << zVector << "A\\H\\R";

    QTest::newRow("Y, -X Vectors")
        << yVector << xMinusVector << "P\\R\\H";

    QTest::newRow("-Y, -X Vectors")
        << yMinusVector << xMinusVector << "A\\R\\F";

    QTest::newRow("-Y, X Vectors")
        << yMinusVector << xVector << "A\\L\\H";

    QTest::newRow("Sagittal Lumbar MR example")
        << QVector3D(-0.0488199964165, 0.99880760908126, 1.7226173612e-11)
        << QVector3D(0.01777809672057, 0.00086896272841, -0.9998415708541)
        << "PR\\FLP\\RAF";

    QTest::newRow("Axial Lumbar MR example")
        << QVector3D(0.99864935874938, 0.04881225898861, 0.01779931783676)
        << QVector3D(-0.0516529306769, 0.89577716588974, 0.44149202108383)
        << "LPH\\PHR\\HAL";

    QTest::newRow("Axial Lumbar MR example (2)")
        << QVector3D(0.99962580204010, -0.0025094528682, 0.02723966166377)
        << QVector3D(-0.0061289877630, 0.94992113113403, 0.31242957711219)
        << "LHA\\PHR\\HAR";

    QTest::newRow("Axial Lumbar MR example (3)")
        << QVector3D(0.99928122758865, 0.00065188325243, 0.03790331259369)
        << QVector3D(0.00063067907467, 0.99942785501480, -0.0338159352540)
        << "LHP\\PFL\\HRP";

    QTest::newRow("Axial Brain MR example")
        << QVector3D(1, 0, 0)
        << QVector3D(0, 1, 0)
        << "L\\P\\H";
    
    QTest::newRow("Sagittal Brain MR example")
        << QVector3D(0, 1, 0)
        << QVector3D(0, 0, -1)
        << "P\\F\\R";
    
    QTest::newRow("Coronal Brain MR example")
        << QVector3D(1, 0, 0)
        << QVector3D(0, 0, -1)
        << "L\\F\\P";
 
    QTest::newRow("Axial Diffusion Brain MR example")
        << QVector3D(0.99786239862442, 0.06464176625013, -0.0095974076539)
        << QVector3D(-0.0645796582102, 0.95291411876678, -0.2962839603424)
        << "LPF\\PFR\\HPR";

    QTest::newRow("Sagittal Diffusion Brain MR example")
        << QVector3D(-0.0524810999631, 0.98934137821197, -0.1358285695314)
        << QVector3D(0.00632020272314, -0.1356842368841, -0.9907319545745)
        << "PFR\\FAL\\RAH";

    QTest::newRow("Coronal Diffusion Brain MR example")
        << QVector3D(0.99754029512405, 0.06982568651437, -0.0061442572623)
        << QVector3D(0.00340184569358, -0.1357780992984, -0.9907334446907)
        << "LPF\\FAL\\PFR";

    QTest::newRow("Axial CT example")
        << QVector3D(1, 0, 0)
        << QVector3D(0, 0.956304756, -0.292371705)
        << "L\\PF\\HP";

    QTest::newRow("Sagittal CT example")
        << QVector3D(0, 1, 0)
        << QVector3D(-0.0428148313, 0, -0.999083025)
        << "P\\FR\\RH";
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

DECLARE_TEST(test_PatientOrientation)

#include "test_patientorientation.moc"
