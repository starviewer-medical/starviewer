#include "volumepixeldatatesthelper.h"

#include <itkImageRegionIterator.h>

namespace testing {

VolumePixelData* VolumePixelDataTestHelper::createVolumePixelData(int dimensions[3], int extent[6], double spacing[3], double origin[3])
{
    vtkImageData *vtkImage = vtkImageData::New();
    vtkImage->SetDimensions(dimensions);
    vtkImage->SetExtent(extent);
    vtkImage->SetSpacing(spacing);
    vtkImage->SetOrigin(origin);
    vtkImage->AllocateScalars();

    VolumePixelData::VoxelType *vtkPointer = static_cast<VolumePixelData::VoxelType*>(vtkImage->GetScalarPointer());
    int size = vtkImage->GetNumberOfPoints();

    for (int i = 0; i < size; i++)
    {
        vtkPointer[i] = i;
    }

    VolumePixelData *volumePixelData = new VolumePixelData();
    volumePixelData->setData(vtkImage);

    vtkImage->Delete();

    return volumePixelData;
}

void VolumePixelDataTestHelper::createItkAndVtkImages(int dimensions[3], int startIndex[3], double spacing[3], double origin[3],
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

}
