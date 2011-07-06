#include "autotest.h"
#include "anatomicalplane.h"
#include "patientorientation.h"

using namespace udg;

class test_AnatomicalPlane : public QObject {
Q_OBJECT

private slots:
    void getPlaneOrientationLabel_ShouldReturnRightLabel_data();
    void getPlaneOrientationLabel_ShouldReturnRightLabel();

    void getLabelFromPatientOrientation_ShouldReturnNAOrEmpty_data();
    void getLabelFromPatientOrientation_ShouldReturnNAOrEmpty();
    void getLabelFromPatientOrientation_ShouldReturnConcretLabel_data();
    void getLabelFromPatientOrientation_ShouldReturnConcretLabel();
};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType);

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnRightLabel_data()
{
    QTest::addColumn<AnatomicalPlane::AnatomicalPlaneType>("planeType");
    QTest::addColumn<QString>("planeLabel");

    QTest::newRow("Axial") << AnatomicalPlane::Axial << tr("AXIAL");
    QTest::newRow("Sagittal") << AnatomicalPlane::Sagittal << tr("SAGITTAL");
    QTest::newRow("Coronal") << AnatomicalPlane::Coronal << tr("CORONAL");
    QTest::newRow("Oblique") << AnatomicalPlane::Oblique << tr("OBLIQUE");
    QTest::newRow("NotAvailable") << AnatomicalPlane::NotAvailable << tr("N/A");
}

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnRightLabel()
{
    QFETCH(AnatomicalPlane::AnatomicalPlaneType, planeType);
    QFETCH(QString, planeLabel);

    QCOMPARE(AnatomicalPlane::getLabel(planeType), planeLabel);
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnNAOrEmpty_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    const QString NotAvailableResult = "N/A";
    const QString Separator = "\\";
    const QString AnyCorrectData1 = "A";
    const QString AnyCorrectData2 = "P";

    QTest::newRow("empty") << "" << NotAvailableResult;
    QTest::newRow("only separator") << Separator << NotAvailableResult;
    QTest::newRow("more than one separator") << Separator + Separator << NotAvailableResult;
    QTest::newRow("more than one separator with data") << AnyCorrectData1 + Separator + Separator + AnyCorrectData2 << NotAvailableResult;
    QTest::newRow("only one data1") << Separator + AnyCorrectData2 << NotAvailableResult;
    QTest::newRow("only one data2") << AnyCorrectData1 + Separator << NotAvailableResult;
    QTest::newRow("2 invalid items") << "x\\x" << NotAvailableResult;
    QTest::newRow("3 invalid items") << "x\\x\\x" << NotAvailableResult;
    QTest::newRow("3 invalid (multivalued) items") << "asdf\\asdf\\asdf" << NotAvailableResult;
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnNAOrEmpty()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getLabelFromPatientOrientation(patientOrientation), result);
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnConcretLabel_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("AXIAL1") << "R\\A" << "AXIAL";
    QTest::newRow("AXIAL2") << "R\\P" << "AXIAL";
    QTest::newRow("AXIAL3") << "L\\A" << "AXIAL";
    QTest::newRow("AXIAL4") << "L\\P" << "AXIAL";

    QTest::newRow("AXIAL5") << "A\\R" << "AXIAL";
    QTest::newRow("AXIAL6") << "A\\L" << "AXIAL";
    QTest::newRow("AXIAL7") << "P\\R" << "AXIAL";
    QTest::newRow("AXIAL8") << "P\\L" << "AXIAL";

    QTest::newRow("CORONAL1") << "R\\H" << "CORONAL";
    QTest::newRow("CORONAL2") << "R\\F" << "CORONAL";
    QTest::newRow("CORONAL3") << "L\\H" << "CORONAL";
    QTest::newRow("CORONAL4") << "L\\F" << "CORONAL";

    QTest::newRow("CORONAL5") << "H\\R" << "CORONAL";
    QTest::newRow("CORONAL6") << "H\\L" << "CORONAL";
    QTest::newRow("CORONAL7") << "F\\R" << "CORONAL";
    QTest::newRow("CORONAL8") << "F\\L" << "CORONAL";

    QTest::newRow("SAGITTAL1") << "A\\H" << "SAGITTAL";
    QTest::newRow("SAGITTAL2") << "A\\F" << "SAGITTAL";
    QTest::newRow("SAGITTAL3") << "P\\H" << "SAGITTAL";
    QTest::newRow("SAGITTAL4") << "P\\F" << "SAGITTAL";

    QTest::newRow("SAGITTAL5") << "H\\A" << "SAGITTAL";
    QTest::newRow("SAGITTAL6") << "H\\P" << "SAGITTAL";
    QTest::newRow("SAGITTAL7") << "F\\A" << "SAGITTAL";
    QTest::newRow("SAGITTAL8") << "F\\P" << "SAGITTAL";

    QTest::newRow("OBLIQUE3") << "L\\L" << "OBLIQUE";
    QTest::newRow("OBLIQUE4") << "L\\R" << "OBLIQUE";
    QTest::newRow("OBLIQUE5") << "R\\L" << "OBLIQUE";
    QTest::newRow("OBLIQUE6") << "R\\R" << "OBLIQUE";
    QTest::newRow("OBLIQUE7") << "A\\A" << "OBLIQUE";
    QTest::newRow("OBLIQUE8") << "A\\P" << "OBLIQUE";
    QTest::newRow("OBLIQUE9") << "P\\A" << "OBLIQUE";
    QTest::newRow("OBLIQUE10") << "P\\P" << "OBLIQUE";
    QTest::newRow("OBLIQUE11") << "H\\H" << "OBLIQUE";
    QTest::newRow("OBLIQUE12") << "H\\F" << "OBLIQUE";
    QTest::newRow("OBLIQUE13") << "F\\H" << "OBLIQUE";
    QTest::newRow("OBLIQUE14") << "F\\F" << "OBLIQUE";
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnConcretLabel()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getLabelFromPatientOrientation(patientOrientation), result);
}

DECLARE_TEST(test_AnatomicalPlane)

#include "test_anatomicalplane.moc"
