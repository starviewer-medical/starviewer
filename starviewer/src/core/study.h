/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTUDY_H
#define UDGSTUDY_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QHash>
#include "serie.h"

namespace udg {

/**
Classe que encapsula l'estudi d'un pacient

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Patient;

class Study : public QObject
{
Q_OBJECT
public:
    Study( Patient *parentPatient, QObject *parent = 0);

    ~Study();

    /// Assignar/Obtenir l'identificador universal de l'estudi
    void setUID( QString uid );
    QString getUID() const { return m_studyUID; };

    /// Assignar/Obtenir l'identificador de l'estudi
    void setID( QString id );
    QString getID() const { return m_studyID; };

    /// Assignar/Obtenir l'accession number de l'estudi
    void setAccessionNumber( QString accessionNumber );
    QString getAccesssionNumber() const { return m_accessionNumber; };

    /// Assignar/Obtenir la descripció de l'estudi
    void setDescription( QString description );
    QString getDescription() const { return m_description; };

    /// Assignar/Obtenir la institució on s'ha realitzat l'estudi
    void setInstitutionName( QString institutionName );
    QString getInstitutionName() const { return m_institutionName; };

    /// Retorna el camp clau que identificarà l'estudi de cares a la interfície. Es composarà per la data més la descripció \TODO encara per determinar
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

    /// Afegeix una nova sèrie
    void addSerie( Serie *serie );

    /// Li treu a l'estudi la sèrie amb la clau 'key'
    void removeSerie( QString key );

    /// Obté la sèrie amb clau 'key'
    Serie *getSerie( QString key );

private:
    /// Identificador Universal de l'estudi
    QString m_studyUID;

    /// Identificador de l'estudi
    QString m_studyID;

    /// Accession Number
    QString m_accessionNumber;

    /// Descripció de l'estudi
    QString m_description;

    /// Nom de l'institució en la que s'ha fet
    QString m_institutionName;

    /// Data i hora de l'adquisició de l'estudi
    QDateTime m_dateTime;

    /// Taula de Hash que conté les sèries de l'estudi
    typedef QHash< QString , Serie* > SeriesHashType;
    SeriesHashType m_seriesHash;

    /// L'entitat Patient a la qual pertany aquest estudi
    Patient *m_parentPatient;
};

}

#endif
