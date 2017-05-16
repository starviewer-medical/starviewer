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

#ifndef UDGIMAGE_H
#define UDGIMAGE_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QStringList>
#include <QPixmap>

#include "dicomsource.h"
#include "imageorientation.h"
#include "patientorientation.h"
#include "photometricinterpretation.h"
#include "imageoverlay.h"
#include "displayshutter.h"
#include "voilut.h"
#include "pixelspacing2d.h"

namespace udg {

class Series;

/**
    Classe que encapsula les propietats d'una imatge d'una sèrie de la classe Series
  */
class Image : public QObject {
Q_OBJECT
public:
    Image(QObject *parent = 0);
    ~Image();

    /// Assigna/obté el SOPInstanceUID de la imatge
    void setSOPInstanceUID(const QString &uid);
    QString getSOPInstanceUID() const;

    /// Assigna/obté l'instance number
    void setInstanceNumber(const QString &number);
    QString getInstanceNumber() const;

    /// Assignar/Obtenir la orientació del pla de la imatge, també anomenat direction cosines.
    /// Els valors són els vectors que formen el pla d'imatge.
    /// A partir d'aquests dos vectors, es calcula la normal del pla d'imatge
    /// @param orientation[] Els valors dels vectors que defineixen el pla d'imatge.
    void setImageOrientationPatient(const ImageOrientation &imageOrientation);
    ImageOrientation getImageOrientationPatient() const;

    /// Assignar/Obtenir l'orientació del pacient
    void setPatientOrientation(const PatientOrientation &orientation);
    PatientOrientation getPatientOrientation() const;

    /// Assignar/Obtenir l'espaiat dels pixels
    void setPixelSpacing(double x, double y);
    void setPixelSpacing(const PixelSpacing2D &spacing);
    PixelSpacing2D getPixelSpacing() const;

    /// Set/Get Imager Pixel Spacing
    void setImagerPixelSpacing(double x, double y);
    void setImagerPixelSpacing(const PixelSpacing2D &spacing);
    PixelSpacing2D getImagerPixelSpacing() const;

    /// Set/Get Estimated Radiographic Magnification Factor
    void setEstimatedRadiographicMagnificationFactor(double x);
    double getEstimatedRadiographicMagnificationFactor() const;

    /// Returns the preferred pixel spacing to use with this image. This method serves to deal with the cases
    /// where, depending on the modality and the present pixel spacing attributes, choose the pixel spacing value that should be used by default
    /// For instance, if we have an image with MG modality, imager pixel spacing and estimated radiographic factor, the value returned should be
    /// ImagerPixelSpacing corrected by EstimatedRadiographicFactor
    PixelSpacing2D getPreferredPixelSpacing() const;

    /// Assignar/Obtenir l'slice thickness, aka espaiat de les Z
    void setSliceThickness(double z);
    double getSliceThickness() const;

    /// Assignar/Obtenir la posició de la imatge.
    void setImagePositionPatient(double position[3]);
    const double* getImagePositionPatient() const;

    /// Assignar/Obtenir els samples per pixel
    void setSamplesPerPixel(int samples);
    int getSamplesPerPixel() const;

    /// Assignar/Obtenir la interpretació fotomètrica
    void setPhotometricInterpretation(const QString &value);
    PhotometricInterpretation getPhotometricInterpretation() const;

    /// Assignar/Obtenir files/columnes
    void setRows(int rows);
    int getRows() const;
    void setColumns(int columns);
    int getColumns() const;

    /// Assignar/Obtenir els bits allotjats
    void setBitsAllocated(int bits);
    int getBitsAllocated() const;

    /// Assignar/Obtenir els bits emmagatzemats
    void setBitsStored(int bits);
    int getBitsStored() const;

    /// Assignar/Obtenir el bit més alt
    void setHighBit(int highBit);
    int getHighBit() const;

    /// Assignar/Obtenir la representació dels pixels
    void setPixelRepresentation(int representation);
    int getPixelRepresentation() const;

