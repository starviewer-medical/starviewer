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
