/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME_CPP
#define UDGVOLUME_CPP

#include "dicomimagereader.h"
#include "vtkdicomimagereader.h"
#include "dcmtkdicomimagereader.h"
#include "itkgdcmdicomimagereader.h"

// VTK
#include <vtkImageData.h>
#include <vtkExtractVOI.h>
#include <vtkImageChangeInformation.h>
#include <vtkDICOMImageReader.h>

// ITK
#include <itkExtractImageFilter.h>
#include <itkTileImageFilter.h>

#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"

#include "input.h"

//\TODO trobar perquè això és necessari amb les dcmtk
#define HAVE_CONFIG_H 1
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmimgle/dcmimage.h"

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

    m_dataLoaded = false;
}

Volume::~Volume()
{
}

Volume::ItkImageTypePointer Volume::getItkData()
{
    m_vtkToItkFilter->SetInput( this->getVtkData() );
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        WARN_LOG( QString("Excepció en el filtre vtkToItk :: Volume::getItkData() -> ") + excep.GetDescription() );
    }
    return m_vtkToItkFilter->GetImporter()->GetOutput();
}

Volume::VtkImageTypePointer Volume::getVtkData()
{
    if( !m_dataLoaded )
    {
        this->loadWithPreAllocateAndInsert();
    }
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
        WARN_LOG( QString("Excepció en el filtre itkToVtk :: Volume::setData( ItkImageTypePointer itkImage ) -> ") + excep.GetDescription() );
    }
    this->setData( m_itkToVtkFilter->GetOutput() );
}