    /// Assignar/Obtenir els valors del rescalat de la MODALITY LUT que s'apliquen sobre la imatge
    /// la fòrmula és f(x) = a*x + b, on 'x' és el valor del pixel de la imatge, 'a' l'Slope i 'b' l'Intercept
    void setRescaleSlope(double slope);
    double getRescaleSlope() const;
    void setRescaleIntercept(double intercept);
    double getRescaleIntercept() const;

    /// Assignar/Obtenir els valors del rescalat de la VOI LUT que s'apliquen sobre la imatge
    void addVoiLut(const VoiLut &voiLut);
    VoiLut getVoiLut(int index = 0) const;

    /// Assignem el llistat de WindowLevels de la imatge. Si el llistat conté algun WW/WL invàlid, no s'afegeix
    /// Sempre s'esborrarà qualsevol llistat de window level que hi hagués anteriorment
    void setVoiLutList(const QList<VoiLut> &voiLutList);
    
    /// Ens retorna el nombre de window levels que tenim
    int getNumberOfVoiLuts() const;

    /// Li indiquem quina és la sèrie pare a la qual pertany
    void setParentSeries(Series *series);
    Series* getParentSeries() const;

    /// Assigna/retorna el path absolut de la imatge
    void setPath(const QString &path);
    QString getPath() const;

    /// Assigna / retorna el slice location de la imatge
    void setSliceLocation(const QString &sliceLocation);
    QString getSliceLocation() const;

    /// Assignar/Obtenir la data i hora en que la sèrie s'ha descarregat a la base de dades Local
    void setRetrievedDate(QDate date);
    void setRetrievedTime(QTime time);
    QDate getRetrievedDate() const;
    QTime getRetrievedTime() const;

    /// Returns the Acquisition Number (0020,0012).
    const QString& getAcquisitionNumber() const;
    /// Sets the Acquisition Number (0020,0012).
    void setAcquisitionNumber(QString acquisitionNumber);

    /// Assignar/Obtenir la descripció del tipus d'imatge
    void setImageType(const QString &imageType);
    QString getImageType() const;

    /// Assignar/Obtenir la viewPosition
    void setViewPosition(const QString &viewPosition);
    QString getViewPosition() const;

    /// Assignar/Obtenir la lateritat de la imatge
    void setImageLaterality(const QChar &imageLaterality);
    QChar getImageLaterality() const;

    /// Assignar/Obtenir la descripció del View Code. De moment només s'aplicarà per imatges de mammografia.
    void setViewCodeMeaning(const QString &viewCodeMeaning);
    QString getViewCodeMeaning() const;

    /// Assignar/Obtenir el número de frame
    void setFrameNumber(int frameNumber);
    int getFrameNumber() const;

    /// Assignar/Obtenir el número de fase
    void setPhaseNumber (int phaseNumber);
    int getPhaseNumber() const;

    /// Assigna/Obtenir el número de volum al qual pertany la imatge dins la sèrie
    void setVolumeNumberInSeries (int volumeNumberInSeries);
    int getVolumeNumberInSeries() const;

    /// Assignar/Obtenir el número que ocupa la imatge dins volum
    void setOrderNumberInVolume(int orderNumberInVolume);
    int getOrderNumberInVolume() const;

    /// Assignar/Obtenir el Content Time (moment de creació de les dades)
    void setImageTime(const QString &imageTime);
    QString getImageTime() const;

    /// Ens retorna la hora en format hh:mm:ss en que va començar la creació de la imatge
    QString getFormattedImageTime() const;

    /// Sets the transfer syntax UID.
    void setTransferSyntaxUID(const QString &transferSyntaxUID);
    /// Returns the transfer syntax UID.
    const QString& getTransferSyntaxUID() const;

    /// Ens retorna la distància de l'orígen de la imatge passada per paràmetre respecte a un orígen 0, 0, 0, segons la normal del pla
    /// TODO Assignar-li un nom més entenedor
    static double distance(Image *image);
    
