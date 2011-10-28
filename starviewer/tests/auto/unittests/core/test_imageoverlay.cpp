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

    void isValid_ReturnsExpectedValue_data();
    void isValid_ReturnsExpectedValue();

    void fromGDCMOverlay_ReturnsExpectedValues_data();
    void fromGDCMOverlay_ReturnsExpectedValues();

    void mergeOverlays_ReturnsExpectedImageOverlay_data();
    void mergeOverlays_ReturnsExpectedImageOverlay();
};

Q_DECLARE_METATYPE(gdcm::Overlay);
Q_DECLARE_METATYPE(ImageOverlay);
Q_DECLARE_METATYPE(QList<ImageOverlay>);

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

void test_ImageOverlay::isValid_ReturnsExpectedValue_data()
{
    QTest::addColumn<ImageOverlay>("imageOverlay");
    QTest::addColumn<bool>("expectedValue");

    ImageOverlay imageOverlay;
    unsigned char data;

    imageOverlay.setRows(0);
    imageOverlay.setColumns(0);
    imageOverlay.setData(0);
    QTest::newRow("no rows, no columns, no data") << imageOverlay << false;

    imageOverlay.setRows(0);
    imageOverlay.setColumns(0);
    imageOverlay.setData(&data);
    QTest::newRow("no rows, no columns, data") << imageOverlay << false;

    imageOverlay.setRows(0);
    imageOverlay.setColumns(1);
    imageOverlay.setData(0);
    QTest::newRow("no rows, columns, no data") << imageOverlay << false;

    imageOverlay.setRows(0);
    imageOverlay.setColumns(2);
    imageOverlay.setData(&data);
    QTest::newRow("no rows, columns, data") << imageOverlay << false;

    imageOverlay.setRows(3);
    imageOverlay.setColumns(0);
    imageOverlay.setData(0);
    QTest::newRow("rows, no columns, no data") << imageOverlay << false;

    imageOverlay.setRows(4);
    imageOverlay.setColumns(0);
    imageOverlay.setData(&data);
    QTest::newRow("rows, no columns, data") << imageOverlay << false;

    imageOverlay.setRows(5);
    imageOverlay.setColumns(6);
    imageOverlay.setData(0);
    QTest::newRow("rows, columns, no data") << imageOverlay << false;

    imageOverlay.setRows(7);
    imageOverlay.setColumns(8);
    imageOverlay.setData(&data);
    QTest::newRow("rows, columns, data") << imageOverlay << true;
}

