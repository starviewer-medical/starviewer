#include "autotest.h"
#include "image.h"

#include "volumepixeldata.h"

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

    void getDisplayShutterForDisplayAsPixelData_ShouldReturnNull_data();
    void getDisplayShutterForDisplayAsPixelData_ShouldReturnNull();

    void getDisplayShutterForDisplayAsPixelData_ShouldReturnNullWhenThereAreNoShutters();
    
    void getDisplayShutterForDisplayAsPixelData_ShouldReturnExpectedValues_data();
    void getDisplayShutterForDisplayAsPixelData_ShouldReturnExpectedValues();
};

Q_DECLARE_METATYPE(QList<DisplayShutter>);
Q_DECLARE_METATYPE(DisplayShutter);
Q_DECLARE_METATYPE(VolumePixelData*);
Q_DECLARE_METATYPE(double*);

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

void test_Image::getDisplayShutterForDisplayAsPixelData_ShouldReturnNull_data()
{
    QTest::addColumn<QList<DisplayShutter> >("shutters");

    QList<DisplayShutter> shuttersList;
    
    shuttersList << DisplayShutter();
    QTest::newRow("single undefined shape shutter") << shuttersList;

    DisplayShutter shutter1;
    shutter1.setPoints(QPoint(1, 1), 5);
    
    DisplayShutter shutter2;
    shutter2.setPoints(QPoint(10, 10), 2);
    
    shuttersList.clear();
    shuttersList << shutter1 << shutter2;
    QTest::newRow("two shutters with no intersection") << shuttersList;
}

void test_Image::getDisplayShutterForDisplayAsPixelData_ShouldReturnNull()
{
    QFETCH(QList<DisplayShutter>, shutters);

    Image *image = new Image;
    image->setDisplayShutters(shutters);
    
    QVERIFY(!image->getDisplayShutterForDisplayAsPixelData());
}

void test_Image::getDisplayShutterForDisplayAsPixelData_ShouldReturnNullWhenThereAreNoShutters()
{
    Image *image = new Image;
    QVERIFY(!image->getDisplayShutterForDisplayAsPixelData());
}

void test_Image::getDisplayShutterForDisplayAsPixelData_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");
    QTest::addColumn<double*>("imageOrigin");
    QTest::addColumn<double*>("imageSpacing");
    QTest::addColumn<DisplayShutter>("shutter");
    QTest::addColumn<int>("zSlice");

    double *origin1 = new double[3];
    origin1[0] = 1.3;
    origin1[1] = 2.1;
    origin1[2] = 5.6;

    double *spacing1 = new double[3];
    spacing1[0] = 0.7;
    spacing1[1] = 0.7;
    spacing1[2] = 10.0;

    DisplayShutter shutter1;
    shutter1.setPoints(QPoint(1,3), 2);
    
    QTest::newRow("image1 with circular shutter, zSlice = 3") << 5 << 6 << origin1 << spacing1 << shutter1 << 3;
}

void test_Image::getDisplayShutterForDisplayAsPixelData_ShouldReturnExpectedValues()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(double*, imageOrigin);
    QFETCH(double*, imageSpacing);
    QFETCH(DisplayShutter, shutter);
    QFETCH(int, zSlice);

    Image *image = new Image;

    image->setRows(rows);
    image->setColumns(columns);
    image->setImagePositionPatient(imageOrigin);
    image->setPixelSpacing(imageSpacing[0], imageSpacing[1]);
    image->addDisplayShutter(shutter);
    
    VolumePixelData *shutterData = image->getDisplayShutterForDisplayAsPixelData(zSlice);

    int extent[6];
    shutterData->getVtkData()->GetExtent(extent);
    
    QCOMPARE(extent[0], 0);
    QCOMPARE(extent[2], 0);
    QCOMPARE(extent[1], columns - 1);
    QCOMPARE(extent[3], rows - 1);
    QCOMPARE(extent[4], zSlice);
    QCOMPARE(extent[5], zSlice);

    double shutterSpacing[3];
    shutterData->getVtkData()->GetSpacing(shutterSpacing);

    QCOMPARE(shutterSpacing[0], imageSpacing[0]);
    QCOMPARE(shutterSpacing[1], imageSpacing[1]);
    QCOMPARE(shutterSpacing[2], 1.0);

    double shutterOrigin[3];
    shutterData->getVtkData()->GetOrigin(shutterOrigin);

    for (int i = 0; i < 3; ++i)
    {
        QCOMPARE(shutterOrigin[i], imageOrigin[i]);
    }
}

DECLARE_TEST(test_Image)

#include "test_image.moc"
