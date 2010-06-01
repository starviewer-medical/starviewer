/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME_CPP
#define UDGVOLUME_CPP

#include "volume.h"

#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "dicomtagreader.h"
#include "dicomimagereader.h"
#include "dicomimagereadervtk.h"
#include "dicomimagereaderdcmtk.h"
#include "dicomimagereaderitkgdcm.h"
#include "mathtools.h"
#include "starviewerapplication.h"
// VTK
#include <vtkImageData.h>
#include <vtkExtractVOI.h>
#include <vtkImageChangeInformation.h>
#include <vtkDICOMImageReader.h>
// Voxel information
#include <vtkPointData.h>
#include <vtkCell.h>
// ITK
#include <itkTileImageFilter.h>
// Esxtra per INPUT
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

namespace udg {

Volume::Volume(QObject *parent)
: QObject(parent)
{
    init();
}

Volume::Volume(ItkImageTypePointer itkImage, QObject *parent)
 : QObject(parent)
{
    init();
    this->setData(itkImage);
}

Volume::Volume(VtkImageTypePointer vtkImage, QObject *parent)
 : QObject(parent)
{
    init();
    this->setData(vtkImage);
}

void Volume::init()
{
    m_numberOfPhases = 1;
    m_numberOfSlicesPerPhase = 1;
    // TODO És millor crear un objecte o assignar-li NUL a l'inicialitzar? 
    // Així potser és més segur des del punt de vista de si li demanem propietats al volum com origen, espaiat, etc
    m_imageDataVTK = vtkImageData::New();

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();
    m_dataLoaded = false;
    m_progressSignalAdaptor = 0;
    inputConstructor();
}

Volume::~Volume()
{
    inputDestructor();
}

Volume::ItkImageTypePointer Volume::getItkData()
{
    m_vtkToItkFilter->SetInput(this->getVtkData() );
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch (itk::ExceptionObject & excep )
    {
        WARN_LOG(QString("Excepció en el filtre vtkToItk :: Volume::getItkData() -> ") + excep.GetDescription() );
    }
    return m_vtkToItkFilter->GetImporter()->GetOutput();
}

Volume::VtkImageTypePointer Volume::getVtkData()
{
    if ( !m_dataLoaded )
    {
        // TODO Ara mateix llegim a partir de Input. Més endavant s'haurà de llegir a partir de les classes DICOMImageReader
        QStringList fileList;
        foreach (Image *image, m_imageSet)
        {
            if ( !fileList.contains(image->getPath()) )// Evitem afegir més vegades l'arxiu si aquest és multiframe
                fileList << image->getPath();
        }
        if ( !fileList.isEmpty() )
        {
            switch ( this->readFiles(fileList) )
            {
            case OutOfMemory: 
                WARN_LOG("No podem carregar els arxius següents perquè no caben a memòria\n" + fileList.join("\n") );
                createNeutralVolume();
                m_dataLoaded = true;
                QMessageBox::warning(0, tr("Out of memory"), tr("There's not enough memory to load the Series you requested. Try to close all the opened %1 windows and restart the application and try again. If the problem persists, adding more RAM memory or switching to a 64 bit operating system may solve the problem.").arg(ApplicationNameString) );
                break;

            case MissingFile:
                // Fem el mateix que en el cas OutOfMemory, canviant el missatge d'error
                createNeutralVolume();
                m_dataLoaded = true;
                QMessageBox::warning(0, tr("Missing Files"), tr("%1 could not find the corresponding files for this Series. Maybe they had been removed or are corrupted.").arg(ApplicationNameString) );
                break;

            case UnknownError:
                // Hi ha hagut un error no controlat, creem el volum neutral per evitar desastres majors
                createNeutralVolume();
                m_dataLoaded = true;
                QMessageBox::warning(0, tr("Unkwown Error"), tr("%1 found an unexpected error reading this Series. No Series data has been loaded.").arg(ApplicationNameString) );
                break;
            }
        }
        /* TODO Descomentar per llegir amb classes DICOMImageReader
        if ( !m_imageSet.isEmpty() )
        {
            this->loadWithPreAllocateAndInsert();
        }
        */
    }
    return m_imageDataVTK;
}

void Volume::setData(ItkImageTypePointer itkImage)
{
    m_itkToVtkFilter->SetInput(itkImage);
    try
    {
        m_itkToVtkFilter->Update();
    }
    catch (itk::ExceptionObject & excep)
    {
        WARN_LOG(QString("Excepció en el filtre itkToVtk :: Volume::setData(ItkImageTypePointer itkImage) -> ") + excep.GetDescription() );
    }
    this->setData(m_itkToVtkFilter->GetOutput() );
}

void Volume::setData(VtkImageTypePointer vtkImage)
{
    // TODO Fer còpia local, no només punter-> com fer-ho?
    if ( m_imageDataVTK )
        m_imageDataVTK->ReleaseData();
    m_imageDataVTK = vtkImage;
    m_dataLoaded = true;
}

void Volume::getOrigin(double xyz[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetOrigin(xyz);
}

double *Volume::getOrigin()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetOrigin();
}

void Volume::getSpacing(double xyz[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetSpacing(xyz);
}

double *Volume::getSpacing()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetSpacing();
}

void Volume::getWholeExtent(int extent[6])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetWholeExtent(extent);
}

