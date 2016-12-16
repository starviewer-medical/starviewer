#include "autotest.h"
#include "measurementmanager.h"

#include "image.h"
#include "series.h"
#include "drawerline.h"
#include "drawerpolygon.h"
#include "fuzzycomparetesthelper.h"
#include "seriestesthelper.h"
#include "mathtools.h"
#include "distancemeasurecomputer.h"
#include "areameasurecomputer.h"

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

    void getMeasurementForDisplay_ReturnsExpectedString_data();
    void getMeasurementForDisplay_ReturnsExpectedString();
    
    void getMeasurementForDisplay_DistanceComputer_ReturnsExpectedString_data();
    void getMeasurementForDisplay_DistanceComputer_ReturnsExpectedString();

    void getMeasurementForDisplay_AreaComputer_ReturnsExpectedString_data();
    void getMeasurementForDisplay_AreaComputer_ReturnsExpectedString();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementUnitsType)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementType)
Q_DECLARE_METATYPE(MeasureComputer*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementDisplayVerbosityType)

void test_MeasurementManager::setupGetMeasurementUnitsData()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<MeasurementManager::MeasurementUnitsType>("expectedResult");
    QTest::addColumn<QString>("expectedString");

    Image *image = 0;
    Series *series = 0;
    
    QTest::newRow("null image") << image << MeasurementManager::NotAvailable << tr("N/A");

    image = new Image(this);
    QTest::newRow("default pixel spacing attributes values") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    QTest::newRow("pixel spacing = 0.0, 0.0") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setImagerPixelSpacing(0.0, 0.0);
    QTest::newRow("imager pixel spacing = 0.0, 0.0") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    image->setImagerPixelSpacing(0.0, 0.0);
    QTest::newRow("imager pixel spacing && pixel spacing = 0.0, 0.0") << image << MeasurementManager::Pixels << tr("px");

    PixelSpacing2D randomSpacing(MathTools::randomDouble(0.1, 3.5), MathTools::randomDouble(0.1, 3.5));
    QStringList knownModalitiesInMM;
    knownModalitiesInMM << "CR" << "DX" << "RF" << "XA" << "MG" << "IO" << "OP" << "XC" << "ES" << "CT" << "MR" << "PT" << "SC" << "US" << "NM";
    
    foreach (const QString &modality, knownModalitiesInMM)
    {
        image = new Image(this);
        image->setPixelSpacing(randomSpacing);
        series = new Series(this);
        series->setModality(modality);
        image->setParentSeries(series);
        QTest::newRow(qPrintable(modality + ": valid pixel spacing = > mm")) << image << MeasurementManager::Millimetres << tr("mm");
    }

    foreach (const QString &modality, knownModalitiesInMM)
    {
        image = new Image(this);
        image->setImagerPixelSpacing(randomSpacing);
        series = new Series(this);
        series->setModality(modality);
        image->setParentSeries(series);
        QTest::newRow(qPrintable(modality + ": valid imager pixel spacing = > mm")) << image << MeasurementManager::Millimetres << tr("mm");
    }

    PixelSpacing2D randomSpacing2(MathTools::randomDouble(0.1, 3.5), MathTools::randomDouble(0.1, 3.5));
    foreach (const QString &modality, knownModalitiesInMM)
    {
        image = new Image(this);
        image->setPixelSpacing(randomSpacing);
        image->setImagerPixelSpacing(randomSpacing2);
        series = new Series(this);
        series->setModality(modality);
        image->setParentSeries(series);
        QTest::newRow(qPrintable(modality + ": valid imager pixel spacing & pixel spacing (both present)= > mm")) << image << MeasurementManager::Millimetres << tr("mm");
    }
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

