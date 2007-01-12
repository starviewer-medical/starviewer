/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME_CPP
#define UDGVOLUME_CPP

// VTK
#include <vtkImageData.h>

// ITK
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
// CPP
#include <iostream>

#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"

namespace udg {


Volume::Volume()
{
    init();
}

Volume::Volume( ItkImageTypePointer itkImage )
{
    init();
    this->setData( itkImage );
}

Volume::Volume( VtkImageTypePointer vtkImage )
{
    init();
    this->setData( vtkImage );
}

void Volume::init()
{
    // \TODO és millor crear un objecte o assignar-li NUL a l'inicialitzar? Així potser és més segur des del punt de vista de si li demanem propietats al volum com origen, espaiat, etc
    m_imageDataVTK = vtkImageData::New();

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();

    m_volumeInformation = new VolumeSourceInformation;
}

Volume::~Volume()
{
}

Volume::ItkImageTypePointer Volume::getItkData()
{
    m_vtkToItkFilter->SetInput( m_imageDataVTK );
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        WARN_LOG( qPrintable( QString("Excepció en el filtre vtkToItk :: Volume::getItkData() -> ") + excep.GetDescription() ) )
        std::cerr << excep << std::endl;
    }
    return m_vtkToItkFilter->GetImporter()->GetOutput();
}

Volume::VtkImageTypePointer Volume::getVtkData()
{
    return m_imageDataVTK;
}

void Volume::setData( ItkImageTypePointer itkImage  )
{
    m_itkToVtkFilter->SetInput( itkImage );
    try
    {
        m_itkToVtkFilter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        WARN_LOG( qPrintable( QString("Excepció en el filtre itkToVtk :: Volume::setData( ItkImageTypePointer itkImage ) -> ") + excep.GetDescription() ) )
        std::cerr << excep << std::endl;
    }
    if( m_imageDataVTK )
        m_imageDataVTK->ReleaseData();
    m_imageDataVTK = m_itkToVtkFilter->GetOutput();
}

void Volume::setData( VtkImageTypePointer vtkImage )
{
    // \TODO fer còpia local, no només punter-> com fer-ho?
    if( m_imageDataVTK )
        m_imageDataVTK->ReleaseData();
    m_imageDataVTK = vtkImage;
}

void Volume::updateInformation()
{
    getVtkData()->UpdateInformation();
}

void Volume::getOrigin( double xyz[3] )
{
    getVtkData()->GetOrigin( xyz );
}

double *Volume::getOrigin()
{
    return getVtkData()->GetOrigin();
}

void Volume::getSpacing( double xyz[3] )
{
    getVtkData()->GetSpacing( xyz );
}

double *Volume::getSpacing()
{
    return getVtkData()->GetSpacing();
}

void Volume::getWholeExtent( int extent[6] )
{
    getVtkData()->GetWholeExtent( extent );
}

int *Volume::getWholeExtent()
{
    return getVtkData()->GetWholeExtent();
}

int *Volume::getDimensions()
{
    return getVtkData()->GetDimensions();
}

void Volume::getDimensions( int dims[3] )
{
    getVtkData()->GetDimensions( dims );
}

};

#endif
