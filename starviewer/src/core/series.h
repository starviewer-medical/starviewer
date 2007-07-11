/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSERIES_H
#define UDGSERIES_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QImage>
#include "identifier.h"

namespace udg {

/**
Classe que encapsula la sèrie d'un pacient.

La classe conté tot tipu d'informació relacionada amb la sèrie d'un pacient. Una sèrie pot equivaler a un o més volums, per tant tindrem la llista de Volums corresponents a la sèrie.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Volume;

class Series : public QObject
{
Q_OBJECT
public:
    Series(QObject *parent = 0);

    ~Series();

    /// Assignar/Obtenir l'identificador universal de la sèrie
    void setUID( QString uid );
    QString getUID() const { return m_seriesUID; };

    /// Assignar/Obtenir l'identificador de la sèrie
    void setID( QString id );
    QString getID() const { return m_seriesID; };

    /// Assignar/Obtenir l'orientació del pacient
    void setPatientOrientation( QString orientation );
    QString getPatientOrientation() const { return m_patientOrientation; };

    /// Assignar/Obtenir la modalitat de la sèrie
    void setModality( QString modality );
    QString getModality() const { return m_modality; };

    /// Assignar/Obtenir la descripció de la sèrie
    void setDescription( QString description );
    QString getDescription() const { return m_description; };

    /// Assignar/Obtenir el protocol de la sèrie
    void setProtocolName( QString protocolName );
    QString getProtocolName() const { return m_protocolName; };

    /// Assignar/Obtenir el path de les imatges de la sèrie
    void setSeriessPath( QString seriessPath );
    QString getSeriessPath() const { return m_seriessPath; };

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

    /// Assignar/Obtenir nivells de contrast \TODO això hauria de ser a nivell de subvolum o a nivell d'imatge però no pas a nivell de series
    void setWindowLevel( double window , double level );
    double *getWindowLevel();
    void setWindow( double window );
    double getWindow() const { return m_window; }
    void setLevel( double level );
    double getLevel() const { return m_level; }

    /// Assignar/Obtenir dimensions de la sèrie \TODO això hauria de ser a nivell de subvolum o a nivell d'imatge però no pas a nivell de series
    void setDimensions( double dimensions[3] );
    void setDimensions( double x , double y , double z );
    double *getDimensions();
    void getDimensions( double dimensions[3] );

    /// Assignar/Obtenir espaiats de la sèrie \TODO això hauria de ser a nivell de subvolum o a nivell d'imatge però no pas a nivell de series
    void setSpacing( double spacing[3] );
    void setSpacing( double x , double y , double z );
    double *getSpacing();
    void getSpacing( double spacing[3] );

    /// Assignar/Obtenir identificador del volum al repositori corresponent a la sèrie \TODO estem assumint que un volum = una sèrie i això no és del tot cert. L'id, en tot cas, hauria d'anar relacionat amb el subvolum
    void setVolumeIdentifier( Identifier id );
    Identifier getVolumeIdentifier() const { return m_volumeID; }

    /// Retorna el nombre de volums dels que es composa la sèrie.
    int getNumberOfVolumes();

    /// Retorna el subvolum amb índex 'index', per defecte, el 0
    Volume *getVolume( int index = 0 );

private:
    /// Identidicador universal de la sèrie
    QString m_seriesUID;

    /// Identificador de la sèrie
    QString m_seriesID;

    /// Orientació del pacient ( LR/AP/SI )
    QString m_patientOrientation;

    /// Modalitat d'imatge
    QString m_modality;

    /// Descripció de la sèrie
    QString m_description;

    /// Protocol que 'sha aplicat per obtenir la imatge
    QString m_protocolName;

    /// Directori sota el qual es trobem les imatges
    QString m_seriessPath;

    /// Data i hora en que s'ha adquirit la sèrie
    QDateTime m_dateTime;

    /// Imatge de previsualització associada a la sèrie
    QImage m_previewImage;

    /// Window width i window level del contrast de la imatge \TODO podem tenir més d'un en una sèrie per tant això tendirà a quedar obsolet en aquest nivell
    double m_window;
    double m_level;

    /// Dimensions ( x , y , z [nombre de llesques] )
    double m_dimensions[3];

    /// Espaiat en cada dimensió
    double m_spacing[3];

    /// Identificador del volum al repositori
    Identifier m_volumeID;
//     TransferFunctionList m_transferFunctionList;

    /// Llista de volums que composen la sèrie. La sèrie es pot separar en diversos volums per diverses raons, com pot ser mides d'imatge diferent, sèries amb dinàmics o fases, stacks, etc.
    QList<Volume *> m_volumesList;
};

}

#endif
