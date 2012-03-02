#include "diagnosistestproblem.h"

namespace udg {

DiagnosisTestProblem::DiagnosisTestProblem(DiagnosisTestProblem::DiagnosisTestProblemState state, QString description, QString solution)
{
    m_state = state;
    m_description = description;
    m_solution = solution;
}

DiagnosisTestProblem::~DiagnosisTestProblem()
{

}

DiagnosisTestProblem::DiagnosisTestProblemState DiagnosisTestProblem::getState() const
{
    return m_state;
}

void DiagnosisTestProblem::setState(DiagnosisTestProblem::DiagnosisTestProblemState state)
{
    m_state = state;
}

QString DiagnosisTestProblem::getDescription() const
{
    return m_description;
}

void DiagnosisTestProblem::setDescription(const QString &description)
{
    m_description = description;
}

QString DiagnosisTestProblem::getSolution() const
{
    return m_solution;
}

void DiagnosisTestProblem::setSolution(const QString &solution)
{
    m_solution = solution;
}

}
