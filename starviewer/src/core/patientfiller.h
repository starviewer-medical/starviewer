/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTFILLER_H
#define UDGPATIENTFILLER_H

#include <QString>
#include <QList>

namespace udg {

class PatientFillerInput;
class PatientFillerStep;

/**
Classe encarregada de a partir d'un Patient i un conjunt d'arxius classificar-los dins de l'estructura Patient i omplir les dades corresponents segons el tipus d'arxiu que es tracti.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientFiller{
public:
    PatientFiller();

    ~PatientFiller();

    /// Li assignem les dades d'entrada que li caldrà processar
    void setInput( PatientFillerInput *input );

    /// Donat l'input i els steps disponibles omple l'estructura Patient
    void fill();

    /// Ídem fill() però en aquest cas té un criteri d'aturada. Quan hagi conseguit l'etiqueta indicada s'aturarà i no processarà més mòduls
    void fillUntil( QString label );

private:
    /// S'encarrega de registrar els mòduls/steps que processaran l'input. \TODO això en un futur ho farà una classe registradora, ara es fa tot aquí per conveniència
    void registerSteps();

private:
    /// L'input a tractar
    PatientFillerInput *m_input;

    /// Registre d'steps
    QList<PatientFillerStep *> m_registeredSteps;
};

}

#endif
