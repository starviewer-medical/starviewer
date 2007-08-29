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

    /// Assignar/Obtenir la data i hora d'adquisició de la imatge. El format de la data serà YYYYMMDD i el del
    /// time hhmmss.frac on frac és una fracció de segon de rang 000000-999999
    ///  Retorna fals si hi ha algun error en el format
    bool setContentDateTime(int day, int month, int year, int hour, int minute, int second = 0 );
    bool setContentDateTime(QString date, QString time);
    bool setContentDate(int day, int month, int year);
    bool setContentDate(QString date);
    bool setContentDate(QDate date);
    bool setContentTime(int hour, int minute, int second = 0);
    bool setContentTime(QString time);
    bool setContentTime(QTime time);
    QDate getContentDate() const;
    QString getContentDateAsString();
    QTime getContentTime() const;
    QString getContentTimeAsString();

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
    void setPhotometricInterpretation( int value );
    int getPhotometricInterpretation() const;

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

    /// Li indiquem quina és la sèrie pare a la qual pertany
    void setParentSeries( Series *series );
    Series *getParentSeries() const;

    /// assigna/retorna el path absolut de la imatge
    void setPath( QString path );
    QString getPath() const;

    /// afegeix un objecte imatge a la imatge
    void addReferencedImage( Image *image );

    /// obté l'objecte imatge pel sopInstanceUID donat. Si no existeix cap imatge amb aquest UID es retorna NUL
    Image *getReferencedImage( QString SOPInstanceUID );

    /// Retorna una llista de totes les imatges relacionades amb la imatge
    QList<Image *> getReferencedImages();

    /// Indica si una imatge té imatges relacionades
    bool hasReferencedImages() const;

    /// Ens indica si aquesta imatge és un localitzador sempre que la modalitat sigui CT
    void setCTLocalizer( bool localizer );
    bool isCTLocalizer() const;

    /**
     * El mètode ens retorna el thumbnail de la imatge. Es crearà el primer cop que es demani
     * @param resolution La resolució amb la que volem el thumbnail
     * @return Un QPixmap amb el thumnail
     */
    QPixmap getThumbnail( int resolution = 100 );

private:

    /**
     * Mètode encarregat de fer el thumbnail de la imatge.
     * @return Un QPixmap amb la imatge
     */
    void createThumbnail( int resolution );

private:
    /// Atributs DICOM

    /// identificador de la imatge/arxiu. (0008,0018)
    QString m_SOPInstanceUID;

    /// Informació general de la imatge. C.7.6 General Image Module - PS 3.3.

    /// Nombre que identifica la imatge. (0020,0013) Tipus 2
    QString m_instanceNumber;

    /// Direcció de les files i columnes de la imatge ( LR/AP/SI ). Requerit si la imatge no requereix Image Orientation(Patient)(0020,0037) i Image Position(Patient)(0020,0032). Veure C.6.7.1.1.1. (0020,0020) Tipus 2C.
    QString m_patientOrientation;

    /// La data i hora en que la imatge es va començar a generar. Requerit si la imatge és part d'una sèrie en que les imatges estan temporalment relacionades. (0008,0023)(0008,0033) Tipus 2C
    QDate m_contentDate;
    QTime m_contentTime;

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

    /// situació espaial de la llesca. (0020,1041) Tipus 3
    double m_sliceLocation;

    // Image Pixel Module C.6.7.3
    /// Nombre de mostres per pixel en la imatge. Veure C.6.7.3.1.1. (0028,0002) Tipus 1.
    int m_samplesPerPixel;

    /// interpretació fotomètrica (monocrom,color...). Veure C.6.7.3.1.2. (0028,0004) Tipus 1.
    int m_photometricInterpretation;

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

    //\TODO C.7.6.5 CINE MODULE: Multi-frame Cine Image
    /// Atributs NO-DICOM

    /// el path absolut de la imatge
    QString m_path;

    /// La sèrie pare
    Series *m_parentSeries;

    /// Llista de les imatges relacionades amb l'actual. Estan ordenades amb el mateix ordre que estan al DICOM
    QList<Image *> m_referencedImageSequence;

    /// Ens indica si aquesta imatge és un localitzador sempre que la modalitat sigui CT
    bool m_CTLocalizer;

    /// Imatge de previsualització
    QPixmap m_thumbnail;
};

}

#endif
