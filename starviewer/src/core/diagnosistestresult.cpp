#include "diagnosistestresult.h"

namespace udg {

DiagnosisTestResult::DiagnosisTestResult()
{
}

DiagnosisTestResult::~DiagnosisTestResult()
{

}

DiagnosisTestResult::DiagnosisTestResultState DiagnosisTestResult::getState() const
{
    if (!m_errors.isEmpty())
    {
        return DiagnosisTestResult::Error;
    }
    else if (!m_warnings.isEmpty())
    {
        return DiagnosisTestResult::Warning;
    }
    else
    {
        return DiagnosisTestResult::Ok;
    }
}

void DiagnosisTestResult::addError(const DiagnosisTestProblem &problem)
{
    m_errors.append(problem);
}

void DiagnosisTestResult::addWarning(const DiagnosisTestProblem &problem)
{
    m_warnings.append(problem);
}

QList<DiagnosisTestProblem> DiagnosisTestResult::getErrors() const
{
    return m_errors;
}

QList<DiagnosisTestProblem> DiagnosisTestResult::getWarnings() const
{
    return m_warnings;
}

}
