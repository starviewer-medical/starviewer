#include "autotest.h"
#include "volumepixeldata.h"

#include "itkandvtkimagetesthelper.h"
#include "volumepixeldatatesthelper.h"
#include "fuzzycomparetesthelper.h"

#include "vtkImageData.h"

using namespace udg;
using namespace testing;

class test_VolumePixelData : public QObject {
Q_OBJECT
private slots:
    void constructor_ShouldCreateDefaultVtkData();

    void setData_itk_ShouldCreateExpectedVtkData_data();
    void setData_itk_ShouldCreateExpectedVtkData();

    void setData_vtk_ShouldSetDataCorrectly_data();
    void setData_vtk_ShouldSetDataCorrectly();

    void setData_WrongParametersFromArrayShouldReturn_data();
    void setData_WrongParametersFromArrayShouldReturn();
    
    void setData_ShouldSetDataFromArray_data();
    void setData_ShouldSetDataFromArray();

    void convertToNeutralPixelData_ShouldActAsExpected_data();
    void convertToNeutralPixelData_ShouldActAsExpected();

    void getScalarComponentAsDouble_ShouldReturnExpectedValueFromNeutral_data();
    void getScalarComponentAsDouble_ShouldReturnExpectedValueFromNeutral();

    void getScalarComponentAsDouble_ShouldReturnExpectedValue_data();
    void getScalarComponentAsDouble_ShouldReturnExpectedValue();
};

Q_DECLARE_METATYPE(unsigned char*)
Q_DECLARE_METATYPE(int*)
Q_DECLARE_METATYPE(VolumePixelData::ItkImageTypePointer)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkImageData>)
Q_DECLARE_METATYPE(VolumePixelData*)

void test_VolumePixelData::constructor_ShouldCreateDefaultVtkData()
{
    VolumePixelData volumePixelData;
    QVERIFY(volumePixelData.getVtkData() != 0);
}

void test_VolumePixelData::setData_itk_ShouldCreateExpectedVtkData_data()
{
    QTest::addColumn<VolumePixelData::ItkImageTypePointer>("itkData");
    QTest::addColumn<vtkSmartPointer<vtkImageData>>("expectedVtkData");

    {
        VolumePixelData::ItkImageTypePointer itkData = VolumePixelData::ItkImageType::New();
        vtkSmartPointer<vtkImageData> vtkData = vtkSmartPointer<vtkImageData>::New();
        QTest::newRow("default") << itkData << vtkData;
    }

    {
        int dimensions[3] = { 200, 200, 200 };
        int startIndex[3] = { 0, 0, 0 };
        double spacing[3] = { 0.33, 0.33, 1.20 };
        double origin[3] = { 0.0, 0.0, 0.0 };
        VolumePixelData::ItkImageTypePointer itkData;
        vtkSmartPointer<vtkImageData> vtkData;
        ItkAndVtkImageTestHelper::createItkAndVtkImages(dimensions, startIndex, spacing, origin, itkData, vtkData);
        QTest::newRow("random #1") << itkData << vtkData;
    }

    {
        int dimensions[3] = { 33, 124, 6 };
        int startIndex[3] = { 200, 169, 156 };
        double spacing[3] = { 2.2, 0.74, 1.44 };
        double origin[3] = { 48.0, 41.0, -68.0 };
        VolumePixelData::ItkImageTypePointer itkData;
        vtkSmartPointer<vtkImageData> vtkData;
        ItkAndVtkImageTestHelper::createItkAndVtkImages(dimensions, startIndex, spacing, origin, itkData, vtkData);
        QTest::newRow("random #2") << itkData << vtkData;
    }
}

void test_VolumePixelData::setData_itk_ShouldCreateExpectedVtkData()
{
    QFETCH(VolumePixelData::ItkImageTypePointer, itkData);
    QFETCH(vtkSmartPointer<vtkImageData>, expectedVtkData);

    VolumePixelData volumePixelData;
    volumePixelData.setData(itkData);
    vtkImageData *vtkData = volumePixelData.getVtkData();

    for (int i = 0; i < 3; i++)
    {
        QCOMPARE(vtkData->GetDimensions()[i], expectedVtkData->GetDimensions()[i]);
        QCOMPARE(vtkData->GetSpacing()[i], expectedVtkData->GetSpacing()[i]);
        QCOMPARE(vtkData->GetOrigin()[i], expectedVtkData->GetOrigin()[i]);
    }

    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(vtkData->GetExtent()[i], expectedVtkData->GetExtent()[i]);
    }

    VolumePixelData::VoxelType *data = static_cast<VolumePixelData::VoxelType*>(vtkData->GetScalarPointer());
    VolumePixelData::VoxelType *expectedData = static_cast<VolumePixelData::VoxelType*>(expectedVtkData->GetScalarPointer());
    int size = vtkData->GetNumberOfPoints();

    for (int i = 0; i < size; i++)
    {
        QCOMPARE(data[i], expectedData[i]);
    }
}

