#include "autotest.h"
#include "measurementmanager.h"

#include "image.h"
#include "series.h"
#include "drawerline.h"
#include "drawerpolygon.h"
#include "fuzzycomparetesthelper.h"
#include "mathtools.h"

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
    
    void getDefaultMeasurementType_ReturnsExpectedValues_data();
    void getDefaultMeasurementType_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementUnitsType)
Q_DECLARE_METATYPE(DrawerLine*)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(DrawerPolygon*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementType)

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

    PixelSpacing2D randomSpacing(MathTools::randomDouble(0.1, 3.5), MathTools::randomDouble(0.1, 3.5));
    image = new Image(this);
    image->setPixelSpacing(randomSpacing);
    QTest::newRow("any valid pixel spacing => mm") << image << MeasurementManager::Millimetres << tr("mm");
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

    line = 0;
    QTest::newRow("null line") << line << image << spacing << 0.0;

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

void test_MeasurementManager::getDefaultMeasurementType_ReturnsExpectedValues_data()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<MeasurementManager::MeasurementType>("expectedType");

    // Pixel Spacing + Estimated Radiographic Magnification Factor tests
    // It does not care about estimated radiographic factor values
    PixelSpacing2D pixelSpacing(0.5, 0.5);
    double dummyFactor = MathTools::randomDouble(-20.5, 50.5);
    
    {
        QStringList unkownMeaningModalities;
        unkownMeaningModalities << "CR" << "DX" << "RF" << "XA" << "MG" << "IO" << "OP" << "XC" << "ES";
        foreach (const QString &modality, unkownMeaningModalities)
        {
            Image *image = new Image(this);
            Series *series = new Series(this);
            series->setModality(modality);
            image->setParentSeries(series);
            image->setPixelSpacing(pixelSpacing);
            image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

            QTest::newRow(qPrintable(QString("%1 - PixelSpacing & EstimatedRadiographicMagnificationFactor(any) present => Unknown meaning").arg(modality))) 
                << image << MeasurementManager::UnknownMeaning;
        }

        QStringList physicalModalities;
        physicalModalities << "CT"  <<  "MR"  <<  "PT"  <<  "SC"  <<  "US"  <<  "NM";
        foreach (const QString &modality, physicalModalities)
        {
            Image *image = new Image(this);
            Series *series = new Series(this);
            series->setModality(modality);
            image->setParentSeries(series);
            image->setPixelSpacing(pixelSpacing);
            image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

            QTest::newRow(qPrintable(QString("%1 - PixelSpacing & EstimatedRadiographicMagnificationFactor(any) present => Physical").arg(modality))) 
                << image << MeasurementManager::Physical;
        }

    }

    // Pixel Spacing + Imager Pixel Spacing + Estimated Radiographic Magnification Factor tests
    // It does not care about modality nor estimated radographic factor values
    // Only if pixel spacing and imager pixel spacing are equal or not
    PixelSpacing2D imagerPixelSpacing(0.25, 0.25);
    {
        Image *image = new Image(this);
        image->setPixelSpacing(pixelSpacing);
        image->setImagerPixelSpacing(imagerPixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

        QTest::newRow("Any modality -PixelSpacing != ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor present(any) => Calibrated") << image << MeasurementManager::Calibrated;
    }

    double randomX = MathTools::randomDouble(2.5, 5.5);
    double randomY = MathTools::randomDouble(2.5, 5.5);
    PixelSpacing2D randomSpacing(randomX, randomY);
    {
        Image *image = new Image(this);
        image->setPixelSpacing(randomSpacing);
        image->setImagerPixelSpacing(randomSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

        QTest::newRow("Any modality -PixelSpacing == ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor present(any) => Calibrated") << image << MeasurementManager::Detector;
    }

    // Imager Pixel Spacing test + Estimated Radiographic Magnification Factor tests
    // Magnification factor only will be applied on MG studies
    {
        Image *image = new Image(this);
        image->setImagerPixelSpacing(imagerPixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);
        QTest::newRow("Any modality - ImagerPixelSpacing && EstimatedRadiographicMagnificationFactor present(any) => Detector") << image << MeasurementManager::Detector;
    }

    {
        double randomFactor = MathTools::randomDouble(0.1, 5.6);
        Image *image = new Image(this);
        Series *series = new Series(this);
        series->setModality("MG");
        image->setParentSeries(series);
        image->setImagerPixelSpacing(imagerPixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(randomFactor);
        QTest::newRow("MG - ImagerPixelSpacing && EstimatedRadiographicMagnificationFactor present(any != 0.0) => Magnified") << image << MeasurementManager::Magnified;
    }
    
    // No pixel spacing attribute is present
    // It doesn't care about any modality
    {
        Image *image = new Image(this);
        QTest::newRow("any modality - no pixel spacing attributes present => NoDefinedUnits") << image << MeasurementManager::NoDefinedUnits;
    }
}

void test_MeasurementManager::getDefaultMeasurementType_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(MeasurementManager::MeasurementType, expectedType);

    QCOMPARE(MeasurementManager::getDefaultMeasurementType(image), expectedType);
}

DECLARE_TEST(test_MeasurementManager)

#include "test_measurementmanager.moc"
