#include "autotest.h"
#include "preferredpixelspacingselector.h"

#include "image.h"
#include "series.h"
#include "mathtools.h"

using namespace udg;

class test_PreferredPixelSpacingSelector : public QObject {
Q_OBJECT

private slots:
    void getPreferredPixelSpacing_ReturnsExpectedSpacing_data();
    void getPreferredPixelSpacing_ReturnsExpectedSpacing();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(PixelSpacing2D)

void test_PreferredPixelSpacingSelector::getPreferredPixelSpacing_ReturnsExpectedSpacing_data()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<PixelSpacing2D>("expectedSpacing");

    PixelSpacing2D pixelSpacing(0.5, 0.5);
    double dummyFactor = MathTools::randomDouble(-20.5, 50.5);
    
    // Pixel Spacing + Estimated Radiographic Magnification Factor tests
    // It does not care about modality nor estimated radographic factor values
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        image->setParentSeries(series);
        image->setPixelSpacing(pixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

        QTest::newRow("PixelSpacing & EstimatedRadiographicMagnificationFactor(any) present => pixel spacing") << image << pixelSpacing;
    }
    
    double dummyX = MathTools::randomDouble(2.5, 5.5);
    double dummyY = MathTools::randomDouble(2.5, 5.5);
    PixelSpacing2D dummySpacing(dummyX, dummyY);
    // Pixel Spacing + Imager Pixel Spacing + Estimated Radiographic Magnification Factor tests
    // It does not care about modality, imager pixel spacing nor estimated radographic factor values
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        image->setParentSeries(series);
        image->setPixelSpacing(pixelSpacing);
        image->setImagerPixelSpacing(dummySpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

        QTest::newRow("Any modality -PixelSpacing & ImagerPixelSpacing(any) & EstimatedRadiographicMagnificationFactor present(any) => pixel spacing") << image << pixelSpacing;
    }

    PixelSpacing2D imagerPixelSpacing(0.25, 0.25);
    // Imager Pixel Spacing test
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        image->setParentSeries(series);
        image->setImagerPixelSpacing(imagerPixelSpacing);

        QTest::newRow("Any modality - ImagerPixelSpacing present => imager pixel spacing") << image << imagerPixelSpacing;
    }

    // If modality is MG, default magnification factor is 1, so unmodified Imager Pixel Spacing should be returned
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        series->setModality("MG");
        image->setParentSeries(series);
        image->setImagerPixelSpacing(imagerPixelSpacing);

        QTest::newRow("MG - ImagerPixelSpacing present => imager pixel spacing") << image << imagerPixelSpacing;
    }
    
    double factor = 1.5;
    PixelSpacing2D magnifiedSpacing;
    magnifiedSpacing.setX(imagerPixelSpacing.x() / factor);
    magnifiedSpacing.setY(imagerPixelSpacing.y() / factor);
    // Imager Pixel Spacing + Estimated Radiographic Magnification Factor tests
    // Pixel spacing should be empty. It does not care about modality.
    // Estimated Radiographic Magnification Factor is only taken into account when modality is MG
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        series->setModality("MG");
        image->setParentSeries(series);
        image->setImagerPixelSpacing(imagerPixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(factor);

        QTest::newRow("MG - ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor present => imager pixel spacing corrected by magnification factor") 
            << image << magnifiedSpacing;
    }

    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        image->setParentSeries(series);
        image->setImagerPixelSpacing(imagerPixelSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);
        
        QTest::newRow("Any modality != MG - ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor(any) present => imager pixel spacing, magnification is not used") 
            << image << imagerPixelSpacing;
    }

    // No pixel spacing attributes present. Default constructed value should be returned
    {
        Image *image = new Image(this);
        Series *series = new Series(this);
        image->setParentSeries(series);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);
        
        QTest::newRow("Any modality - no pixel spacing attributes present & EstimatedRadiographicMagnificationFactor(any) present => default constructed value") 
            << image << PixelSpacing2D();
    }
}

void test_PreferredPixelSpacingSelector::getPreferredPixelSpacing_ReturnsExpectedSpacing()
{
    QFETCH(Image*, image);
    QFETCH(PixelSpacing2D, expectedSpacing);

    PreferredPixelSpacingSelector preferredSpacingSelector;

    QVERIFY(preferredSpacingSelector.getPreferredPixelSpacing(image).isEqual(expectedSpacing));
}

DECLARE_TEST(test_PreferredPixelSpacingSelector)

#include "test_preferredpixelspacingselector.moc"
