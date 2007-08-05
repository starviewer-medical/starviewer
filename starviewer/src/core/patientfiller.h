/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTFILLER_H
#define UDGPATIENTFILLER_H

#include <QObject>
#include <QString>
#include <QList>

namespace udg {

class PatientFillerInput;
class PatientFillerStep;

/**
Classe encarregada de a partir d'un Patient i un conjunt d'arxius classificar-los dins de l'estructura Patient i omplir les dades corresponents segons el tipus d'arxiu que es tracti.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientFiller : public QObject {
    Q_OBJECT
public:
    PatientFiller(QObject * parent = 0);

    ~PatientFiller();

    /// Donat l'input i els steps disponibles omple l'estructura Patient
    void fill(PatientFillerInput *input);

    /// Ídem fill() però en aquest cas té un criteri d'aturada. Quan hagi conseguit l'etiqueta indicada s'aturarà i no processarà més mòduls
    void fillUntil(PatientFillerInput *input, QString stopLabel);

signals:
    /// Senyal que s'emet regularment al fer un fill o fillUntil indicant el % realitzat.
    void progress(int);

private:
    /// S'encarrega de registrar els mòduls/steps que processaran l'input.
    // \TODO això en un futur ho farà una classe registradora, ara es fa tot aquí per conveniència
    void registerSteps();

    /// Processa un PatientFillerStep amb les dades d'input
    void processPatientFillerStep(PatientFillerStep *patientFillerStep, PatientFillerInput *input);

private:
    const QString UntilEndLabel; //< Label que, teòricament, és impossible que sigui definida per un FillerStep
    /// Registre d'steps
    QList<PatientFillerStep *> m_registeredSteps;
};

}

#endif
