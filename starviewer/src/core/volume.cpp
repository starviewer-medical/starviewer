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

Volume::Volume( QObject *parent )
: QObject( parent )
{
    init();
}

Volume::Volume( ItkImageTypePointer itkImage, QObject *parent )
 : QObject( parent )
{
    init();
    this->setData( itkImage );
}

Volume::Volume( VtkImageTypePointer vtkImage, QObject *parent )
 : QObject( parent )
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
        // ara mateix tenen preferència els arxius
        if( !m_fileList.isEmpty() ) // si li hem donat com a input un conjunt d'arxius, llegim aquests arxius amb Input
        {
            Input *input = new Input;
            connect( input, SIGNAL( progress(int) ), this, SIGNAL( progress(int) ) );
            switch( input->readFiles( m_fileList ) )
            {
                case Input::NoError:
                    this->setData( input->getData()->getVtkData() );
                    break;

                case Input::InvalidFileName:
                    break;

                case Input::SizeMismatch:
                    break;
            }
        }
        else if( !m_imageSet.isEmpty() ) // si li hem donat com a input una llista d'imatges llegim les imatges
        {
            this->loadWithPreAllocateAndInsert();
        }


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
    int slices = this->getSeries()->getNumberOfSlicesPerPhase();
    int *size = this->getWholeExtent();

    vtkExtractVOI * extractedVolume = vtkExtractVOI::New();
    vtkImageChangeInformation * vtkChange = vtkImageChangeInformation::New();

    extractedVolume->SetInput( this->getVtkData() );
    extractedVolume->SetVOI( size[0] , size[1] , size[2] , size[3] , ( index * slices ) ,  ( ( index * slices ) + slices - 1 ) );

    vtkChange->SetInput( extractedVolume->GetOutput() );
    vtkChange->SetOutputOrigin( 0.0 , 0.0 , 0.0 );
    vtkChange->SetOutputExtentStart( 0 , 0 , 0 );

    Volume *subVolume = new Volume( vtkChange->GetOutput() );
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

    phases = this->getSeries()->getNumberOfPhases();
    slices = this->getSeries()->getNumberOfSlicesPerPhase();

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
//     m_imageSet.clear();
//     m_imageSet = imageList;
//     // perquè s'ompli la informació DICOM
//     m_volumeInformation->setFilenames( m_imageSet.at(0)->getPath() );
//     m_dataLoaded = false;
    //TODO hauria de ser tal i com està comentat, però de moment serà així perquè tenim problemes
    // al accedir a algunes llistes d'imatges, ja que sembla que d'avegades apunten a memòria no allotjada
    // és un problema que cal investigar. Així de moment deixem això que és estable i funciona
    m_imageSet.clear();
    m_imageSet = imageList;
    this->setInputFiles( imageList.at(0)->getParentSeries()->getFilesPathList() );
}

QList<Image *> Volume::getImages() const
{
    return m_imageSet;
}

void Volume::setInputFiles( const QStringList &filenames )
{
    m_fileList.clear();
    m_fileList = filenames;
    m_dataLoaded = false;
}

QStringList Volume::getInputFiles() const
{
    return m_fileList;
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

Study *Volume::getStudy()
{
    if( this->getSeries() )
    {
        return this->getSeries()->getParentStudy();
    }
    else
        return NULL;
}

Patient *Volume::getPatient()
{
    if( this->getStudy() )
    {
        return this->getStudy()->getParentPatient();
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
//     m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetScalarTypeToInt();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
}

void Volume::loadWithPreAllocateAndInsert()
{
    if( !m_imageSet.isEmpty() )
    {
        this->allocateImageData();
        this->loadSlicesWithReaders(2); // 0: vtk, 1: dcmtk, 2: itkGdcm
        m_imageDataVTK->Update();
        m_dataLoaded = true;
    }
    else
    {
        DEBUG_LOG("No tenim cap imatge per carregar");
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
