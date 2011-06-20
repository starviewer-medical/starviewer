#include "autotest.h"
#include "anatomicalplane.h"

using namespace udg;

class test_AnatomicalPlane : public QObject {
Q_OBJECT

private slots:
    void getPlaneOrientationLabel_ShouldReturnRightLabel_data();
    void getPlaneOrientationLabel_ShouldReturnRightLabel();
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

DECLARE_TEST(test_AnatomicalPlane)

#include "test_anatomicalplane.moc"
