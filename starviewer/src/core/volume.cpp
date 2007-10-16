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
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"

// extra per INPUT
#include <QFileInfo>
#include <QDir>

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
    m_numberOfPhases = 1;
    // \TODO és millor crear un objecte o assignar-li NUL a l'inicialitzar? Així potser és més segur des del punt de vista de si li demanem propietats al volum com origen, espaiat, etc
    m_imageDataVTK = vtkImageData::New();

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();
    m_dataLoaded = false;

    inputConstructor();
}

Volume::~Volume()
{
    inputDestructor();
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
        // TODO Ara mateix llegim a partir de Input. Més endavant s'haurà de llegir a partir de les classes DICOMImageReader
        QStringList fileList = getInputFiles();
        if( !fileList.isEmpty() )
        {
            this->readFiles( fileList );
        }
        /* TODO Descomentar per llegir amb classes DICOMImageReader
        if( !m_imageSet.isEmpty() )
        {
            this->loadWithPreAllocateAndInsert();
        }
        */
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

void Volume::setNumberOfPhases( int phases )
{
    if( phases > 1 )
        m_numberOfPhases = phases;
    else
        DEBUG_LOG( QString("Nombre de phases >1 :: %1").arg(phases) );
}

int Volume::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

Volume *Volume::getPhaseVolume( int index )
{
    int phases = this->getSeries()->getNumberOfPhases();
    Volume *result = NULL;
    if( index >= 0 && index < phases )
    {
        int slices = this->getSeries()->getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        QList<Image *> phaseImages;
        QList<Image *> seriesImages = this->getSeries()->getImages();
        for( int i = 0; i < slices; i++ )
        {
            phaseImages << seriesImages.at( currentImageIndex );
            currentImageIndex += slices;
        }
        result->setImages( phaseImages );
    }
    return result;
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

    //\TODO Això és una solució temporal. S'ha de mirar com fer-ho perquè el nou volum només tingui la llista d'imatges del subvolum que desitgem extreure.
    Volume *subVolume = new Volume(  );
    subVolume->setImages( this->getImages() );
    subVolume->setData( vtkChange->GetOutput() );
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

    //\TODO Això és una solució temporal. S'ha de mirar com fer-ho perquè el nou volum tingui les imatges en el seu ordre correcte ja que ara només es reordena el model.
    orderedVolume = new Volume(  );
    orderedVolume->setImages( this->getImages() );
    orderedVolume->setData( tileFilter->GetOutput() );
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
    m_dataLoaded = false;
}

QList<Image *> Volume::getImages() const
{
    return m_imageSet;
}

QStringList Volume::getInputFiles() const
{
    QStringList filepaths;
    foreach (Image *image, this->getImages())
    {
        filepaths << image->getPath();
    }

    return filepaths;
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

//
//
//
// PART ADAPTADA d'INPUT
// TODO tot això és temporal, quan es faci la lectura tal i com volem desapareixerà tot aquest codi
//
//
void Volume::inputConstructor()
{
    m_reader = ReaderType::New();
    m_seriesReader = SeriesReaderType::New();

    m_gdcmIO = ImageIOType::New();

    itk::QtSignalAdaptor *m_progressSignalAdaptor = new itk::QtSignalAdaptor;
    //   Connect the adaptor as an observer of a Filter's event
    m_seriesReader->AddObserver( itk::ProgressEvent(),  m_progressSignalAdaptor->GetCommand() );
//
//  Connect the adaptor's Signal to the Qt Widget Slot
   connect( m_progressSignalAdaptor, SIGNAL( Signal() ), this, SLOT( slotProgress() ) );
}

void Volume::slotProgress()
{
    emit progress( (int)( m_seriesReader->GetProgress() * 100 ) );
}

void Volume::inputDestructor()
{
//     m_seriesReader->Delete();
//     m_reader->Delete();
//     m_gdcmIO->Delete();
}

int Volume::openFile( QString fileName )
{
    ProgressCommand::Pointer observer = ProgressCommand::New();
    m_reader->AddObserver( itk::ProgressEvent(), observer );

    int errorCode = NoError;

    m_reader->SetFileName( qPrintable(fileName) );
    emit progress(0);
    try
    {
        m_reader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        ERROR_LOG( QString("Excepció llegint els arxius del directori [%1]\nDescripció: [%2]")
                .arg( QFileInfo( fileName ).dir().path() )
                .arg( e.GetDescription() )
                );
        // llegim el missatge d'error per esbrinar de quin error es tracta
        QString errorMessage( e.GetDescription() );
        if( errorMessage.contains("Size mismatch") )
        {
            errorCode = SizeMismatch;
        }
        emit progress( -1 ); // això podria indicar excepció
    }
    if ( errorCode == NoError )
    {
        this->setData( m_reader->GetOutput() );
        emit progress( 100 );
    }
    return errorCode;
}

int Volume::readFiles( QStringList filenames )
{
    int errorCode = NoError;
    if( filenames.isEmpty() )
    {
        WARN_LOG( "La llista de noms de fitxer per carregar és buida" );
        errorCode = InvalidFileName;
        return errorCode;
    }

    if( filenames.size() > 1 )
    {
        // això és necessari per després poder demanar-li el diccionari de meta-dades i obtenir els tags del DICOM
        m_seriesReader->SetImageIO( m_gdcmIO );

        // convertim la QStringList al format std::vector< std::string > que s'esperen les itk
        std::vector< std::string > stlFilenames;
        for( int i = 0; i < filenames.size(); i++ )
        {
            stlFilenames.push_back( filenames.at(i).toStdString() );
        }

        m_seriesReader->SetFileNames( stlFilenames );

        emit progress( 0 );

        try
        {
            m_seriesReader->Update();
        }
        catch ( itk::ExceptionObject & e )
        {
            ERROR_LOG( QString("Excepció llegint els arxius del directori [%1]\nDescripció: [%2]")
                .arg( QFileInfo( filenames.at(0) ).dir().path() )
                .arg( e.GetDescription() )
                );
            errorCode = SizeMismatch;
            emit progress( -1 ); // això podria indicar excepció
        }
        if ( errorCode == NoError )
        {
            this->setData( m_seriesReader->GetOutput() );
            emit progress( 100 );
        }
    }
    else
    {
        this->openFile( filenames.at(0) );
    }
    return errorCode;
}

};

#endif
