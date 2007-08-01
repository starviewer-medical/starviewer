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

    /// Obté la llista d'arxius
    QStringList getFilesList() const { return m_fileList; }

    /// Afegeix/esborra un arxiu a la llista
    // \TODO què passa si un arxiu està repetit?
    void addFile( QString filename );
    void removeFile( QString filename );

    /// Afegim etiquetes a nivell global/Series
    // \TODO a nivell de series podríem posar el paràmetre el propi punter a Series* que no pas l'UID-> s'ha d'aclarir
    void addLabel( QString label );
    void addLabelToSeries( QString label, QString seriesUID );

    /// Obtenim les etiquetes a nivell global/Series
    QStringList getLabels() const { return m_globalLabels; }
    QStringList getLabelsFromSeries( QString seriesUID );

    /// Retorna true en el cas que es tinguin tots els labels (ja sigui a nivell de sèrie o global)
    bool hasAllLabels(QStringList requiredLabelsList) const;

private:
    /// Llista de pacients a omplir
    QList<Patient *> m_patientList;

    /// Llista d'arxius que cal tractar per omplir la llista de pacients
    QStringList m_fileList;

    /// Llista d'etiquetes assignades a nivell global
    QStringList m_globalLabels;

    /// Llista d'etiquetes assignades a nivell de sèries. Per cada uid tenim vàries etiquetes \TODO ara fem una associació Series UID-Label, però es podria fer també Series*-Label. Una altre alternativa seria ampliar l'estructura de Patient per tal que Series tingués els labels.
    QMultiMap< QString, QString > m_seriesLabels;
};

}

#endif
