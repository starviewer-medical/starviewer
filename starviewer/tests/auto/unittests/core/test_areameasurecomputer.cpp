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

using Spacing = std::array<double, 3>;

Q_DECLARE_METATYPE(DrawerPolygon*)
Q_DECLARE_METATYPE(Spacing)
Q_DECLARE_METATYPE(PixelSpacing2D)

void test_AreaMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerPolygon*>("polygon");
    QTest::addColumn<Spacing>("dataSpacing");
    QTest::addColumn<PixelSpacing2D>("desiredSpacing");
    QTest::addColumn<double>("expectedMeasure");

    PixelSpacing2D dummySpacing2D(MathTools::randomDouble(0.1, 6.7), MathTools::randomDouble(0.1, 6.7));
    Spacing dummySpacing;
    dummySpacing[0] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[1] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[2] = MathTools::randomDouble(0.1, 6.7);
    
    DrawerPolygon *drawerPolygon = 0;
    QTest::newRow("null polygon") << drawerPolygon << dummySpacing << dummySpacing2D << 0.0;
    
    Spacing spacing;
    PixelSpacing2D desiredSpacing(1.2, 1.2);
    PixelSpacing2D invalidSpacing(0.0, 0.0);
    
    Vector3 p1{0.0, 0.0, 0.0};
    Vector3 p2{1.0, 1.0, 0.0};
    Vector3 p3{3.0, 4.0, 0.0};
    Vector3 p4{4.0, 3.0, 0.0};
    
    // Axial areas
    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - axial plane") << drawerPolygon << spacing << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - axial plane") << drawerPolygon << spacing << desiredSpacing << 7.68;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - axial plane") << drawerPolygon << spacing << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - axial plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;
    
    // Sagital areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap(p2[0], p2[2]);
    qSwap(p3[0], p3[2]);
    qSwap(p4[0], p4[2]);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - sagital plane") << drawerPolygon << spacing << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - sagital plane") << drawerPolygon << spacing  << desiredSpacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - sagital plane") << drawerPolygon << spacing  << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - sagital plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;

    // Coronal areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap(p2[0], p2[1]);
    qSwap(p3[0], p3[1]);
    qSwap(p4[0], p4[1]);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - coronal plane") << drawerPolygon << spacing  << desiredSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - coronal plane") << drawerPolygon << spacing  << desiredSpacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - coronal plane") << drawerPolygon << spacing  << invalidSpacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertex(p1);
    drawerPolygon->addVertex(p2);
    drawerPolygon->addVertex(p3);
    drawerPolygon->addVertex(p4);
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - coronal plane") << drawerPolygon << spacing  << invalidSpacing << 0.75;
}

void test_AreaMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues()
{
    QFETCH(DrawerPolygon*, polygon);
    QFETCH(Spacing, dataSpacing);
    QFETCH(PixelSpacing2D, desiredSpacing);
    QFETCH(double, expectedMeasure);
    
    AreaMeasureComputer computer(polygon);
    QCOMPARE(computer.computeMeasureExplicit(dataSpacing.data(), desiredSpacing), expectedMeasure);
}

DECLARE_TEST(test_AreaMeasureComputer)

#include "test_areameasurecomputer.moc"