void Volume::setData( VtkImageTypePointer vtkImage )
{
    // \TODO fer còpia local, no només punter-> com fer-ho?
    if( m_imageDataVTK )
        m_imageDataVTK->ReleaseData();
    m_imageDataVTK = vtkImage;
    m_dataLoaded = true;
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

Volume *Volume::getSubVolume( int index  )
{
    int slices = this->getVolumeSourceInformation()->getNumberOfSlices();
    int *size = this->getWholeExtent();

    vtkExtractVOI * extractedVolume = vtkExtractVOI::New();
    vtkImageChangeInformation * vtkChange = vtkImageChangeInformation::New();

    extractedVolume->SetInput( this->getVtkData() );
    extractedVolume->SetVOI( size[0] , size[1] , size[2] , size[3] , ( index * slices ) ,  ( ( index * slices ) + slices - 1 ) );

    vtkChange->SetInput( extractedVolume->GetOutput() );
    vtkChange->SetOutputOrigin( 0.0 , 0.0 , 0.0 );
    vtkChange->SetOutputExtentStart( 0 , 0 , 0 );

    Volume *subVolume = new Volume( vtkChange->GetOutput() );
    subVolume->setVolumeSourceInformation( m_volumeInformation );
    subVolume->getVtkData()->Update();

    return subVolume;
}

Volume * Volume::orderSlices()
{
    int phases, slices;
    Volume * orderedVolume;

    int *dimensions = this->getDimensions();
    ItkImageType::SizeType size;
    size[0] = dimensions[0];
    size[1] = dimensions[1];
    size[2] = 0; // Volem una imatge 2D

    ItkImageType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;

    phases = this->getVolumeSourceInformation()->getNumberOfPhases();
    slices = this->getVolumeSourceInformation()->getNumberOfSlices();

    typedef ItkImageType ItkImageType3D;
    typedef itk::Image<ItkPixelType, 2 > ItkImageType2D;

    typedef itk::ExtractImageFilter< ItkImageType3D , ItkImageType2D > ExtractImageType;
    typedef itk::TileImageFilter< ItkImageType2D , ItkImageType3D  > TileFilterType;

    ExtractImageType::Pointer extractFilter = ExtractImageType::New();
    extractFilter->SetInput( this->getItkData() );

    TileFilterType::Pointer tileFilter = TileFilterType::New();

    // El layout ens serveix per indicar cap on creix la cua. En aquest cas volem fer creixer la coordenada Z
    TileFilterType::LayoutArrayType layout;
    layout[0] = 1;
    layout[1] = 1;
    layout[2] = 0;

    tileFilter->SetLayout( layout );

    ItkImageType::RegionType region;
    region.SetSize( size );

    std::vector< ItkImageType2D::Pointer > extracts;

    for ( int i = 0 ; i < phases ; i++ )
    {
        for (int j = 0 ; j < slices ; j++ )
        {
            index[2] = ( ( phases * j ) + i ) ;

            region.SetIndex( index );

            extractFilter->SetExtractionRegion( region );

            extractFilter->Update();

            extracts.push_back( extractFilter->GetOutput() );
            extracts.back()->DisconnectPipeline(); //S'ha de desconnectar la pipeline per forçar a l'extracFilter a generar un nou output.

            tileFilter->PushBackInput( extracts.back() );

        }
    }

    tileFilter->Update();

    orderedVolume = new Volume( tileFilter->GetOutput() );
    orderedVolume->setVolumeSourceInformation( m_volumeInformation );
    //orderedVolume->getVtkData()->Update();

    return orderedVolume;
}

void Volume::setImageOrderCriteria( unsigned int orderCriteria )
{
    m_imageOrderCriteria = orderCriteria;
}

unsigned int Volume::getImageOrderCriteria() const
{
    return m_imageOrderCriteria;
}

void Volume::addImage( Image *image )
{
    if( !m_imageSet.contains(image) )
    {
        m_imageSet << image;
        m_dataLoaded = false;
    }
}

void Volume::setImages( const QList<Image *> &imageList )
{
    m_imageSet.clear();
    m_imageSet = imageList;
    // perquè s'ompli la informació DICOM
    m_volumeInformation->setFilenames( m_imageSet.at(0)->getPath() );
    m_dataLoaded = false;
}

void Volume::setImagesFromSeries( Series *series )
{
    //\TODO hauria de ser com setImages, però encara no ens interessa que torni a carregar les dades, només volem la informació i prou
    m_imageSet.clear();
    m_imageSet = series->getImages();
    // perquè s'ompli la informació DICOM
    m_volumeInformation->setFilenames( m_imageSet.at(0)->getPath() );
}

Series *Volume::getSeries()
{
    if( !m_imageSet.isEmpty() )
    {
        return m_imageSet.at(0)->getParentSeries();
    }
    else
        return NULL;
}

void Volume::allocateImageData()
{
    //\TODO si les dades estan allotjades per defecte, fer un delete primer i després fer un new? o amb un ReleaseData n'hi ha prou?
    m_imageDataVTK->Delete();
    m_imageDataVTK = vtkImageData::New();

    // Inicialitzem les dades
    double origin[3];
    origin[0] = m_imageSet.at(0)->getImagePositionPatient()[0];
    origin[1] = m_imageSet.at(0)->getImagePositionPatient()[1];
    origin[2] = m_imageSet.at(0)->getImagePositionPatient()[2];
    m_imageDataVTK->SetOrigin( origin );
    double spacing[3];
    spacing[0] = m_imageSet.at(0)->getPixelSpacing()[0];
    spacing[1] = m_imageSet.at(0)->getPixelSpacing()[1];
    spacing[2] = m_imageSet.at(0)->getSliceThickness();
    m_imageDataVTK->SetSpacing( spacing );
    m_imageDataVTK->SetDimensions( m_imageSet.at(0)->getRows(), m_imageSet.at(0)->getColumns(), m_imageSet.size() );
    //\TODO de moment assumim que sempre seran ints i ho mapejem així,potser més endavant podria canviar, però és el tipus que tenim fixat desde les itk
    m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
}

void Volume::loadWithPreAllocateAndInsert()
{
    if( !m_imageSet.isEmpty() )
    {
        this->allocateImageData();
//         this->loadSlices(2); // 0: DcmFileFormat, 1: DicomImage, 2: vtkDICOMImageReader
        this->loadSlicesWithReaders(2); // 0: vtk, 1: dcmtk, 2: itkGdcm
        m_imageDataVTK->Update();
        m_dataLoaded = true;
    }
    else
    {
        DEBUG_LOG("No tenim cap imatge per carregar");
    }
}

void Volume::loadSlices( int method )
{
    typedef unsigned char * bufferDataType;
    // llesca Z
    int zSlice = 0;
    // punter on copiarem les dades d'imatge. Apunta al buffer vtk
    bufferDataType vtkBuffer = NULL;
    // buffer on colocarem la llesca que hem llegit
    bufferDataType imageBuffer = NULL;
    // nombre de bytes que copiem
    unsigned long bytes = 0;
    // Per cada imatge
    foreach( Image *image, m_imageSet )
    {
        DEBUG_LOG( QString("Llesca que vull carregar: %1").arg( zSlice ) );
        bytes = m_imageDataVTK->GetDimensions()[0]*m_imageDataVTK->GetDimensions()[1]*2;
        switch( method )
        {
        case 0: // dcmtk, DcmFileFormat
        {
            DcmFileFormat dicomFile;
            OFCondition status = dicomFile.loadFile( qPrintable( image->getPath() ) );
            if( status.good() )
            {
                status = dicomFile.getDataset()->findAndGetUint16Array(DCM_PixelData, (const Uint16 *&)imageBuffer);
                if( !status.good() )
                    DEBUG_LOG( QString( "Error en llegir les dades dels pixels. Error: %1 ").arg( status.text() ) );
            }
            else
                DEBUG_LOG( QString( "Error en llegir l'arxiu [%1]\n%2 ").arg( image->getPath() ).arg( status.text() ) );
        }
        break;

        case 1: // dcmtk DicomImage
        {
            DicomImage *dicomImage = new DicomImage( qPrintable( image->getPath() ) );
            if( dicomImage != NULL )
            {
                if( dicomImage->getStatus() == EIS_Normal )
                {
                    dicomImage->setMinMaxWindow();
                    imageBuffer = (bufferDataType)dicomImage->getOutputData();
                    bytes = dicomImage->getOutputDataSize();
                }
                else
                    DEBUG_LOG( QString( "Error en carregar la DicomImage. Error: %1 ").arg( DicomImage::getString( dicomImage->getStatus() ) ) );
            }
        }
        break;

        case 2: // vtkDICOMImageReader
        {
            ::vtkDICOMImageReader *reader = ::vtkDICOMImageReader::New();
            reader->SetFileName( qPrintable( image->getPath() ) );
            reader->Update();
            imageBuffer = (bufferDataType)reader->GetOutput()->GetScalarPointer();
        }
        break;

        default:
            DEBUG_LOG("Mètode de lectura erroni");
            return;
        break;
        }

        vtkBuffer = (unsigned char *)m_imageDataVTK->GetScalarPointer(0,0,zSlice);

        // copiem les dades del buffer d'imatge cap a vtk
        memcpy( vtkBuffer, imageBuffer, bytes  );


        DEBUG_LOG( QString("Valor d'un pixel del mig(vtkBuffer): %1, valor del mateix del buffer d'imatge: %2, nombre de bytes que copiem: %3").arg( vtkBuffer[256*256+256]).arg( imageBuffer[256*256+256] ).arg( bytes ) );
        zSlice++;
    }
}

void Volume::loadSlicesWithReaders( int method )
{
    DICOMImageReader *reader;
    switch( method )
    {
    case 0: // vtk
        reader = new vtkDICOMImageReader;
        break;

    case 1: // dcmtk
        reader = new dcmtkDICOMImageReader;
        break;

    case 2: // itkGDCM
        reader = new itkGdcmDICOMImageReader;
        break;

    default:
        break;
    }
    DEBUG_LOG( QString("Scalar size: %1\nIncrements: %2,%3,%4\n Bytes per slice: %5 ")
        .arg( m_imageDataVTK->GetScalarSize() )
        .arg( m_imageDataVTK->GetIncrements()[0] )
        .arg( m_imageDataVTK->GetIncrements()[1] )
        .arg( m_imageDataVTK->GetIncrements()[2] )
        .arg( m_imageDataVTK->GetDimensions()[0]*m_imageDataVTK->GetDimensions()[1]*m_imageDataVTK->GetScalarSize() )
    );
    reader->setInputImages( m_imageSet );
    reader->setBufferPointer( m_imageDataVTK->GetScalarPointer() );
    reader->setSliceByteIncrement( m_imageDataVTK->GetIncrements()[2]*m_imageDataVTK->GetScalarSize() );
    reader->load();
}

};

#endif
