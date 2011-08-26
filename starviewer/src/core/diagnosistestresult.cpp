#include "diagnosistestresult.h"

namespace udg {

DiagnosisTestResult::DiagnosisTestResult(DiagnosisTestResult::DiagnosisTestResultState state, QString description, QString solution)
{
    m_state = state;
    m_description = description;
    m_solution = solution;
}

DiagnosisTestResult::~DiagnosisTestResult()
{

}

DiagnosisTestResult::DiagnosisTestResultState DiagnosisTestResult::getState() const
{
    return m_state;
}

void DiagnosisTestResult::setState(DiagnosisTestResult::DiagnosisTestResultState state)
{
    m_state = state;
}

QString DiagnosisTestResult::getDescription() const
{
    return m_description;
}

void DiagnosisTestResult::setDescription(const QString &description)
{
    m_description = description;
}

QString DiagnosisTestResult::getSolution() const
{
    return m_solution;
}

void DiagnosisTestResult::setSolution(const QString &solution)
{
    m_solution = solution;
}

}
