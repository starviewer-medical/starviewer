#include "autotest.h"

#include "drawerbitmap.h"
#include "fuzzycomparetesthelper.h"
#include "itkandvtkimagetesthelper.h"

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
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
Q_DECLARE_METATYPE(Vector3)

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
    QTest::addColumn<Vector3>("point");
    QTest::addColumn<Vector3>("origin");
    QTest::addColumn<Vector3>("spacing");
    QTest::addColumn<unsigned int>("width");
    QTest::addColumn<unsigned int>("height");
    QTest::addColumn<double>("distance");
    QTest::addColumn<Vector3>("closestPoint");

    QTest::newRow("default values for empty bitmap") << Vector3(2.3, 5.1, 0.6) << Vector3(0.0, 0.0, 0.0) << Vector3(1.0, 1.0, 1.0) << 0u << 0u << 5.62672
                                                     << Vector3(0.0, 0.0, 0.0);

    QTest::newRow("point outside bounds") << Vector3(2.3, 5.1, 0.6) << Vector3(9.7, 1.3, 8.0) << Vector3(9.7, 1.3, 8.0) << 124u << 38u << 10.4652
                                          << Vector3(9.7, 5.1, 8.0);
    
    QTest::newRow("point inside bounds") << Vector3(1.8, 52.2, 9.6) << Vector3(0.3, 4.6, 2.7) << Vector3(3.2, 6.7, 1.2) << 256u << 128u << 0.0 <<
                                            Vector3(1.8, 52.2, 9.6);
}

void test_DrawerBitmap::getDistanceToPoint_ReturnsExpectedValues()
{
    QFETCH(Vector3, point);
    QFETCH(Vector3, origin);
    QFETCH(Vector3, spacing);
    QFETCH(unsigned int, width);
    QFETCH(unsigned int, height);
    QFETCH(double, distance);
    QFETCH(Vector3, closestPoint);

    DrawerBitmap *bitmap = new DrawerBitmap(this);
    bitmap->setOrigin(origin.data());
    bitmap->setSpacing(spacing.data());
    bitmap->setData(width, height, 0);
    
    Vector3 computedClosestPoint;
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(bitmap->getDistanceToPoint(point, computedClosestPoint), distance, 0.0001));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedClosestPoint, closestPoint));
}

void test_DrawerBitmap::getAsVtkProp_ShouldReturnNull()
{
    QCOMPARE((new DrawerBitmap(this))->getAsVtkProp(), static_cast<vtkProp*>(NULL));
}

void test_DrawerBitmap::getAsVtkProp_ShouldReturnPropLikeExpected_data()
{
    QTest::addColumn<DrawerBitmap*>("drawerBitmap");
    QTest::addColumn< vtkSmartPointer<vtkImageActor> >("expectedProp");

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
    imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    memcpy(imageData->GetScalarPointer(), data, width * height * sizeof(unsigned char));

    vtkLookupTable *lookupTable = vtkLookupTable::New();
    lookupTable->SetNumberOfTableValues(2);
    lookupTable->SetRange(0.0, 1.0);
    lookupTable->SetTableValue(0, backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundOpacity);
    lookupTable->SetTableValue(1, foregroundColor.redF(), foregroundColor.greenF(), foregroundColor.blueF(), 1.0);
    lookupTable->Build();

    vtkImageMapToColors *mapTransparency = vtkImageMapToColors::New();
    mapTransparency->SetLookupTable(lookupTable);
    mapTransparency->SetInputData(imageData);
    mapTransparency->PassAlphaToOutputOn();

    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
    imageActor->GetMapper()->SetInputConnection(mapTransparency->GetOutputPort());
    imageActor->SetDisplayExtent(0, width - 1, 0, height - 1, 0, 0);
    imageActor->SetVisibility(true);
    imageActor->Update();

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
