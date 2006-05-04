/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTSTUDY_H
#define UDGPATIENTSTUDY_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <list>
#include "patientserie.h"

namespace udg {

/**
Classe que encapsula l'estudi d'un pacient

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientStudy : public QObject
{
Q_OBJECT
public:
    PatientStudy(QObject *parent = 0);

    ~PatientStudy();

    /// Assignar/Obtenir l'identificador universal de l'estudi
    void setUID( const char *uid );
    QString getUID() const { return m_studyUID; };

    /// Assignar/Obtenir l'identificador de l'estudi
    void setID( const char *id );
    QString getID() const { return m_studyID; };

    /// Assignar/Obtenir l'accession number de l'estudi
    void setAccessionNumber( const char *accessionNumber );
    QString getAccesssionNumber() const { return m_accessionNumber; };

    /// Assignar/Obtenir la descripció de l'estudi
    void setDescription( const char *description );
    QString getDescription() const { return m_description; };

    /// Assignar/Obtenir la institució on s'ha realitzat l'estudi
    void setInstitutionName( const char *institutionName );
    QString getInstitutionName() const { return m_institutionName; };

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

    /// Llista de sèries \TODO això podria ser un map o un hash, per determinar
    std::list< PatientSerie > m_seriesList;
};

}

#endif
