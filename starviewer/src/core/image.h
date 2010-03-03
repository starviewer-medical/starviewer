/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGE_H
#define UDGIMAGE_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QStringList>
#include <QPixmap>

namespace udg {

class Series;

/**
Classe que encapsula les propietats d'una imatge d'una sèrie de la classe Series

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Image : public QObject
{
Q_OBJECT
public:
    Image(QObject *parent = 0);

    ~Image();

    /// assigna/obté el SOPInstanceUID de la imatge
    void setSOPInstanceUID( QString uid );
    QString getSOPInstanceUID() const;

    /// assigna/obté l'instance number
    void setInstanceNumber( QString number );
    QString getInstanceNumber() const;

    /**
     * Assignar/Obtenir la orientació del pla de la imatge, també anomenat direction cosines.
     * Els valors són els vectors que formen el pla d'imatge.
     * A partir d'aquests dos vectors, es calcula la normal del pla d'imatge
     * @param orientation[] Els valors dels vectors que defineixen el pla d'imatge.
     */
    void setImageOrientationPatient( double orientation[6] );
    void setImageOrientationPatient( double xVector[3], double yVector[3] );
    const double *getImageOrientationPatient() const;

    /// Assignar/Obtenir l'string d'orientació del pacient
    void setPatientOrientation( QString orientation );
    QString getPatientOrientation() const;

     /**
     * A partir dels eixos de la imatge, ens diu en un string quina és
     * la orientació de la imatge
     * @return Els valors podran ser AXIAL, SAGITAL, CORONAL, OBLIQUE o N/A (not available)
     */
    QString getOrientationLabel();

    /// Assignar/Obtenir l'espaiat dels pixels
    void setPixelSpacing( double x, double y );
    const double *getPixelSpacing() const;

    /// Assignar/Obtenir l'slice thickness, aka espaiat de les Z
    void setSliceThickness( double z );
    double getSliceThickness() const;

    /// Assignar/Obtenir la posició de la imatge.
    void setImagePositionPatient( double position[3] );
    const double *getImagePositionPatient() const;
	/// Mètode per obtenir la normal del pla de la imatge. 
	/// Equivalent a demanar getImagePostionPatient() i quedar-nos 
	/// amb els tres últims valors (índexs 6,7,8)
	void getImagePlaneNormal( double normal[3] );

    /// Assignar/Obtenir els samples per pixel
    void setSamplesPerPixel( int samples );
    int getSamplesPerPixel() const;

    /// Assignar/Obtenir la interpretació fotomètrica
    void setPhotometricInterpretation( QString value );
    QString getPhotometricInterpretation() const;

    /// Assignar/Obtenir files/columnes
    void setRows( int rows );
    int getRows() const;
    void setColumns( int columns );
    int getColumns() const;

    /// Assignar/Obtenir els bits allotjats
    void setBitsAllocated( int bits );
    int getBitsAllocated() const;

    /// Assignar/Obtenir els bits emmagatzemats
    void setBitsStored( int bits );
    int getBitsStored() const;

    /// Assignar/Obtenir el bit més alt
    void setHighBit( int highBit );
    int getHighBit() const;

    /// Assignar/Obtenir la representació dels pixels
    void setPixelRepresentation( int representation );
    int getPixelRepresentation() const;

    /// Assignar/Obtenir els valors del rescalat de la MODALITY LUT que s'apliquen sobre la imatge
    /// la fòrmula és f(x) = a*x + b, on 'x' és el valor del pixel de la imatge, 'a' l'Slope i 'b' l'Intercept
    void setRescaleSlope( double slope );
    double getRescaleSlope() const;
    void setRescaleIntercept( double intercept );
    double getRescaleIntercept() const;

    /// Assignar/Obtenir els valors del rescalat de la VOI LUT que s'apliquen sobre la imatge
    void addWindowLevel( double window, double level );
    double getWindowCenter( int index = 0 ) const;
    double getWindowWidth( int index = 0 ) const;
    QPair<double,double> getWindowLevel( int index = 0 ) const;

    /// Ens retorna el nombre de window levels que tenim
    int getNumberOfWindowLevels();

    /// Assignar/Obtenir textes descriptius dels window level
    void addWindowLevelExplanation( QString explanation );
    void setWindowLevelExplanations( const QStringList &explanations );
    QString getWindowLevelExplanation( int index = 0 ) const;

    /// Li indiquem quina és la sèrie pare a la qual pertany
    void setParentSeries( Series *series );
    Series *getParentSeries() const;

    /// assigna/retorna el path absolut de la imatge
    void setPath( QString path );
    QString getPath() const;

    ///assigna / retorna el slice location de la imatge
    void setSliceLocation( QString sliceLocation );
    QString getSliceLocation() const;

    ///Assignar/Obtenir la data i hora en que la sèrie s'ha descarregat a la base de dades Local
    void setRetrievedDate(QDate date);
    void setRetrievedTime(QTime time);
    QDate getRetrievedDate();
    QTime getRetrievedTime();

