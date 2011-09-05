#include "autotest.h"
#include "imageoverlay.h"

#include "imageoverlaytesthelper.h"

#include <gdcmOverlay.h>

using namespace udg;
using namespace testing;

class test_ImageOverlay : public QObject {
Q_OBJECT

private slots:
    void ImageOverlay_InitializesClassAsExpected();

    void fromGDCMOverlay_ReturnsExpectedValues_data();
    void fromGDCMOverlay_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(gdcm::Overlay);
Q_DECLARE_METATYPE(ImageOverlay);

void test_ImageOverlay::ImageOverlay_InitializesClassAsExpected()
{    
    ImageOverlay overlay;
    unsigned int uintZero = 0;
    QCOMPARE(overlay.getRows(), uintZero);
    QCOMPARE(overlay.getColumns(), uintZero);
    QCOMPARE(overlay.getXOrigin(), 1);
    QCOMPARE(overlay.getYOrigin(), 1);
    unsigned char *nullUCharPointer = 0;
    QCOMPARE(overlay.getData(), nullUCharPointer);
}

void test_ImageOverlay::fromGDCMOverlay_ReturnsExpectedValues_data()
{
    QTest::addColumn<gdcm::Overlay>("gdcmOverlay");
    QTest::addColumn<ImageOverlay>("imageOverlay");

    gdcm::Overlay emptyGDCMOverlay;
    ImageOverlay imageOverlayLikeEmptyGDCMOverlay;
    imageOverlayLikeEmptyGDCMOverlay.setOrigin(0, 0);
    QTest::newRow("empty gdcm overlay") << emptyGDCMOverlay << imageOverlayLikeEmptyGDCMOverlay;

    int rows = 5;
    int columns = 7;
    short origin[2] = { 22, -12 };
   
    const char *gdcmBuffer = new char[rows * columns];
    
    gdcm::Overlay overlayWithData;
    overlayWithData.SetColumns(columns);
    overlayWithData.SetRows(rows);
    overlayWithData.SetOrigin(origin);
    overlayWithData.SetOverlay(gdcmBuffer, rows * columns);

    unsigned char *imageOverlayBuffer = new unsigned char[rows * columns];
    overlayWithData.GetUnpackBuffer(imageOverlayBuffer);
    
    ImageOverlay imageOverlay;
    imageOverlay.setRows(rows);
    imageOverlay.setColumns(columns);
    imageOverlay.setOrigin(origin[0], origin[1]);
    imageOverlay.setData(imageOverlayBuffer);

    QTest::newRow("gdcm overlay with data") << overlayWithData << imageOverlay;
}

void test_ImageOverlay::fromGDCMOverlay_ReturnsExpectedValues()
{
    QFETCH(gdcm::Overlay, gdcmOverlay);
    QFETCH(ImageOverlay, imageOverlay);

    QVERIFY(ImageOverlayTestHelper::areEqual(ImageOverlay::fromGDCMOverlay(gdcmOverlay), imageOverlay));
}

DECLARE_TEST(test_ImageOverlay)

#include "test_imageoverlay.moc"
