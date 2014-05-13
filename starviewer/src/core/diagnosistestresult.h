/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGDIAGNOSISTESTRESULT_H
#define UDGDIAGNOSISTESTRESULT_H

#include "diagnosistestproblem.h"
#include <QList>

namespace udg {

class DiagnosisTestResult {
public:
    /// Representa l'estat del test, Invalid significa o bé que el test no s'ha fet encara,
    /// o bé que l'estat del test no s'ha assignat correctament
    enum DiagnosisTestResultState { Ok, Warning, Error };

    DiagnosisTestResult();
    ~DiagnosisTestResult();

    /// Retorna l'estat del test
    DiagnosisTestResultState getState() const;
    /// Afegeix un error
    void addError(const DiagnosisTestProblem &problem);
    /// Afegeix un warning
    void addWarning(const DiagnosisTestProblem &problem);

    /// Retorna la llista amb els errors
    QList<DiagnosisTestProblem> getErrors() const;
    /// Retorna la llista amb els warnings
    QList<DiagnosisTestProblem> getWarnings() const;

private:
    /// Llista d'errors
    QList<DiagnosisTestProblem> m_errors;
    /// Llista de warnings
    QList<DiagnosisTestProblem> m_warnings;
};

}

#endif
