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
                problem.setDescription(tr("Echo to PACS with AE Title '%1' failed").arg(pacsList.at(i).getAETitle()));
                problem.setSolution(tr("Check PACS configuration at Tools > Configuration > PACS"));
            }
            else
            {
                problem.setDescription(tr("Unable to connect to PACS with AE Title '%1'").arg(pacsList.at(i).getAETitle()));
                problem.setSolution(tr("Check internet connection and PACS configuration at Tools > Configuration > PACS"));
            }
            testResults.addError(problem);
        }
    }

    return testResults;
}

QString EchoToPACSTest::getDescription() const
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
