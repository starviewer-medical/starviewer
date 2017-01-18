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

#ifndef UDGVOLUME_H
#define UDGVOLUME_H

#include <itkImage.h>

#include "identifier.h"
#include "volumepixeldata.h"
#include "anatomicalplane.h"
#include "orthogonalplane.h"
// Qt
#include <QPixmap>
#include <QVector>
// FWD declarations
class vtkImageData;

namespace udg {

class Image;
class Series;
class Study;
class Patient;
class VolumeReader;
class ImagePlane;

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
    /// L'assignació no accepta punters nuls.
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
    void getExtent(int extent[6]);
    int* getExtent();

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

    /// Returns the modality, if available, of the images that compose the volume
    QString getModality() const;

    /// Afegim una imatge al conjunt d'imatges que composen el volum
    void addImage(Image *image);

    /// Assignem directament el conjunt d'imatges que composen aquest volum
    void setImages(const QList<Image*> &imageList);

    /// Retorna les imatges que composen el volum
    QList<Image*> getImages() const;

    /// Retorna el nombre total de frames que té el volum amb els objectes Image que conté
    int getNumberOfFrames() const;

    /// Retorna cert si el volum és multiframe.
    bool isMultiframe() const;

    /// Shortcut methods to get the parent series/study/patient which this volume belongs to
    Series* getSeries() const;
    Study* getStudy() const;
    Patient* getPatient() const;

    /// Volcat d'informació en un string per poder-ho printar on interessi
    QString toString(bool verbose = false);

    /// Ens dóna la imatge corresponent a la llesca i fase donats
    /// Per defecte, només especificarem la imatge sense tenir en compte la fase
    /// @param sliceNumber llesca
    /// @param phaseNumber fase
    /// @return la imatge en cas que els índexs siguin correctes, NULL altrament
    Image* getImage(int sliceNumber, int phaseNumber = 0) const;

    /// Given a slice and an orthogonal plane, returns the corresponding ImagePlane
    /// @param vtkReconstructionHack HACK enables a hack for cases which the "real" plane is not the really wanted
    /// applying a correction to satisfy some restrictions with vtk. This should only be used on very concrete cases. Default value should be used.
    /// @return The corresponding image plane
    QSharedPointer<ImagePlane> getImagePlane(int sliceNumber, const OrthogonalPlane &plane, bool vtkReconstructionHack = false);
    
    /// Returns the pixel units for this volume. If the units cannot be specified, an empty string will be returned
    QString getPixelUnits();
    
    /// Returns the slice range of the current volume corresponding to an specified orthogonal plane
    void getSliceRange(int &min, int &max, const OrthogonalPlane &plane);

    /// Returns the maximum/minimum slice for an specified orthogonal plane
    int getMaximumSlice(const OrthogonalPlane &plane);
    int getMinimumSlice(const OrthogonalPlane &plane);

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

    /// Returns a pointer to the raw pixel data at index [x, y, z]. Avoid its use if possible and prefer using an iterator instead.
    void* getScalarPointer(int x = 0, int y = 0, int z = 0);
    /// Returns a pointer to the raw pixel data at the given index. Avoid its use if possible and prefer using an iterator instead.
    void* getScalarPointer(int index[3]);

    /// Returns a VolumePixelDataIterator pointing to the voxel at index [x, y, z].
    VolumePixelDataIterator getIterator(int x, int y, int z);
    /// Returns a VolumePixelDataIterator pointing to the first voxel.
    VolumePixelDataIterator getIterator();

    /// Returns value of voxel at index [x, y, z].
    double getScalarValue(int x, int y, int z);

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
    AnatomicalPlane getAcquisitionPlane() const;

    /// Returns which orthogonal plane of the current volume corresponds to the given anatomical plane
    OrthogonalPlane getCorrespondingOrthogonalPlane(const AnatomicalPlane &anatomicalPlane) const;

    /// Ens retorna l'índex intern d'imatge corresponent a la llesca i fase indicats
    int getImageIndex(int sliceNumber, int phaseNumber) const;

    /// Donada una coordenada de món, ens dóna l'índex del vòxel corresponent.
    /// Si la coordenada està dins del volum retorna true, false altrament.
    /// TODO S'espera que la coordenada sigui dins del món VTK!
    bool computeCoordinateIndex(const double coordinate[3], int index[3]);

    /// Retorna el nombre de components
    int getNumberOfScalarComponents();

     /// Retorna la mida dels escalars
    int getScalarSize();

    /// Retorna l'scalar pointer de la imatge en forma de QByteArray.
    QByteArray getImageScalarPointer(int imageNumber);

    /// Returns true if all the images in this volume are in the same anatomical plane.
    bool areAllImagesInTheSameAnatomicalPlane() const;
    
signals:
    /// Emet l'estat del progrés en el que es troba la càrrega de dades del volum
    /// @param progress progrés de la càrrega en una escala de 1 a 100
    void progress(int);

private:

    virtual VolumeReader* createVolumeReader();

    /// Lazy loading of the units of the pixels of PT series
    QString getPTPixelUnits(const Image *image);

private:

    /// Conjunt d'imatges que composen el volum
    QList<Image*> m_imageSet;
    /// True if it has been checked that all images are in the same anatomical plane since the last change in the image set.
    mutable bool m_checkedImagesAnatomicalPlane;
    /// True if all the images in this volume are in the same anatomical plane.
    mutable bool m_allImagesAreInTheSameAnatomicalPlane;

    /// Identificador de volum
    Identifier m_identifier;

    /// Thumbnail del volum
    QPixmap m_thumbnail;

    /// Pixel data del volume
    VolumePixelData *m_volumePixelData;

    /// TODO membre temporal per la transició al tractament de fases
    int m_numberOfPhases;
    int m_numberOfSlicesPerPhase;

    /// Stores the units of the pixel values of PT series. getPTPixelUnits should always be used to get this value
    QString m_PTPixelUnits;
};

}  // End namespace udg

#endif
