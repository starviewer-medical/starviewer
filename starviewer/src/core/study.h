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

    /// Assignar/Obtenir l'edat del pacient
    void setPatientAge( const QString &age );
    QString getPatientAge() const;

    /// Assignar/Obtenir el pes del pacient
    void setWeight( double weight );
    double getWeight() const;

    /// Assignar/Obtenir l'aclçada del pacient
    void setHeight( double height );
    double getHeight() const;

    /**
     * Retorna les modalitats que conté l'estudi.
     * @return la llista de modalitats en un string delimitades pel caràcter '/'
     */
    QString getModalitiesAsSingleString() const;

    /**
     * Retorna les modalitats que conté l'estudi. 
     * @return una llista d'strings
     */
    QStringList getModalities() const;

    ///Assigna/Obtenir el referring physician's name de l'estudi
    void setReferringPhysiciansName(QString referringPhysiciansName);
    QString getReferringPhysiciansName();

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

    ///Assignar/Obtenir la data i hora en que l'estudi s'ha descarregat a la base de dades Local
    void setRetrievedDate(QDate date);
    void setRetrievedTime(QTime time);
    QDate getRetrievedDate();
    QTime getRetrievedTime();

    /// Afegeix una nova sèrie i li assigna com a parent aquest objecte study. Retorna fals si existeix una sèrie amb el mateix uid
    bool addSeries( Series *series );

    /// Li treu a l'estudi la sèrie amb l'UID donat. Si no existeix cap amb aquest uid retorna nul.
    void removeSeries( QString uid );

    /// Obté la sèrie amb l'UID donat. NUL si no hi és
    Series *getSeries( QString uid );

    /**
     * Ens diu si existeix una serie amb aquest uid a la llista
     * @param uid l'uid que busquem
     * @return Cert si existeix, fals altrament
     */
    bool seriesExists( QString uid );

    /// retorna una llista de les sèries marcades com a seleccionades
    QList<Series *> getSelectedSeries();

    /// Mètode per obtenir el nombre de series que conté l'estudi
    int getNumberOfSeries();

    /// Mètode per obtenir la llista de series de l'estudi
    QList<Series *> getSeries();

    QString toString();

private:
    /**
     * Inserta una serie a la llista de series ordenat per SeriesNumber.
     * Pre: se presuposa que s'ha comprovat anteriorment que la serie no existeix a la llista
     * @param serie
     */
    void insertSeries( Series *series );

    /**
     * Troba l'índex de la serie amb l'uid donat a la llista de series
     * @param uid L'uid de la serie que volem trobar
     * @return L'índex d'aquella serie dins de la llista, -1 si no existeix la serie amb aquell uid.
     */
    int findSeriesIndex( QString uid );

private:
    /// Informació comuna de l'estudi. C.7.2.1 General Study Module - PS 3.3.

    /// Identificador únic de l'estudi. (0020,000D) Tipus 1.
    QString m_studyInstanceUID;

    /// Llista de les modalitats de l'estudi
    QStringList m_modalities;
    
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
    QString m_age;

    /// Alçada ( en metres ) del pacient (0010,1020) Tipus 3
    double m_height;

    /// Pes ( en Kg. ) del pacient (0010,1030) Tipus 3
    double m_weight;

    ///Metge que atén el pacient durant visita, relacionada amb la prova exploratòria (0008,0090) Tipus 2
    QString m_referringPhysiciansName;

    ///Data en que l'estudi s'ha descarregat a la base de dades local
    QDate m_retrievedDate;
    QTime m_retrieveTime;

    /// Llista de les Series de l'estudi ordenades per número de serie
    QList<Series *> m_seriesSet;

    /// L'entitat Patient a la qual pertany aquest estudi
    Patient *m_parentPatient;
};

}

#endif
