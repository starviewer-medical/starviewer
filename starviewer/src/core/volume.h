#ifndef UDGVOLUME_H
#define UDGVOLUME_H

#include <itkImage.h>

#include "identifier.h"
#include "volumepixeldata.h"
#include "anatomicalplane.h"
// Qt
#include <QPixmap>
#include <QVector>
// FWD declarations
class vtkImageData;

namespace udg {

class Image;
class Study;
class Patient;

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
    // TODO: Typedef's duplicats de VolumePixelData, es manté de moment mentre no es va substituïnt arreu on s'hi fa referència
    typedef VolumePixelData::VoxelType VoxelType;
    typedef VolumePixelData::ItkPixelType ItkPixelType;
    static const unsigned int VDimension = VolumePixelData::VDimension;
    typedef VolumePixelData::ItkImageType ItkImageType;
    typedef VolumePixelData::ItkImageTypePointer ItkImageTypePointer;

    Volume(QObject *parent = 0);
    ~Volume();

    /// Assignem/Retornem les dades de pixel data en format ITK
    void setData(ItkImageTypePointer itkImage);
    ItkImageTypePointer getItkData();

    /// Assignem/Retornem les dades de pixel data en format VTK
    void setData(vtkImageData *vtkImage);
    vtkImageData* getVtkData();

    /// Assigna/Retorna el Volume Pixel Data
    /// Pressuposa que les dades assignades no estan per carregar i que les que retorna només són vàlides
    /// si isPixelDataLoaded() retorna true.
    void setPixelData(VolumePixelData *pixelData);
    VolumePixelData* getPixelData();

    /// Ens indica si té el pixel data carregat.
    /// Si no el té els mètodes que pregunten sobre dades del volum poden donar respostes incorrectes.
    bool isPixelDataLoaded() const;

    /// Obté l'origen del volum
    void getOrigin(double xyz[3]);
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

    /// Ens retornar el rang de valors del volum (valor mínim i màxim).
    void getScalarRange(double range[2]);

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
    QList<Image*> getPhaseImages(int index);
    void setNumberOfSlicesPerPhase(int slicesPerPhase);
    int getNumberOfSlicesPerPhase() const;

    /// Afegim una imatge al conjunt d'imatges que composen el volum
    void addImage(Image *image);

    /// Assignem directament el conjunt d'imatges que composen aquest volum
    void setImages(const QList<Image*> &imageList);

    /// Retorna les imatges que composen el volum
    QList<Image*> getImages() const;

    /// Retorna el nombre total de frames que té el volum amb els objectes Image que conté
    int getNumberOfFrames() const;

    /// Mètodes ràpids per obtenir estudi/pacient al que pertany aquest volum
    Study* getStudy();
    Patient* getPatient();

    /// Volcat d'informació en un string per poder-ho printar on interessi
    QString toString(bool verbose = false);

    /// Ens dóna la imatge corresponent a la llesca i fase donats
    /// Per defecte, només especificarem la imatge sense tenir en compte la fase
    /// @param sliceNumber llesca
    /// @param phaseNumber fase
    /// @return la imatge en cas que els índexs siguin correctes, NULL altrament
    Image* getImage(int sliceNumber, int phaseNumber = 0) const;

    /// Ens retorna la direcció REAL(DICOM) en la que es troben apilades
    /// les imatges que formen el volum. Com que dins d'un mateix volum podem tenir més
    /// d'un frame/stack, hem d'indicar de quin frame/stack volem la direcció
    /// TODO de moment, com que el suport a stacks/frames és bastant patètic, assumim que només hi ha un, però cal corretgir això
    /// Caldrà també tenir com a mínim dues imatges en el mateix stack/frame per donar una direcció fiable.
    /// En cas que només tinguem una sola imatge pel frame/stack donat, retornarem la normal d'aquella imatge
    /// que és el que més se li pot aproximar
    /// @param stack
    /// @param direction[]
    void getStackDirection(double direction[3], int stack = 0);

    /// Obtenim un punter a les dades del VolumePixelData. Veure VolumePixelData::getScalarPointer per més informació.
    VoxelType* getScalarPointer(int x = 0, int y = 0, int z = 0);
    VoxelType* getScalarPointer(int index[3]);

    /// Ens calcula si el volum quep a memòria. Si el volum ja ha estat carregat prèviament amb èxit, retornarà cert
    bool fitsIntoMemory();

    /// S'encarrega de convertir el volum a un volum "de mínims" per donar un output en casos que
    /// ens quedem sense memòria o ens trobem amb altres problemes. Vindria a ser un
    /// volum neutre per evitar que l'aplicació peti en casos d'error no controlats
    /// TODO Aquest mètode potser s'hauria de transformar en una subclasse de Volume que
    /// únicament creïi aquest tipu de volum
    void convertToNeutralVolume();

    /// Ens retorna el pla d'adquisició del volum
    /// En el cas que no tinguem imatges retornarà NotAvailable.
    /// TODO Com que el volum pot estar format per imatges de diferents plans, el pla d'adquisició 
    /// es calcula en base a la primera imatge i prou
    AnatomicalPlane::AnatomicalPlaneType getAcquisitionPlane() const;

    /// Ens retorna l'índex intern d'imatge corresponent a la llesca i fase indicats
    int getImageIndex(int sliceNumber, int phaseNumber) const;

signals:
    /// Emet l'estat del progrés en el que es troba la càrrega de dades del volum
    /// @param progress progrés de la càrrega en una escala de 1 a 100
    void progress(int);

private:
    /// Ens diu si les dades han estat carregades ja en memòria o no.
    /// Aquest membre el farem servir per aplicar el lazy loading
    bool m_volumePixelDataLoaded;

    /// Conjunt d'imatges que composen el volum
    QList<Image*> m_imageSet;

    /// Identificador de volum
    Identifier m_identifier;

    /// Thumbnail del volum
    QPixmap m_thumbnail;

    /// Pixel data del volume
    VolumePixelData *m_volumePixelData;

    /// TODO membre temporal per la transició al tractament de fases
    int m_numberOfPhases;
    int m_numberOfSlicesPerPhase;
};

}  // End namespace udg

#endif
