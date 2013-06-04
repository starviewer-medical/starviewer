#include "autotest.h"
#include "defaultmeasurementtypeselector.h"

#include "image.h"
#include "series.h"
#include "mathtools.h"

using namespace udg;

class test_DefaultMeasurementTypeSelector : public QObject {
Q_OBJECT

private slots:
    void getDefaultMeasurementType_ReturnsExpectedValues_data();
    void getDefaultMeasurementType_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementType)

void test_DefaultMeasurementTypeSelector::getDefaultMeasurementType_ReturnsExpectedValues_data()
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

        QTest::newRow("Any modality -PixelSpacing != ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor present(any) => Calibrated") 
            << image << MeasurementManager::Calibrated;
    }

    double randomX = MathTools::randomDouble(2.5, 5.5);
    double randomY = MathTools::randomDouble(2.5, 5.5);
    PixelSpacing2D randomSpacing(randomX, randomY);
    {
        Image *image = new Image(this);
        image->setPixelSpacing(randomSpacing);
        image->setImagerPixelSpacing(randomSpacing);
        image->setEstimatedRadiographicMagnificationFactor(dummyFactor);

        QTest::newRow("Any modality -PixelSpacing == ImagerPixelSpacing & EstimatedRadiographicMagnificationFactor present(any) => Calibrated") 
            << image << MeasurementManager::Detector;
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

void test_DefaultMeasurementTypeSelector::getDefaultMeasurementType_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(MeasurementManager::MeasurementType, expectedType);

    DefaultMeasurementTypeSelector measurementTypeSelector;
    
    QCOMPARE(measurementTypeSelector.getDefaultMeasurementType(image), expectedType);
}

DECLARE_TEST(test_DefaultMeasurementTypeSelector)

#include "test_defaultmeasurementtypeselector.moc"
