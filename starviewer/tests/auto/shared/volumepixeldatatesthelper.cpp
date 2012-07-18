#include "volumepixeldatatesthelper.h"

#include "volumepixeldata.h"

using namespace udg;

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

}
