#include "autotest.h"
#include "anatomicalplane.h"

using namespace udg;

class test_AnatomicalPlane : public QObject {
Q_OBJECT

private slots:
    void getPlaneOrientationLabel_ShouldReturnRightLabel_data();
    void getPlaneOrientationLabel_ShouldReturnRightLabel();

    void getProjectionLabelFromPlaneOrientation_ShouldReturnNAOrEmpty_data();
    void getProjectionLabelFromPlaneOrientation_ShouldReturnNAOrEmpty();
    void getProjectionLabelFromPlaneOrientation_ShouldReturnConcretLabel_data();
    void getProjectionLabelFromPlaneOrientation_ShouldReturnConcretLabel();
};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType);

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnRightLabel_data()
{
    QTest::addColumn<AnatomicalPlane::AnatomicalPlaneType>("planeType");
    QTest::addColumn<QString>("planeLabel");

    QTest::newRow("Axial")
        << AnatomicalPlane::Axial << tr("AXIAL");

    QTest::newRow("Sagittal")
        << AnatomicalPlane::Sagittal << tr("SAGITTAL");

    QTest::newRow("Coronal")
        << AnatomicalPlane::Coronal << tr("CORONAL");

    QTest::newRow("Oblique")
        << AnatomicalPlane::Oblique << tr("OBLIQUE");

    QTest::newRow("NotAvailable")
        << AnatomicalPlane::NotAvailable << tr("N/A");
}

void test_AnatomicalPlane::getPlaneOrientationLabel_ShouldReturnRightLabel()
{
    QFETCH(AnatomicalPlane::AnatomicalPlaneType, planeType);
    QFETCH(QString, planeLabel);

    QCOMPARE(AnatomicalPlane::getLabel(planeType), planeLabel);
}

void test_AnatomicalPlane::getProjectionLabelFromPlaneOrientation_ShouldReturnNAOrEmpty_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    const QString NotAvailableResult = "N/A";
    const QString Separator = "\\";
    const QString AnyCorrectData1 = "Axxx";
    const QString AnyCorrectData2 = "Pxxx";

    QTest::newRow("empty") << "" << "";
    QTest::newRow("only separator") << Separator << NotAvailableResult;
    QTest::newRow("more than one separator") << Separator + Separator << NotAvailableResult;
    QTest::newRow("more than one separator with data") << AnyCorrectData1 + Separator + Separator + AnyCorrectData2 << NotAvailableResult;
    QTest::newRow("only one data1") << Separator + AnyCorrectData2 << NotAvailableResult;
    QTest::newRow("only one data2") << AnyCorrectData1 + Separator << NotAvailableResult;
}

void test_AnatomicalPlane::getProjectionLabelFromPlaneOrientation_ShouldReturnNAOrEmpty()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(AnatomicalPlane::getProjectionLabelFromPlaneOrientation(string), result);
}

void test_AnatomicalPlane::getProjectionLabelFromPlaneOrientation_ShouldReturnConcretLabel_data()
{

    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    // xxx vol dir "qualsevol cosa"; només es té en compte la primera lletra
    QTest::newRow("AXIAL1") << "Rxxx\\Axxx" << "AXIAL";
    QTest::newRow("AXIAL2") << "Rxxx\\Pxxx" << "AXIAL";
    QTest::newRow("AXIAL3") << "Lxxx\\Axxx" << "AXIAL";
    QTest::newRow("AXIAL4") << "Lxxx\\Pxxx" << "AXIAL";

    QTest::newRow("AXIAL5") << "Axxx\\Rxxx" << "AXIAL";
    QTest::newRow("AXIAL6") << "Axxx\\Lxxx" << "AXIAL";
    QTest::newRow("AXIAL7") << "Pxxx\\Rxxx" << "AXIAL";
    QTest::newRow("AXIAL8") << "Pxxx\\Lxxx" << "AXIAL";

    QTest::newRow("CORONAL1") << "Rxxx\\Hxxx" << "CORONAL";
    QTest::newRow("CORONAL2") << "Rxxx\\Fxxx" << "CORONAL";
    QTest::newRow("CORONAL3") << "Lxxx\\Hxxx" << "CORONAL";
    QTest::newRow("CORONAL4") << "Lxxx\\Fxxx" << "CORONAL";

    QTest::newRow("CORONAL5") << "Hxxx\\Rxxx" << "CORONAL";
    QTest::newRow("CORONAL6") << "Hxxx\\Lxxx" << "CORONAL";
    QTest::newRow("CORONAL7") << "Fxxx\\Rxxx" << "CORONAL";
    QTest::newRow("CORONAL8") << "Fxxx\\Lxxx" << "CORONAL";

    QTest::newRow("SAGITTAL1") << "Axxx\\Hxxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL2") << "Axxx\\Fxxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL3") << "Pxxx\\Hxxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL4") << "Pxxx\\Fxxx" << "SAGITTAL";

    QTest::newRow("SAGITTAL5") << "Hxxx\\Axxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL6") << "Hxxx\\Pxxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL7") << "Fxxx\\Axxx" << "SAGITTAL";
    QTest::newRow("SAGITTAL8") << "Fxxx\\Pxxx" << "SAGITTAL";

    QTest::newRow("OBLIQUE1") << "x\\x" << "OBLIQUE";
    QTest::newRow("OBLIQUE2") << "x\\x\\x" << "OBLIQUE";
    QTest::newRow("OBLIQUE2") << "asdf\\asdf\\asdf" << "OBLIQUE";
    QTest::newRow("OBLIQUE3") << "Lxxx\\Lxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE4") << "Lxxx\\Rxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE5") << "Rxxx\\Lxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE6") << "Rxxx\\Rxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE7") << "Axxx\\Axxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE8") << "Axxx\\Pxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE9") << "Pxxx\\Axxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE10") << "Pxxx\\Pxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE11") << "Hxxx\\Hxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE12") << "Hxxx\\Fxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE13") << "Fxxx\\Hxxx" << "OBLIQUE";
    QTest::newRow("OBLIQUE14") << "Fxxx\\Fxxx" << "OBLIQUE";
}

void test_AnatomicalPlane::getProjectionLabelFromPlaneOrientation_ShouldReturnConcretLabel()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(AnatomicalPlane::getProjectionLabelFromPlaneOrientation(string), result);
}

DECLARE_TEST(test_AnatomicalPlane)

#include "test_anatomicalplane.moc"
