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

    /// Assigna/Obté el pacient
    void setPatient( Patient *patient ){ m_patient = patient; }
    Patient *getPatient() const { return m_patient; }

    /// Obté la llista d'arxius
    QStringList getFilesList() const { return m_fileList; }

    /// Afegeix/esborra un arxiu a la llista \TODO què passa si un arxiu està repetit?
    void addFile( QString filename );
    void removeFile( QString filename );

    /// Afegim etiquetes a nivell global/Series \TODO a nivell de series podríem posar el paràmetre el propi punter a Series* que no pas l'UID-> s'ha d'aclarir
    void addLabel( QString label );
    void addLabelToSeries( QString label, QString seriesUID );

    /// Obtenim les etiquetes a nivell global/Series
    QStringList getLabels() const { return m_globalLabels; }
    QStringList getLabelsFromSeries( QString seriesUID );

private:
    /// Pacient a omplir
    Patient *m_patient;

    /// Llista d'arxius que cal tractar per omplir m_patient
    QStringList m_fileList;

    /// Llista d'etiquetes assignades a nivell global
    QStringList m_globalLabels;

    /// Llista d'etiquetes assignades a nivell de sèries. \TODO ara fem una associació Label-Series UID, però es podria fer també Label-Series*. Una altre alternativa seria ampliar l'estructura de Patient per tal que Series tingués els labels.
    QMultiMap< QString, QString > m_seriesLabels;
};

}

#endif
