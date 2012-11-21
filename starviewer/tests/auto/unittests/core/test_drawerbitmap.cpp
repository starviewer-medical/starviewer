#include "autotest.h"

#include "drawerbitmap.h"
#include "fuzzycomparetesthelper.h"
#include "itkandvtkimagetesthelper.h"

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

using namespace udg;
using namespace testing;

class test_DrawerBitmap : public QObject {
Q_OBJECT

private slots:
    void getBounds_ReturnsExpectedValues_data();
    void getBounds_ReturnsExpectedValues();

    void getDistanceToPoint_ReturnsExpectedValues_data();
    void getDistanceToPoint_ReturnsExpectedValues();

    void getAsVtkProp_ShouldReturnNull();

    void getAsVtkProp_ShouldReturnPropLikeExpected_data();
    void getAsVtkProp_ShouldReturnPropLikeExpected();
};

Q_DECLARE_METATYPE(DrawerBitmap*)
Q_DECLARE_METATYPE(QVector<double>)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkImageActor>)

void test_DrawerBitmap::getBounds_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerBitmap*>("drawerBitmap");
    QTest::addColumn<QVector<double> >("bounds");

    DrawerBitmap *emptyBitmap = new DrawerBitmap(this);
    QVector<double> emptyBounds(6, 0.0);
    emptyBounds[5] = 1.0;
    QTest::newRow("Empty drawer bitmap") << emptyBitmap << emptyBounds;

    DrawerBitmap *bitmap = new DrawerBitmap(this);
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

    DrawerBitmap *bitmap = new DrawerBitmap(this);
    bitmap->setOrigin(origin.data());
    bitmap->setSpacing(spacing.data());
    bitmap->setData(width, height, 0);
    
    QVector<double> computedClosestPoint(3);
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(bitmap->getDistanceToPoint(point.data(), computedClosestPoint.data()), distance, 0.0001));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedClosestPoint, closestPoint));
}

void test_DrawerBitmap::getAsVtkProp_ShouldReturnNull()
{
    QCOMPARE((new DrawerBitmap(this))->getAsVtkProp(), static_cast<vtkProp*>(nullptr));
}

void test_DrawerBitmap::getAsVtkProp_ShouldReturnPropLikeExpected_data()
{
    QTest::addColumn<DrawerBitmap*>("drawerBitmap");
    QTest::addColumn<vtkSmartPointer<vtkImageActor>>("expectedProp");

    double origin[3] = { 1.0, 2.0, 3.0 };
    double spacing[3] = { 0.5, 0.5, 2.5 };
    unsigned int width = 256;
    unsigned int height = 256;
    unsigned char *data = new unsigned char[width * height];
    for (unsigned int i = 0; i < width * height; i++)
    {
        data[i] = i;
    }
    double backgroundOpacity = 0.1;
    QColor backgroundColor(12, 12, 14);
    QColor foregroundColor(2, 25, 255);

    DrawerBitmap *drawerBitmap = new DrawerBitmap(this);
    drawerBitmap->setOrigin(origin);
    drawerBitmap->setSpacing(spacing);
    drawerBitmap->setData(width, height, data);
    drawerBitmap->setBackgroundOpacity(backgroundOpacity);
    drawerBitmap->setBackgroundColor(backgroundColor);
    drawerBitmap->setForegroundColor(foregroundColor);

    vtkImageData *imageData = vtkImageData::New();
    imageData->SetOrigin(origin);
    imageData->SetSpacing(spacing);
    imageData->SetExtent(0, width - 1, 0, height - 1, 0, 0);
    imageData->SetScalarTypeToUnsignedChar();
    imageData->SetNumberOfScalarComponents(1);
    imageData->AllocateScalars();
    memcpy(imageData->GetScalarPointer(), data, width * height * sizeof(unsigned char));

    vtkLookupTable *lookupTable = vtkLookupTable::New();
    lookupTable->SetNumberOfTableValues(2);
    lookupTable->SetRange(0.0, 1.0);
    lookupTable->SetTableValue(0, backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundOpacity);
    lookupTable->SetTableValue(1, foregroundColor.redF(), foregroundColor.greenF(), foregroundColor.blueF(), 1.0);
    lookupTable->Build();

    vtkImageMapToColors *mapTransparency = vtkImageMapToColors::New();
    mapTransparency->SetLookupTable(lookupTable);
    mapTransparency->SetInput(imageData);
    mapTransparency->PassAlphaToOutputOn();

    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInput(mapTransparency->GetOutput());
    imageActor->SetDisplayExtent(0, width - 1, 0, height - 1, 0, 0);
    imageActor->SetVisibility(true);

    imageData->Delete();
    lookupTable->Delete();
    mapTransparency->Delete();

    QTest::newRow("non-empty drawer bitmap") << drawerBitmap << imageActor;
}

void test_DrawerBitmap::getAsVtkProp_ShouldReturnPropLikeExpected()
{
    QFETCH(DrawerBitmap*, drawerBitmap);
    QFETCH(vtkSmartPointer<vtkImageActor>, expectedProp);

    vtkImageActor *imageActor = vtkImageActor::SafeDownCast(drawerBitmap->getAsVtkProp());
    QVERIFY(imageActor != 0);
    QCOMPARE(imageActor->GetVisibility(), expectedProp->GetVisibility());
    
    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(imageActor->GetDisplayExtent()[i], expectedProp->GetDisplayExtent()[i]);
    }

    bool equal;
    ItkAndVtkImageTestHelper::compareVtkImageData(imageActor->GetInput(), expectedProp->GetInput(), equal);
    QVERIFY(equal);
}

DECLARE_TEST(test_DrawerBitmap)

#include "test_drawerbitmap.moc"