    /// Mètodes per obtenir/assignar el número d'overlays que té la imatge
    bool hasOverlays() const;
    unsigned short getNumberOfOverlays() const;
    void setNumberOfOverlays(unsigned short overlays);

    /// Obté la llista d'overlays
    QList<ImageOverlay> getOverlays();

    /// Obté una llista amb la divisió en regions de tots els overlays. Es fusionen tots els overlays originals en un de sol 
    /// i després es fa la partició òptima de les diferents parts que el composen
    QList<ImageOverlay> getOverlaysSplit();

    /// Ens diu si té shutters o no
    bool hasDisplayShutters() const;
    
    /// Afegeix un Display Shutter a l'imatge
    void addDisplayShutter(const DisplayShutter &shutter);
    
    /// Assigna una llista de Display Shutters a l'imatge. Aquesta sobreescriu l'anterior.
    void setDisplayShutters(const QList<DisplayShutter> &shuttersList);
    
    /// Obté la llista de Display Shutters
    QList<DisplayShutter> getDisplayShutters() const;

    /// Ens retorna la composició de DisplayShutters adequada per display
    /// Si no existeixen shutters o no hi ha cap disposició vàlida ens tornarà un DisplayShutter buit
    DisplayShutter getDisplayShutterForDisplay();

    /// Returns display shutter for display in vtkImageData format, with the same dimensions as the image.
    /// The vtkImageData object is created only the first time, subsequent calls return the same object.
    vtkImageData* getDisplayShutterForDisplayAsVtkImageData();

    /// Assingar/Obtenir el DICOMSource de la imatge. Indica quin és l'origen dels fitxers DICOM que conté la imatge
    void setDICOMSource(const DICOMSource &imageDICOMSource);
    DICOMSource getDICOMSource() const;

    /// Ens retorna la clau que identifica la imatge
    QString getKeyIdentifier() const;

    /// El mètode ens retorna el thumbnail de la imatge. Es crearà el primer cop que es demani
    /// @param getFromCache Si és cert intentarà carregar el thumbnail si es troba creat a la cache.
    ///                     Altrament, simplement comprobarà que no estigui creat a memòria i prou
    /// @param resolution La resolució amb la que volem el thumbnail
    /// @return Un QPixmap amb el thumbnail
    QPixmap getThumbnail(bool getFromCache = false, int resolution = 100);

    /// Ens retorna una llista amb les modalitats que suportem com a Image
    static QStringList getSupportedModalities();

private:
    /// Llegeix els overlays. Si splitOverlays és true, els guarda fent una divisió de les regions òptimes a la llista m_overlaysSplit
    /// Sinó els llegeix per separat i els guarda a la llista m_overlaysList
    bool readOverlays(bool splitOverlays = true);

private:
    /// Atributs DICOM

    /// Identificador de la imatge/arxiu. (0008,0018)
    QString m_SOPInstanceUID;

    /// Informació general de la imatge. C.7.6 General Image Module - PS 3.3.

    /// Nombre que identifica la imatge. (0020,0013) Tipus 2
    QString m_instanceNumber;

    /// Orientació anatòmica de les files i columnes de la imatge (LR/AP/HF). Requerit si la imatge no requereix Image Orientation(Patient)(0020,0037) i
    /// Image Position(Patient)(0020,0032). Veure C.6.7.1.1.1. (0020,0020) Tipus 2C.
    PatientOrientation m_patientOrientation;

    // TODO Referenced Image Sequence (0008,1140) Tipus 3. Seqüència que referència altres imatges significativament relacionades amb aquestes,
    // com un post-localizer per CT.

    // TODO Icon Image Sequence (0088,0200) Tipus 3. La següent imatge d'icona és representativa d'aquesta imatge. veure C.7.6.1.1.6

