#include "autotest.h"
#include "imageoverlayregionfinder.h"

#include "imageoverlay.h"
#include "imageoverlaytesthelper.h"

using namespace udg;
using namespace testing;

namespace {

class TestingImageOverlayRegionFinder : public ImageOverlayRegionFinder {
public:
    using ImageOverlayRegionFinder::distanceBetweenRegions;
};

}

namespace QTest {

template<>
char* toString(const QList<QRect> &regionList)
{
    QByteArray ba = "[";
    for (int i = 0; i < regionList.size(); i++)
    {
        char *regionString = toString(regionList[i]);
        ba += regionString;
        delete[] regionString;
        if (i < regionList.size() - 1)
        {
            ba += ", ";
        }
    }
    ba += "]";
    return qstrdup(ba.data());
}

}

class test_ImageOverlayRegionFinder : public QObject {

    Q_OBJECT

private slots:

    void findRegions_ShouldFindCorrectRegions_data();
    void findRegions_ShouldFindCorrectRegions();

    void distanceBetweenRegions_ShouldReturnCorrectDistance_data();
    void distanceBetweenRegions_ShouldReturnCorrectDistance();

};

Q_DECLARE_METATYPE(ImageOverlay)
Q_DECLARE_METATYPE(QList<QRect>)

void test_ImageOverlayRegionFinder::findRegions_ShouldFindCorrectRegions_data()
{
    QList<ImageOverlay> overlays = ImageOverlayTestHelper::createImageOverlays();
    QList< QList<QRect> > allRegions = ImageOverlayTestHelper::createSubOverlayRegions();

    QTest::addColumn<ImageOverlay>("imageOverlay");
    QTest::addColumn<bool>("optimizeForPowersOf2");
    QTest::addColumn< QList<QRect> >("regions");

    QTest::newRow("invalid overlay") << ImageOverlay() << false << QList<QRect>();
    QTest::newRow("invalid overlay (powers of 2)") << ImageOverlay() << true << QList<QRect>();

    {
        const ImageOverlay &overlay = overlays[0];
        const QList<QRect> &regions = allRegions[0];
        QTest::newRow("{D}, one region") << overlay << false << regions;
    }

    {
        const ImageOverlay &overlay = overlays[0];
        const QList<QRect> &regions = allRegions[0];
        QTest::newRow("{D}, one region (powers of 2)") << overlay << true << regions;
    }

    {
        const ImageOverlay &overlay = overlays[1];
        const QList<QRect> &regions = allRegions[1];
        QTest::newRow("{Tra, O, [F]}, three regions, discontinuity") << overlay << false << regions;
    }

    {
        const ImageOverlay &overlay = overlays[4];
        const QList<QRect> &regions = allRegions[4];
        QTest::newRow("{Tra, O [F]}, two regions, discontinuity (powers of 2)") << overlay << true << regions;
    }

    {
        const ImageOverlay &overlay = overlays[2];
        const QList<QRect> &regions = allRegions[2];
        QTest::newRow("{...}, one big region") << overlay << false << regions;
    }

    {
        const ImageOverlay &overlay = overlays[2];
        const QList<QRect> &regions = allRegions[2];
        QTest::newRow("{...}, one big region (powers of 2)") << overlay << true << regions;
    }

    {
        const ImageOverlay &overlay = overlays[3];
        const QList<QRect> &regions = allRegions[3];
        QTest::newRow("{Tra}, one region (special for padding)") << overlay << false << regions;
    }

    {
        const ImageOverlay &overlay = overlays[3];
        const QList<QRect> &regions = allRegions[3];
        QTest::newRow("{Tra}, one region (special for padding) (powers of 2)") << overlay << true << regions;
    }
}

void test_ImageOverlayRegionFinder::findRegions_ShouldFindCorrectRegions()
{
    QFETCH(ImageOverlay, imageOverlay);
    QFETCH(bool, optimizeForPowersOf2);
    QFETCH(QList<QRect>, regions);

    ImageOverlayRegionFinder regionFinder(imageOverlay);
    regionFinder.findRegions(optimizeForPowersOf2);

    QCOMPARE(regionFinder.regions(), regions);
}

