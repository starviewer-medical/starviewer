#include "autotest.h"

#include "drawerbitmap.h"
#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_DrawerBitmap : public QObject {
Q_OBJECT

private slots:
    void getBounds_ReturnsExpectedValues_data();
    void getBounds_ReturnsExpectedValues();

    void getDistanceToPoint_ReturnsExpectedValues_data();
    void getDistanceToPoint_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DrawerBitmap*)
Q_DECLARE_METATYPE(QVector<double>)

void test_DrawerBitmap::getBounds_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerBitmap*>("drawerBitmap");
    QTest::addColumn<QVector<double> >("bounds");

    DrawerBitmap *emptyBitmap = new DrawerBitmap;
    QVector<double> emptyBounds(6, 0.0);
    emptyBounds[5] = 1.0;
    QTest::newRow("Empty drawer bitmap") << emptyBitmap << emptyBounds;

    DrawerBitmap *bitmap = new DrawerBitmap;
    double origin[3] = { 1.5, 3.8, 7.2 };
    double spacing[3] = { 0.5, 2.0, 1.0 };
    unsigned int width = 256;
    unsigned int height = 128;
    bitmap->setOrigin(origin);
    bitmap->setSpacing(spacing);
    bitmap->setData(width, height, 0);
    QVector<double> bounds(6);
    bounds[0] = origin[0];
    bounds[1] = origin[0] + spacing[0] * width;
    bounds[2] = origin[1];
    bounds[3] = origin[1] + spacing[1] * height;
    bounds[4] = origin[2];
    bounds[5] = origin[2] + spacing[2];

    QTest::newRow("bitmap with data") << bitmap << bounds;
}

void test_DrawerBitmap::getBounds_ReturnsExpectedValues()
{
    QFETCH(DrawerBitmap*, drawerBitmap);
    QFETCH(QVector<double>, bounds);
    
    QVector<double> computedBounds(6);
    drawerBitmap->getBounds(computedBounds.data());

    QCOMPARE(computedBounds, bounds);
}

void test_DrawerBitmap::getDistanceToPoint_ReturnsExpectedValues_data()
{
    QTest::addColumn<QVector<double> >("point");
    QTest::addColumn<QVector<double> >("origin");
    QTest::addColumn<QVector<double> >("spacing");
    QTest::addColumn<unsigned int>("width");
    QTest::addColumn<unsigned int>("height");
    QTest::addColumn<double>("distance");
    QTest::addColumn<QVector<double> >("closestPoint");

    QVector<double> point(3);
    QVector<double> origin(3);
    QVector<double> spacing(3);
    QVector<double> closestPoint(3);
    
    unsigned int width;
    unsigned int height;

    width = 0;
    height = 0;

    point[0] = 2.3;
    point[1] = 5.1;
    point[2] = 0.6;
    
    origin[0] = origin[1] = origin[2] = 0.0;
    
    spacing[0] = spacing[1] = spacing[2] = 1.0;
    
    closestPoint[0] = closestPoint[1] = closestPoint[2] = 0.0;

    QTest::newRow("default values for empty bitmap") << point << origin << spacing << width << height << 5.62672 << closestPoint;
    
    width  = 124;
    height = 38;
    
    point[0] = 2.3;
    point[1] = 5.1;
    point[2] = 0.6;
    
    origin[0] = 9.7;
    origin[1] = 1.3;
    origin[2] = 8.0;
    
    spacing[0] = 9.7;
    spacing[1] = 1.3;
    spacing[2] = 8.0;
    
    closestPoint[0] = 9.7;
    closestPoint[1] = 5.1;
    closestPoint[2] = 8.0;

    QTest::newRow("point outside bounds") << point << origin << spacing << width << height << 10.4652 << closestPoint;

    width  = 256;
    height = 128;
    
    point[0] = 1.8;
    point[1] = 52.2;
    point[2] = 9.6;
    
    origin[0] = 0.3;
    origin[1] = 4.6;
    origin[2] = 2.7;
    
    spacing[0] = 3.2;
    spacing[1] = 6.7;
    spacing[2] = 1.2;
    
    closestPoint[0] = point[0];
    closestPoint[1] = point[1];
    closestPoint[2] = point[2];
    
    QTest::newRow("point inside bounds") << point << origin << spacing << width << height << 0.0 << closestPoint;
}

void test_DrawerBitmap::getDistanceToPoint_ReturnsExpectedValues()
{
    QFETCH(QVector<double>, point);
    QFETCH(QVector<double>, origin);
    QFETCH(QVector<double>, spacing);
    QFETCH(unsigned int, width);
    QFETCH(unsigned int, height);
    QFETCH(double, distance);
    QFETCH(QVector<double>, closestPoint);

    DrawerBitmap *bitmap = new DrawerBitmap;
    bitmap->setOrigin(origin.data());
    bitmap->setSpacing(spacing.data());
    bitmap->setData(width, height, 0);
    
    QVector<double> computedClosestPoint(3);
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(bitmap->getDistanceToPoint(point.data(), computedClosestPoint.data()), distance, 0.0001));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedClosestPoint, closestPoint));
}

DECLARE_TEST(test_DrawerBitmap)

#include "test_drawerbitmap.moc"
