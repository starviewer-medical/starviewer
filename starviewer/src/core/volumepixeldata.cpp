/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "volumepixeldata.h"

#include "volumepixeldataiterator.h"
#include "voxel.h"
#include "mathtools.h"

#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>

#include "logging.h"

namespace udg {

VolumePixelData::VolumePixelData(QObject *parent) :
    QObject(parent), m_loaded(false)
{
    setNumberOfPhases(1);
    
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
    if (m_imageDataVTK == vtkImage)
    {
        return;
    }
    
    if (m_imageDataVTK)
    {
        m_imageDataVTK->ReleaseData();
    }
    m_imageDataVTK = vtkImage;
    // Si el punter que ens assignen no és nul considerem que són dades carregades
    m_loaded = vtkImage != 0;
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
    imageData->AllocateScalars(VTK_UNSIGNED_CHAR, bytesPerPixel);
    
    int size = (extent[1] - extent[0] + 1) * (extent[3] - extent[2] + 1) * (extent[5] - extent[4] + 1) * bytesPerPixel;
    memcpy(imageData->GetScalarPointer(), data, size);
    
    if (deleteData)
    {
        delete[] data;
    }

    this->setData(imageData);
    imageData->Delete();
}

void VolumePixelData::setNumberOfPhases(int numberOfPhases)
{
    if (numberOfPhases > 0)
    {
        m_numberOfPhases = numberOfPhases;
    }
}

bool VolumePixelData::isLoaded() const
{
    return m_loaded;
}

void* VolumePixelData::getScalarPointer(int x, int y, int z)
{
    return this->getVtkData()->GetScalarPointer(x, y, z);
}

void* VolumePixelData::getScalarPointer()
{
    return this->getVtkData()->GetScalarPointer();
}

VolumePixelDataIterator VolumePixelData::getIterator(int x, int y, int z)
{
    return VolumePixelDataIterator(this, x, y, z);
}

VolumePixelDataIterator VolumePixelData::getIterator()
{
    return VolumePixelDataIterator(this);
}

bool VolumePixelData::computeCoordinateIndex(const double coordinate[3], int index[3], int phaseNumber)
{
    if (!this->getVtkData())
    {
        DEBUG_LOG("Dades VTK nul·les!");
        return false;
    }

    double *origin = this->getVtkData()->GetOrigin();
    double *spacing = this->getVtkData()->GetSpacing();

    for (int i = 0; i < 3; i++)
    {
        index[i] = qRound((coordinate[i] - origin[i]) / spacing[i]);
    }

    // Apply phase correction (Safety check, phaseNumber and numberOfPhases must be coherent to apply it)
    if (MathTools::isInsideRange(phaseNumber, 0, m_numberOfPhases - 1))
    {
        // HACK Aquest càlcul és necessari per pal·liar la manca de coneixement de la fase
        // TODO Cal resoldre això d'una forma més elegant, el qual comporta un redisseny del tractament de fases i volums
        // Calculem l'índex correcte en cas que tinguem fases
        index[2] = index[2] * m_numberOfPhases + phaseNumber;
    }

    int *extent = this->getVtkData()->GetExtent();
    bool inside = index[0] >= extent[0] && index[0] <= extent[1] &&
                  index[1] >= extent[2] && index[1] <= extent[3] &&
                  index[2] >= extent[4] && index[2] <= extent[5];

    return inside;
}

Voxel VolumePixelData::getVoxelValue(double coordinate[3], int phaseNumber)
{
    if (!this->getVtkData())
    {
        DEBUG_LOG("Dades VTK nul·les!");
        
        return Voxel();
    }

    int voxelIndex[3];
    bool inside = this->computeCoordinateIndex(coordinate, voxelIndex, phaseNumber);

    if (inside)
    {
        return getVoxelValue(voxelIndex);
    }
    else
    {
        return Voxel();
    }
}

Voxel VolumePixelData::getVoxelValue(int index[3])
{
    int extent[6];
    getExtent(extent);

    bool inside = true;
    for (int i = 0; i < 3; ++i)
    {
        inside = inside && MathTools::isInsideRange(index[i], extent[i * 2], extent[i * 2 + 1]);
    }
    
    Voxel voxelValue;
    if (inside)
    {
        for (int i = 0; i < getVtkData()->GetNumberOfScalarComponents(); i++)
        {
            voxelValue.addComponent(m_imageDataVTK->GetScalarComponentAsDouble(index[0], index[1], index[2], i));
        }
    }

    return voxelValue;
}

void VolumePixelData::convertToNeutralPixelData()
{
    // Creem un objecte vtkImageData "neutre"
    m_imageDataVTK = vtkSmartPointer<vtkImageData>::New();
    // Inicialitzem les dades
    m_imageDataVTK->SetOrigin(.0, .0, .0);
    m_imageDataVTK->SetSpacing(1., 1., 1.);
    m_imageDataVTK->SetDimensions(10, 10, 1);
    m_imageDataVTK->SetExtent(0, 9, 0, 9, 0, 0);
    m_imageDataVTK->AllocateScalars(VTK_SHORT, 1);
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
    m_loaded = true;
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
    vtkImageChangeInformation *changeInformation = vtkImageChangeInformation::New();
    changeInformation->SetInputData(m_imageDataVTK);
    changeInformation->SetOutputSpacing(x, y, z);
    changeInformation->Update();
    this->setData(changeInformation->GetOutput());
    changeInformation->Delete();
}

void VolumePixelData::getSpacing(double spacing[3])
{
    return m_imageDataVTK->GetSpacing(spacing);
}

void VolumePixelData::getExtent(int extent[6])
{
    return m_imageDataVTK->GetExtent(extent);
}

int VolumePixelData::getNumberOfScalarComponents()
{
    return m_imageDataVTK->GetNumberOfScalarComponents();
}

int VolumePixelData::getScalarSize()
{
    return m_imageDataVTK->GetScalarSize();
}  

int VolumePixelData::getScalarType()
{
    return m_imageDataVTK->GetScalarType();
}

int VolumePixelData::getNumberOfPoints()
{
    return m_imageDataVTK->GetNumberOfPoints();
} 
} // End namespace udg
