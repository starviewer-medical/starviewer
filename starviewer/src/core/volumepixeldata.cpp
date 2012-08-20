#include "volumepixeldata.h"

#include <vtkImageData.h>
// Voxel information
#include <vtkPointData.h>
// Pel setData(unsigned char *...)
#include <vtkUnsignedCharArray.h>

#include "logging.h"

namespace udg {

VolumePixelData::VolumePixelData(QObject *parent) :
    QObject(parent)
{
    m_imageDataVTK = vtkSmartPointer<vtkImageData>::New();

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
    catch (itk::ExceptionObject &excep)
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

void VolumePixelData::setData(unsigned char *data, int extent[6], int bytesPerPixel, bool deleteData)
{
    if (!data)
    {
        return;
    }
    
    if (bytesPerPixel < 1)
    {
        DEBUG_LOG("Bytes per pixel should be > 0");
        return;
    }
    
    vtkImageData *imageData = vtkImageData::New();
    imageData->SetExtent(extent);
    imageData->SetScalarTypeToUnsignedChar();
    imageData->SetNumberOfScalarComponents(bytesPerPixel);
    imageData->AllocateScalars();
    
    int size = (extent[1] - extent[0] + 1) * (extent[3] - extent[2] + 1) * (extent[5] - extent[4] + 1) * bytesPerPixel;
    vtkUnsignedCharArray *ucharArray = vtkUnsignedCharArray::New();
    ucharArray->SetNumberOfTuples(size);
    
    int save;
    if (deleteData)
    {
        // save == 0, NO guardar les dades == fer delete, 
        save = 0;
    }
    else
    {
        // save == 1, guardar les dades == NO fer delete, 
        save = 1;
    }
    ucharArray->SetArray(data, size, save);
    imageData->GetPointData()->SetScalars(ucharArray);
    ucharArray->Delete();

    this->setData(imageData);
    imageData->Delete();
}

VolumePixelData::VoxelType* VolumePixelData::getScalarPointer(int x, int y, int z)
{
    return static_cast<VolumePixelData::VoxelType*>(this->getVtkData()->GetScalarPointer(x, y, z));
}

bool VolumePixelData::computeCoordinateIndex(const double coordinate[3], int index[3])
{
    if (!this->getVtkData())
    {
        DEBUG_LOG("Dades VTK nul·les!");
        return false;
    }

    double *origin = this->getVtkData()->GetOrigin();
    double *spacing = this->getVtkData()->GetSpacing();
    int *extent = this->getVtkData()->GetExtent();
    bool inside = true;

    for (int i = 0; i < 3; i++)
    {
        index[i] = qRound((coordinate[i] - origin[i]) / spacing[i]);
        // TODO És sempre correcte això?
        inside = inside && index[i] >= extent[2 * i] && index[i] <= extent[2 * i + 1];
    }

    return inside;
}

bool VolumePixelData::getVoxelValue(double coordinate[3], QVector<double> &voxelValue, int phaseNumber, int numberOfPhases)
{
    if (!this->getVtkData())
    {
        DEBUG_LOG("Dades VTK nul·les!");
        voxelValue.clear();
        return false;
    }

    int voxelIndex[3];
    bool inside = this->computeCoordinateIndex(coordinate, voxelIndex);

    if (inside)
    {
        // HACK Aquest càlcul és necessari per pal·liar la manca de coneixement de la fase
        // TODO Cal resoldre això d'una forma més elegant, el qual comporta un redisseny del tractament de fases i volums
        // Calculem l'índex correcte en cas que tinguem fases
        voxelIndex[2] = voxelIndex[2] * numberOfPhases + phaseNumber;
        
        vtkIdType pointId = this->getVtkData()->ComputePointId(voxelIndex);
        vtkDataArray *scalars = this->getVtkData()->GetPointData()->GetScalars();
        int numberOfComponents = scalars->GetNumberOfComponents();
        voxelValue.resize(numberOfComponents);

        for (int i = 0; i < numberOfComponents; i++)
        {
            voxelValue[i] = scalars->GetComponent(pointId, i);
        }

        return true;
    }
    else
    {
        voxelValue.clear();
        return false;
    }
}

void VolumePixelData::convertToNeutralPixelData()
{
    // Creem un objecte vtkImageData "neutre"
    m_imageDataVTK = vtkSmartPointer<vtkImageData>::New();
    // Inicialitzem les dades
    m_imageDataVTK->SetOrigin(.0, .0, .0);
    m_imageDataVTK->SetSpacing(1., 1., 1.);
    m_imageDataVTK->SetDimensions(10, 10, 1);
    m_imageDataVTK->SetWholeExtent(0, 9, 0, 9, 0, 0);
    m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
    // Omplim el dataset perquè la imatge resultant quedi amb un cert degradat
    signed short *scalarPointer = (signed short*) m_imageDataVTK->GetScalarPointer();
    signed short value;
    for (int i = 0; i < 10; i++)
    {
        value = 150 - i * 20;
        if (i > 4)
        {
            value = 150 - (10 - i - 1) * 20;
        }

        for (int j = 0; j < 10; j++)
        {
            *scalarPointer = value;
            *scalarPointer++;
        }
    }
}

void VolumePixelData::setOrigin(double origin[3])
{
    this->setOrigin(origin[0], origin[1], origin[2]);
}

void VolumePixelData::setOrigin(double x, double y, double z)
{
    m_imageDataVTK->SetOrigin(x, y, z);
}

void VolumePixelData::getOrigin(double origin[3])
{
    return m_imageDataVTK->GetOrigin(origin);
}

void VolumePixelData::setSpacing(double spacing[3])
{
    this->setSpacing(spacing[0], spacing[1], spacing[2]);
}

void VolumePixelData::setSpacing(double x, double y, double z)
{
   m_imageDataVTK->SetSpacing(x, y, z);
}

void VolumePixelData::getSpacing(double spacing[3])
{
    return m_imageDataVTK->GetSpacing(spacing);
}

void VolumePixelData::getExtent(int extent[6])
{
    return m_imageDataVTK->GetExtent(extent);
}

} // End namespace udg