int *Volume::getWholeExtent()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetWholeExtent();
}

int *Volume::getDimensions()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetDimensions();
}

void Volume::getDimensions(int dims[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetDimensions(dims);
}

void Volume::setIdentifier(const Identifier &id)
{
    m_identifier = id;
}

Identifier Volume::getIdentifier() const
{
    return m_identifier;
}

void Volume::setThumbnail(const QPixmap &thumbnail)
{
    m_thumbnail = thumbnail;
}

QPixmap Volume::getThumbnail() const
{
    return m_thumbnail;
}

void Volume::setNumberOfPhases(int phases)
{
    if ( phases >= 1 )
        m_numberOfPhases = phases;
}

int Volume::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

Volume *Volume::getPhaseVolume(int index)
{
    Volume *result = NULL;
    if ( m_numberOfPhases == 1 )
    {
        // Si només tenim una sola fase, retornem totes les imatges que conté el volum
        result = new Volume();
        result->setImages(m_imageSet);
    }
    else if ( index >= 0 && index < m_numberOfPhases )
    {
        result = new Volume();
        // Obtenim el nombre d'imatges per fase
        int slices = getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        QList<Image *> phaseImages;
        for (int i = 0; i < slices; i++)
        {
            phaseImages << m_imageSet.at(currentImageIndex);
            currentImageIndex += m_numberOfPhases;
        }
        result->setImages(phaseImages);
    }
    return result;
}

QList<Image *> Volume::getPhaseImages(int index)
{
    QList<Image *> phaseImages;
    if ( index >= 0 && index < m_numberOfPhases )
    {
        // Obtenim el nombre d'imatges per fase
        int slices = getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        for (int i = 0; i < slices; i++)
        {
            phaseImages << m_imageSet.at(currentImageIndex);
            currentImageIndex += m_numberOfPhases;
        }
    }
    return phaseImages;
}

void Volume::setNumberOfSlicesPerPhase(int slicesPerPhase)
{
    m_numberOfSlicesPerPhase = slicesPerPhase;
}

int Volume::getNumberOfSlicesPerPhase() const
{
    return m_numberOfSlicesPerPhase;
}

void Volume::setImageOrderCriteria(unsigned int orderCriteria)
{
    m_imageOrderCriteria = orderCriteria;
}

unsigned int Volume::getImageOrderCriteria() const
{
    return m_imageOrderCriteria;
}

void Volume::addImage(Image *image)
{
    if ( !m_imageSet.contains(image) )
    {
        m_imageSet << image;
        m_dataLoaded = false;
    }
}

void Volume::setImages(const QList<Image *> &imageList)
{
    m_imageSet.clear();
    m_imageSet = imageList;
    m_dataLoaded = false;
}

QList<Image *> Volume::getImages() const
{
    return m_imageSet;
}

int Volume::getNumberOfFrames() const
{
    return m_imageSet.count();
}

Study *Volume::getStudy()
{
    if ( !m_imageSet.isEmpty() )
    {
        return m_imageSet.at(0)->getParentSeries()->getParentStudy();
    }
    else
        return NULL;
}

Patient *Volume::getPatient()
{
    if ( this->getStudy() )
    {
        return this->getStudy()->getParentPatient();
    }
    else
        return NULL;
}

QString Volume::toString(bool verbose)
{
    Q_UNUSED(verbose);
    QString result;

    if ( m_dataLoaded )
    {
        int dims[3];
        double origin[3];
        double spacing[3];
        int extent[6];
        double bounds[6];

        this->getDimensions(dims);
        this->getOrigin(origin);
        this->getSpacing(spacing);
        this->getWholeExtent(extent);
        this->getVtkData()->GetBounds(bounds);

        result += QString("Dimensions: %1, %2, %3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
        result += QString("\nOrigin: %1, %2, %3").arg(origin[0]).arg(origin[1]).arg(origin[2]);
        result += QString("\nSpacing: %1, %2, %3").arg(spacing[0]).arg(spacing[1]).arg(spacing[2]);
        result += QString("\nExtent: %1..%2, %3..%4, %5..%6").arg(extent[0]).arg(extent[1]).arg(extent[2]).arg(extent[3]).arg(extent[4]).arg(extent[5]);
        result += QString("\nBounds: %1..%2, %3..%4, %5..%6").arg(bounds[0]).arg(bounds[1]).arg(bounds[2]).arg(bounds[3]).arg(bounds[4]).arg(bounds[5]);
    }
    else
    {
        result = "The data is not loaded";
    }

    return result;
}

Image *Volume::getImage(int sliceNumber, int phaseNumber) const
{
    Image *image = NULL;

    if ( !m_imageSet.isEmpty() )
    {
        if ( (sliceNumber*m_numberOfPhases + phaseNumber) < m_imageSet.count() )
        {
            image = m_imageSet.at(sliceNumber*m_numberOfPhases + phaseNumber);
        }
    }

    return image;
}

void Volume::getStackDirection(double direction[3], int stack)
{
    // TODO Encara no suportem múltiples stacks!!!!
    // Fem el tractament com si només hi hagués un sol
    Q_UNUSED(stack);
    Image *firstImage = this->getImage(0);
    Image *secondImage = this->getImage(1);
    if ( !firstImage )
    {
        DEBUG_LOG("Error gravísim. No hi ha 'primera' imatge!");
        return;
    }

    if ( !secondImage )
    {
        DEBUG_LOG("Només hi ha una imatge per stack! Retornem la normal del pla");
        const double *directionCosines = firstImage->getImageOrientationPatient();
        for (int i=0; i<3; i++)
            direction[i] = directionCosines[i + 6];
    }
    else
    {
        const double *firstOrigin = firstImage->getImagePositionPatient();
        const double *secondOrigin = secondImage->getImagePositionPatient();
        // calculem la direcció real de com estan apilades
        double *zDirection = MathTools::directorVector(firstOrigin, secondOrigin);
        MathTools::normalize(zDirection);
        for (int i=0; i<3; i++)
            direction[i] = zDirection[i];
    }
}

Volume::VoxelType *Volume::getScalarPointer(int x, int y, int z)
{
	// TODO Caldria posar static/dynamic_cast? o en aquest cas ja és suficient així?
	return (Volume::VoxelType *)this->getVtkData()->GetScalarPointer(x,y,z);
}

Volume::VoxelType *Volume::getScalarPointer(int index[3])
{
	// TODO Caldria posar static/dynamic_cast? o en aquest cas ja és suficient així?
	return this->getScalarPointer(index[0], index[1], index[2]);
}

bool Volume::getVoxelValue(double coordinate[3], Volume::VoxelType &voxelValue)
{
    vtkImageData *vtkData = getVtkData();
    if ( !vtkData )
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
    if ( cell )
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
void Volume::allocateImageData()
{
    // TODO Si les dades estan allotjades per defecte, fer un delete primer i després fer un new? o amb un ReleaseData n'hi ha prou?
    m_imageDataVTK->Delete();
    m_imageDataVTK = vtkImageData::New();

    // Inicialitzem les dades
    double origin[3];
    origin[0] = m_imageSet.at(0)->getImagePositionPatient()[0];
    origin[1] = m_imageSet.at(0)->getImagePositionPatient()[1];
    origin[2] = m_imageSet.at(0)->getImagePositionPatient()[2];
    m_imageDataVTK->SetOrigin(origin);
    double spacing[3];
    spacing[0] = m_imageSet.at(0)->getPixelSpacing()[0];
    spacing[1] = m_imageSet.at(0)->getPixelSpacing()[1];
    spacing[2] = m_imageSet.at(0)->getSliceThickness();
    m_imageDataVTK->SetSpacing(spacing);
    m_imageDataVTK->SetDimensions(m_imageSet.at(0)->getRows(), m_imageSet.at(0)->getColumns(), m_imageSet.size() );
    // TODO De moment assumim que sempre seran ints i ho mapejem així,potser més endavant podria canviar, però és el tipus que tenim fixat desde les itk
    m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
}

void Volume::loadWithPreAllocateAndInsert()
{
    if ( !m_imageSet.isEmpty() )
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

void Volume::loadSlicesWithReaders(int method)
{
    DICOMImageReader *reader;
    switch ( method )
    {
    case 0: // vtk
        reader = new DICOMImageReaderVTK;
        break;

    case 1: // dcmtk
        reader = new DICOMImageReaderDCMTK;
        break;

    case 2: // itkGDCM
        reader = new DICOMImageReaderITKGdcm;
        break;

    default:
        break;
    }
    DEBUG_LOG(QString("Scalar size: %1\nIncrements: %2,%3,%4\n Bytes per slice: %5 ")
        .arg(m_imageDataVTK->GetScalarSize())
        .arg(m_imageDataVTK->GetIncrements()[0])
        .arg(m_imageDataVTK->GetIncrements()[1])
        .arg(m_imageDataVTK->GetIncrements()[2])
        .arg(m_imageDataVTK->GetDimensions()[0]*m_imageDataVTK->GetDimensions()[1]*m_imageDataVTK->GetScalarSize() ) );
    reader->setInputImages(m_imageSet);
    reader->setBufferPointer(m_imageDataVTK->GetScalarPointer() );
    reader->setSliceByteIncrement(m_imageDataVTK->GetIncrements()[2]*m_imageDataVTK->GetScalarSize() );
    reader->load();
}

void Volume::readDifferentSizeImagesIntoOneVolume(const QStringList &filenames)
{
    int errorCode = NoError;
    // Declarem el filtre de tiling
    typedef itk::TileImageFilter< ItkImageType, ItkImageType  > TileFilterType;
    TileFilterType::Pointer tileFilter = TileFilterType::New();
    // Inicialitzem les seves variables
    // El layout ens serveix per indicar cap on creix la cua. En aquest cas volem fer creixer la coordenada Z
    TileFilterType::LayoutArrayType layout;
    layout[0] = 1;
    layout[1] = 1;
    layout[2] = 0;
    tileFilter->SetLayout(layout);

    int progressCount = 0;
    int progressIncrement = static_cast<int>((1.0/(double)filenames.count()) * 100);

    m_reader->SetImageIO(m_gdcmIO);
    foreach (QString file, filenames)
    {
        emit progress(progressCount);
        // Declarem la imatge que volem carregar
        ItkImageType::Pointer itkImage;
        m_reader->SetFileName(qPrintable(file));
        try
        {
            m_reader->UpdateLargestPossibleRegion();
        }
        catch (itk::ExceptionObject & e)
        {
            WARN_LOG(QString("Excepció llegint els arxius del directori [%1] Descripció: [%2]")
                    .arg(QFileInfo(filenames.at(0)).dir().path() )
                    .arg(e.GetDescription() )
                   );

            // Llegim el missatge d'error per esbrinar de quin error es tracta
            errorCode = identifyErrorMessage(QString(e.GetDescription()));
        }
        if ( errorCode == NoError )
        {
            itkImage = m_reader->GetOutput();
            m_reader->GetOutput()->DisconnectPipeline();
        }
        // TODO No es fa tractament d'errors!

        // Un cop llegit el block, fem el tiling
        tileFilter->PushBackInput(itkImage);
        progressCount += progressIncrement;
    }
    tileFilter->Update();
    this->setData(tileFilter->GetOutput() );
    emit progress(100);
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

    if ( !m_progressSignalAdaptor )
        m_progressSignalAdaptor = new itk::QtSignalAdaptor();
    // Connect the adaptor as an observer of a Filter's event
    m_seriesReader->AddObserver(itk::ProgressEvent(), m_progressSignalAdaptor->GetCommand() );
//
//  Connect the adaptor's Signal to the Qt Widget Slot
    connect(m_progressSignalAdaptor, SIGNAL( Signal() ), SLOT( slotProgress() ) );
}

void Volume::slotProgress()
{
    emit progress( (int)(m_seriesReader->GetProgress() * 100) );
}

void Volume::inputDestructor()
{
    delete m_progressSignalAdaptor;
//     m_seriesReader->Delete();
//     m_reader->Delete();
//     m_gdcmIO->Delete();
}

int Volume::readSingleFile(QString fileName)
{
    int errorCode = NoError;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(qPrintable(fileName));
    emit progress(0);
    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
        WARN_LOG(QString("Excepció llegint l'arxiu [%1] Descripció: [%2]")
                .arg(fileName)
                .arg(e.GetDescription())
               );
        // Llegim el missatge d'error per esbrinar de quin error es tracta
        errorCode = identifyErrorMessage(QString(e.GetDescription()) );

        // Emetem progress 100, perquè el corresponent diàleg de progrés es tanqui
        emit progress(100);
    }
    catch (std::bad_alloc)
    {
        errorCode = OutOfMemory;
        // Emetem progress 100, perquè el corresponent diàleg de progrés es tanqui
        emit progress(100);
    }
    
    if ( errorCode == NoError )
    {
        // HACK En els casos que les imatges siguin enhanced, les gdcm no omplen correctament
        // ni l'origen ni l'sapcing x,y i és per això que li assignem l'origen i l'spacing 
        // que hem llegit correctament a Image
        // TODO Quan solucionem correctament el ticket #1166 (actualització a gdcm 2.0.x) aquesta assignació desapareixerà
        if ( !m_imageSet.isEmpty() )
        {
            reader->GetOutput()->SetOrigin(m_imageSet.first()->getImagePositionPatient());
            // Cal tenir en compte si la imatge original conté informació d'spacing vàlida per fer l'assignació
            const double *imageSpacing = m_imageSet.first()->getPixelSpacing();
            if ( imageSpacing[0] > 0.0 )
            {
                double spacing[3];
                spacing[0] = imageSpacing[0];
                spacing[1] = imageSpacing[1];
                // HACK ticket #1204 - Degut a bugs en les gdcm integrades amb itk, l'spacing between slices no es calcula
                // correctament i se li assigna l'slice thickness al z-spacing. Una solució temporal i ràpida és llegir el tag
                // Spacing Between Slices i actualitzar el z-spacing, si aquest tag existeix
                // El cost de llegir aquest tag per un fitxer de 320 imatges és d'uns 470 milisegons aproximadament 
                // TODO un cop actualitzats a gdcm 2.0.x, aquest HACK serà innecessari
                DICOMTagReader *dicomReader = new DICOMTagReader(m_imageSet.first()->getPath());
                double zSpacing = dicomReader->getValueAttributeAsQString(DICOMSpacingBetweenSlices).toDouble();
                if ( zSpacing == 0.0 )
                    zSpacing = reader->GetOutput()->GetSpacing()[2];
                
                spacing[2] = zSpacing;                
                reader->GetOutput()->SetSpacing(spacing);
            }
        }
        
        this->setData(reader->GetOutput());
        // Emetem progress 100, perquè el corresponent diàleg de progrés es tanqui
        emit progress(100);
    }
    // TODO Falta tractament d'errors!?
    return errorCode;
}

int Volume::readFiles(QStringList filenames)
{
    int errorCode = NoError;
    if ( filenames.isEmpty() )
    {
        WARN_LOG("La llista de noms de fitxer per carregar és buida");
        errorCode = InvalidFileName;
        return errorCode;
    }

    if ( filenames.size() > 1 )
    {
        // Això és necessari per després poder demanar-li el diccionari de meta-dades i obtenir els tags del DICOM
        m_seriesReader->SetImageIO(m_gdcmIO);

        // Convertim la QStringList al format std::vector< std::string > que s'esperen les itk
        std::vector< std::string > stlFilenames;
        for (int i = 0; i < filenames.size(); i++)
        {
            stlFilenames.push_back(filenames.at(i).toStdString());
        }

        m_seriesReader->SetFileNames(stlFilenames);

        try
        {
            m_seriesReader->Update();
        }
        catch (itk::ExceptionObject & e)
        {
            WARN_LOG(QString("Excepció llegint els arxius del directori [%1] Descripció: [%2]")
                .arg(QFileInfo(filenames.at(0)).dir().path() )
                .arg(e.GetDescription() )
               );
            // Llegim el missatge d'error per esbrinar de quin error es tracta
            errorCode = identifyErrorMessage(QString(e.GetDescription()) );
        }
        switch ( errorCode )
        {
        case NoError:
            this->setData(m_seriesReader->GetOutput());
            emit progress(100);
            break;

        case SizeMismatch:
            // TODO Això es podria fer a ::getVtkData o ja està bé aquí?
            errorCode = NoError;
            readDifferentSizeImagesIntoOneVolume(filenames);
            emit progress(100);
            break;
        }
    }
    else
    {
        errorCode = this->readSingleFile(filenames.at(0));
    }
    return errorCode;
}

int Volume::identifyErrorMessage(const QString &errorMessage)
{
    if ( errorMessage.contains("Size mismatch") || errorMessage.contains("ImageIO returns IO region that does not fully contain the requested regionRequested") )
        return SizeMismatch;
    else if ( errorMessage.contains("Failed to allocate memory for image") )
        return OutOfMemory;
    else if ( errorMessage.contains("The file doesn't exists") )
        return MissingFile;
    else
        return UnknownError;
}

void Volume::createNeutralVolume()
{
    if ( m_imageDataVTK )
        m_imageDataVTK->Delete();
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
    signed short * scalarPointer = (signed short *) m_imageDataVTK->GetScalarPointer();
    signed short value;
    for (int i=0; i<10; i++)
    {
        value = 150-i*20;
        if ( i>4 )
            value = 150-(10-i-1)*20;

        for (int j = 0; j<10; j++)
        {            
            *scalarPointer = value;
            *scalarPointer++;
        }
    }
    // Quan creem el volum neutre indiquem que només tenim 1 sola fase 
    // TODO Potser s'haurien de crear tantes fases com les que indiqui la sèrie?
    this->setNumberOfPhases(1);
}

bool Volume::fitsIntoMemory()
{
    if ( m_dataLoaded )
        return true;
    
    unsigned long long int size = 0;
    foreach (Image *image, m_imageSet)
    {
        size += image->getColumns() * image->getRows() * sizeof(VoxelType);
    }

    char *p = 0;
    try
    {
        p = new char[size];
        delete[] p;
        return true;
    }
    catch (std::bad_alloc &ba)
    {
        return false;
    }
}

};

#endif
