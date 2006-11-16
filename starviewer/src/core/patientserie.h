/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTSERIE_H
#define UDGPATIENTSERIE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "identifier.h"

namespace udg {

/**
Classe que encapsula la sèrie d'un pacient.

La classe conté tot tipu d'informació relacionada amb la sèrie d'un pacient. Una sèrie equival a un volum per tant tindrem l'identificador del corresponent volum al repositori. Si l'id del volum és nul voldrà dir que no està carregat a memòria, en cas contrari estarà al repositori

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientSerie : public QObject
{
Q_OBJECT
public:
    PatientSerie(QObject *parent = 0);

    ~PatientSerie();

    /// Assignar/Obtenir l'identificador universal de la sèrie
    void setUID( const char *uid );
    QString getUID() const { return m_serieUID; };

    /// Assignar/Obtenir l'identificador de la sèrie
    void setID( const char *id );
    QString getID() const { return m_serieID; };

    /// Assignar/Obtenir l'orientació del pacient
    void setPatientOrientation( const char *orientation );
    QString getPatientOrientation() const { return m_patientOrientation; };

    /// Assignar/Obtenir la modalitat de la sèrie
    void setModality( const char *modality );
    QString getModality() const { return m_modality; };

    /// Assignar/Obtenir la descripció de la sèrie
    void setDescription( const char *description );
    QString getDescription() const { return m_description; };

    /// Assignar/Obtenir el protocol de la sèrie
    void setProtocolName( const char *protocolName );
    QString getProtocolName() const { return m_protocolName; };

    /// Assignar/Obtenir el path de les imatges de la sèrie
    void setSeriesPath( const char *seriesPath );
    QString getSeriesPath() const { return m_seriesPath; };

    /// Retorna el camp clau que identificarà la sèrie de cares a la interfície. Es composarà pel protocolName més la descripció \TODO encara per determinar
    QString getKey();
    
    /// Assignar/Obtenir la data i hora d'adquisició de la sèrie en format DD/MM/AAAA HH:MM. Retorna fals si hi ha algun error en el format
    bool setDateTime( int day , int month , int year , int hour , int minute );
    bool setDateTime( QString date , QString time );
    bool setDateTime( QString dateTime );
    QDateTime getDateTime() const { return m_dateTime; };
    QString getDateTimeAsString();
    bool setDate( int day , int month , int year );
    bool setDate( QString date );
    bool setTime( int hour , int minute );
    bool setTime( QString time );
    QDate getDate();
    QString getDateAsString();
    QTime getTime();
    QString getTimeAsString();

    /// Assignar/Obtenir nivells de contrast
    void setWindowLevel( double window , double level );
    double *getWindowLevel();
    void setWindow( double window );
    double getWindow() const { return m_window; }
    void setLevel( double level );
    double getLevel() const { return m_level; }

    /// Assignar/Obtenir dimensions de la sèrie
    void setDimensions( double dimensions[3] );
    void setDimensions( double x , double y , double z );
    double *getDimensions();
    void getDimensions( double dimensions[3] );
    
    /// Assignar/Obtenir espaiats de la sèrie
    void setSpacing( double spacing[3] );
    void setSpacing( double x , double y , double z );
    double *getSpacing();
    void getSpacing( double spacing[3] );
    
    /// Assignar/Obtenir identificador del volum al repositori corresponent a la sèrie
    void setVolumeIdentifier( Identifier id );
    Identifier getVolumeIdentifier() const { return m_volumeID; }
    
private:
    /// Identidicador universal de la sèrie
    QString m_serieUID;
    
    /// Identificador de la sèrie
    QString m_serieID;

    /// Orientació del pacient ( LR/AP/SI )
    QString m_patientOrientation;

    /// Modalitat d'imatge
    QString m_modality; 

    /// Descripció de la sèrie
    QString m_description;

    /// Protocol que 'sha aplicat per obtenir la imatge
    QString m_protocolName;

    /// Directori sota el qual es trobem les imatges
    QString m_seriesPath;

    /// Data i hora en que s'ha adquirit la sèrie
    QDateTime m_dateTime;
    
    /// Window width i window level del contrast de la imatge
    double m_window;
    double m_level;

    /// Dimensions ( x , y , z [nombre de llesques] )
    double m_dimensions[3];

    /// Espaiat en cada dimensió
    double m_spacing[3];

    /// Identificador del volum al repositori
    Identifier m_volumeID;
//     TransferFunctionList m_transferFunctionList;
};

}

#endif