void test_VolumePixelData::setData_vtk_ShouldSetDataCorrectly_data()
{
    QTest::addColumn<vtkSmartPointer<vtkImageData>>("vtkData");

    QTest::newRow("null") << vtkSmartPointer<vtkImageData>();
    QTest::newRow("not null") << vtkSmartPointer<vtkImageData>::New();
}

void test_VolumePixelData::setData_vtk_ShouldSetDataCorrectly()
{
    QFETCH(vtkSmartPointer<vtkImageData>, vtkData);

    VolumePixelData volumePixelData;
    volumePixelData.setData(vtkData);

    QCOMPARE(volumePixelData.getVtkData(), vtkData.GetPointer());
}

void test_VolumePixelData::setData_WrongParametersFromArrayShouldReturn_data()
{
    QTest::addColumn<unsigned char*>("data");
    QTest::addColumn<int>("bytesPerPixel");

    unsigned char *data1 = 0;
    QTest::newRow("null data") << data1 << 1;

    unsigned char c;
    unsigned char *data2 = &c;
    QTest::newRow("0 bytes per pixel") << data2 << 0;
}

void test_VolumePixelData::setData_WrongParametersFromArrayShouldReturn()
{
    QFETCH(unsigned char*, data);
    QFETCH(int, bytesPerPixel);
    
    VolumePixelData pixelData;
    vtkImageData *vtkOldData = pixelData.getVtkData();
    
    pixelData.setData(data, 0, bytesPerPixel, false);
    vtkImageData *vtkData = pixelData.getVtkData();

    QVERIFY(vtkOldData == vtkData);
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

    int vtkExtent[6];
    pixelData->getExtent(vtkExtent);
    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(extent[i], vtkExtent[i]);
    }
    QCOMPARE(pixelData->getNumberOfScalarComponents(), bytesPerPixel);
    QCOMPARE(pixelData->getScalarType(), VTK_UNSIGNED_CHAR);

    unsigned char* dataPointer = reinterpret_cast<unsigned char*>(pixelData->getScalarPointer());
    
    for (int i = 0; i < pixelData->getNumberOfPoints(); ++i)
    {
        QCOMPARE(*dataPointer, data[i]);
        ++dataPointer;
    }

    delete[] extent;
    delete pixelData;
}

void test_VolumePixelData::convertToNeutralPixelData_ShouldActAsExpected_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");

    {
        QTest::newRow("default") << new VolumePixelData();
    }

    {
        int dimensions[3] = { 200, 200, 200 };
        int extent[6] = { 0, 199, 0, 199, 0, 199 };
        double spacing[3] = { 0.33, 0.33, 1.20 };
        double origin[3] = { 0.0, 0.0, 0.0 };
        QTest::newRow("random #1") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
    }

    {
        int dimensions[3] = { 33, 124, 6 };
        int extent[6] = { 200, 232, 169, 292, 156, 161 };
        double spacing[3] = { 2.2, 0.74, 1.44 };
        double origin[3] = { 48.0, 41.0, -68.0 };
        QTest::newRow("random #2") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
    }
}

