/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTFILLERINPUT_H
#define UDGPATIENTFILLERINPUT_H

#include <QStringList>
#include <QMultiMap>

namespace udg {

class Patient;
class Series;

/**
Classe que encapsula els paràmetres d'input que es faran servir a PatientFiller

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientFillerInput{
public:
    PatientFillerInput();

    ~PatientFillerInput();

    /// Afegeix un pacient a la llista
    void addPatient( Patient *patient );

    /// Obté un pacient de la llista indexat. Si l'índex supera el nombre de membres de la llista es retorna NUL
    Patient *getPatient( int index = 0 );

    /// Obté un pacient identificat pel seu nom
    Patient *getPatientByName( QString name );

    /// Obté un pacient identificat pel seu ID
    Patient *getPatientByID( QString id );

    /// Retorna el nombre de pacients que tenim a la llista
    unsigned int getNumberOfPatients();

    /// Assigna/Obté la llista d'arxius. A l'assignar els arxius, s'esborren les dades de pacient que hi puguin haver-hi
    /// ja que es considera que donar tota la llista d'arxius és com un 'reset'
    void setFilesList( QStringList files );
    QStringList getFilesList() const;

    /// Afegeix/esborra un arxiu a la llista. No hi haurà elements repetits a la llista ja qu es comprova abans d0afegir
    void addFile( QString filename );
    void removeFile( QString filename );

    /// Afegim etiquetes a nivell global/Series
    void addLabel( QString label );
    void addLabelToSeries( QString label, Series *series );

    /**
     * Obtenim totes les etiquetes que s'han aplicat fins al moment, tant a nivell global com a nivell de sèries
     * @return Una llista composada per les etiquetes globals i de series
     */
    QStringList getLabels() const;

    /// Retorna true en el cas que es tinguin tots els labels (ja sigui a nivell de sèrie o global)
    bool hasAllLabels(QStringList requiredLabelsList) const;

    /// Retorna les series que continguin almenys les etiquetes contingudes en la llista d'etiquetes que li passem
    QList<Series *> getSeriesWithLabels( QStringList labels );

private:
    /// Llista de pacients a omplir
    QList<Patient *> m_patientList;

    /// Llista d'arxius que cal tractar per omplir la llista de pacients
    QStringList m_fileList;

    /// Llista d'etiquetes assignades a nivell global
    QStringList m_globalLabels;

    /// Llista que té totes les labels aplicades sense repeticions de labels globals i de series.
    /// Aquesta variable la tenim per ser més àgils en el getLabels per ser una mica més ràpids.
    QStringList m_allLabels;

    /// Llista d'etiquetes assignades a nivell de sèries. Per cada Series tenim vàries etiquetes
    QMultiMap<Series *, QString> m_seriesLabels;
};

}

#endif
