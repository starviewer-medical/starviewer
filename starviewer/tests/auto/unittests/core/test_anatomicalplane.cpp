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

    void getPlaneFromPatientOrientation_ShouldReturnNotAvailable_data();
    void getPlaneFromPatientOrientation_ShouldReturnNotAvailable();
    void getPlaneFromPatientOrientation_ShouldReturnConcreteLabel_data();
    void getPlaneFromPatientOrientation_ShouldReturnConcreteLabel();

    void getDefaultRadiologicalOrientation_ReturnsExpectedValues_data();
    void getDefaultRadiologicalOrientation_ReturnsExpectedValues();

private:
    void setupShouldReturnNotAvailableData();
    void setupShouldReturnConcreteLabelData();
};

Q_DECLARE_METATYPE(AnatomicalPlane)
Q_DECLARE_METATYPE(PatientOrientation)

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnExpectedLabel_data()
{
    QTest::addColumn<AnatomicalPlane>("plane");
    QTest::addColumn<QString>("planeLabel");

    QTest::newRow("Axial") << AnatomicalPlane(AnatomicalPlane::Axial) << tr("AXIAL");
    QTest::newRow("Sagittal") << AnatomicalPlane(AnatomicalPlane::Sagittal) << tr("SAGITTAL");
    QTest::newRow("Coronal") << AnatomicalPlane(AnatomicalPlane::Coronal) << tr("CORONAL");
    QTest::newRow("Oblique") << AnatomicalPlane(AnatomicalPlane::Oblique) << tr("OBLIQUE");
    QTest::newRow("NotAvailable") << AnatomicalPlane(AnatomicalPlane::NotAvailable) << tr("N/A");
}

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnExpectedLabel()
{
    QFETCH(AnatomicalPlane, plane);
    QFETCH(QString, planeLabel);

    QCOMPARE(plane.getLabel(), planeLabel);
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

void test_AnatomicalPlane::getPlaneFromPatientOrientation_ShouldReturnNotAvailable_data()
{
    this->setupShouldReturnNotAvailableData();
}

void test_AnatomicalPlane::getPlaneFromPatientOrientation_ShouldReturnNotAvailable()
{
    QFETCH(QString, string);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getPlaneFromPatientOrientation(patientOrientation), AnatomicalPlane(AnatomicalPlane::NotAvailable));
}

void test_AnatomicalPlane::getPlaneFromPatientOrientation_ShouldReturnConcreteLabel_data()
{
    this->setupShouldReturnConcreteLabelData();
}

void test_AnatomicalPlane::getPlaneFromPatientOrientation_ShouldReturnConcreteLabel()
{
    QFETCH(QString, string);
    QFETCH(AnatomicalPlane, result);

    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(string);
    
    QCOMPARE(AnatomicalPlane::getPlaneFromPatientOrientation(patientOrientation), result);
}

void test_AnatomicalPlane::getDefaultRadiologicalOrientation_ReturnsExpectedValues_data()
{
    QTest::addColumn<AnatomicalPlane>("plane");
    QTest::addColumn<PatientOrientation>("expectedOrientation");

    PatientOrientation axialOrientation;
    axialOrientation.setLabels(PatientOrientation::LeftLabel, PatientOrientation::PosteriorLabel);
    QTest::newRow("Default Axial orientation") << AnatomicalPlane(AnatomicalPlane::Axial) << axialOrientation;

    PatientOrientation sagittalOrientation;
    sagittalOrientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::FeetLabel);
    QTest::newRow("Default Sagittal orientation") << AnatomicalPlane(AnatomicalPlane::Sagittal) << sagittalOrientation;

    PatientOrientation coronalOrientation;
    coronalOrientation.setLabels(PatientOrientation::LeftLabel, PatientOrientation::FeetLabel);
    QTest::newRow("Default Coronal orientation") << AnatomicalPlane(AnatomicalPlane::Coronal) << coronalOrientation;

    QTest::newRow("Default Oblique orientation") << AnatomicalPlane(AnatomicalPlane::Oblique) << PatientOrientation();
    QTest::newRow("Default N/A orientation") << AnatomicalPlane(AnatomicalPlane::NotAvailable) << PatientOrientation();
}

void test_AnatomicalPlane::getDefaultRadiologicalOrientation_ReturnsExpectedValues()
{
    QFETCH(AnatomicalPlane, plane);
    QFETCH(PatientOrientation, expectedOrientation);

    QCOMPARE(plane.getDefaultRadiologicalOrientation(), expectedOrientation);
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
    QTest::addColumn<AnatomicalPlane>("result");

    QTest::newRow("AXIAL1") << "R\\A" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL2") << "R\\P" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL3") << "L\\A" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL4") << "L\\P" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);

    QTest::newRow("AXIAL5") << "A\\R" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL6") << "A\\L" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL7") << "P\\R" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);
    QTest::newRow("AXIAL8") << "P\\L" << "AXIAL" << AnatomicalPlane(AnatomicalPlane::Axial);

    QTest::newRow("CORONAL1") << "R\\H" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL2") << "R\\F" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL3") << "L\\H" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL4") << "L\\F" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);

    QTest::newRow("CORONAL5") << "H\\R" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL6") << "H\\L" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL7") << "F\\R" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);
    QTest::newRow("CORONAL8") << "F\\L" << "CORONAL" << AnatomicalPlane(AnatomicalPlane::Coronal);

    QTest::newRow("SAGITTAL1") << "A\\H" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL2") << "A\\F" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL3") << "P\\H" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL4") << "P\\F" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);

    QTest::newRow("SAGITTAL5") << "H\\A" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL6") << "H\\P" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL7") << "F\\A" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);
    QTest::newRow("SAGITTAL8") << "F\\P" << "SAGITTAL" << AnatomicalPlane(AnatomicalPlane::Sagittal);

    QTest::newRow("OBLIQUE3") << "L\\L" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE4") << "L\\R" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE5") << "R\\L" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE6") << "R\\R" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE7") << "A\\A" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE8") << "A\\P" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE9") << "P\\A" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE10") << "P\\P" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE11") << "H\\H" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE12") << "H\\F" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE13") << "F\\H" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
    QTest::newRow("OBLIQUE14") << "F\\F" << "OBLIQUE" << AnatomicalPlane(AnatomicalPlane::Oblique);
}

DECLARE_TEST(test_AnatomicalPlane)

#include "test_anatomicalplane.moc"
