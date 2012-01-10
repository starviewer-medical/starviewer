#include "portinusetest.h"
#include "portinuse.h"
#include "inputoutputsettings.h"

namespace udg {

PortInUseTest::PortInUseTest(QObject *parent)
 : DiagnosisTest(parent)
{
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

    if (portInUse->getStatus() == PortInUse::PortIsAvailable || portInUse->getOwner() == PortInUse::PortUsedByStarviewer)
    {
        testResultState = DiagnosisTestResult::Ok;
    }
    else
    {
        testResultState = DiagnosisTestResult::Error;
        if (portInUse->getStatus() == PortInUse::PortIsInUse)
        {
            testResultDescription = tr("Port is already in use");
            testResultSolution = tr("Try another port or shut down the application using this port");
        }
        else
        {
            testResultDescription = tr("Unable to test if port %1 is in use due to error: %2").arg(m_port).arg(portInUse->getErrorString());
        }
    }

    delete portInUse;
    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
}

QString PortInUseTest::getDescription()
{
    return tr("Port %1 is in use by another application").arg(m_port);
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
