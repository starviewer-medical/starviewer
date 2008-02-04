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
    
    ///assigna / retorna el KiloVoltatge de la imatge
    void setKiloVoltagePeak( double kvp );
    double getKiloVoltagePeak();
    
    ///assigna / retorna el microAmperatge de la imatge
    void setMicroAmpersSecond( double uas );
    double getMicroAmpersSecond();
    
    ///assigna / retorna el miliAperatge de la imatge
    void setMilliAmpersSecond( double mas );
    double getMilliAmpersSecond();
    
    /// assigna / retorna el RepetitionTime de la imatge
    void setRepetitionTime( QString repetitionTime );
    QString getRepetitionTime();

    ///assigna / retorna el EchoTime de la imatge
    void setEchoTime( QString echoTime );
    QString getEchoTime();

    ///assigna / retorna el InversionTime de la imatge
    void setInversionTime( QString inversionTime );
    QString getInversionTime();

    ///assigna / retorna l'espai entre llesques de la imatge
    void setSpacingBetweenSlices( QString spacingBetweenSlices );
    QString getSpacingBetweenSlices();

    ///assigna / retorna el flip angle de la imatge
    void setFlipAngle( QString philipsFlipAngle );
    QString getFlipAngle();

    ///assigna / retorna el factor turbo de la imatge
    void setPhilipsTurboFactor( QString philipsTurboFactor );
    QString getPhilipsTurboFactor();
    
    ///assigna / retorna el factor Epi de la imatge
    void setPhilipsEPIFactor( QString philipsEPIFactor );
    QString getPhilipsEPIFactor();

    ///assigna / retorna la posició del pla de la imatge a l'espai
    void setPhilipsSpacialPlane( QString philipsSpacialPlane );
    QString getPhilipsSpacialPlane();

    ///assigna / retorna el Number Of Averages de la imatge
    void setNumberOfAverages( QString numberOfAverages );
    QString getNumberOfAverages();

    ///assigna / retorna ratio del field of view de la imatge
    void setPercentPhaseFieldOfView( QString percentPhaseFieldOfView );
    QString getPercentPhaseFieldOfView();

    ///assigna / retorna el nom de la bobina utiltizada per la imatge
    void setReceiveCoilName( QString receiveCoilName );
    QString getReceiveCoilName();

    ///assigna / retorna el factor b de la imatge
    void setPhilipsBFactor( QString philipsBFactor );
    QString getPhilipsBFactor();

    ///assigna / retorna el reconstruction diameter de la imatge
    void setReconstructionDiameter( QString reconstructionDiameter );
    QString getReconstructionDiameter();

    ///assigna / retorna el exposure time de la imatge
    void setExposureTime( QString exposureTime );
    QString getExposureTime();

    ///assigna / retorna el scan length de la imatge
    void setPhilipsScanLength( QString philipsScanLength );
    QString getPhilipsScanLength();

    ///assigna / retorna el table height de la imatge
    void setTableHeight( QString tableHeight );
    QString getTableHeight();

    ///assigna / retorna el slice location de la imatge
    void setSliceLocation( QString sliceLocation );
    QString getSliceLocation();

    ///assigna / retorna el reconstruction diameter de la imatge
    void setPhilipsViewConvention( QString philipsViewConvention );
    QString getPhilipsViewConvention();

    ///assigna / retorna el filter type de la imatge
    void setFilterType( QString filterType );
    QString getFilterType();

    ///assigna / retorna el image type de la imatge
    void setImageType( QString ImageType );
    QString getImageType();

    ///assigna / retorna el table speed de la imatge
    void setPhilipsTableSpeed( QString philipsTableSpeed );
    QString getPhilipsTableSpeed();

    ///assigna / retorna el collimation de la imatge
    void setPhilipsCollimation( QString philipsCollimation );
    QString getPhilipsCollimation();

    ///assigna / retorna scan arc de la imatge
    void setScanArc( QString scanArc );
    QString getScanArc();

    ///assigna / retorna el tilt de la imatge
    void setTilt( QString tilt );
    QString getTilt();

    ///assigna / retorna el cycle time de la imatge
    void setPhilipsCycleTime( QString philipsXycleTime );
    QString getPhilipsCycleTime();

    ///assigna / retorna el pitch de la imatge
    void setPhilipsPitch( QString philipsPitch );
    QString getPhilipsPitch();

    ///assigna / retorna el rotation time de la imatge
    void setPhilipsRotationTime( QString philipsRotationTime );
    QString getPhilipsRotationTime();

    ///assigna / retorna el table increment de la imatge
    void setPhilipsTableIncrement( QString tableIncrement );
    QString getPhilipsTableIncrement();

    ///assigna / retorna el scan time de la imatge
    void setPhilipsScanTime( QString philipsScanTime );
    QString getPhilipsScanTime();

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
    
    /** Kilo voltatge de sortida del generador de raigs-X. (0018,0060) 
        El tipus depèn de la modalitat:
        
        CR IMAGE->tipus 3
        CT IMAGE->tipus 2
        X-RAY->   tipus 2
        X-RAY SEQUENCE->tipus 1C: condició: requerit si el tipus de frame té valor 1.
        X-RAY FRAME-> tipus 1.
        XA/XRF->tipus 1.
    */
    double m_KiloVoltagePeak;
    
    /** Exposició als raigs-X expressada en micro ampers per segon. (0018,1153) 
        El tipus depèn de la modalitat:
        
        CR IMAGE->tipus 3
        CT IMAGE->tipus 2
        X-RAY->   tipus 3
        X-RAY SEQUENCE->no en té.
        X-RAY FRAME->no en té.
    */
    double m_microAmpersSecond;
    
     /** Exposició als raigs-X expressada en mi·liampers per segon. (0018,1153) 
        El tipus depèn de la modalitat:
        
        XA/XRF->tipus 1C: condició: requerit si el temps d'exposició en ms (0018,9328) o el corrent del tub de raigs-X en mA no estan presents.
                                    Altrament també pot ser-hi.
    */
    double m_milliAmpersSecond;
    
    /** Temps en ms entre cada puls en MR (0018,0080) Tipus 2
     */
    QString m_repetitionTime;

    /** Temps d'echo en ms en MR (0018,0081) Tipus 2.
     */
    QString m_echoTime;

    /** Temps en ms entre la meitat del puls RF invers i la la meita del puls d'exitació (0018,0082)
        Tipus 2C , només apareix quan ImageSequence (0018,0020) = IR (Inversion Recovery)
     */
    QString m_inversionTime;

    /** Espai en mm entre les llesques (0018,0088)
        MR IMAGE-> tipus 3
        NM IMAGE-> tipus 2 
    */
    QString m_spacingBetweenSlices;

    /** diferencia de graus entre el vector magnètic i el vector magnètic del camp primari per MR (0018,1314). Tipus 3 
     */
    QString m_flipAngle;
    
    //TODO Esbrinar significat camp
    /** Per MR  (0018,0083)
     */
    QString m_numberOfAverages;

    /** Ratio del field of view expressat en percentatge per MR (0018,0094). Tipus 3
      */
    QString m_percentPhaseFieldOfView;

    /** Nom de bobina utilitzada en MR (0018,1250). Tipus 3
      */
    QString m_receiveCoilName;
    
    /** Diametre en mm de la regió de la qual s'han utilitzat dades per crear la reconstrucció del a imatge (0018,1100)
        CT Image->tipus 3
        MR Image->tipus 3
        PET Imate->tipus 3
     */
    QString m_reconstructionDiameter;
    
    /** Temps d'exposició als raigs X en ms. (0018,1150)
        CT-> tipus 3
        NM-> tipus 3
        CR-> tipus 3
        X-ray -> tipus 2C
        DX-> tipus 3
     */
    QString m_exposureTime;

    /** Distància en mm del capdamunt de la taula de pacient fins al centre de rotació. (0018,1130)
        CT->tipus 3
        NM->tipus 3
        X-ray->tipus 3
     */
    QString m_tableHeight;

    //TODO millorar definició
    /** Situació especial de la llesca en mm. (0020,1041)
        SC->tipus 3
        NM->tipus 3
        CT-> A la documentació dicom aquest camp no hi figura però philips l'utiliza com a Table Position
      */
    QString m_sliceLocation;

    /** Nom del filtre utilitzat en el raig X. (0018,1160)
       CT->tipus 3
       CR->tipus 3
       X-Ray->tipus 3
      */
    QString m_filterType;
  
    /** Indentificador de les característiques de la imatge. (0008,0008)
     * Obligatoria per la majoria de modalitats en tipus 1 (CT,MR,DX,VL,US,...)
     */  
    QString m_imageType;

    /** Angle d'escaneig de les dades. (0018,1143)
     */
    QString m_scanArc;

    /** Angle d'inclinació del detector (0018,1120)
        CT->Tipus 3
        NM->Tipus 3
     */
    QString m_tilt;

    /** Factor turbo per MR. Tag de philips (2001,1082)
     */
    QString m_philipsTurboFactor;

    /** Factor EPI per MR. Tag de philips (2001,1013)
     */
    QString m_philipsEPIFactor;

    //TODO Esbrinar si el nom del camp correcte
    /** Posició de espaial del pla de la imatge per MR. Tag de philips (2001,100b)
     */
    QString m_philipsSpacialPlane;

    /** Factor b corresponent a una imatge per MR. Tag de Philips (2001,1003)
     */
    QString m_philipsBFactor;

    //TODO preguntar significat signifcat camp
    /** Tag de Philips per CT. (01f1,1008)
     */
    QString m_philipsScanLength;

    //TODO demanar definició
    /** Tag CT Philips S'utilitza per Surveys, Helicoïdals i no Helicoïdals (01f1,1032)
      */
    QString m_philipsViewConvention;

    //TODO buscar significat tag
    /** Tag de Philips per CT helicoïdal. (01f1,1026)
     */
    QString m_philipsPitch;
 
    //TODO buscar significat tag
    /** Tag de Philips per CT helicoïdal i no helicoïdal. (01f1,1027)
     */
    QString m_philipsRotationTime;

    //TODO buscar significat tag
    /** Tag de Philips per CT helicoïdal. (01f1,1007)
     */
    QString m_philipsTableSpeed;

    //TODO buscar significat tag
    /** Tag de philips per CT helicoïdal i no helicoïdal. (01f1,104b)
     */
    QString m_philipsCollimation;

    //TODO buscar significat tag
    /** Tag de philips per CT no helicoïdal. (01f1,1033)
     */
    QString m_philipsCycleTime;

    //TODO buscar significat tag
    /** Tag de philips per CT no helicoïdal. (01f1,1028)
     */
    QString m_philipsTableIncrement;

    //TODO buscar significat tag
    /** Tag de philips per CT no helicoïdal. (00e1,1050)
     */
    QString m_philipsScanTime;

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

    /// Cache de la imatge de previsualització
    QImage m_thumbnail;
};

}

#endif
