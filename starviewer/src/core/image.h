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
#include <QImage>
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
     * @return Els valors podran ser AXIAL, SAGITAL, CORONAL o OBLIQUE
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

    /// Assigna/Obté el nombre de frames de la imatge
    void setNumberOfFrames( int frames );
    int getNumberOfFrames() const;

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

    /**
     * Mètode encarregat de fer el thumbnail de la imatge. Es retorna a partir d'un QImage de manera que es pugui generar en
     * threads que no siguin el del GUI.
     * \TODO Aquest mètode hauria de desaparèixer, ara es manté ja que es necessita per crear les previsualitzacions del queryscreen.
     * @return Un QImage amb la imatge del thumbnail
     */
    QImage createThumbnail(int resolution = 128);

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
    QImage m_thumbnail;
};

}

#endif
