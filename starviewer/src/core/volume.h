/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME_H
#define UDGVOLUME_H

#include <itkImage.h>
// Els filtres per passar itk<=>vtk: InsightApplications/auxiliary/vtk --> ho tenim a /tools
#include "itkImageToVTKImageFilter.h" //Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.
#include "itkVTKImageToImageFilter.h" // Converts a VTK image into an ITK image and plugs a vtk data pipeline to an ITK datapipeline.
#include <QStringList>

// itk - input
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkCommand.h>
#include <itkSmartPointer.h>
#include "itkQtAdaptor.h"
#include "logging.h"
// FWD declarations
class vtkImageData;

namespace udg {

class VolumeSourceInformation;
class Image;
class Series;
class Study;
class Patient;

/**
    Aquesta classe respresenta un volum de dades. Aquesta serà la classe on es guardaran les dades que voldrem tractar. Ens donarà mètodes per poder obtenir les dades en el format que volguem: ITK, VTK, etc.

    Es pot inicialitzar amb dades de tipus itk o vtk amb el mètode \c setData() .
    Per raons d'eficiència, com que el programa principalment el que farà serà visualitzar es retindran les dades en format natiu vtk. Només es convertiran a itk quan es demanin explícitament.

    @author Grup de Gràfics de Girona  ( GGG )
*/
class Volume : public QObject
{
Q_OBJECT
public:
    /// Tipus d'imatge intern per defecte d'itk
    typedef signed short int ItkPixelType;
    static const unsigned int VDimension = 3;

    typedef itk::Image<ItkPixelType, VDimension >    ItkImageType;
    typedef ItkImageType::Pointer  ItkImageTypePointer;

    /// tipus de punter de dades vtk
    typedef vtkImageData *VtkImageTypePointer;

    /// Aquests enums indiquem quin criteri escollim per ordenar les imatges
    enum ImageOrderType{ OrderImageByNumber, OrderImageBySliceLocation, OrderImageByTemporalDimension };

    Volume( QObject *parent = 0 );
    Volume( ItkImageTypePointer itkImage, QObject *parent = 0 );
    Volume( VtkImageTypePointer vtkImage, QObject *parent = 0 );
    ~Volume();

    /**
       Li donem les dades en format ITK
       \sa getVtkData(), getItkData()
    */
    void setData( ItkImageTypePointer itkImage );

    /**
       Li donem les dades en format VTK
       \sa getVtkData(), getItkData()
    */
    void setData( VtkImageTypePointer vtkImage );

    /**
        Ens retorna la dades en format VTK
        \sa setData()
    */
    VtkImageTypePointer getVtkData();

    /**
        Ens retorna les dades en format ITK
        \sa setData()
    */
    ItkImageTypePointer getItkData();

    /// Actualitza la informació del volum
    void updateInformation();

    /// Obté l'origen del volum
    void getOrigin( double  xyz[3] );
    double *getOrigin();

    /// Obté l'espaiat del model al llarg dels eixos
    void getSpacing( double xyz[3] );
    double *getSpacing();

    /// Retorna la ¿bounding box?
    void getWholeExtent( int extent[6] );
    int *getWholeExtent();

    /// Retorna les dimensions del volum
    int *getDimensions();
    void getDimensions( int dims[3] );

    /// TODO Mètodes transitoris pels canvis de disseny del tema de fases
    void setNumberOfPhases( int phases );
    int getNumberOfPhases() const;
    Volume *getPhaseVolume( int index );

    /// TODO: Repassar: això no hauria d'anar així.
    /// Retorna un nou Volume on la Data és només del sub volume indicat
    Volume *getSubVolume( int index );
    /// Reordena les llesques: específic CardiacMPRExtension
    Volume *orderSlices();

    /// Assignar/Obtenir el criteri d'ordenació de les imatges
    void setImageOrderCriteria( unsigned int orderCriteria );
    unsigned int getImageOrderCriteria() const;

    /// Afegim una imatge al conjunt d'imatges que composen el volum
    void addImage( Image *image );

    /// Assignem directament el conjunt d'imatges que composen aquest volum
    void setImages( const QList<Image *> &imageList );

    /// Retorna les imatges que composen el volum
    QList<Image *> getImages() const;

    /// Mètode de conveniència en el que obtenim la llista de fitxers a partir de totes les imatges.
    /// \deprecated TODO Es manté per dependència amb codi que ha de canviar
    QStringList getInputFiles() const;

    /// Mètodes ràpids per obtenir la series/estudis/pacient a les que pertany aquest volum
    Series *getSeries();
    Study *getStudy();
    Patient *getPatient();

    /// Volcat d'informació en un string per poder-ho printar on interessi
    QString toString( bool verbose = false );

    /**
     * Ens dóna la imatge corresponent a la llesca i fase donats
     * @param sliceNumber llesca
     * @param phaseNumber fase
     * @return la imatge en cas que els índexs siguin correctes, NULL altrament
     */
    Image *getImage( int sliceNumber, int phaseNumber ) const;