    /// Assignar/Obtenir la descripció del tipus d'imatge
    void setImageType( const QString &imageType );
    QString getImageType() const;
    
    /// Assignar/Obtenir la descripció del tipus de frame. 
    /// En cas d'imatges single-frame el seu valor serà idèntic a Image Type
    void setFrameType( const QString &frameType );
    QString getFrameType() const;

    /// Assignar/Obtenir la viewPosition
    void setViewPosition( const QString &viewPosition );
    QString getViewPosition() const;

    /// Assignar/Obtenir la lateritat de la imatge
    void setImageLaterality( const QChar &imageLaterality );
    QChar getImageLaterality() const;

    /// Assignar/Obtenir la descripció del View Code. De moment només s'aplicarà per imatges de mammografia.
    void setViewCodeMeaning( const QString &viewCodeMeaning );
    QString getViewCodeMeaning() const;
    
    /// Assignar/Obtenir el número de frame
    void setFrameNumber( int frameNumber );
    int getFrameNumber() const;

    ///Assignar/Obtenir el número de fase
    void setPhaseNumber (int phaseNumber);
    int getPhaseNumber() const;

    ///Assigna/Obtenir el número de volum al qual pertany la imatge dins la sèrie
    void setVolumeNumberInSeries (int volumeNumberInSeries); 
    int getVolumeNumberInSeries() const;

    ///Assignar/Obtenir el número que ocupa la imatge dins volum
    void setOrderNumberInVolume(int orderNumberInVolume);
    int getOrderNumberInVolume() const;
    
    /// Assignar/Obtenir el Content Time ( moment de creació de les dades )
    void setImageTime( const QString &imageTime );
    QString getImageTime() const;

    /// Ens retorna la hora en format hh:mm:ss en que va començar la creació de la imatge
    QString getFormattedContentTime() const;
    
    /// Ens retorna la clau que identifica la imatge
    QString getKeyIdentifier() const;
    
    /// afegeix un objecte imatge a la imatge
    void addReferencedImage( Image *image );

    /// obté l'objecte imatge pel sopInstanceUID donat. Si no existeix cap imatge amb aquest UID es retorna NUL
    Image *getReferencedImage( QString SOPInstanceUID ) const;

    /// Retorna una llista de totes les imatges relacionades amb la imatge
    QList<Image *> getReferencedImages() const;

    /// Indica si una imatge té imatges relacionades
    bool hasReferencedImages() const;

    /**
     * El mètode ens retorna el thumbnail de la imatge. Es crearà el primer cop que es demani
     * @param resolution La resolució amb la que volem el thumbnail
     * @return Un QPixmap amb el thumbnail
     */
    QPixmap getThumbnail(int resolution = 100);

    /// Ens diu quin és el pla de projecció d'una imatge segons les etiquetes d'orientació (R/L,A/P,F/H)
    /// El format serà "direccióFiles\\direccióColumnes"
    /// Valors: AXIAL, SAGITAL, CORONAL, OBLIQUE o N/A
    static QString getProjectionLabelFromPlaneOrientation( const QString &orientation );

    /// Ens retorna una llista amb les modalitats que suportem com a Image
    static QStringList getSupportedModalities();

private:
    /// Atributs DICOM

    /// identificador de la imatge/arxiu. (0008,0018)
    QString m_SOPInstanceUID;

    /// Informació general de la imatge. C.7.6 General Image Module - PS 3.3.

    /// Nombre que identifica la imatge. (0020,0013) Tipus 2
    QString m_instanceNumber;

    /// Direcció de les files i columnes de la imatge ( LR/AP/SI ). Requerit si la imatge no requereix Image Orientation(Patient)(0020,0037) i Image Position(Patient)(0020,0032). Veure C.6.7.1.1.1. (0020,0020) Tipus 2C.
    QString m_patientOrientation;

    //\TODO Referenced Image Sequence (0008,1140) Tipus 3. Seqüència que referència altres imatges significativament relacionades amb aquestes, com un post-localizer per CT.

    // \TODO Icon Image Sequence (0088,0200) Tipus 3. La següent imatge d'icona és representativa d'aquesta imatge. veure C.7.6.1.1.6

    // Image Plane Module C.6.7.2
    /// Distància física entre el centre de cada píxel (row,column) en mm. Veure 10.7.1.3. (0028,0030) Tipus 1
    double m_pixelSpacing[2];

    /// Orientació de la imatge. Els direction cosines de la primera fila(0,1,2) i de la primera columna(3,4,5) respecte al pacient. Veure C.6.7.2.1.1. (020,0037) Tipus 1. Adicionalment hi guardmem el vector normal del pla (6,7,8)
    double m_imageOrientationPatient[9];

    /// posició de la imatge. Les coordenades x,y,z la cantonada superior esquerre (primer pixel transmés) de la imatge, en mm. Veure C.6.7.2.1.1. (0020,0032) Tipus 1. \TODO aka origen?.
    double m_imagePositionPatient[3];

