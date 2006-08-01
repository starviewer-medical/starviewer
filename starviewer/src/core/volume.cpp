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
    m_imageDataITK = 0;
    m_imageDataVTK = 0;
    
    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();

    m_volumeInformation = new VolumeSourceInformation;
}
 
Volume::Volume( ItkImageTypePointer itkImage ) 
{
    m_imageDataITK = 0;
    m_imageDataVTK = 0;

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();

    m_volumeInformation = new VolumeSourceInformation;
    
    this->setData( itkImage );
}

Volume::Volume( VtkImageTypePointer vtkImage )
{
    m_imageDataITK = 0;
    m_imageDataVTK = 0;
    
    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();

    m_volumeInformation = new VolumeSourceInformation;

    this->setData( vtkImage );
}

Volume::~Volume()
{

   if( m_imageDataITK  )
       m_imageDataITK->Delete(); // necessari???
   if( m_imageDataVTK )
       m_imageDataVTK->Delete();

}

Volume::ItkImageTypePointer Volume::getItkData(  )
{
    return m_imageDataITK; 
}

Volume::VtkImageTypePointer Volume::getVtkData(  )
{
    if( m_imageDataVTK == 0  && m_imageDataITK )
    {
        m_itkToVtkFilter->SetInput( m_imageDataITK );
        m_imageDataVTK = m_itkToVtkFilter->GetOutput(); // ficar GetImporter/Exporter?
        try
        {
            m_itkToVtkFilter->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            WARN_LOG( "Excepció en el filtre itkToVtk :: Volume::getVtkData " )
            std::cerr << excep << std::endl;
          
        }
    }
    return m_imageDataVTK;
    

}

void Volume::setData( ItkImageTypePointer itkImage  )
{
    m_imageDataITK = itkImage; 
}

void Volume::setData( VtkImageTypePointer vtkImage )
{
    // \TODO fer còpia local, no només punter-> com fer-ho?
    m_imageDataVTK = vtkImage;
    
    m_vtkToItkFilter->SetInput( m_imageDataVTK );
    m_imageDataITK = m_vtkToItkFilter->GetImporter()->GetOutput();
    
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch(itk::ExceptionObject & excep)
    {
        WARN_LOG( "Excepció en el filtre vtkToItk :: Volume::setData " )
        std::cerr << excep << std::endl;    
    }
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
