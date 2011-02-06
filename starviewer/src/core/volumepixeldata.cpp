#include "volumepixeldata.h"

#include <vtkImageData.h>
// Voxel information
#include <vtkPointData.h>
#include <vtkCell.h>

#include "logging.h"

namespace udg {

VolumePixelData::VolumePixelData(QObject *parent) :
    QObject(parent)
{
    m_imageDataVTK = vtkImageData::New();

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();
}

VolumePixelData::ItkImageTypePointer VolumePixelData::getItkData()
{
    m_vtkToItkFilter->SetInput(this->getVtkData());
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch (itk::ExceptionObject &excep)
    {
        WARN_LOG(QString("Excepció en el filtre vtkToItk :: Volume::getItkData() -> ") + excep.GetDescription());
    }
    return m_vtkToItkFilter->GetImporter()->GetOutput();
}

void VolumePixelData::setData(ItkImageTypePointer itkImage)
{
    m_itkToVtkFilter->SetInput(itkImage);
    try
    {
        m_itkToVtkFilter->Update();
    }
    catch (itk::ExceptionObject & excep)
    {
        WARN_LOG(QString("Excepció en el filtre itkToVtk :: Volume::setData(ItkImageTypePointer itkImage) -> ") + excep.GetDescription());
    }
    this->setData(m_itkToVtkFilter->GetOutput());
}

vtkImageData* VolumePixelData::getVtkData()
{
    return m_imageDataVTK;
}

void VolumePixelData::setData(vtkImageData *vtkImage)
{
    if (m_imageDataVTK)
    {
        m_imageDataVTK->ReleaseData();
    }
    m_imageDataVTK = vtkImage;
}

VolumePixelData::VoxelType* VolumePixelData::getScalarPointer(int x, int y, int z)
{
    return static_cast<VolumePixelData::VoxelType *>(this->getVtkData()->GetScalarPointer(x,y,z));
}

bool VolumePixelData::getVoxelValue(double coordinate[3], VolumePixelData::VoxelType &voxelValue)
{
    vtkImageData *vtkData = this->getVtkData();
    if (!vtkData)
    {
        DEBUG_LOG("Dades VTK nul·les!");
        return false;
    }

    // Use tolerance as a function of size of source data
    double tolerance = vtkData->GetLength();
    tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

    int subCellId;
    double parametricCoordinates[3], interpolationWeights[8];
    bool found = false;

    // Find the cell that contains q and get it
    vtkCell *cell = vtkData->FindAndGetCell(coordinate, NULL, -1, tolerance, subCellId, parametricCoordinates, interpolationWeights);
    if (cell)
    {
        vtkPointData *pointData = vtkData->GetPointData();
        vtkPointData *outPointData = vtkPointData::New();
        outPointData->InterpolateAllocate(pointData, 1, 1);
        // Interpolate the point data
        outPointData->InterpolatePoint(pointData, 0, cell->PointIds, interpolationWeights);
        voxelValue = outPointData->GetScalars()->GetTuple1(0);
        found = true;
        outPointData->Delete();
    }

    return found;
}

void VolumePixelData::convertToNeutralPixelData()
{
    if (m_imageDataVTK)
    {
        m_imageDataVTK->Delete();
    }

    // Creem un objecte vtkImageData "neutre"
    m_imageDataVTK = vtkImageData::New();
    // Inicialitzem les dades
    m_imageDataVTK->SetOrigin(.0, .0, .0);
    m_imageDataVTK->SetSpacing(1., 1., 1.);
    m_imageDataVTK->SetDimensions(10, 10, 1);
    m_imageDataVTK->SetWholeExtent(0, 9, 0, 9, 0, 0);
    m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
    // Omplim el dataset perquè la imatge resultant quedi amb un cert degradat
    signed short *scalarPointer = (signed short *) m_imageDataVTK->GetScalarPointer();
    signed short value;
    for (int i = 0; i < 10; i++)
    {
        value = 150 - i * 20;
        if (i > 4)
        {
            value = 150 - (10 - i - 1)*20;
        }

        for (int j = 0; j < 10; j++)
        {
            *scalarPointer = value;
            *scalarPointer++;
        }
    }
}

} // End namespace udg
