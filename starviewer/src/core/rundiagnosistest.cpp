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

#include "rundiagnosistest.h"

#include "diagnosistest.h"

namespace udg {

RunDiagnosisTest::RunDiagnosisTest(QList<DiagnosisTest *> diagnosisTestsToRun)
{
    m_diagnosisTestsToRun = diagnosisTestsToRun;
}

QList<QPair<DiagnosisTest *, DiagnosisTestResult> > RunDiagnosisTest::getRunTests()
{
    return m_executedDiagnosisTest;
}

QList<DiagnosisTest *> RunDiagnosisTest::getDiagnosisTestToRun() const
{
    return m_diagnosisTestsToRun;
}

void RunDiagnosisTest::run()
{
    m_executedDiagnosisTest.clear();

    foreach(DiagnosisTest *diagnosisTest, m_diagnosisTestsToRun)
    {
        emit runningDiagnosisTest(diagnosisTest);

        DiagnosisTestResult diagnosisTestResult = diagnosisTest->run();

        QPair<DiagnosisTest *, DiagnosisTestResult> executedTest;
        executedTest.first = diagnosisTest;
        executedTest.second = diagnosisTestResult;

        m_executedDiagnosisTest.append(executedTest);
    }

    emit finished();
}

}