    // Image Plane Module C.6.7.2
    /// Distància física entre el centre de cada píxel (row,column) en mm. Veure 10.7.1.3. (0028,0030) Tipus 1
    PixelSpacing2D m_pixelSpacing;
    
    /// Imager Pixel spacing (0018,1164)
    /// Physical distance measured at the front plane of the Image Receptor housing between the center of each pixel.
    /// Present in CR (3), DX, MG, IO (1), Enhanced XA/XRF (1C), 3D XA (1C)
    PixelSpacing2D m_imagerPixelSpacing;

    /// Estimated Radiographic Magnification Factor (0018,1114)
    /// Ratio of Source Image Receptor Distance (SID) over Source Object Distance (SOD).
    /// May be present (3) in DX Positioning Module (C.8.11.5) (MG, DX, IO), XA Positioner Module C.8.7.5 (XA),
    /// XRF Positioner Module C.8.7.6 (RF), and must be present (1) in Breast Tomosynthesis Acquisition Module C.8.21.3.4 (Br To)
    double m_estimatedRadiographicMagnificationFactor;

    /// Vectors d'orientació de la imatge respecte al pacient.
    /// Veure C.6.7.2.1.1. (020,0037) Tipus 1.
    ImageOrientation m_imageOrientationPatient;

    /// Posició de la imatge. Les coordenades x,y,z la cantonada superior esquerre (primer pixel transmés) de la imatge, en mm.
    /// Veure C.6.7.2.1.1. (0020,0032) Tipus 1. \TODO aka origen?.
    double m_imagePositionPatient[3];

    /// Gruix de llesca en mm. (0018,0050) Tipus 2.
    double m_sliceThickness;

    // Image Pixel Module C.6.7.3
    /// Nombre de mostres per pixel en la imatge. Veure C.6.7.3.1.1. (0028,0002) Tipus 1.
    int m_samplesPerPixel;

    /// Interpretació fotomètrica (monocrom,color...). Veure C.6.7.3.1.2. (0028,0004) Tipus 1.
    PhotometricInterpretation m_photometricInterpretation;

    /// Files i columnes de la imatge. (0028,0010),(0028,0011) Tipus 1
    int m_rows;
    int m_columns;

    /// Bits allotjats per cada pixel. Cada mostra ha de tenir el mateix nombre de pixels allotjats. Veure PS 3.5 (0028,0100)
    int m_bitsAllocated;

    /// Bits emmagatzemats per cada pixel. Cada mostra ha de tenir el mateix nombre de pixels emmagatzemats. Veure PS 3.5 (0028,0101)
    int m_bitsStored;

    /// Bit més significant. Veure PS 3.5. (0028,0102) Tipus 1
    int m_highBit;

    /// Representació de cada mostra. Valors enumerats 0000H=unsigned integer, 0001H=complement a 2. (0028,0103) Tipus 1
    int m_pixelRepresentation;

    /// Valors de rescalat de la MODALITY LUT. (0028,1053),(0028,1054). Tipus 1
    double m_rescaleSlope, m_rescaleIntercept;

    /// Valors de rescalat de la VOI LUT. (0028,1050),(0028,1051) Tipus 1C, present si no hi ha VOI LUT Sequence
    /// Poden incloure "Explicació" dels window levels si n'hi ha, texte descriptiu.(0028,1055) Tipus 3.
    /// Com que podem tenir més d'un tindrem una llista
    QList<VoiLut> m_voiLutList;

    // TODO millorar definició
    /// Situació especial de la llesca en mm. (0020,1041)
    /// SC->tipus 3
    /// NM->tipus 3
    /// CT-> A la documentació dicom aquest camp no hi figura però philips l'utiliza com a Table Position
    QString m_sliceLocation;

    /// Tipus d'imatge. Ens pot definir si es tracta d'un localizer, per exemple. Conté els valors separats per '\\'
    /// Es troba al mòdul General Image C.7.6.1 i als mòduls Enhanced MR/CT/XA/XRF Image (C.8.13.1/C.8.15.2/C.8.19.2)
    /// En el cas d'imatges Enhanced CT/MR l'omplirem amb el valor FrameType contingut al functional group CT/MR Image Frame Type
    QString m_imageType;

