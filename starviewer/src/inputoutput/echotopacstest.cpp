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
        descriptionStringList.append(tr("There are no PACS defined"));
        solutionStringList.append(tr(""));
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
                descriptionStringList.append(tr("Echo to pacs with AETitle '%1' failed").arg(pacsList.at(i).getAETitle()));
                solutionStringList.append(tr("Contact PACS supervisor"));
            }
            else
            {
                descriptionStringList.append(tr("Unable to connect to PACS with AETitle '%1'").arg(pacsList.at(i).getAETitle()));
                solutionStringList.append(tr("Check PACS url, or internet connection"));
            }
        }
    }

    QString testResultDescription = descriptionStringList.join("\n");
    QString testResultSolution = solutionStringList.join("\n");

    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
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
