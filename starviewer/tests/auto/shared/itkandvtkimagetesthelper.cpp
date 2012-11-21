#include "itkandvtkimagetesthelper.h"

#include "fuzzycomparetesthelper.h"

#include <QTest>

#include <itkImageRegionIterator.h>

namespace testing {

VolumePixelData::ItkImageTypePointer ItkAndVtkImageTestHelper::createItkImage(int dimensions[3], int startIndex[3], double spacing[3], double origin[3])
{
    VolumePixelData::ItkImageType::SizeType size = { dimensions[0], dimensions[1], dimensions[2] };
    VolumePixelData::ItkImageType::IndexType index = { startIndex[0], startIndex[1], startIndex[2] };
    VolumePixelData::ItkImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(index);
    VolumePixelData::ItkImageTypePointer itkImage = VolumePixelData::ItkImageType::New();
    itkImage->SetRegions(region);
    itkImage->SetSpacing(spacing);
    itkImage->SetOrigin(origin);
    itkImage->Allocate();

    itk::ImageRegionIterator<VolumePixelData::ItkImageType> itkIterator(itkImage, itkImage->GetLargestPossibleRegion());
    itkIterator.GoToBegin();
    VolumePixelData::VoxelType value = 0;

    while (!itkIterator.IsAtEnd())
    {
        itkIterator.Set(value);
        ++itkIterator;
        ++value;
    }

    return itkImage;
}

void ItkAndVtkImageTestHelper::createItkAndVtkImages(int dimensions[3], int startIndex[3], double spacing[3], double origin[3],
                                                     VolumePixelData::ItkImageTypePointer &itkImage, vtkSmartPointer<vtkImageData> &vtkImage)
{
    VolumePixelData::ItkImageType::SizeType size = { dimensions[0], dimensions[1], dimensions[2] };
    VolumePixelData::ItkImageType::IndexType index = { startIndex[0], startIndex[1], startIndex[2] };
    VolumePixelData::ItkImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(index);
    itkImage = VolumePixelData::ItkImageType::New();
    itkImage->SetRegions(region);
    itkImage->SetSpacing(spacing);
    itkImage->SetOrigin(origin);
    itkImage->Allocate();

    vtkImage = vtkSmartPointer<vtkImageData>::New();
    vtkImage->SetDimensions(dimensions);
    int extent[6] = { startIndex[0], startIndex[0] + dimensions[0] - 1, startIndex[1], startIndex[1] + dimensions[1] - 1, startIndex[2],
                      startIndex[2] + dimensions[2] - 1 };
    vtkImage->SetExtent(extent);
    vtkImage->SetWholeExtent(extent);
    vtkImage->SetSpacing(spacing);
    vtkImage->SetOrigin(origin);
    vtkImage->AllocateScalars();

    itk::ImageRegionIterator<VolumePixelData::ItkImageType> itkIterator(itkImage, itkImage->GetLargestPossibleRegion());
    itkIterator.GoToBegin();
    VolumePixelData::VoxelType *vtkPointer = static_cast<VolumePixelData::VoxelType*>(vtkImage->GetScalarPointer());
    VolumePixelData::VoxelType value = 0;

    while (!itkIterator.IsAtEnd())
    {
        itkIterator.Set(value);
        *vtkPointer = value;
        ++itkIterator;
        ++vtkPointer;
        ++value;
    }
}

void ItkAndVtkImageTestHelper::compareVtkImageData(vtkImageData *actualImageData, vtkImageData *expectedImageData, bool &equal)
{
    equal = false;

    if (actualImageData == expectedImageData)
    {
        equal = true;
        return;
    }

    QVERIFY(actualImageData != 0);
    QVERIFY(expectedImageData != 0);

    for (int i = 0; i < 3; i++)
    {
        QCOMPARE(actualImageData->GetDimensions()[i], expectedImageData->GetDimensions()[i]);
        QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(actualImageData->GetSpacing()[i], expectedImageData->GetSpacing()[i]),
            qPrintable(QString("actualSpacing[%1] = %2, expectedSpacing[%1] = %3").arg(i).arg(actualImageData->GetSpacing()[i])
                                                                                  .arg(expectedImageData->GetSpacing()[i])));
        QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(actualImageData->GetOrigin()[i], expectedImageData->GetOrigin()[i]),
            qPrintable(QString("actualOrigin[%1] = %2, expectedOrigin[%1] = %3").arg(i).arg(actualImageData->GetOrigin()[i])
                                                                                .arg(expectedImageData->GetOrigin()[i])));
    }

    for (int i = 0; i < 6; i++)
    {
        QCOMPARE(actualImageData->GetExtent()[i], expectedImageData->GetExtent()[i]);
        QCOMPARE(actualImageData->GetWholeExtent()[i], expectedImageData->GetWholeExtent()[i]);
    }

    QCOMPARE(actualImageData->GetScalarType(), expectedImageData->GetScalarType());
    QCOMPARE(actualImageData->GetNumberOfScalarComponents(), expectedImageData->GetNumberOfScalarComponents());

    QVERIFY(memcmp(actualImageData->GetScalarPointer(), expectedImageData->GetScalarPointer(),
                   actualImageData->GetNumberOfPoints() * actualImageData->GetNumberOfScalarComponents() * actualImageData->GetScalarSize()) == 0);

    equal = true;
}

}