void test_ImageOverlayRegionFinder::distanceBetweenRegions_ShouldReturnCorrectDistance_data()
{
    QTest::addColumn<QRect>("region1");
    QTest::addColumn<QRect>("region2");
    QTest::addColumn<int>("distance");

    QTest::newRow("equal regions") << QRect(0, 0, 1, 1) << QRect(0, 0, 1, 1) << 0;

    QTest::newRow("region1 inside region2") << QRect(2, 2, 2, 2) << QRect(1, 1, 4, 4) << 0;
    QTest::newRow("region2 inside region1") << QRect(1, 1, 4, 4) << QRect(2, 2, 2, 2) << 0;

    QTest::newRow("intersection top")          << QRect(10, 10, 10, 10) << QRect(12,  6, 6, 6) << 0;
    QTest::newRow("intersection top right")    << QRect(10, 10, 10, 10) << QRect(18,  6, 6, 6) << 0;
    QTest::newRow("intersection right")        << QRect(10, 10, 10, 10) << QRect(18, 12, 6, 6) << 0;
    QTest::newRow("intersection bottom right") << QRect(10, 10, 10, 10) << QRect(18, 18, 6, 6) << 0;
    QTest::newRow("intersection bottom")       << QRect(10, 10, 10, 10) << QRect(12, 18, 6, 6) << 0;
    QTest::newRow("intersection bottom left")  << QRect(10, 10, 10, 10) << QRect( 6, 18, 6, 6) << 0;
    QTest::newRow("intersection left")         << QRect(10, 10, 10, 10) << QRect( 6, 12, 6, 6) << 0;
    QTest::newRow("intersection top left")     << QRect(10, 10, 10, 10) << QRect( 6,  6, 6, 6) << 0;

    QTest::newRow("touching top")                 << QRect(10, 10, 10, 10) << QRect(12,  5, 6, 5) << 0;
    QTest::newRow("touching top top right")       << QRect(10, 10, 10, 10) << QRect(16,  5, 6, 5) << 0;
    QTest::newRow("touching top right")           << QRect(10, 10, 10, 10) << QRect(20,  5, 5, 5) << 0;
    QTest::newRow("touching right top right")     << QRect(10, 10, 10, 10) << QRect(20,  8, 5, 6) << 0;
    QTest::newRow("touching right")               << QRect(10, 10, 10, 10) << QRect(20, 12, 5, 6) << 0;
    QTest::newRow("touching right bottom right")  << QRect(10, 10, 10, 10) << QRect(20, 16, 5, 6) << 0;
    QTest::newRow("touching bottom right")        << QRect(10, 10, 10, 10) << QRect(20, 20, 5, 5) << 0;
    QTest::newRow("touching bottom bottom right") << QRect(10, 10, 10, 10) << QRect(16, 20, 6, 5) << 0;
    QTest::newRow("touching bottom")              << QRect(10, 10, 10, 10) << QRect(12, 20, 6, 5) << 0;
    QTest::newRow("touching bottom bottom left")  << QRect(10, 10, 10, 10) << QRect( 8, 20, 6, 5) << 0;
    QTest::newRow("touching bottom left")         << QRect(10, 10, 10, 10) << QRect( 5, 20, 5, 5) << 0;
    QTest::newRow("touching left bottom left")    << QRect(10, 10, 10, 10) << QRect( 5, 16, 5, 6) << 0;
    QTest::newRow("touching left")                << QRect(10, 10, 10, 10) << QRect( 5, 12, 5, 6) << 0;
    QTest::newRow("touching left top left")       << QRect(10, 10, 10, 10) << QRect( 5,  8, 5, 6) << 0;
    QTest::newRow("touching top left")            << QRect(10, 10, 10, 10) << QRect( 5,  5, 5, 5) << 0;
    QTest::newRow("touching top top left")        << QRect(10, 10, 10, 10) << QRect( 8,  5, 6, 5) << 0;

    QTest::newRow("non-touching top")                 << QRect(10, 10, 10, 10) << QRect(12,  0, 6, 5) << 5;
    QTest::newRow("non-touching top top right")       << QRect(10, 10, 10, 10) << QRect(18,  0, 6, 5) << 5;
    QTest::newRow("non-touching top right")           << QRect(10, 10, 10, 10) << QRect(25,  0, 5, 5) << 5;
    QTest::newRow("non-touching right top right")     << QRect(10, 10, 10, 10) << QRect(25,  6, 5, 6) << 5;
    QTest::newRow("non-touching right")               << QRect(10, 10, 10, 10) << QRect(25, 12, 5, 6) << 5;
    QTest::newRow("non-touching right bottom right")  << QRect(10, 10, 10, 10) << QRect(25, 18, 5, 6) << 5;
    QTest::newRow("non-touching bottom right")        << QRect(10, 10, 10, 10) << QRect(25, 25, 5, 5) << 5;
    QTest::newRow("non-touching bottom bottom right") << QRect(10, 10, 10, 10) << QRect(18, 25, 6, 5) << 5;
    QTest::newRow("non-touching bottom")              << QRect(10, 10, 10, 10) << QRect(12, 25, 6, 5) << 5;
    QTest::newRow("non-touching bottom bottom left")  << QRect(10, 10, 10, 10) << QRect( 6, 25, 6, 5) << 5;
    QTest::newRow("non-touching bottom left")         << QRect(10, 10, 10, 10) << QRect( 0, 25, 5, 5) << 5;
    QTest::newRow("non-touching left bottom left")    << QRect(10, 10, 10, 10) << QRect( 0, 18, 5, 6) << 5;
    QTest::newRow("non-touching left")                << QRect(10, 10, 10, 10) << QRect( 0, 12, 5, 6) << 5;
    QTest::newRow("non-touching left top left")       << QRect(10, 10, 10, 10) << QRect( 0,  6, 5, 6) << 5;
    QTest::newRow("non-touching top left")            << QRect(10, 10, 10, 10) << QRect( 0,  0, 5, 5) << 5;
    QTest::newRow("non-touching top top left")        << QRect(10, 10, 10, 10) << QRect( 6,  0, 6, 5) << 5;

    QTest::newRow("random (1)") << QRect(20, 38, 2, 96) << QRect(26, 7, 60, 52) << 4;
    QTest::newRow("random (2)") << QRect(34, 80, 33, 26) << QRect(87, 33, 68, 54) << 20;
    QTest::newRow("random (3)") << QRect(57, 49, 42, 2) << QRect(10, 43, 92, 1) << 5;
}

void test_ImageOverlayRegionFinder::distanceBetweenRegions_ShouldReturnCorrectDistance()
{
    QFETCH(QRect, region1);
    QFETCH(QRect, region2);
    QFETCH(int, distance);

    QCOMPARE(TestingImageOverlayRegionFinder::distanceBetweenRegions(region1, region2), distance);
}

DECLARE_TEST(test_ImageOverlayRegionFinder)

#include "test_imageoverlayregionfinder.moc"
