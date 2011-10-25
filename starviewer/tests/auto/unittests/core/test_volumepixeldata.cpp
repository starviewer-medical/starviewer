#include "autotest.h"

#include "volumepixeldata.h"

using namespace udg;

class test_VolumePixelData : public QObject {
Q_OBJECT
private slots:
    void setData_WrongParametersFromArrayShouldReturn_data();
    void setData_WrongParametersFromArrayShouldReturn();
    
    void setData_ShouldSetDataFromArray_data();
    void setData_ShouldSetDataFromArray();
};

Q_DECLARE_METATYPE(unsigned char*);
Q_DECLARE_METATYPE(int*);

void test_VolumePixelData::setData_WrongParametersFromArrayShouldReturn_data()
{
    QTest::addColumn<unsigned char*>("data");
    QTest::addColumn<int*>("extent");
    QTest::addColumn<int>("bytesPerPixel");

    int *extent1 = new int[6];
    unsigned char *data1 = 0;
    QTest::newRow("null data") << data1 << extent1 << 1;

    int *extent2 = new int[6];
    extent2[0] = extent2[2] = extent2[4] = extent2[5] = 0;
    extent2[1] = extent2[3] = 9;
    unsigned char *data2 = new unsigned char[100];
    memset(data2, 255, 100);
    data2[21] = 50;
    data2[17] = 90;
    data2[99] = 13;
    QTest::newRow("0 bytes per pixel") << data2 << extent2 << 0;
}

void test_VolumePixelData::setData_WrongParametersFromArrayShouldReturn()
{
    QFETCH(unsigned char*, data);
    QFETCH(int*, extent);
    QFETCH(int, bytesPerPixel);
    
    VolumePixelData *pixelData = new VolumePixelData();
    vtkImageData *vtkOldData = pixelData->getVtkData();
    
    pixelData->setData(data, extent, bytesPerPixel, false);
    vtkImageData *vtkData = pixelData->getVtkData();

    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(vtkData->GetExtent()[i], vtkOldData->GetExtent()[i]);
    }

    QCOMPARE(vtkData->GetNumberOfScalarComponents(), vtkOldData->GetNumberOfScalarComponents());
    QCOMPARE(vtkData->GetScalarType(), vtkOldData->GetScalarType());

    QCOMPARE(vtkData->GetNumberOfPoints(), vtkOldData->GetNumberOfPoints());
    
    unsigned char *dataPointer = reinterpret_cast<unsigned char*>(vtkData->GetScalarPointer());
    unsigned char *oldDataPointer = reinterpret_cast<unsigned char*>(vtkOldData->GetScalarPointer());
    for (int i = 0; i < vtkData->GetNumberOfPoints(); ++i)
    {
        QCOMPARE(*dataPointer, *oldDataPointer);
        ++dataPointer;
        ++oldDataPointer;
    }
}

void test_VolumePixelData::setData_ShouldSetDataFromArray_data()
{
    QTest::addColumn<unsigned char*>("data");
    QTest::addColumn<int*>("extent");
    QTest::addColumn<int>("bytesPerPixel");
    
    int *extent1 = new int[6];
    extent1[0] = extent1[2] = extent1[4] = extent1[5] = 0;
    extent1[1] = extent1[3] = 9;
    unsigned char *data1 = new unsigned char[100];
    memset(data1, 255, 100);
    data1[21] = 50;
    data1[17] = 90;
    data1[99] = 13;

    QTest::newRow("some data with 1 byte per pixel") << data1 << extent1 << 1;

    int *extent2 = new int[6];
    extent2[0] = extent2[2] = 10;
    extent2[1] = extent2[3] = 19;
    extent2[4] = extent2[5] = 6;
    unsigned char *data2 = new unsigned char[200];
    memset(data2, 255, 200);
    data2[150] = 124;
    data2[199] = 37;
    data2[101] = 13;
    data2[85] = 55;

    QTest::newRow("some data with 2 bytes per pixel, extent [10,19;10,19;6,6]") << data2 << extent2 << 2;
}

void test_VolumePixelData::setData_ShouldSetDataFromArray()
{
    QFETCH(unsigned char*, data);
    QFETCH(int*, extent);
    QFETCH(int, bytesPerPixel);
    
    VolumePixelData *pixelData = new VolumePixelData();
    pixelData->setData(data, extent, bytesPerPixel, true);

    vtkImageData *vtkData = pixelData->getVtkData();

    int vtkExtent[6];
    vtkData->GetExtent(vtkExtent);
    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(extent[i], vtkExtent[i]);
    }
    QCOMPARE(vtkData->GetNumberOfScalarComponents(), bytesPerPixel);
    QCOMPARE(vtkData->GetScalarType(), VTK_UNSIGNED_CHAR);

    unsigned char *dataPointer = reinterpret_cast<unsigned char*>(vtkData->GetScalarPointer());
    for (int i = 0; i < vtkData->GetNumberOfPoints(); ++i)
    {
        QCOMPARE(*dataPointer, data[i]);
        ++dataPointer;
    }
}

DECLARE_TEST(test_VolumePixelData)

#include "test_volumepixeldata.moc"