    /**
     * Ens retorna la direcció REAL(DICOM) en la que es troben apilades
     * les imatges que formen el volum. Com que dins d'un mateix volum podem tenir més
     * d'un frame/stack, hem d'indicar de quin frame/stack volem la direcció
     * TODO de moment, com que el suport a stacks/frames és bastant patètic, assumim que només hi ha un, però cal corretgir això
     * Caldrà també tenir com a mínim dues imatges en el mateix stack/frame per donar una direcció fiable.
     * En cas que només tinguem una sola imatge pel frame/stack donat, retornarem la normal d'aquella imatge
     * que és el que més se li pot aproximar
     * @param stack
     * @param direction[]
     */
    void getStackDirection( double direction[3], int stack = 0 );

signals:
    /**
     * Emet l'estat del progrés en el que es troba la càrrega de dades del volum
     * @param progress progrés de la càrrega en una escala de 1 a 100
     */
    void progress( int );

private:
    /// Mètode d'inicialització d'objectes comuns per als constructors
    void init();

    /// Mètodes de prova per tractar diferents models de càrrega de dades "lazy"
    /// Allotja l'espai a memòria primer, després va inserint les imatges una a una
    void loadWithPreAllocateAndInsert();

    /// carrega les llesques a partir dels objectes Image
    void loadSlicesWithReaders( int method );

    /// reserva l'espai per la imatge vtk segons l'input d'imatges que tenim
    void allocateImageData();

    /// Donades unes imatges que tenen diferents mides, les llegim en un sol
    /// volum adaptant la mida als valors maxims de row i column. S'executarà quan volguem llegir
    /// una sèrie que conté imatges amb diferents mides
    void readDifferentSizeImagesIntoOneVolume( QStringList filenames );

private:
    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter< ItkImageType > ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter< ItkImageType > VtkToItkFilterType;

    /// Les dades en format vtk
    VtkImageTypePointer m_imageDataVTK;

    /// Filtres per passar de vtk a itk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;

    /// Ens diu si les dades han estat carregades ja en memòria o no.
    /// Aquest membre el farem servir per aplicar el lazy loading
    bool m_dataLoaded;

    /// Criteri d'ordenació de les imatges
    unsigned int m_imageOrderCriteria;

    /// Conjunt d'imatges que composen el volum
    QList<Image *> m_imageSet;

//
// TOT AIXÒ ESTÀ PER L'ADAPTACIÓ D'INPUT NOMÉS!
// TODO tot això és temporal, quan es faci la lectura tal i com volem desapareixerà tot aquest codi
//
private slots:
    void slotProgress();
private:
    /// Tipus d'error que podem tenir
    enum { NoError = 1, SizeMismatch, InvalidFileName, UnknownError };
    void inputConstructor();
    void inputDestructor();
    /**
     * Carrega un volum a partir del nom de fitxer que se li passi
     * @param fileName
     * @return noError en cas que tot hagi anat bé, el tipus d'error altrament
     */
    int openFile( QString fileName );

    /**
     * Donat un conjunt de fitxers els carrega en una única sèrie/volum
     * @param filenames
     * @return noError en cas que tot hagi anat bé, el tipus d'error altrament
     */
    int readFiles( QStringList filenames );

private:
    typedef itk::ImageFileReader< ItkImageType >  ReaderType;
    typedef ReaderType::Pointer    ReaderTypePointer;

    typedef itk::ImageSeriesReader< ItkImageType >     SeriesReaderType;
    typedef itk::GDCMImageIO                        ImageIOType;
    typedef itk::GDCMSeriesFileNames                NamesGeneratorType;

    /// El lector de sèries dicom
    SeriesReaderType::Pointer m_seriesReader;

    /// El lector estàndar de fitxers singulars, normalment servirà per llegir *.mhd's
    ReaderTypePointer    m_reader;

    /// el lector de DICOM
    ImageIOType::Pointer m_gdcmIO;

    /// TODO membre temporal per la transició al tractament de fases
    int m_numberOfPhases;
};

/**
    Classe auxiliar per monitorejar el progrés de la lectura del fitxer
*/
class ProgressCommand : public itk::Command
{
public:
    typedef  ProgressCommand   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

protected:
    ProgressCommand() {};

public:
    typedef itk::ImageFileReader< Volume::ItkImageType >  ReaderType;
    typedef const ReaderType *ReaderTypePointer;

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        ReaderTypePointer m_reader = dynamic_cast< ReaderTypePointer >( object );
        if( typeid( event ) == typeid( itk::ProgressEvent ) )
        {
            DEBUG_LOG( QString("Progressant...%1").arg( m_reader->GetProgress() ) );
        }
        else
        {
            DEBUG_LOG( QString("No s'ha invocat ProgressEvent") );
        }
    }
};

};  // end namespace udg

#endif
