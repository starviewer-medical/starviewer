/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMESOURCEINFORMATION_H
#define UDGVOLUMESOURCEINFORMATION_H

#include <QString>
#include <QStringList>
#include <vector>

namespace udg {

class DICOMTagReader;

/**
Conté informació relacionada amb el volum, com per exemple dades del pacient.

@author Grup de Gràfics de Girona  ( GGG )
*/
class VolumeSourceInformation
{
public:
    VolumeSourceInformation();

    ~VolumeSourceInformation();


    /** Indiquem l'string que defineix la orientació del pacient respecte el seu sistema de coordenades
        Per determinar la dreta o esquerra del pacient farem servir L, per LEFT i R per RIGHT.
        Per determinar la part davantera o del darrera farem servir A, per ANTERIOR , i P, per POSTERIOR.
        Per deteminar amunt i avall farem servir H/S, per HEAD/SUPERIOR, i F/I, per FEET/INFERIOR)

        En un scan típic de cervell tindríem un string LPS, ja que l'eix X avança cap a l'esquerra del pacient, l'eix Y cap al darrera del pacient i les Z en direcció cap al cap del pacient. Així doncs en les respectives visualitzacions tindríem, començant pel cantó esquerra les següents referències i en sentit anti-horari:
        Axial: Left , Anterior , Right , Posterior
        Sagital: Posterior , Inferior , Anterior , Superior
        Coronal: Right , Inferior , Left , Superior

        En un scan tipu "radiografia" frontal ( com si ens fessin una foto de cares ) l'string seria LIP, ja que l'eix X avança cap a l'esquerra del pacient, l'eix Y avança cap als peus (part inferior) del pacient i l'eix Z avança cap a l'esquena del pacient

        Més informació a l'standard DICOM capítols C.7.6.2.1.1 , C.7.6.1.1.1

        (
        especifica els "direction cosines" de la primera fila i de la primera columna respecte al pacient. Aquests atributs vindran en parells. El valor de la fila pels eixos x, y, i z respectivament seguits pels valors de la Columna per els eixos x, y, i z respectivament. La direcció dels eixos està definida completament per l'orientació del pacient.
         L'eix de les X incrementa cap a l'esquerra del pacient.
         L'eix de les Y incrementa cap a la cara posterior del pacient.
         L'eix de les Z incrementa cap al cap del pacient.
      El sistema de coordenades del pacient és un sistema de mà dreta, éa dir, el producte vectorial de un vector unitari a través de l'eix positiu de les X i el vector unitari sobre l'eix positiu de les Y és = al vector unitari sobre l'eix positiu de les Z
      )

    */
    void setPatientOrientationString( QString patientOrientationString )
    {
        m_patientOrientationString = patientOrientationString;
    };

    /// Retorna l'string d'orientació del pacient
    QString getPatientOrientationString( )
    {
        return m_patientOrientationString;
    };

    /// Retorna l'string d'orientació del pacient invertit, és a dir L <=> R , P <=> A , S <=> I
    QString getRevertedPatientOrientationString();

    /// Assigna/Retorna nom del pacient
    void setPatientName( QString patientName )
    {
        m_patientName = patientName;
    };
    QString getPatientName()
    {
        return m_patientName;
    };

    /// Assigna/Retorna l'id del pacient
    void setPatientID( QString patientID )
    {
        m_patientID = patientID;
    };
    QString getPatientID()
    {
        return m_patientID;
    };

    /// Assigna/Retorna nom de la institució
    void setInstitutionName( QString institution )
    {
        m_institution = institution;
    };
    QString getInstitutionName()
    {
        return m_institution;
    };

    /// Assigna/Retorna la data de l'estudi
    void setStudyDate( QString studyDate )
    {
        m_studyDate = studyDate;
    };
    QString getStudyDate()
    {
        return m_studyDate;
    };

    /// Assigna/Retorna l'hora de l'estudi
    void setStudyTime( QString studyTime )
    {
        m_studyTime = studyTime;
    };
    QString getStudyTime()
    {
        return m_studyTime;
    };

    /// Assigna/Retorna l'accession number
    void setAccessionNumber( QString acc )
    {
        m_accessionNumber = acc;
    };
    QString getAccessionNumber()
    {
        return m_accessionNumber;
    };

    /// Assigna/ retorna el window/level que ens proporciona el DICOM
    void setWindowLevel( double window , double level )
    {
        m_windowLevel[0] = window;
        m_windowLevel[1] = level;
    }
    void setWindowLevel( double windowLevel[2] )
    {
        this->setWindowLevel( windowLevel[0] , windowLevel[1] );
    }
    void setWindow( double window )
    {
        m_windowLevel[0] = window;
    }
    void setLevel( double level )
    {
        m_windowLevel[1] = level;
    }
    /// Retorna els valors de window level. Per defecte en retorna el primer, però en poden haver-hi més. Si poition està fora de rang retornarà el primer.
    double getWindow( int position = 0 );
    double getLevel( int position = 0 );
    double *getWindowLevel( int position = 0 );
    void getWindowLevel( double &window , double &level, int position = 0 );
    void getWindowLevel( double windowLevel[2], int position = 0 );

    /// Retorna les descripcions associades als window levels
    QString getWindowLevelDescription( int position = 0 );

    /// Ens diu si té un window level vàlid o no
    bool hasWindowLevel();

    /// Ens retorna en nombre de windows levels que conté la imatge
    int getNumberOfWindowLevels();

    /// Assigna/Retorna el nom del protocol
    void setProtocolName( QString protocol )
    {
        m_protocolName = protocol;
    };
    QString getProtocolName()
    {
        return m_protocolName;
    };

