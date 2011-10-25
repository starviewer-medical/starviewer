#include "autotest.h"
#include "image.h"

using namespace udg;

class test_Image : public QObject {
Q_OBJECT

private slots:
    void hasOverlays_ReturnExpectedValues_data();
    void hasOverlays_ReturnExpectedValues();

    void getDisplayShutterForDisplay_ShouldReturnExpectedValues_data();
    void getDisplayShutterForDisplay_ShouldReturnExpectedValues();
};

Q_DECLARE_METATYPE(QList<DisplayShutter>);
Q_DECLARE_METATYPE(DisplayShutter);

void test_Image::hasOverlays_ReturnExpectedValues_data()
{
    QTest::addColumn<unsigned short>("numberOfOverlays");
    QTest::addColumn<bool>("hasOverlays");
    unsigned short ushortZero = 0;
    QTest::newRow("Zero overlays") << ushortZero << false;
    for (unsigned short i = 1; i < 17; ++i)
    {
        QTest::newRow(qPrintable(QString("%1 overlays").arg(i))) << i << true;
    }
}

void test_Image::hasOverlays_ReturnExpectedValues()
{
    QFETCH(unsigned short, numberOfOverlays);
    QFETCH(bool, hasOverlays);

    Image image;
    QVERIFY(!image.hasOverlays());

    image.setNumberOfOverlays(numberOfOverlays);
    
    QCOMPARE(image.hasOverlays(), hasOverlays);
}

void test_Image::getDisplayShutterForDisplay_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");
    QTest::addColumn<QList<DisplayShutter> >("imageShuttersList");
    QTest::addColumn<DisplayShutter>("expectedShutterForDisplay");

    DisplayShutter rectangularShutter;
    rectangularShutter.setPoints(QPoint(1, 1), QPoint(10, 10)); 
    
    QTest::newRow("Image with no shutters") << 10 << 10 << QList<DisplayShutter>() << DisplayShutter();

    QList<DisplayShutter> shuttersList;
    
    shuttersList.clear();
    shuttersList << rectangularShutter;
    QTest::newRow("1 shutter, rectangular shape, same size as image") << 10 << 10 << shuttersList << DisplayShutter();
    
    shuttersList.clear();
    shuttersList << DisplayShutter();
    QTest::newRow("1 shutter, undefined shape") << 10 << 10 << shuttersList << DisplayShutter();

    shuttersList.clear();
    shuttersList << rectangularShutter;
    QTest::newRow("1 shutter, rectangular shape, smaller than image's size") << 15 << 15 << shuttersList << rectangularShutter;

    DisplayShutter rectangularShutter2;
    rectangularShutter2.setPoints(QPoint(-5, -5), QPoint(20, 20));
    
    shuttersList.clear();
    shuttersList << rectangularShutter2;
    QTest::newRow("1 shutter, rectangular shape, larger than image's size") << 10 << 10 << shuttersList << DisplayShutter();
    
    DisplayShutter rectangularShutter3;
    rectangularShutter3.setPoints(QPoint(5, 5), QPoint(20, 20));

    DisplayShutter intersectionShutter;
    QPolygon intersectionPolygon; 
    intersectionPolygon << QPoint(10, 5) << QPoint(10, 10) << QPoint(5, 10) << QPoint(5, 5) << QPoint(10, 5);
    intersectionShutter.setPoints(intersectionPolygon);
    
    shuttersList.clear();
    shuttersList << rectangularShutter;
    shuttersList << rectangularShutter3;
    QTest::newRow("2 shutters, intersection gives rectangular shape, smaller than image's size") << 15 << 15 << shuttersList << intersectionShutter;

    DisplayShutter circularShutter;
    circularShutter.setPoints(QPoint(5,5), 2);
    
    shuttersList.clear();
    shuttersList << DisplayShutter();
    shuttersList << circularShutter;
    shuttersList << rectangularShutter;
    QTest::newRow("3 shutters, 2 no appliable (empty and same image size rectangle)") << 10 << 10 << shuttersList << circularShutter;
}

void test_Image::getDisplayShutterForDisplay_ShouldReturnExpectedValues()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(QList<DisplayShutter>, imageShuttersList);
    QFETCH(DisplayShutter, expectedShutterForDisplay);

    Image *image = new Image;
    image->setRows(rows);
    image->setColumns(columns);
    image->setDisplayShutters(imageShuttersList);

    DisplayShutter shutterForDisplay = image->getDisplayShutterForDisplay();

    QCOMPARE(shutterForDisplay.getShape(), expectedShutterForDisplay.getShape());
    QCOMPARE(shutterForDisplay.getAsQPolygon(), expectedShutterForDisplay.getAsQPolygon());
}

DECLARE_TEST(test_Image)

#include "test_image.moc"