void test_MeasurementManager::getMeasurementForDisplay_ReturnsExpectedString_data()
{
    QTest::addColumn<MeasureComputer*>("measureComputer");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<MeasurementManager::MeasurementDisplayVerbosityType>("verbosity");
    QTest::addColumn<QString>("expectedString");

    // Setup measure computer
    DrawerLine *line = new DrawerLine(this);
    Vector3 p1{8.99, 10.2, 8.97};
    Vector3 p2{2.34, 9.02, 8.97};
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    MeasureComputer *anyMeasureComputer = new DistanceMeasureComputer(line);
    
    PixelSpacing2D pixelSpacing(1.2, 1.2);
    
    // Image with no pixel spacing and no imager pixel spacing
    Image *imageWithNoPixelSpacingAttributes = new Image(this);
    Series *seriesWithRandomModality = new Series(this);
    seriesWithRandomModality->setModality(SeriesTestHelper::getRandomModality());
    imageWithNoPixelSpacingAttributes->setParentSeries(seriesWithRandomModality);

    QList<MeasurementManager::MeasurementDisplayVerbosityType> verbosities;
    verbosities << MeasurementManager::Minimal << MeasurementManager::MinimalExplicit << MeasurementManager::Verbose << MeasurementManager::VerboseExplicit;
    double *nullSpacing = 0;
    double randomZSpacing = MathTools::randomDouble(0.1, 9.8);
    double *spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = randomZSpacing;
    
    foreach (MeasurementManager::MeasurementDisplayVerbosityType verbosity, verbosities)
    {
        QTest::newRow("Image with no pixel spacing attributes, any modality, null data spacing, any verbosity") << anyMeasureComputer << imageWithNoPixelSpacingAttributes << nullSpacing << verbosity << "7 px";
        QTest::newRow("Image with no pixel spacing attributes, any modality, data spacing, , any verbosity") << anyMeasureComputer << imageWithNoPixelSpacingAttributes << spacing << verbosity << "6 px";
    }
    
    // Image with pixel spacing, no imager pixel spacing
    Image *imageWithPixelSpacing = new Image(this);
    imageWithPixelSpacing->setPixelSpacing(pixelSpacing);
    imageWithPixelSpacing->setParentSeries(seriesWithRandomModality);

    foreach (MeasurementManager::MeasurementDisplayVerbosityType verbosity, verbosities)
    {
        QTest::newRow("Image with pixel spacing, any modality, data spacing, any verbosity") << anyMeasureComputer << imageWithPixelSpacing << spacing << verbosity << "6.75 mm";
    }
    
    // Image with pixel spacing and imager pixel spacing
    // Same values
    Image *imageWithSamePixelSpacingAndImagerPixelSpacing = new Image(this);
    imageWithSamePixelSpacingAndImagerPixelSpacing->setPixelSpacing(pixelSpacing);
    imageWithSamePixelSpacingAndImagerPixelSpacing->setImagerPixelSpacing(pixelSpacing);
    imageWithSamePixelSpacingAndImagerPixelSpacing->setParentSeries(seriesWithRandomModality);

    QTest::newRow("Image with pixel spacing & imager pixel spacing, same values, any modality, Minimal") 
        << anyMeasureComputer << imageWithSamePixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::Minimal << "6.75 mm";
    
    QTest::newRow("Image with pixel spacing & imager pixel spacing, same values, any modality, MinimalExplicit") 
        << anyMeasureComputer << imageWithSamePixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::MinimalExplicit << "6.75 mm (detector)";
    
    QTest::newRow("Image with pixel spacing & imager pixel spacing, same values, any modality, Verbose") 
        << anyMeasureComputer << imageWithSamePixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::Verbose << "6.75 mm";
    
    QTest::newRow("Image with pixel spacing & imager pixel spacing, same values, any modality, VerboseExplicit") 
        << anyMeasureComputer << imageWithSamePixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::VerboseExplicit << "6.75 mm (detector)";
    
    // Different values
    PixelSpacing2D imagerPixelSpacing(0.8, 0.8);
    Image *imageWithDifferentPixelSpacingAndImagerPixelSpacing = new Image(this);
    imageWithDifferentPixelSpacingAndImagerPixelSpacing->setPixelSpacing(pixelSpacing);
    imageWithDifferentPixelSpacingAndImagerPixelSpacing->setImagerPixelSpacing(imagerPixelSpacing);
    imageWithDifferentPixelSpacingAndImagerPixelSpacing->setParentSeries(seriesWithRandomModality);

    QTest::newRow("Image with pixel spacing & imager pixel spacing, different values, any modality, Minimal") 
        << anyMeasureComputer << imageWithDifferentPixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::Minimal << "6.75 mm (calibrated)";
    
    QTest::newRow("Image with pixel spacing & imager pixel spacing, different values, any modality, MinimalExplicit") 
        << anyMeasureComputer << imageWithDifferentPixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::MinimalExplicit << "6.75 mm (calibrated)";

    QTest::newRow("Image with pixel spacing & imager pixel spacing, different values, any modality, Verbose") 
        << anyMeasureComputer << imageWithDifferentPixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::Verbose << "6.75 mm (calibrated)\n4.50 mm";

    QTest::newRow("Image with pixel spacing & imager pixel spacing, different values, any modality, VerboseExplicit") 
        << anyMeasureComputer << imageWithDifferentPixelSpacingAndImagerPixelSpacing << spacing << MeasurementManager::VerboseExplicit << "6.75 mm (calibrated)\n4.50 mm (detector)";
    
    // Image with imager pixel spacing
    Image *imageWithImagerPixelSpacingOnly = new Image(this);
    imageWithImagerPixelSpacingOnly->setImagerPixelSpacing(imagerPixelSpacing);
    imageWithImagerPixelSpacingOnly->setParentSeries(seriesWithRandomModality);

    QTest::newRow("Image with imager pixel spacing, any modality, Minimal") 
        << anyMeasureComputer << imageWithImagerPixelSpacingOnly << spacing << MeasurementManager::Minimal << "4.50 mm";
    
    QTest::newRow("Image with imager pixel spacing, any modality, MinimalExplicit") 
        << anyMeasureComputer << imageWithImagerPixelSpacingOnly << spacing << MeasurementManager::MinimalExplicit << "4.50 mm (detector)";

    QTest::newRow("Image with imager pixel spacing, any modality, Verbose") 
        << anyMeasureComputer << imageWithImagerPixelSpacingOnly << spacing << MeasurementManager::Verbose << "4.50 mm";

    QTest::newRow("Image with imager pixel spacing, any modality, VerboseExplicit") 
        << anyMeasureComputer << imageWithImagerPixelSpacingOnly << spacing << MeasurementManager::VerboseExplicit << "4.50 mm (detector)";
    
    // MG Image, with magnification factor
    double magnificationFactor = 1.25;
    Image *MGimageWithImagerPixelSpacingAndMagnificationFactor = new Image(this);
    MGimageWithImagerPixelSpacingAndMagnificationFactor->setImagerPixelSpacing(imagerPixelSpacing);
    MGimageWithImagerPixelSpacingAndMagnificationFactor->setEstimatedRadiographicMagnificationFactor(magnificationFactor);
    Series *seriesMG = new Series(this);
    seriesMG->setModality("MG");
    MGimageWithImagerPixelSpacingAndMagnificationFactor->setParentSeries(seriesMG);

    QTest::newRow("MG Image with imager pixel spacing + magnificationFactor, Minimal") 
        << anyMeasureComputer << MGimageWithImagerPixelSpacingAndMagnificationFactor << spacing << MeasurementManager::Minimal << "3.60 mm (magnified)";
    
    QTest::newRow("MG Image with imager pixel spacing + magnificationFactor, MinimalExplicit") 
        << anyMeasureComputer << MGimageWithImagerPixelSpacingAndMagnificationFactor << spacing << MeasurementManager::MinimalExplicit << "3.60 mm (magnified)";

    QTest::newRow("MG Image with imager pixel spacing + magnificationFactor, Verbose") 
        << anyMeasureComputer << MGimageWithImagerPixelSpacingAndMagnificationFactor << spacing << MeasurementManager::Verbose << "3.60 mm (magnified)\n4.50 mm";

    QTest::newRow("MG Image with imager pixel spacing + magnificationFactor, VerboseExplicit") 
        << anyMeasureComputer << MGimageWithImagerPixelSpacingAndMagnificationFactor << spacing << MeasurementManager::VerboseExplicit << "3.60 mm (magnified)\n4.50 mm (detector)";
}

