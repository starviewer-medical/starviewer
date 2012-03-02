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
    DiagnosisTestResult::DiagnosisTestResultState testResultState = DiagnosisTestResult::Invalid;
    QStringList descriptionStringList;
    QStringList solutionStringList;

    QList<PacsDevice> pacsList = getPacsDeviceList();
    
    if (pacsList.count() > 0)
    {
        /// TODO: Si la llista de pacs és buida, retornar OK o Error?
        testResultState = DiagnosisTestResult::Ok;
    }
    else
    {
        testResultState = DiagnosisTestResult::Warning;
        descriptionStringList << tr("There are no PACS defined");
        solutionStringList << tr("New PACS can be defined at Tools > Configuration > PACS");
    }
    
    for (int i = 0; i < pacsList.count(); i++)
    {
        EchoToPACS::EchoRequestStatus status = echo(pacsList.at(i));

        if (status != EchoToPACS::EchoOk)
        {
            testResultState = DiagnosisTestResult::Error;

            if (status == EchoToPACS::EchoFailed)
            {
                // TODO: Fer lo de concatenar l'String
                descriptionStringList << tr("Echo to PACS with AETitle '%1' failed").arg(pacsList.at(i).getAETitle());
                solutionStringList << tr("Check PACS configuration at Tools > Configuration > PACS");
            }
            else
            {
                descriptionStringList << tr("Unable to connect to PACS with AETitle '%1'").arg(pacsList.at(i).getAETitle());
                solutionStringList << tr("Check internet connection and PACS configuration at Tools > Configuration > PACS");
            }
        }
    }

    QString testResultDescription = descriptionStringList.join("\n");
    QString testResultSolution = solutionStringList.join("\n");

    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
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
