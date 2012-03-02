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
    DiagnosisTestResult result;

    PortInUse *portInUse = createPortInUse();
    checkIfPortIsInUse(portInUse);

    if (portInUse->getStatus() != PortInUse::PortIsAvailable && portInUse->getOwner() != PortInUse::PortUsedByStarviewer)
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Error);
        if (portInUse->getStatus() == PortInUse::PortIsInUse)
        {
            problem.setDescription(tr("Port %1 is already in use by other application").arg(m_port));
            problem.setSolution(tr("Try another port or shutdown the application using this port."));
        }
        else
        {
            problem.setDescription(tr("Unable to test if port %1 is in use due to error: %2").arg(m_port).arg(portInUse->getErrorString()));
            problem.setSolution(tr("Contact technical service to evaluate the problem."));
        }
        result.addError(problem);
    }

    delete portInUse;
    return result;
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
