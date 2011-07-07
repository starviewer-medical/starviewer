#include "autotest.h"
#include "anatomicalplane.h"
#include "patientorientation.h"

using namespace udg;

class test_AnatomicalPlane : public QObject {
Q_OBJECT

private slots:
    void getPlaneOrientationLabel_ShouldReturnExpectedLabel_data();
    void getPlaneOrientationLabel_ShouldReturnExpectedLabel();

    void getLabelFromPatientOrientation_ShouldReturnNotAvailable_data();
    void getLabelFromPatientOrientation_ShouldReturnNotAvailable();
    void getLabelFromPatientOrientation_ShouldReturnConcreteLabel_data();
    void getLabelFromPatientOrientation_ShouldReturnConcreteLabel();

    void getPlaneTypeFromPatientOrientation_ShouldReturnNotAvailable_data();
    void getPlaneTypeFromPatientOrientation_ShouldReturnNotAvailable();
    void getPlaneTypeFromPatientOrientation_ShouldReturnConcreteLabel_data();
    void getPlaneTypeFromPatientOrientation_ShouldReturnConcreteLabel();

private:
    void setupShouldReturnNotAvailableData();
    void setupShouldReturnConcreteLabelData();
};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType);

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnExpectedLabel_data()
{
    QTest::addColumn<AnatomicalPlane::AnatomicalPlaneType>("planeType");
    QTest::addColumn<QString>("planeLabel");

    QTest::newRow("Axial") << AnatomicalPlane::Axial << tr("AXIAL");
    QTest::newRow("Sagittal") << AnatomicalPlane::Sagittal << tr("SAGITTAL");
    QTest::newRow("Coronal") << AnatomicalPlane::Coronal << tr("CORONAL");
    QTest::newRow("Oblique") << AnatomicalPlane::Oblique << tr("OBLIQUE");
    QTest::newRow("NotAvailable") << AnatomicalPlane::NotAvailable << tr("N/A");
}

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnExpectedLabel()
{
    QFETCH(AnatomicalPlane::AnatomicalPlaneType, planeType);
    QFETCH(QString, planeLabel);

    QCOMPARE(AnatomicalPlane::getLabel(planeType), planeLabel);
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnNotAvailable_data()
{
    this->setupShouldReturnNotAvailableData();
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnNotAvailable()
{
    QFETCH(QString, string);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getLabelFromPatientOrientation(patientOrientation), tr("N/A"));
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnConcreteLabel_data()
{
    this->setupShouldReturnConcreteLabelData();
}

void test_AnatomicalPlane::getLabelFromPatientOrientation_ShouldReturnConcreteLabel()
{
    QFETCH(QString, string);
    QFETCH(QString, labelResult);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getLabelFromPatientOrientation(patientOrientation), labelResult);
}

void test_AnatomicalPlane::getPlaneTypeFromPatientOrientation_ShouldReturnNotAvailable_data()
{
    this->setupShouldReturnNotAvailableData();
}

void test_AnatomicalPlane::getPlaneTypeFromPatientOrientation_ShouldReturnNotAvailable()
{
    QFETCH(QString, string);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getPlaneTypeFromPatientOrientation(patientOrientation), AnatomicalPlane::NotAvailable);
}

void test_AnatomicalPlane::getPlaneTypeFromPatientOrientation_ShouldReturnConcreteLabel_data()
{
    this->setupShouldReturnConcreteLabelData();
}

void test_AnatomicalPlane::getPlaneTypeFromPatientOrientation_ShouldReturnConcreteLabel()
{
    QFETCH(QString, string);
    QFETCH(AnatomicalPlane::AnatomicalPlaneType, typeResult);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getPlaneTypeFromPatientOrientation(patientOrientation), typeResult);
}

void test_AnatomicalPlane::setupShouldReturnNotAvailableData()
{
    QTest::addColumn<QString>("string");

    const QString Separator = "\\";
    const QString AnyCorrectData1 = "A";
    const QString AnyCorrectData2 = "P";

    QTest::newRow("empty") << "";
    QTest::newRow("only separator") << Separator;
    QTest::newRow("more than one separator") << Separator + Separator;
    QTest::newRow("more than one separator with data") << AnyCorrectData1 + Separator + Separator + AnyCorrectData2;
    QTest::newRow("only one data1") << Separator + AnyCorrectData2;
    QTest::newRow("only one data2") << AnyCorrectData1 + Separator;
    QTest::newRow("2 invalid items") << "x\\x";
    QTest::newRow("3 invalid items") << "x\\x\\x";
    QTest::newRow("3 invalid (multivalued) items") << "asdf\\asdf\\asdf";
}

void test_AnatomicalPlane::setupShouldReturnConcreteLabelData()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("labelResult");
    QTest::addColumn<AnatomicalPlane::AnatomicalPlaneType>("typeResult");

    QTest::newRow("AXIAL1") << "R\\A" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL2") << "R\\P" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL3") << "L\\A" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL4") << "L\\P" << "AXIAL" << AnatomicalPlane::Axial;

    QTest::newRow("AXIAL5") << "A\\R" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL6") << "A\\L" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL7") << "P\\R" << "AXIAL" << AnatomicalPlane::Axial;
    QTest::newRow("AXIAL8") << "P\\L" << "AXIAL" << AnatomicalPlane::Axial;

    QTest::newRow("CORONAL1") << "R\\H" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL2") << "R\\F" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL3") << "L\\H" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL4") << "L\\F" << "CORONAL" << AnatomicalPlane::Coronal;

    QTest::newRow("CORONAL5") << "H\\R" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL6") << "H\\L" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL7") << "F\\R" << "CORONAL" << AnatomicalPlane::Coronal;
    QTest::newRow("CORONAL8") << "F\\L" << "CORONAL" << AnatomicalPlane::Coronal;

    QTest::newRow("SAGITTAL1") << "A\\H" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL2") << "A\\F" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL3") << "P\\H" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL4") << "P\\F" << "SAGITTAL" << AnatomicalPlane::Sagittal;

    QTest::newRow("SAGITTAL5") << "H\\A" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL6") << "H\\P" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL7") << "F\\A" << "SAGITTAL" << AnatomicalPlane::Sagittal;
    QTest::newRow("SAGITTAL8") << "F\\P" << "SAGITTAL" << AnatomicalPlane::Sagittal;

    QTest::newRow("OBLIQUE3") << "L\\L" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE4") << "L\\R" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE5") << "R\\L" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE6") << "R\\R" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE7") << "A\\A" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE8") << "A\\P" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE9") << "P\\A" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE10") << "P\\P" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE11") << "H\\H" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE12") << "H\\F" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE13") << "F\\H" << "OBLIQUE" << AnatomicalPlane::Oblique;
    QTest::newRow("OBLIQUE14") << "F\\F" << "OBLIQUE" << AnatomicalPlane::Oblique;
}

DECLARE_TEST(test_AnatomicalPlane)

#include "test_anatomicalplane.moc"