    /// Assigna/Retorna els cosinus dels vectors de direccions dels eixos. Així sabem quina és la orientació en la que s'ha agafat el pacient
    void setDirectionCosines( double directionCosines[9] );
    void setDirectionCosines( double xCosines[3], double yCosines[3], double zCosines[3] );
    void setXDirectionCosines( double xCosines[3] );
    void setXDirectionCosines( double x1Cosines, double x2Cosines, double x3Cosines );
    void setYDirectionCosines( double yCosines[3] );
    void setYDirectionCosines( double y1Cosines, double y2Cosines, double y3Cosines );
    void setZDirectionCosines( double zCosines[3] );
    void setZDirectionCosines( double z1Cosines, double z2Cosines, double z3Cosines );
    void getDirectionCosines( double directionCosines[9] );
    void getDirectionCosines( double xCosines[3], double yCosines[3], double zCosines[3] );
    void getXDirectionCosines( double xCosines[3] );
    void getYDirectionCosines( double yCosines[3] );
    void getZDirectionCosines( double zCosines[3] );

    /// Indiquem la llista de fitxers que formen el volum i es carrega el dicomdataset (privat)
    void setFilenames( QStringList filenames );
    void setFilenames( QString filename ); // ho sobrecarreguem per quan només es tracti d'un sol arxiu

    /// Obtenim la llista d'arxius que conformen el volum
    QStringList getFilenames() const { return m_filenamesList; }

    enum PhotometricInterpretationType{ Monochrome1 , Monochrome2 , PaletteColor, RGB, YBRFull, YBRFull422, YBRPartial422, YBRPartial420, YBRICT, YBRRCT,  Unknown };
    /// Retorna la intepretació fotomètrica d'una imatge d'escala de grisos. Pot ser Monochrome1 (vídeo invers ) o MonocMonochrome2 (normal, el més usual) pel que a nosaltres ens interessa de moment
    unsigned getPhotometricInterpretation();
    QString getPhotometricInterpretationAsString();

    /// Ens diu si la imatge és de tipus MONOCHROME1, fals altrament. NOTA: que no sigui MONOCHROME1 no implica que sigui MONOCHROME2!!
    bool isMonochrome1();

    /// Obtenim els bits que s'han fet servir per allotjar cada pixel i quants per codificar el rang.
    unsigned getBitsStored();
    unsigned getBitsAllocated();

    enum { UnsignedPixelRepresentation , SignedPixelRepresentation };
    /// Ens diu si els pixels són signed o unsigned (0,1)
    unsigned getPixelRepresentation();

    /// Diu si existeixen valors de rescale
    bool hasModalityRescale();

    /// Recupera els valors de rescale slope/intercept
    double getRescaleSlope();
    double getRescaleIntercept();

    /// Assigna/Obté la descripció de la sèrie
    QString getSeriesDescription(){ return m_seriesDescription; };
    void setSeriesDescription( QString description ){ m_seriesDescription = description; };

    /// Assigna/obté l'UID de la sèrie
    QString getSeriesInstanceUID(){ return m_seriesInstanceUID; }
    void setSeriesInstanceUID( QString uid ){ m_seriesInstanceUID = uid; }

    // Mètodes per tractar imatges de tipus multi-frame
    /// Diu si el volum és de tipus multiframe \TODO encara no estem segurs de que es faci aquesta consulta de forma correcta
    bool isMultiFrame();

    /// Ens diu el nombre de frames del que està composat el volum
    int getNumberOfFrames();

    /// Ens retorna el  el SOP Instance UID d'una imatge en el volum
    QString getImageSOPInstanceUID( int index );

    /// Assigna/Retorna el numero de fases (TAG PRIVAT PHILIPS)
    void setNumberOfPhases( int phases )
    {
        m_numberOfPhases = phases;
    };
    int getNumberOfPhases() const
    {
        return m_numberOfPhases;
    };

    /// Assigna/Retorna el numero de llesques (TAG PRIVAT PHILIPS)
    void setNumberOfSlices( int slices )
    {
        m_numberOfSlices = slices;
    };
    int getNumberOfSlices() const
    {
        return m_numberOfSlices;
    };

    /// Assigna/retorna la posició del pacient respecte la màquina. Quan assignem, crea automàticament la cadena d'orientació del pacient que s'usarà per fer les anotacions corresponents.
    void setPatientPosition( QString patientPosition );
    QString getPatientPosition() const
    {
        return m_patientPosition;
    }

private:
    /// Carrega les dades dicom d'un arxiu
    bool loadDicomData( QString filename );

    /// a partir dels direction cosines d'un eix ens dóna l'orientació referent al pacient en string
    QString getOrientation( double vector[3] );
private:

    QString m_patientName;
    QString m_patientID;
    QString m_patientOrientationString;
    QString m_accessionNumber;
    QString m_institution;
    QString m_studyDate;
    QString m_studyTime;
    QString m_protocolName;
    double m_windowLevel[2];
    double m_directionCosines[9];
    QString m_seriesDescription;
    QString m_seriesInstanceUID;
    /// ens indica la posició del pacient respecte la màquina. Útil per les annotacions de referència. Vàlid per CT i MR de moment. Veure C.7.3.1.1.2 per més explicacions
    QString m_patientPosition;

    /// tags privats PHILIPS per controlar nombre de fases i llesques diferents en tot el conjunt
    int m_numberOfPhases, m_numberOfSlices;

    /// Llista dels windows levels de la imatge
    std::vector< double* > m_windowLevelList;

    /// Llista de les descripcions dels windows levels ( si n'hi ha )
    QStringList m_windowLevelDescriptions;

    /// Vector amb els noms dels fitxers que conformen el volum
    QStringList m_filenamesList;

    /// Lector d'informació DICOM
    DICOMTagReader *m_dicomTagReader;
};

};  //  end  namespace udg

#endif
