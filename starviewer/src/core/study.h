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
#include "series.h"

namespace udg {

class Patient;

/**
Classe que encapsula l'estudi d'un pacient

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Study : public QObject
{
Q_OBJECT
public:
    Study( Patient *parentPatient = 0, QObject *parent = 0);

    ~Study();

    /// Assigna/Obtè l'objecte Patient pare de l'estudi
    void setParentPatient( Patient *patient );
    Patient *getParentPatient() const;

    /// Assignar/Obtenir l'identificador universal de l'estudi
    void setInstanceUID( QString uid );
    QString getInstanceUID() const;

    /// Assignar/Obtenir l'identificador de l'estudi
    void setID( QString id );
    QString getID() const;

    /// Assignar/Obtenir l'accession number de l'estudi
    void setAccessionNumber( QString accessionNumber );
    QString getAccessionNumber() const;

    /// Assignar/Obtenir la descripció de l'estudi
    void setDescription( QString description );
    QString getDescription() const;

    /// Assignar/Obtenir el pes del pacient
    void setPatientAge( short int age );
    short int getPatientAge() const;

    /// Assignar/Obtenir el pes del pacient
    void setWeight( double weight );
    double getWeight() const;

    /// Assignar/Obtenir l'aclçada del pacient
    void setHeight( double height );
    double getHeight() const;

    /// Assignar/Obtenir la data i hora d'adquisició de l'estudi. El format de la data serà YYYYMMDD i el del
    /// time hhmmss.frac on frac és una fracció de segon de rang 000000-999999
    ///  Retorna fals si hi ha algun error en el format
    bool setDateTime( int day , int month , int year , int hour , int minute, int second = 0 );
    bool setDateTime( QString date , QString time );
    bool setDate( int day , int month , int year );
    bool setDate( QString date );
    bool setDate( QDate date );
    bool setTime( int hour , int minute, int second = 0 );
    bool setTime( QString time );
    bool setTime( QTime time );
    QDate getDate();
    QString getDateAsString();
    QTime getTime();
    QString getTimeAsString();
    QDateTime getDateTime();

    /// Afegeix una nova sèrie i li assigna com a parent aquest objecte study. Retorna fals si existeix una sèrie amb el mateix uid
    bool addSeries( Series *series );

    /// Li treu a l'estudi la sèrie amb l'UID donat. Si no existeix cap amb aquest uid retorna nul.
    void removeSeries( QString uid );

    /// Obté la sèrie amb l'UID donat. NUL si no hi és
    Series *getSeries( QString uid );

    /// retorna una llista de les sèries marcades com a seleccionades
    QList<Series *> getSelectedSeries();

    /// Mètode per obtenir el nombre de series que conté l'estudi
    int getNumberOfSeries();

    /// Mètode per obtenir la llista de series de l'estudi
    QList<Series *> getSeries();

    QString toString();

private:
    /// Informació comuna de l'estudi. C.7.2.1 General Study Module - PS 3.3.

    /// Identificador únic de l'estudi. (0020,000D) Tipus 1.
    QString m_studyInstanceUID;

    /// Data i hora en que l'estudi va començar. (0008,0020),(0008,0030) Tipus 2.
    QDate m_date;
    QTime m_time;

    /// Identificador de l'estudi generat per l'equipament o per l'usuari. (0020,0010) Tipus 2.
    QString m_studyID;

    /// Accession Number. A RIS generated number that identifies the order for the Study. (0008,0050) Tipus 2.
    QString m_accessionNumber;

    /// Descripció de l'estudi generada per la institució. (0008,1030) Tipus 3
    QString m_description;

    /// Informació del pacient, relacionada únicament amb aquest estudi. Aquesta és la informació que podem tenir d'un pacient en el moment en el que se li va fer l'estudi. C.7.2.2 Patient Study Module - PS 3.3.

    /// edad del pacient. (0010,1010) Tipus 3
    unsigned short int m_age;

    /// Alçada ( en metres ) del pacient (0010,1020) Tipus 3
    double m_height;

    /// Pes ( en Kg. ) del pacient (0010,1030) Tipus 3
    double m_weight;

    /// Taula de Hash que conté les sèries de l'estudi
    QHash< QString , Series* > m_seriesSet;

    /// L'entitat Patient a la qual pertany aquest estudi
    Patient *m_parentPatient;
};

}

#endif
