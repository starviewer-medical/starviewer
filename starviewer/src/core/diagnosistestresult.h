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
