#include "portinusetest.h"
#include "portinuse.h"

namespace udg {

PortInUseTest::PortInUseTest(int port)
{
    m_port = port;
}

PortInUseTest::~PortInUseTest()
{
}

DiagnosisTestResult PortInUseTest::run()
{
    DiagnosisTestResult::DiagnosisTestResultState testResultState = DiagnosisTestResult::Invalid;
    QString testResultDescription;
    QString testResultSolution;

    PortInUse portInUse;
    portInUse.isPortInUse(m_port);

    if (portInUse.getStatus() == PortInUse::PortIsAvailable)
    {
        testResultState = DiagnosisTestResult::Ok;
    }
    else
    {
        testResultState = DiagnosisTestResult::Error;
        if (portInUse.getStatus() == PortInUse::PortIsInUse)
        {
            testResultDescription = QString("Port is already in use");
            testResultSolution = QString("Try another port or shut down the application using this port");
        }
        else
        {
            testResultDescription = QString("Unable to test if port " + QString().setNum(m_port) +
                                            " is in use due to error: " + portInUse.getErrorString());
        }
    }

    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
}

} // end namespace udg
