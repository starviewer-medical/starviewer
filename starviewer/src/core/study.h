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

    /// Assignar/Obtenir l'identificador universal de l'estudi
    void setInstanceUID( QString uid );
    QString getInstanceUID() const { return m_studyInstanceUID; };

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

    /// Assignar/Obtenir el pes del pacient
    void setWeight( double weight ){ m_weight = weight; };
    double getWeight() const { return m_weight; };

    /// Assignar/Obtenir l'aclçada del pacient
    void setHeight( double height ){ m_height = height; };
    double getHeight() const { return m_height; };

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
    void addSeries( Series *series );

    /// Li treu a l'estudi la sèrie amb l'UID donat.
    void removeSeries( QString uid );

    /// Obté la sèrie amb l'UID donat. NUL si no hi és
    Series *getSeries( QString uid );

    /// retorna una llista de les sèries marcades com a seleccionades
    QList<Series *> getSelectedSeries();

    /// Retorna la serie indexada per 0..n-1 (n=total de series). Retorna nul si l'índex està fora de rang \TODO ara mateix agafa a saco del hash, no es té ne compte cap criteri en l'ordre i l'índex
    Series *getSeries( int index );

private:
    /// Informació comuna de l'estudi. C.7.2.1 General Study Module - PS 3.3.

    /// Identificador únic de l'estudi. (0020,000D) Tipus 1.
    QString m_studyInstanceUID;

    /// Data i hora en que l'estudi va començar. (0008,0020),(0008,0030) Tipus 2. \TODO mantenir aquesta info junta o separada?
    QDateTime m_dateTime;

    /// Identificador de l'estudi generat per l'equipament o per l'usuari. (0020,0010) Tipus 2.
    QString m_studyID;

    /// Accession Number. A RIS generated number that identifies the order for the Study. (0008,0050) Tipus 2.
    QString m_accessionNumber;

    /// Descripció de l'estudi generada per la institució. (0008,1030) Tipus 3
    QString m_description;

    //\TODO aquest tag està dins de l'Equipment IE module. No se sap si es mes correcte posar-ho a nivell de sèrie. Llegir A.1.2.3 SERIES IE, apartat c., A.1.2.4 EQUIPMENT IE i C.7.5 Common Equipment IE Modules - PS 3.3
    /// Nom de l'institució en la que s'ha fet
    QString m_institutionName;

    /// Informació del pacient, relacionada únicament amb aquest estudi. Aquesta és la informació que podem tenir d'un pacient en el moment en el que se li va fer l'estudi. C.7.2.2 Patient Study Module - PS 3.3.

    /// edad del pacient. (0010,1010) Tipus 3
    unsigned short int m_age;

    /// Alçada ( en metres ) del pacient (0010,1020) Tipus 3
    double m_height;

    /// Pes ( en Kg. ) del pacient (0010,1030) Tipus 3
    double m_weight;

    /// Taula de Hash que conté les sèries de l'estudi
    typedef QHash< QString , Series* > SeriesHashType;
    SeriesHashType m_seriesHash;

    /// L'entitat Patient a la qual pertany aquest estudi
    Patient *m_parentPatient;
};

}

#endif
