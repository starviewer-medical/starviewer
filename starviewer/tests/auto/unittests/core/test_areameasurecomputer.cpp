#include "autotest.h"

#include "areameasurecomputer.h"
#include "drawerpolygon.h"
#include "pixelspacing2d.h"
#include "mathtools.h"

using namespace udg;

class test_AreaMeasureComputer : public QObject {
Q_OBJECT

private slots:
    void computeMeasureExplicit_ReturnsExpectedValues_data();
    void computeMeasureExplicit_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DrawerPolygon*)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(PixelSpacing2D)

void test_AreaMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerPolygon*>("polygon");
    QTest::addColumn<double*>("dataSpacing");
    QTest::addColumn<PixelSpacing2D>("desiredSpacing");
    QTest::addColumn<double>("expectedMeasure");

    PixelSpacing2D dummySpacing2D(MathTools::randomDouble(0.1, 6.7), MathTools::randomDouble(0.1, 6.7));
    double *dummySpacing = new double[3];
    dummySpacing[0] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[1] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[2] = MathTools::randomDouble(0.1, 6.7);
    
    DrawerPolygon *drawerPolygon = 0;
    QTest::newRow("null polygon") << drawerPolygon << dummySpacing << dummySpacing2D << 0.0;
    
    double *spacing = 0;
    PixelSpacing2D desiredSpacing(1.2, 1.2);
    PixelSpacing2D invalidSpacing(0.0, 0.0);
    
    double p1[3] = { 0.0, 0.0, 0.0 };
    double p2[3] = { 1.0, 1.0, 0.0 };
    double p3[3] = { 3.0, 4.0, 0.0 };
    double p4[3] = { 4.0, 3.0, 0.0 };
    
    // Axial areas
    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - axial plane") << drawerPolygon << spacing << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - axial plane") << drawerPolygon << spacing << desiredSpacing << 7.68;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - axial plane") << drawerPolygon << spacing << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - axial plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;
    
    // Sagital areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap<double>(p2[0], p2[2]);
    qSwap<double>(p3[0], p3[2]);
    qSwap<double>(p4[0], p4[2]);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - sagital plane") << drawerPolygon << spacing << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - sagital plane") << drawerPolygon << spacing  << desiredSpacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - sagital plane") << drawerPolygon << spacing  << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - sagital plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;

    // Coronal areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap<double>(p2[0], p2[1]);
    qSwap<double>(p3[0], p3[1]);
    qSwap<double>(p4[0], p4[1]);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - coronal plane") << drawerPolygon << spacing  << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - coronal plane") << drawerPolygon << spacing  << desiredSpacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - coronal plane") << drawerPolygon << spacing  << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - coronal plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;
}

void test_AreaMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues()
{
    QFETCH(DrawerPolygon*, polygon);
    QFETCH(double*, dataSpacing);
    QFETCH(PixelSpacing2D, desiredSpacing);
    QFETCH(double, expectedMeasure);
    
    AreaMeasureComputer computer(polygon);
    QCOMPARE(computer.computeMeasureExplicit(dataSpacing, desiredSpacing), expectedMeasure);
}

DECLARE_TEST(test_AreaMeasureComputer)

#include "test_areameasurecomputer.moc"