    /// Vista radiogràfica associada a Patient Position. El trobem als mòduls CR Series (C.8.1.1) i DX Positioning (C.8.11.5)
    /// Valors definits:
    /// AP = Anterior/Posterior
    /// PA = Posterior/Anterior
    /// LL = Left Lateral
    /// RL = Right Lateral
    /// RLD = Right Lateral Decubitus
    /// LLD = Left Lateral Decubitus
    /// RLO = Right Lateral Oblique
    /// LLO = Left Lateral Oblique
    QString m_viewPosition;

    /// Lateralitat de la possiblement aparellada part del cos examinada.
    /// El trobem als mòduls DX Anatomy (C.8.11.2), Mammography Image (C.8.11.7), Intra-oral Image (C.8.11.9) i Ocular Region Imaged (C.8.17.5)
    /// També el trobem al mòdul Frame Anatomy (C.7.6.16.2.8) comú a tots els enhanced, però el tag s'anomena Frame Laterality en comptes d'Image Laterality.
    /// Valors definits:
    /// R = right
    /// L = left
    /// U = unpaired
    /// B = both left and right
    QChar m_imageLaterality;

    /// Descripció del tipus de vista de la imatge. El seu ús l'aplicarem bàsicament pels casos de mammografia definits a
    /// PS 3.16 - Context ID 4014 (cranio-caudal, medio-lateral oblique, etc...) però podríem extendre el seu ús a d'altres tipus d'imatge
    /// que també fan ús d'aquest tag per guardar aquest tipus d'informació amb altres possibles valors específics.
    QString m_viewCodeMeaning;

    /// Número de frame
    int m_frameNumber;

    /// Número de fase de la imatge
    int m_phaseNumber;

    /// Número de volum al qual pertany la imatge dins la sèrie
    int m_volumeNumberInSeries;

    /// Número d'ordre de la imatge dins el vo
    int m_orderNumberInVolume;

    /// Moment en el que es va crear el pixel data
    QString m_imageTime;

    // TODO C.7.6.5 CINE MODULE: Multi-frame Cine Image

    /// Transfer Syntax UID (0002,0010)
    /// Transfer syntax defines how DICOM objects are serialized.
    QString m_transferSyntaxUID;

    /// Atributs NO-DICOM

    /// El path absolut de la imatge
    QString m_path;

    /// Data en que la imatge s'ha descarregat a la base de dades local
    QDate m_retrievedDate;
    QTime m_retrieveTime;

    /// Acquisition Number (0020,0012). Type 3 in C.7.6.1 General Image Module (type 1 or 2 in other modules).
    /// A number identifying the single continuous gathering of data over a period of time that resulted in this image.
    QString m_acquisitionNumber;

    /// Atribut que ens dirà quants overlays té la imatge
    unsigned short m_numberOfOverlays;
    
    /// Llista d'overlays carregats
    QList<ImageOverlay> m_overlaysList;

    /// Llista que conté la partició en regions òptimes de la fusió de tots els overlays
    QList<ImageOverlay> m_overlaysSplit;

    /// Llista de display shutters
    QList<DisplayShutter> m_shuttersList;
    
    /// DisplayShutter per display que guardarem un cop creat
    DisplayShutter m_displayShutterForDisplay;
    
    /// Ens indica si hem de crear el display shutter per display
    bool m_haveToBuildDisplayShutterForDisplay;

    /// Display shutter for display in vtkImageData format.
    vtkSmartPointer<vtkImageData> m_displayShutterForDisplayVtkImageData;

    /// La sèrie pare
    Series *m_parentSeries;

    /// Cache de la imatge de previsualització
    QPixmap m_thumbnail;

    //Indica quin és l'origen de les imatges DICOM
    DICOMSource m_imageDICOMSource;
};

}

#endif
