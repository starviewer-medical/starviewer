#include "autotest.h"
#include "measurementmanager.h"

#include "image.h"
#include "drawerline.h"
#include "drawerpolygon.h"
#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_MeasurementManager : public QObject {
Q_OBJECT

private:
    void setupGetMeasurementUnitsData();

private slots:
    void getMeasurementUnits_ReturnsExpectedValues_data();
    void getMeasurementUnits_ReturnsExpectedValues();
    
    void getMeasurementUnitsAsQString_ReturnsExpectedValues_data();
    void getMeasurementUnitsAsQString_ReturnsExpectedValues();
    
    void computeDistance_ReturnsExpectedValues_data();
    void computeDistance_ReturnsExpectedValues();
    
    void computeArea_ReturnsExpectedValues_data();
    void computeArea_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementUnitsType)
Q_DECLARE_METATYPE(DrawerLine*)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(DrawerPolygon*)

void test_MeasurementManager::setupGetMeasurementUnitsData()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<MeasurementManager::MeasurementUnitsType>("expectedResult");
    QTest::addColumn<QString>("expectedString");

    Image *image = 0;
    QTest::newRow("null image") << image << MeasurementManager::NotAvailable << tr("N/A");

    image = new Image(this);
    QTest::newRow("default pixel spacing values") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    QTest::newRow("pixel spacing = 0.0, 0.0") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(1.0, 3.0);
    QTest::newRow("pixel spacing = 1.0, 3.0") << image << MeasurementManager::Millimetres << tr("mm");
}

void test_MeasurementManager::getMeasurementUnits_ReturnsExpectedValues_data()
{
    setupGetMeasurementUnitsData();
}

void test_MeasurementManager::getMeasurementUnits_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(MeasurementManager::MeasurementUnitsType, expectedResult);

    QCOMPARE(MeasurementManager::getMeasurementUnits(image), expectedResult);
}

void test_MeasurementManager::getMeasurementUnitsAsQString_ReturnsExpectedValues_data()
{
    setupGetMeasurementUnitsData();
}

void test_MeasurementManager::getMeasurementUnitsAsQString_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(QString, expectedString);

    QCOMPARE(MeasurementManager::getMeasurementUnitsAsQString(image), expectedString);
}

void test_MeasurementManager::computeDistance_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerLine*>("drawerLine");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<double>("expectedDistance");

    DrawerLine *line = 0;
    Image *image = 0;
    double p1[3] = { 8.99, 10.2, 8.97 };
    double p2[3] = { 2.34, 9.02, 8.97 };
    double *spacing = 0;
    
    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2") << line << image << spacing << 6.75388;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75") << line << image << spacing << 10.8062;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    spacing = new double[3];
    spacing[0] = 1.0;
    spacing[1] = 1.0;
    spacing[2] = 1.0;
    QTest::newRow("Image spacing 0,0 - Volume spacing 1, 1") << line << image << spacing << 6.75388;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    spacing = new double[3];
    spacing[0] = 2.0;
    spacing[1] = 2.0;
    spacing[2] = 2.0;
    QTest::newRow("Image spacing 0,0 - Volume spacing 2, 2") << line << image << spacing << 3.37694;
}

void test_MeasurementManager::computeDistance_ReturnsExpectedValues()
{
    QFETCH(DrawerLine*, drawerLine);
    QFETCH(Image*, image);
    QFETCH(double*, spacing);
    QFETCH(double, expectedDistance);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MeasurementManager::computeDistance(drawerLine, image, spacing), expectedDistance, 0.00001));
}

void test_MeasurementManager::computeArea_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerPolygon*>("polygon");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<double>("expectedArea");

    DrawerPolygon *drawerPolygon = 0;
    Image *image = 0;
    double p1[3] = { 0.0, 0.0, 0.0 };
    double p2[3] = { 1.0, 1.0, 0.0 };
    double p3[3] = { 3.0, 4.0, 0.0 };
    double p4[3] = { 4.0, 3.0, 0.0 };
    double *spacing = 0;
    
    QTest::newRow("null polygon") << drawerPolygon << image << spacing << 0.0;
    
    // Axial areas
    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - axial plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - axial plane") << drawerPolygon << image << spacing << 7.68;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - axial plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - axial plane") << drawerPolygon << image << spacing << 0.75;
    
    // Sagital areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap<double>(p2[0], p2[2]);
    qSwap<double>(p3[0], p3[2]);
    qSwap<double>(p4[0], p4[2]);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - sagital plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - sagital plane") << drawerPolygon << image << spacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - sagital plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - sagital plane") << drawerPolygon << image << spacing << 0.75;

    // Coronal areas
    drawerPolygon = new DrawerPolygon(this);
    qSwap<double>(p2[0], p2[1]);
    qSwap<double>(p3[0], p3[1]);
    qSwap<double>(p4[0], p4[1]);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - coronal plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(1.2, 1.2);
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - coronal plane") << drawerPolygon << image << spacing << 4.8;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = 1.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - coronal plane") << drawerPolygon << image << spacing << 3.0;

    drawerPolygon = new DrawerPolygon(this);
    drawerPolygon->addVertix(p1);
    drawerPolygon->addVertix(p2);
    drawerPolygon->addVertix(p3);
    drawerPolygon->addVertix(p4);
    image = new Image(this);
    image->setPixelSpacing(0., 0.);
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = 2.;
    QTest::newRow("Image spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - coronal plane") << drawerPolygon << image << spacing << 0.75;
}

void test_MeasurementManager::computeArea_ReturnsExpectedValues()
{
    QFETCH(DrawerPolygon*, polygon);
    QFETCH(Image*, image);
    QFETCH(double*, spacing);
    QFETCH(double, expectedArea);

    QCOMPARE(MeasurementManager::computeArea(polygon, image, spacing), expectedArea);
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MeasurementManager::computeArea(polygon, image, spacing), expectedArea, 0.00001));
}

DECLARE_TEST(test_MeasurementManager)

#include "test_measurementmanager.moc"
