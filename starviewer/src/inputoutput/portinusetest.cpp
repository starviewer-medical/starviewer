#include "portinusetest.h"
#include "portinuse.h"
#include "inputoutputsettings.h"

namespace udg {

PortInUseTest::PortInUseTest(QObject *parent)
 : DiagnosisTest(parent)
{
    // \TODO Fem que només es miri el port del RIS. S'hauria de valorar si cal comprovar més ports.
    m_port = Settings().getValue(InputOutputSettings::RISRequestsPort).toInt();
}

PortInUseTest::~PortInUseTest()
{
}

DiagnosisTestResult PortInUseTest::run()
{
    DiagnosisTestResult::DiagnosisTestResultState testResultState = DiagnosisTestResult::Invalid;
    QString testResultDescription;
    QString testResultSolution;

    PortInUse *portInUse = createPortInUse();
    checkIfPortIsInUse(portInUse);

    if (portInUse->getStatus() == PortInUse::PortIsAvailable)
    {
        testResultState = DiagnosisTestResult::Ok;
    }
    else
    {
        testResultState = DiagnosisTestResult::Error;
        if (portInUse->getStatus() == PortInUse::PortIsInUse)
        {
            testResultDescription = QString("Port is already in use");
            testResultSolution = QString("Try another port or shut down the application using this port");
        }
        else
        {
            testResultDescription = QString("Unable to test if port " + QString().setNum(m_port) +
                                            " is in use due to error: " + portInUse->getErrorString());
        }
    }

    delete portInUse;
    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
}

PortInUse* PortInUseTest::createPortInUse()
{
    PortInUse *portInUse = new PortInUse();
    return portInUse;
}

void PortInUseTest::checkIfPortIsInUse(PortInUse *portInUse)
{
    portInUse->isPortInUse(m_port);
}

} // end namespace udg