    /// gruix de llesca en mm. (0018,0050) Tipus 2.
    double m_sliceThickness;

    // Image Pixel Module C.6.7.3
    /// Nombre de mostres per pixel en la imatge. Veure C.6.7.3.1.1. (0028,0002) Tipus 1.
    int m_samplesPerPixel;

    /// interpretació fotomètrica (monocrom,color...). Veure C.6.7.3.1.2. (0028,0004) Tipus 1.
    QString m_photometricInterpretation;

    /// files i columnes de la imatge. (0028,0010),(0028,0011) Tipus 1
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
    /// Com que podem tenir més d'un tindrem una llista
    QList< QPair< double, double > > m_windowLevelList;

    /// "Explicació" dels window levels, texte descriptiu.(0028,1055) Tipus 3.
    QStringList m_windowLevelExplanationList;

    /// Nombre de frames de la imatge. (0028,0008) Tipus 1
    int m_numberOfFrames;

    //TODO millorar definició
    /** Situació especial de la llesca en mm. (0020,1041)
        SC->tipus 3
        NM->tipus 3
        CT-> A la documentació dicom aquest camp no hi figura però philips l'utiliza com a Table Position
      */
    QString m_sliceLocation;

    /// Tipus d'imatge. Ens pot definir si es tracta d'un localizer, per exemple. Conté els valors separats per '\\'
    /// Es troba al mòdul General Image C.7.6.1 i als mòduls Enhanced MR/CT/XA/XRF Image (C.8.13.1/C.8.15.2/C.8.19.2)
    QString m_imageType;
    
    /// Tipus de frame. Ens pot definir si es tracta d'un localizer, per exemple. Conté els valors separats per '\\'
    /// El podem trobar a MR Image Frame Type C.8.13.5.1 i CT Image Frame Type C.8.15.3.1. Per més info consultar C.8.16.1 Image Type and Frame Type
    /// El valor d'aquest camp podrà diferir del d'Image Type en el cas que el primer valor d'Image Type sigui MIXED, que seria el cas en que aquest
    /// Image pertanyi a un conjunt d'Images "multiframe" i que els frames tinguin diferents valors d'Image Type (imatges derived amb original ,per exemple
    QString m_frameType;
    
    /**
        Vista radiogràfica associada a Patient Position. El trobem als mòduls CR Series (C.8.1.1) i DX Positioning (C.8.11.5)
        Valors definits:
        AP = Anterior/Posterior
        PA = Posterior/Anterior
        LL = Left Lateral
        RL = Right Lateral
        RLD = Right Lateral Decubitus
        LLD = Left Lateral Decubitus
        RLO = Right Lateral Oblique
        LLO = Left Lateral Oblique
    */
    QString m_viewPosition;
    
    /**
        Lateralitat de la possiblement aparellada part del cos examinada.
        El trobem als mòduls DX Anatomy (C.8.11.2 ), Mammography Image (C.8.11.7), Intra-oral Image (C.8.11.9) i Ocular Region Imaged (C.8.17.5)
        També el trobem al mòdul Frame Anatomy (C.7.6.16.2.8) comú a tots els enhanced, però el tag s'anomena Frame Laterality en comptes d'Image Laterality.
        Valors definits:
        R = right
        L = left
        U = unpaired
        B = both left and right
    */
    QChar m_imageLaterality;
    
    /**
        Descripció del tipus de vista de la imatge. El seu ús l'aplicarem bàsicament pels casos de mammografia definits a 
        PS 3.16 - Context ID 4014 (cranio-caudal, medio-lateral oblique, etc...) però podríem extendre el seu ús a d'altres tipus d'imatge
        que també fan ús d'aquest tag per guardar aquest tipus d'informació amb altres possibles valors específics.
    */
    QString m_viewCodeMeaning;
    
    /// Número de frame
    int m_frameNumber;
    
    ///Número de fase de la imatge
    int m_phaseNumber;

    ///Número de volum al qual pertany la imatge dins la sèrie
    int m_volumeNumberInSeries;

    ///Número d'ordre de la imatge dins el vo
    int m_orderNumberInVolume;

    /// Moment en el que es va crear el pixel data
    QString m_imageTime;
    
    //\TODO C.7.6.5 CINE MODULE: Multi-frame Cine Image
    /// Atributs NO-DICOM

    /// el path absolut de la imatge
    QString m_path;

    ///Data en que la imatge s'ha descarregat a la base de dades local
    QDate m_retrievedDate;
    QTime m_retrieveTime;

    /// La sèrie pare
    Series *m_parentSeries;

    /// Llista de les imatges relacionades amb l'actual. Estan ordenades amb el mateix ordre que estan al DICOM
    QList<Image *> m_referencedImageSequence;

    /// Cache de la imatge de previsualització
    QPixmap m_thumbnail;
};

}

#endif
