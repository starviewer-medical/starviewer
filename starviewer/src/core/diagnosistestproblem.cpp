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