void test_VolumePixelData::convertToNeutralPixelData_ShouldActAsExpected()
{
    QFETCH(VolumePixelData*, volumePixelData);

    volumePixelData->convertToNeutralPixelData();

    vtkImageData *vtkData = volumePixelData->getVtkData();

    QVERIFY(vtkData != 0);

    for (int i = 0; i < 3; i++)
    {
        QCOMPARE(vtkData->GetOrigin()[i], 0.0);
        QCOMPARE(vtkData->GetSpacing()[i], 1.0);
    }

    QCOMPARE(vtkData->GetDimensions()[0], 10);
    QCOMPARE(vtkData->GetDimensions()[1], 10);
    QCOMPARE(vtkData->GetDimensions()[2], 1);

    QCOMPARE(vtkData->GetExtent()[0], 0);
    QCOMPARE(vtkData->GetExtent()[1], 9);
    QCOMPARE(vtkData->GetExtent()[2], 0);
    QCOMPARE(vtkData->GetExtent()[3], 9);
    QCOMPARE(vtkData->GetExtent()[4], 0);
    QCOMPARE(vtkData->GetExtent()[5], 0);

    QCOMPARE(vtkData->GetScalarType(), VTK_SHORT);
    QCOMPARE(vtkData->GetNumberOfScalarComponents(), 1);

    QVERIFY(vtkData->GetScalarPointer() != 0);

    signed short *scalarPointer = static_cast<signed short*>(vtkData->GetScalarPointer());
    signed short expectedValue;

    for (int i = 0; i < 10; i++)
    {
        expectedValue = 150 - i * 20;

        if (i > 4)
        {
            expectedValue = 150 - (10 - i - 1) * 20;
        }

        for (int j = 0; j < 10; j++)
        {
            QCOMPARE(*scalarPointer, expectedValue);
            scalarPointer++;
        }
    }

    delete volumePixelData;
}

void test_VolumePixelData::getScalarComponentAsDouble_ShouldReturnExpectedValueFromNeutral_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    int dimensions[3] = { 0, 0, 0 };
    int extent[6] = { 0, 1, 0, 1, 0, 1 };
    double spacing[3] = { 0.1, 0.1, 0.1 };
    double origin[3] = { 0.0, 0.0, 0.0 };
    VolumePixelData *volumePixelDataTest = VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
    volumePixelDataTest->convertToNeutralPixelData();
    QTest::newRow("Every position of a neutralPixelData") << volumePixelDataTest;
}

void test_VolumePixelData::getScalarComponentAsDouble_ShouldReturnExpectedValueFromNeutral()
{
    QFETCH(VolumePixelData*, volumePixelData);

    signed short expectedValue;

    for (int i = 0; i < 10; i++)
    {
        expectedValue = 150 - i * 20;

        if (i > 4)
        {
            expectedValue = 150 - (10 - i - 1) * 20;
        }

        for (int j = 0; j < 10; j++)
        {
            QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(volumePixelData->getScalarComponentAsDouble(j, i, 0), expectedValue, 0.0000001));
        }
    }
}

void test_VolumePixelData::getScalarComponentAsDouble_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");

    vtkSmartPointer<vtkImageData> imageDataVTK = vtkSmartPointer<vtkImageData>::New();
    // Inicialitzem les dades
    imageDataVTK->SetOrigin(.0, .0, .0);
    imageDataVTK->SetSpacing(1., 1., 1.);
    imageDataVTK->SetDimensions(10, 10, 10);
    imageDataVTK->SetWholeExtent(0, 9, 0, 9, 0, 9);
    imageDataVTK->SetScalarTypeToShort();
    imageDataVTK->SetNumberOfScalarComponents(1);
    imageDataVTK->AllocateScalars();
    // Omplim el dataset perquè la imatge resultant quedi amb un cert degradat
    signed short *scalarPointer = (signed short*) imageDataVTK->GetScalarPointer();
    signed short value;
    for (int i = 0; i < 10; i++)
    {
        value = i;
        for (int j = 0; j < 10; j++)
        {
            value = value + j;
            for (int k = 0; k < 10; k++)
            {
                *scalarPointer = value;
                *scalarPointer++;
            }
        }
    }
    int dimensions[3] = { 10, 10, 10 };
    int extent[6] = { 0, 9, 0, 9, 0, 9 };
    double spacing[3] = { 1., 1., 1. };
    double origin[3] = { .0, .0, .0 };
    VolumePixelData *volumePixelDataTest = VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
    volumePixelDataTest->setData(imageDataVTK);

    QTest::newRow("Every position of a synthetic volumePixelData") << volumePixelDataTest;
}

void test_VolumePixelData::getScalarComponentAsDouble_ShouldReturnExpectedValue()
{
    QFETCH(VolumePixelData*, volumePixelData);

    signed short expectedValue;

    for (int i = 0; i < 10; i++)
    {
        expectedValue = i;
        for (int j = 0; j < 10; j++)
        {
            expectedValue = expectedValue + j;
            for (int k = 0; k < 10; k++)
            {
                QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(volumePixelData->getScalarComponentAsDouble(k, j, i), expectedValue, 0.0000001));
            }
        }
    }
}

DECLARE_TEST(test_VolumePixelData)

#include "test_volumepixeldata.moc"