void test_MeasurementManager::getMeasurementForDisplay_ReturnsExpectedString()
{
    QFETCH(MeasureComputer*, measureComputer);
    QFETCH(Image*, image);
    QFETCH(double*, spacing);
    QFETCH(MeasurementManager::MeasurementDisplayVerbosityType, verbosity);
    QFETCH(QString, expectedString);

    QCOMPARE(MeasurementManager::getMeasurementForDisplay(measureComputer, image, spacing, verbosity), expectedString);
}

void test_MeasurementManager::getMeasurementForDisplay_DistanceComputer_ReturnsExpectedString_data()
{
    QTest::addColumn<MeasureComputer*>("distanceMeasureComputer");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<QString>("expectedString");

    MeasureComputer *distanceMeasureComputer = 0;
    DrawerLine *nullLine = 0;
    Image *imageWithPixelSpacing = 0;
    Series *seriesWithRandomModality = 0;
    double *nullSpacing = 0;
    
    PixelSpacing2D pixelSpacing(1.2, 1.2);
    imageWithPixelSpacing = new Image(this);
    imageWithPixelSpacing->setPixelSpacing(pixelSpacing);
    seriesWithRandomModality = new Series(this);
    seriesWithRandomModality->setModality(SeriesTestHelper::getRandomModality());
    imageWithPixelSpacing->setParentSeries(seriesWithRandomModality);
    distanceMeasureComputer = new DistanceMeasureComputer(nullLine);
    QTest::newRow("null line, image with pixel spacing, null data spacing") << distanceMeasureComputer << imageWithPixelSpacing << nullSpacing << "0.00 mm";
    
    double *randomSpacing = new double[3];
    randomSpacing[0] = MathTools::randomDouble(0.1, 9.7);
    randomSpacing[1] = MathTools::randomDouble(0.1, 9.7);
    randomSpacing[2] = MathTools::randomDouble(0.1, 9.7);
    distanceMeasureComputer = new DistanceMeasureComputer(nullLine);
    QTest::newRow("null line, image with pixel spacing, random data spacing") << distanceMeasureComputer << imageWithPixelSpacing << randomSpacing << "0.00 mm";

    double *spacing = 0;
    double randomZSpacing = MathTools::randomDouble(0.1, 9.8);
    Vector3 p1{8.99, 10.2, 8.97};
    Vector3 p2{2.34, 9.02, 8.97};
    DrawerLine *line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = randomZSpacing;
    distanceMeasureComputer = new DistanceMeasureComputer(line);
    QTest::newRow("Image pixel spacing 1.2, 1.2 - Volume spacing 1.2, 1.2") << distanceMeasureComputer << imageWithPixelSpacing << spacing << "6.75 mm";
    
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = randomZSpacing;
    distanceMeasureComputer = new DistanceMeasureComputer(line);
    QTest::newRow("Image pixel spacing 1.2, 1.2 - Volume spacing 0.75, 0.75") << distanceMeasureComputer << imageWithPixelSpacing << spacing << "10.81 mm";

    PixelSpacing2D invalidPixelSpacing(0.0, 0.0);
    Image *imageWithInvalidPixelSpacing = new Image(this);
    imageWithInvalidPixelSpacing->setPixelSpacing(invalidPixelSpacing);
    imageWithInvalidPixelSpacing->setParentSeries(seriesWithRandomModality);

    distanceMeasureComputer = new DistanceMeasureComputer(nullLine);
    QTest::newRow("null line, image with no pixel spacing, null spacing") << distanceMeasureComputer << imageWithInvalidPixelSpacing << nullSpacing << "0 px";
    distanceMeasureComputer = new DistanceMeasureComputer(nullLine);
    QTest::newRow("null line, image with no pixel spacing, random spacing") << distanceMeasureComputer << imageWithInvalidPixelSpacing << randomSpacing << "0 px";

    spacing = new double[3];
    spacing[0] = 1.0;
    spacing[1] = 1.0;
    spacing[2] = randomZSpacing;
    distanceMeasureComputer = new DistanceMeasureComputer(line);
    QTest::newRow("Image pixel spacing 0,0 - Volume spacing 1, 1") << distanceMeasureComputer << imageWithInvalidPixelSpacing << spacing << "7 px";
    
    spacing = new double[3];
    spacing[0] = 2.0;
    spacing[1] = 2.0;
    spacing[2] = randomZSpacing;
    distanceMeasureComputer = new DistanceMeasureComputer(line);
    QTest::newRow("Image pixel spacing 0,0 - Volume spacing 2, 2") << distanceMeasureComputer << imageWithInvalidPixelSpacing << spacing << "3 px";
}