void test_ImageOverlay::isValid_ReturnsExpectedValue()
{
    QFETCH(ImageOverlay, imageOverlay);
    QFETCH(bool, expectedValue);

    QCOMPARE(imageOverlay.isValid(), expectedValue);
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

void test_ImageOverlay::mergeOverlays_ReturnsExpectedImageOverlay_data()
{
    QTest::addColumn<QList<ImageOverlay> >("overlaysList");
    QTest::addColumn<ImageOverlay>("mergedOverlay");
    QTest::addColumn<bool>("mergeWasSuccessful");

    QList<ImageOverlay> overlaysList;
    QTest::newRow("Empty List") << overlaysList << ImageOverlay() << true;

    ImageOverlay overlay1;
    overlay1.setColumns(512);
    overlay1.setRows(512);
    overlay1.setOrigin(1, 1);
    unsigned char *data1 = new unsigned char[512 * 512];
    memset(data1, 0, sizeof(unsigned char) * 512 * 512);
    memset(data1, 255, sizeof(unsigned char) * 512);
    overlay1.setData(data1);
    
    overlaysList.clear();
    overlaysList << overlay1;
    QTest::newRow("Single element list") << overlaysList << overlay1 << true;

    overlaysList.clear();
    overlaysList << overlay1 << overlay1;
    QTest::newRow("2 elements, same data") << overlaysList << overlay1 << true;
    
    ImageOverlay overlay2 = overlay1;
    unsigned char *data2 = new unsigned char[512 * 512];
    memset(data2, 0, sizeof(unsigned char) * 512 * 512);
    memset(data2 + sizeof(unsigned char) * 512, 255, sizeof(unsigned char) * 512);
    overlay2.setData(data2);

    ImageOverlay mergedOverlay;
    mergedOverlay.setColumns(512);
    mergedOverlay.setRows(512);
    mergedOverlay.setOrigin(1, 1);
    unsigned char *mergedData1 = new unsigned char[512 * 512];
    memset(mergedData1, 0, sizeof(unsigned char) * 512 * 512);
    memset(mergedData1, 255, sizeof(unsigned char) * 1024);
    mergedOverlay.setData(mergedData1);
    
    overlaysList.clear();
    overlaysList << overlay1 << overlay2;
    QTest::newRow("2 overlays, same origin, same cols x rows, different data") << overlaysList << mergedOverlay << true;

    ImageOverlay overlay3;
    overlay3.setColumns(512);
    overlay3.setRows(512);
    overlay3.setOrigin(-10, 5);
    unsigned char *data3 = new unsigned char[512 * 512];
    memset(data3, 0, sizeof(unsigned char) * 512 * 512);
    memset(data3, 255, sizeof(unsigned char) * 512);
    overlay3.setData(data3);

    ImageOverlay mergedOverlay1_3;
    mergedOverlay1_3.setColumns(512 + 11);
    mergedOverlay1_3.setRows(512 + 4);
    mergedOverlay1_3.setOrigin(-10, 1);
    int dataLength = mergedOverlay1_3.getColumns() * mergedOverlay1_3.getRows();
    unsigned char *mergedData1_3 = new unsigned char[dataLength];
    memset(mergedData1_3, 0, sizeof(unsigned char) * dataLength);
    memset(mergedData1_3 + sizeof(unsigned char) * 11, 255, sizeof(unsigned char) * 512);
    memset(mergedData1_3 + sizeof(unsigned char) * mergedOverlay1_3.getColumns() * 4, 255, sizeof(unsigned char) * 512);
    mergedOverlay1_3.setData(mergedData1_3);
    
    overlaysList.clear();
    overlaysList << overlay1 << overlay3;
    QTest::newRow("2 overlays, different origin, same cols x rows") << overlaysList << mergedOverlay1_3 << true;

    ImageOverlay overlay4;
    overlay4.setColumns(10);
    overlay4.setRows(10);
    overlay4.setOrigin(1, 1);
    unsigned char *data4 = new unsigned char[10 * 10];
    memset(data4, 0, sizeof(unsigned char) * 100);
    memset(data4, 255, sizeof(unsigned char) * 10);
    overlay4.setData(data4);
    
    ImageOverlay overlay5;
    overlay5.setColumns(11);
    overlay5.setRows(11);
    overlay5.setOrigin(1, 1);
    unsigned char *data5 = new unsigned char[11 * 11];
    memset(data5, 0, sizeof(unsigned char) * 121);
    memset(data5 + sizeof(unsigned char) * 11 * 10, 255, sizeof(unsigned char) * 11);
    overlay5.setData(data5);

    ImageOverlay mergedOverlay4_5;
    mergedOverlay4_5.setColumns(11);
    mergedOverlay4_5.setRows(11);
    mergedOverlay4_5.setOrigin(1, 1);
    unsigned char *mergedData4_5 = new unsigned char[11 * 11];
    memset(mergedData4_5, 0, sizeof(unsigned char) * 121);
    memset(mergedData4_5, 255, sizeof(unsigned char) * 10);
    memset(mergedData4_5 + sizeof(unsigned char) * 11 * 10, 255, sizeof(unsigned char) * 11);
    mergedOverlay4_5.setData(mergedData4_5);
    
    overlaysList.clear();
    overlaysList << overlay4 << overlay5;
    QTest::newRow("2 overlays, same origin, different cols x rows") << overlaysList << mergedOverlay4_5 << true;

    ImageOverlay overlay6;
    overlay6.setColumns(8);
    overlay6.setRows(6);
    overlay6.setOrigin(2, -10);
    unsigned char *data6 = new unsigned char[6 * 8];
    memset(data6, 0, sizeof(unsigned char) * 48);
    data6[0] = 255;
    data6[13] = 255;
    data6[42] = 255;
    overlay6.setData(data6);
    
    ImageOverlay overlay7;
    overlay7.setColumns(3);
    overlay7.setRows(20);
    overlay7.setOrigin(-6, -7);
    unsigned char *data7 = new unsigned char[3 * 20];
    memset(data7, 0, sizeof(unsigned char) * 60);
    data7[0] = 255;
    data7[1] = 255;
    data7[31] = 255;
    data7[59] = 255;
    overlay7.setData(data7);

    ImageOverlay mergedOverlay6_7;
    mergedOverlay6_7.setColumns(16);
    mergedOverlay6_7.setRows(23);
    mergedOverlay6_7.setOrigin(-6, -10);
    unsigned char *mergedData6_7 = new unsigned char[23 * 16];
    memset(mergedData6_7, 0, sizeof(unsigned char) * 23 * 16);
    mergedData6_7[8] = 255;
    mergedData6_7[29] = 255;
    mergedData6_7[48] = 255;
    mergedData6_7[49] = 255;
    mergedData6_7[90] = 255;
    mergedData6_7[209] = 255;
    mergedData6_7[354] = 255;
    mergedOverlay6_7.setData(mergedData6_7);
    
    overlaysList.clear();
    overlaysList << overlay6 << overlay7;
    QTest::newRow("2 overlays, different origin, different cols x rows") << overlaysList << mergedOverlay6_7 << true;

    ImageOverlay nullRowsOverlay;
    nullRowsOverlay.setColumns(512);
    nullRowsOverlay.setRows(0);
    nullRowsOverlay.setOrigin(3, 25);

    overlaysList.clear();
    overlaysList << overlay1 << nullRowsOverlay;
    QTest::newRow("2 overlays, one invalid (no rows)") << overlaysList << overlay1 << true;

    ImageOverlay nullColumnsOverlay;
    nullColumnsOverlay.setColumns(0);
    nullColumnsOverlay.setRows(128);
    nullColumnsOverlay.setOrigin(2, 5);

    overlaysList.clear();
    overlaysList << overlay1 << nullColumnsOverlay;
    QTest::newRow("2 overlays, one invalid (no columns)") << overlaysList << overlay1 << true;

    ImageOverlay nullDataOverlay;
    nullDataOverlay.setColumns(128);
    nullDataOverlay.setRows(128);
    nullDataOverlay.setOrigin(2, 5);
    nullDataOverlay.setData(0);

    overlaysList.clear();
    overlaysList << overlay1 << nullDataOverlay;
    QTest::newRow("2 overlays, one invalid (no data)") << overlaysList << overlay1 << true;

    overlaysList.clear();
    overlaysList << ImageOverlay() << ImageOverlay();
    QTest::newRow("2 empty overlays") << overlaysList << ImageOverlay() << true;

    overlaysList.clear();
    overlaysList << nullRowsOverlay;
    QTest::newRow("1 invalid overlay (no rows)") << overlaysList << ImageOverlay() << true;

    overlaysList.clear();
    overlaysList << nullColumnsOverlay;
    QTest::newRow("1 invalid overlay (no columns)") << overlaysList << ImageOverlay() << true;

    overlaysList.clear();
    overlaysList << nullDataOverlay;
    QTest::newRow("1 invalid overlay (no data)") << overlaysList << ImageOverlay() << true;

    overlaysList.clear();
    overlaysList << nullColumnsOverlay << nullRowsOverlay << nullDataOverlay;
    QTest::newRow("3 invalid overlays (no rows, no columns, no data)") << overlaysList << ImageOverlay() << true;
}

void test_ImageOverlay::mergeOverlays_ReturnsExpectedImageOverlay()
{
    QFETCH(QList<ImageOverlay>, overlaysList);
    QFETCH(ImageOverlay, mergedOverlay);
    QFETCH(bool, mergeWasSuccessful);

    bool mergeOk;
    QVERIFY(ImageOverlayTestHelper::areEqual(ImageOverlay::mergeOverlays(overlaysList, mergeOk), mergedOverlay));
    QCOMPARE(mergeOk, mergeWasSuccessful);
}

DECLARE_TEST(test_ImageOverlay)

#include "test_imageoverlay.moc"
