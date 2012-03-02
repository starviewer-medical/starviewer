#include "echotopacstest.h"
#include "pacsdevicemanager.h"

#include <QStringList>
#include <QList>

namespace udg {

EchoToPACSTest::EchoToPACSTest(QObject *parent)
 : DiagnosisTest(parent)
{
}

EchoToPACSTest::~EchoToPACSTest()
{
}

DiagnosisTestResult EchoToPACSTest::run()
{
    DiagnosisTestResult testResults;

    QList<PacsDevice> pacsList = getPacsDeviceList();
    
    if (pacsList.count() == 0)
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Warning);
        problem.setDescription(tr("There are no PACS defined"));
        problem.setSolution(tr("New PACS can be defined at Tools > Configuration > PACS"));
        testResults.addWarning(problem);
    }
    
    for (int i = 0; i < pacsList.count(); i++)
    {
        EchoToPACS::EchoRequestStatus status = echo(pacsList.at(i));

        if (status != EchoToPACS::EchoOk)
        {
            DiagnosisTestProblem problem;
            problem.setState(DiagnosisTestProblem::Error);

            if (status == EchoToPACS::EchoFailed)
            {
                problem.setDescription(tr("Echo to PACS with AETitle '%1' failed").arg(pacsList.at(i).getAETitle()));
                problem.setSolution(tr("Check PACS configuration at Tools > Configuration > PACS"));
            }
            else
            {
                problem.setDescription(tr("Unable to connect to PACS with AETitle '%1'").arg(pacsList.at(i).getAETitle()));
                problem.setSolution(tr("Check internet connection and PACS configuration at Tools > Configuration > PACS"));
            }
            testResults.addError(problem);
        }
    }

    return testResults;
}

QString EchoToPACSTest::getDescription()
{
    return tr("All PACS are correctly configured");
}

QList<PacsDevice> EchoToPACSTest::getPacsDeviceList()
{
    return PacsDeviceManager().getPACSList();
}

EchoToPACS::EchoRequestStatus EchoToPACSTest::echo(const PacsDevice &pacs)
{
    EchoToPACS echoToPACS;
    echoToPACS.echo(pacs);

    return echoToPACS.getLastError();
}

} // end namespace udg
