/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMESOURCEINFORMATION_H
#define UDGVOLUMESOURCEINFORMATION_H

#include <QString>

namespace udg {

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
    void setPatientOrientationString( const char* patientOrientationString )
    {
        m_patientOrientationString = QString::fromLatin1( patientOrientationString );
    };

    /// Retorna l'string d'orientació del pacient
    QString getPatientOrientationString( )
    {
        return m_patientOrientationString;
    };

    /// Retorna l'string d'orientació del pacient invertit, és a dir L <=> R , P <=> A , S <=> I
    QString getRevertedPatientOrientationString();
    
    /// Assigna/Retorna nom del pacient
    void setPatientName( const char * patientName )
    {
        m_patientName = QString::fromLatin1( patientName );
    };
    QString getPatientName()
    {
        return m_patientName;
    };

    /// Assigna/Retorna l'id del pacient
    void setPatientID( const char * patientID )
    {
        m_patientID = QString::fromLatin1( patientID );
    };
    QString getPatientID()
    {
        return m_patientID;
    };

    /// Assigna/Retorna nom de la institució
    void setInstitutionName( const char * institution )
    {
        m_institution = QString::fromLatin1( institution );
    };
    QString getInstitutionName()
    {
        return m_institution;
    };

    /// Assigna/Retorna la data de l'estudi
    void setStudyDate( const char * studyDate )
    {
        m_studyDate = QString::fromLatin1( studyDate );
    };
    QString getStudyDate()
    {
        return m_studyDate;
    };

    /// Assigna/Retorna l'hora de l'estudi
    void setStudyTime( const char * studyTime )
    {
        m_studyTime = QString::fromLatin1( studyTime );
    };
    QString getStudyTime()
    {
        return m_studyTime;
    };

    /// Assigna/Retorna l'accession number
    void setAccessionNumber( const char * acc )
    {
        m_accessionNumber = QString::fromLatin1( acc );
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
    double getWindow()
    {
        return m_windowLevel[0];
    }
    double getLevel()
    {
        return m_windowLevel[1];
    }
    double *getWindowLevel()
    {
        return m_windowLevel;
    }
    void getWindowLevel( double &window , double &level )
    {
        window = m_windowLevel[0];
        level = m_windowLevel[1];
    }
    void getWindowLevel( double windowLevel[2] )
    {
        windowLevel[0] = m_windowLevel[0];
        windowLevel[1] = m_windowLevel[1];
    }

    /// Assigna/Retorna el nom del protocol
    void setProtocolName( const char * protocol )
    {
        m_protocolName = QString::fromLatin1( protocol );
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

};

};  //  end  namespace udg 

#endif
