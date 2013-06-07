#include "autotest.h"
#include "imageplane.h"

#include "image.h"

using namespace udg;

class test_ImagePlane : public QObject {
Q_OBJECT

private slots:
    void fillFromImage_fillsExpectedValues_data();
    void fillFromImage_fillsExpectedValues();
};

Q_DECLARE_METATYPE(ImagePlane)

void test_ImagePlane::fillFromImage_fillsExpectedValues_data()
{
    QTest::addColumn<ImagePlane>("filledImagePlane");
    QTest::addColumn<ImagePlane>("expectedImagePlane");

    Image *image = new Image(this);
    ImagePlane filledPlane;
    filledPlane.fillFromImage(image);
    
    ImagePlane expectedPlane;
    expectedPlane.setImageOrientation(image->getImageOrientationPatient());
    expectedPlane.setSpacing(image->getPixelSpacing().x(), image->getPixelSpacing().y());
    expectedPlane.setThickness(image->getSliceThickness());
    expectedPlane.setRows(image->getRows());
    expectedPlane.setColumns(image->getColumns());
    expectedPlane.setOrigin(image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2]);
    
    QTest::newRow("Image plane is properly filled with empty image") << filledPlane << expectedPlane;

    // Some values to fill from
    ImageOrientation orientation;
    orientation.setDICOMFormattedImageOrientation("1\\2\\3\\4\\5\\6");
    PixelSpacing2D spacing(1.2, 2.1);
    double thickness = 3.0;
    int rows = 256;
    int columns = 128;
    double origin[3] = { 1.0, 2.0, 3.0 };
    
    image = new Image(this);
    image->setImageOrientationPatient(orientation);
    image->setPixelSpacing(spacing);
    image->setSliceThickness(thickness);
    image->setRows(rows);
    image->setColumns(columns);
    image->setImagePositionPatient(origin);

    expectedPlane.setImageOrientation(orientation);
    expectedPlane.setSpacing(spacing.x(), spacing.y());
    expectedPlane.setThickness(thickness);
    expectedPlane.setRows(rows);
    expectedPlane.setColumns(columns);
    expectedPlane.setOrigin(origin);

    filledPlane.fillFromImage(image);

    QTest::newRow("Image plane is properly filled from image with random values") << filledPlane << expectedPlane;
}

void test_ImagePlane::fillFromImage_fillsExpectedValues()
{
    QFETCH(ImagePlane, filledImagePlane);
    QFETCH(ImagePlane, expectedImagePlane);

    QVERIFY(filledImagePlane == expectedImagePlane);
}

DECLARE_TEST(test_ImagePlane)

#include "test_imageplane.moc"

