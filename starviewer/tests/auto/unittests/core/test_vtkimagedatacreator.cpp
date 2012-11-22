#include "autotest.h"
#include "vtkimagedatacreator.h"

#include "itkandvtkimagetesthelper.h"

#include <vtkImageData.h>

using namespace udg;
using namespace testing;

class test_VtkImageDataCreator : public QObject {
Q_OBJECT

private slots:

    void createVtkImageData_ShouldReturnNull();

    void createVtkImageData_ShouldReturnExpectedVtkImageData_data();
    void createVtkImageData_ShouldReturnExpectedVtkImageData();
};

Q_DECLARE_METATYPE(QSharedPointer<VtkImageDataCreator>)
Q_DECLARE_METATYPE(QVector<unsigned char>)
Q_DECLARE_METATYPE(QVector<unsigned short>)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkImageData>)

void test_VtkImageDataCreator::createVtkImageData_ShouldReturnNull()
{
    VtkImageDataCreator imageDataCreator;
    QCOMPARE(imageDataCreator.createVtkImageData(1, 1, 1, static_cast<char*>(nullptr)), vtkSmartPointer<vtkImageData>());
}

void test_VtkImageDataCreator::createVtkImageData_ShouldReturnExpectedVtkImageData_data()
{
    QTest::addColumn<QSharedPointer<VtkImageDataCreator>>("imageDataCreator");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<QVector<unsigned char>>("ucharData");
    QTest::addColumn<QVector<unsigned short>>("ushortData");
    QTest::addColumn<vtkSmartPointer<vtkImageData>>("expectedUcharVtkImageData");
    QTest::addColumn<vtkSmartPointer<vtkImageData>>("expectedUshortVtkImageData");

    double origin[3] = { 1.0, 2.0, 3.0 };
    double spacing[3] = { 0.5, 0.5, 2.5 };
    int width = 256;
    int height = 256;
    int depth = 100;
    int size = width * height * depth;
    QVector<unsigned char> ucharData(size);
    QVector<unsigned short> ushortData(size);
    for (int i = 0; i < size; i++)
    {
        ucharData[i] = i;
        ushortData[i] = i;
    }

    auto imageDataCreator = QSharedPointer<VtkImageDataCreator>(new VtkImageDataCreator());
    imageDataCreator->setOrigin(origin);
    imageDataCreator->setSpacing(spacing);

    auto expectedUcharVtkImageData = vtkSmartPointer<vtkImageData>::New();
    expectedUcharVtkImageData->SetOrigin(origin);
    expectedUcharVtkImageData->SetSpacing(spacing);
    expectedUcharVtkImageData->SetExtent(0, width - 1, 0, height - 1, 0, depth - 1);
    expectedUcharVtkImageData->SetScalarTypeToUnsignedChar();
    expectedUcharVtkImageData->SetNumberOfScalarComponents(1);
    expectedUcharVtkImageData->AllocateScalars();
    memcpy(expectedUcharVtkImageData->GetScalarPointer(), ucharData.constData(), size * sizeof(unsigned char));

    auto expectedUshortVtkImageData = vtkSmartPointer<vtkImageData>::New();
    expectedUshortVtkImageData->SetOrigin(origin);
    expectedUshortVtkImageData->SetSpacing(spacing);
    expectedUshortVtkImageData->SetExtent(0, width - 1, 0, height - 1, 0, depth - 1);
    expectedUshortVtkImageData->SetScalarTypeToUnsignedShort();
    expectedUshortVtkImageData->SetNumberOfScalarComponents(1);
    expectedUshortVtkImageData->AllocateScalars();
    memcpy(expectedUshortVtkImageData->GetScalarPointer(), ushortData.constData(), size * sizeof(unsigned short));

    QTest::newRow("non-null data") << imageDataCreator << width << height << depth << ucharData << ushortData << expectedUcharVtkImageData
                                   << expectedUshortVtkImageData;
}

void test_VtkImageDataCreator::createVtkImageData_ShouldReturnExpectedVtkImageData()
{
    QFETCH(QSharedPointer<VtkImageDataCreator>, imageDataCreator);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(QVector<unsigned char>, ucharData);
    QFETCH(QVector<unsigned short>, ushortData);
    QFETCH(vtkSmartPointer<vtkImageData>, expectedUcharVtkImageData);
    QFETCH(vtkSmartPointer<vtkImageData>, expectedUshortVtkImageData);

    bool equal;
    ItkAndVtkImageTestHelper::compareVtkImageData(imageDataCreator->createVtkImageData(width, height, depth, ucharData.constData()),
                                                  expectedUcharVtkImageData, equal);
    QVERIFY2(equal, "not equal for uchar");
    ItkAndVtkImageTestHelper::compareVtkImageData(imageDataCreator->createVtkImageData(width, height, depth, ushortData.constData()),
                                                  expectedUshortVtkImageData, equal);
    QVERIFY2(equal, "not equal for ushort");
}

DECLARE_TEST(test_VtkImageDataCreator)

#include "test_vtkimagedatacreator.moc"
