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
#include "logging.h"
#include "identifier.h"
// Qt
#include <QPixmap>

// FWD declarations
class vtkImageData;

namespace udg {

class Image;
class Study;
class Patient;
class VolumeReader;

/**
    Aquesta classe respresenta un volum de dades. Aquesta serà la classe on es guardaran les dades que voldrem tractar. 
    Ens donarà mètodes per poder obtenir les dades en el format que volguem: ITK, VTK, etc.

    Es pot inicialitzar amb dades de tipus itk o vtk amb el mètode \c setData() .
    Per raons d'eficiència, com que el programa principalment el que farà serà visualitzar es retindran les dades en format natiu vtk. 
    Només es convertiran a itk quan es demanin explícitament.
*/
class Volume : public QObject {
Q_OBJECT
public:
    /// Tipus de vòxel del volum
    typedef signed short int VoxelType;
    /// Tipus d'imatge intern per defecte d'itk
    typedef VoxelType ItkPixelType;
    static const unsigned int VDimension = 3;

    typedef itk::Image<ItkPixelType, VDimension> ItkImageType;
    typedef ItkImageType::Pointer ItkImageTypePointer;

    /// Tipus de punter de dades vtk
    typedef vtkImageData *VtkImageTypePointer;

    Volume(QObject *parent = 0);
    Volume(ItkImageTypePointer itkImage, QObject *parent = 0);
    Volume(VtkImageTypePointer vtkImage, QObject *parent = 0);
    ~Volume();

    /**
       Li donem les dades en format ITK
       \sa getVtkData(), getItkData()
    */
    void setData(ItkImageTypePointer itkImage);

    /**
       Li donem les dades en format VTK
       \sa getVtkData(), getItkData()
    */
    void setData(VtkImageTypePointer vtkImage);

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

    /// Obté l'origen del volum
    void getOrigin(double  xyz[3]);
    double* getOrigin();

    /// Obté l'espaiat del model al llarg dels eixos
    void getSpacing(double xyz[3]);
    double* getSpacing();

    /// Retorna la ¿bounding box?
    void getWholeExtent(int extent[6]);
    int* getWholeExtent();

    /// Retorna les dimensions del volum
    int* getDimensions();
    void getDimensions(int dims[3]);

    /// Assigna/Retorna l'identificador del volum. 
    void setIdentifier(const Identifier &id);
    Identifier getIdentifier() const;

    /// Assigna/Retorna el thumbnail del volum
    void setThumbnail(const QPixmap &thumbnail);
    QPixmap getThumbnail() const;

    /// TODO Mètodes transitoris pels canvis de disseny del tema de fases
    void setNumberOfPhases(int phases);
    int getNumberOfPhases() const;
    Volume* getPhaseVolume(int index);
    QList<Image *> getPhaseImages(int index);
    void setNumberOfSlicesPerPhase(int slicesPerPhase);
    int getNumberOfSlicesPerPhase() const;

    /// Afegim una imatge al conjunt d'imatges que composen el volum
    void addImage(Image *image);

    /// Assignem directament el conjunt d'imatges que composen aquest volum
    void setImages(const QList<Image *> &imageList);

    /// Retorna les imatges que composen el volum
    QList<Image *> getImages() const;

    /// Retorna el nombre total de frames que té el volum amb els objectes Image que conté
    int getNumberOfFrames() const;

    /// Mètodes ràpids per obtenir estudi/pacient al que pertany aquest volum
    Study* getStudy();
    Patient* getPatient();

    /// Volcat d'informació en un string per poder-ho printar on interessi
    QString toString(bool verbose = false);

    /**
     * Ens dóna la imatge corresponent a la llesca i fase donats
     * Per defecte, només especificarem la imatge sense tenir en compte la fase
     * @param sliceNumber llesca
     * @param phaseNumber fase
     * @return la imatge en cas que els índexs siguin correctes, NULL altrament
     */
    Image* getImage(int sliceNumber, int phaseNumber = 0) const;

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
    void getStackDirection(double direction[3], int stack = 0);

    /// Obtenim el punter a les dades que es troben en l'índex donat
    /// És un accés a baix nivell, ja que obtenim el punter de les dades
    /// Retornem el punter transformat al tipus natiu de dades VoxelType
    VoxelType* getScalarPointer(int x = 0, int y = 0, int z = 0);
    VoxelType* getScalarPointer(int index[3]);

    /// Donada una coordenada de món, ens dóna el valor del vòxel corresponent.
    /// Si la coordenada està dins del volum retorna true, false altrament.
    /// TODO S'espera que la coordenada sigui dins del món VTK! 
    /// Caldria determinar si ha de ser així o hauria de ser DICOM o en un altre sistema.
    bool getVoxelValue(double coordinate[3], Volume::VoxelType &voxelValue);

    /// Ens calcula si el volum quep a memòria. Si el volum ja ha estat carregat prèviament amb èxit, retornarà cert
    bool fitsIntoMemory();

    /// S'encarrega de crear un volum "de mínims" per donar un output en casos que
    /// ens quedem sense memòria o ens trobem amb altres problemes. Vindria a ser un 
    /// volum neutre per evitar que l'aplicació peti en casos d'error no controlats
    /// TODO Aquest mètode potser s'hauria de transformar en una subclasse de Volume que 
    /// únicament creïi aquest tipu de volum
    void createNeutralVolume();

signals:
    /**
     * Emet l'estat del progrés en el que es troba la càrrega de dades del volum
     * @param progress progrés de la càrrega en una escala de 1 a 100
     */
    void progress(int);

private:
    /// Mètode d'inicialització d'objectes comuns per als constructors
    void init();

private:
    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter<ItkImageType> ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter<ItkImageType> VtkToItkFilterType;

    /// Les dades en format vtk
    VtkImageTypePointer m_imageDataVTK;

    /// Filtres per passar de vtk a itk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;

    /// Ens diu si les dades han estat carregades ja en memòria o no.
    /// Aquest membre el farem servir per aplicar el lazy loading
    bool m_dataLoaded;

    /// Conjunt d'imatges que composen el volum
    QList<Image *> m_imageSet;

    /// Identificador de volum
    Identifier m_identifier;

    /// Thumbnail del volum
    QPixmap m_thumbnail;

    /// Classe per llegir les dades del volum
    VolumeReader *m_volumeReader;

    /// TODO membre temporal per la transició al tractament de fases
    int m_numberOfPhases;
    int m_numberOfSlicesPerPhase;
};

}  // end namespace udg

#endif
