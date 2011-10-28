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

    void addWindowLevel_ShouldAddWindowLevel_data();
    void addWindowLevel_ShouldAddWindowLevel();
    void addWindowLevel_ShouldNotAddWindowLevel_data();
    void addWindowLevel_ShouldNotAddWindowLevel();
    void addWindowLevel_ShouldAddSameWindowLevelTwoTimes();

    void getWindowLevel_ShouldReturnExpectedWindowLevel();
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

void test_Image::addWindowLevel_ShouldAddWindowLevel_data()
{
    QTest::addColumn<double>("window");
    QTest::addColumn<double>("level");

    QTest::newRow("both positive") << 100.0 << 200.0;
    QTest::newRow("both negative") << -101.0 << -201.0;
    QTest::newRow("window positive, level negative") << 102.0 << -202.0;
    QTest::newRow("window negative, level positive") << -103.0 << 203.0;
    QTest::newRow("level zero") << 100.0 << 0.0;
}

void test_Image::addWindowLevel_ShouldAddWindowLevel()
{
    QFETCH(double, window);
    QFETCH(double, level);

    Image image;
    image.addWindowLevel(window, level);

    QCOMPARE(image.getNumberOfWindowLevels(), 1);
    QCOMPARE(image.getWindowLevel(0).first, window);
    QCOMPARE(image.getWindowLevel(0).second, level);
}

void test_Image::addWindowLevel_ShouldNotAddWindowLevel_data()
{
    QTest::addColumn<double>("window");
    QTest::addColumn<double>("level");

    QTest::newRow("window zero") << 0.0 << 200.0;
}

void test_Image::addWindowLevel_ShouldNotAddWindowLevel()
{
    QFETCH(double, window);
    QFETCH(double, level);

    Image image;
    image.addWindowLevel(window, level);

    QCOMPARE(image.getNumberOfWindowLevels(), 0);
}

void test_Image::addWindowLevel_ShouldAddSameWindowLevelTwoTimes()
{
    double window = 100.0;
    double level = 300.0;
    Image image;
    image.addWindowLevel(window, level);
    image.addWindowLevel(window, level);

    QCOMPARE(image.getNumberOfWindowLevels(), 2);
    QCOMPARE(image.getWindowLevel(0).first, window);
    QCOMPARE(image.getWindowLevel(0).second, level);
    QCOMPARE(image.getWindowLevel(1).first, window);
    QCOMPARE(image.getWindowLevel(1).second, level);
}

void test_Image::getWindowLevel_ShouldReturnExpectedWindowLevel()
{
    typedef QPair<double, double> QPairDoublesType;

    QList<QPairDoublesType> windowLevels;
    windowLevels << QPairDoublesType(10.0, 20.0);
    windowLevels << QPairDoublesType(11.0, 21.0);
    windowLevels << QPairDoublesType(12.0, 22.0);

    Image image;

    foreach (const QPairDoublesType &pair, windowLevels)
    {
        image.addWindowLevel(pair.first, pair.second);
    }

    QCOMPARE(image.getWindowLevel(-1), QPairDoublesType());
    QCOMPARE(image.getWindowLevel(0), windowLevels.at(0));
    QCOMPARE(image.getWindowLevel(1), windowLevels.at(1));
    QCOMPARE(image.getWindowLevel(2), windowLevels.at(2));
    QCOMPARE(image.getWindowLevel(3), QPairDoublesType());
}

DECLARE_TEST(test_Image)

#include "test_image.moc"