void test_MeasurementManager::getMeasurementForDisplay_DistanceComputer_ReturnsExpectedString()
{
    QFETCH(MeasureComputer*, distanceMeasureComputer);
    QFETCH(Image*, image);
    QFETCH(double*, spacing);
    QFETCH(QString, expectedString);

    QCOMPARE(MeasurementManager::getMeasurementForDisplay(distanceMeasureComputer, image, spacing), expectedString);
    delete distanceMeasureComputer;
}

void test_MeasurementManager::getMeasurementForDisplay_AreaComputer_ReturnsExpectedString_data()
{
    QTest::addColumn<MeasureComputer*>("areaMeasureComputer");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<QString>("expectedString");

    MeasureComputer *areaMeasureComputer = 0;
    DrawerPolygon *nullPolygon = 0;
    Image *imageWithPixelSpacing = 0;
    Series *seriesWithRandomModality = 0;
    double *nullSpacing = 0;
    
    PixelSpacing2D pixelSpacing(1.2, 1.2);
    imageWithPixelSpacing = new Image(this);
    imageWithPixelSpacing->setPixelSpacing(pixelSpacing);
    seriesWithRandomModality = new Series(this);
    seriesWithRandomModality->setModality(SeriesTestHelper::getRandomModality());
    imageWithPixelSpacing->setParentSeries(seriesWithRandomModality);
    
    areaMeasureComputer = new AreaMeasureComputer(nullPolygon);
    QTest::newRow("null polygon, image with pixel spacing, null data spacing") << areaMeasureComputer << imageWithPixelSpacing << nullSpacing << "0.00 mm2";
    
    double *randomSpacing = new double[3];
    randomSpacing[0] = MathTools::randomDouble(0.1, 9.7);
    randomSpacing[1] = MathTools::randomDouble(0.1, 9.7);
    randomSpacing[2] = MathTools::randomDouble(0.1, 9.7);
    areaMeasureComputer = new AreaMeasureComputer(nullPolygon);
    QTest::newRow("null polygon, image with pixel spacing, random data spacing") << areaMeasureComputer << imageWithPixelSpacing << randomSpacing << "0.00 mm2";
    
    Vector3 p1{0.0, 0.0, 0.0};
    Vector3 p2{1.0, 1.0, 0.0};
    Vector3 p3{3.0, 4.0, 0.0};
    Vector3 p4{4.0, 3.0, 0.0};
    
    double *spacing = 0;
    
    // Axial areas
    DrawerPolygon *axialPolygon = new DrawerPolygon(this);
    axialPolygon->addVertex(p1);
    axialPolygon->addVertex(p2);
    axialPolygon->addVertex(p3);
    axialPolygon->addVertex(p4);
    
    double randomZSpacing = MathTools::randomDouble(0.1, 9.8);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = randomZSpacing;
    areaMeasureComputer = new AreaMeasureComputer(axialPolygon);
    QTest::newRow("Image pixel spacing 1.2, 1.2 - Volume spacing 1.2, 1.2 - axial plane") << areaMeasureComputer << imageWithPixelSpacing << spacing << "3.00 mm2";
    
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = randomZSpacing;
    areaMeasureComputer = new AreaMeasureComputer(axialPolygon);
    QTest::newRow("Image pixel spacing 1.2, 1.2 - Volume spacing 0.75, 0.75 - axial plane") << areaMeasureComputer << imageWithPixelSpacing << spacing << "7.68 mm2";
    
    PixelSpacing2D invalidPixelSpacing(0.0, 0.0);
    Image *imageWithInvalidPixelSpacing = new Image(this);
    imageWithInvalidPixelSpacing->setPixelSpacing(invalidPixelSpacing);
    imageWithInvalidPixelSpacing->setParentSeries(seriesWithRandomModality);
    spacing = new double[3];
    spacing[0] = 1.;
    spacing[1] = 1.;
    spacing[2] = randomZSpacing;
    areaMeasureComputer = new AreaMeasureComputer(axialPolygon);
    QTest::newRow("Image pixel spacing 0.0, 0.0 - Volume spacing 1.0, 1.0 - axial plane") << areaMeasureComputer << imageWithInvalidPixelSpacing << spacing << "3 px2";
    
    spacing = new double[3];
    spacing[0] = 2.;
    spacing[1] = 2.;
    spacing[2] = randomZSpacing;
    areaMeasureComputer = new AreaMeasureComputer(axialPolygon);
    QTest::newRow("Image pixel spacing 0.0, 0.0 - Volume spacing 2.0, 2.0 - axial plane") << areaMeasureComputer << imageWithInvalidPixelSpacing << spacing << "1 px2";
}

void test_MeasurementManager::getMeasurementForDisplay_AreaComputer_ReturnsExpectedString()
{
    QFETCH(MeasureComputer*, areaMeasureComputer);
    QFETCH(Image*, image);
    QFETCH(double*, spacing);
    QFETCH(QString, expectedString);

    QCOMPARE(MeasurementManager::getMeasurementForDisplay(areaMeasureComputer, image, spacing), expectedString);
    delete areaMeasureComputer;
}

DECLARE_TEST(test_MeasurementManager)

#include "test_measurementmanager.moc"
