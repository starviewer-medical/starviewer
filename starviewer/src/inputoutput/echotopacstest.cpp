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

#include "echotopacs.h"
#include "echotowadopacs.h"
#include "pacsdevice.h"
#include "pacsdevicemanager.h"

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
        if (QThread::currentThread()->isInterruptionRequested())
        {
            break;
        }

        DiagnosisTestProblem problem = echo(pacsList.at(i));

        if (problem.getState() != DiagnosisTestProblem::Ok)
        {
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
    return PacsDeviceManager::getPacsList();
}

DiagnosisTestProblem EchoToPACSTest::echo(const PacsDevice &pacs)
{
    if (pacs.getType() == PacsDevice::Type::Dimse)
    {
        EchoToPACS echoToPACS;

        if (echoToPACS.echo(pacs))
        {
            return DiagnosisTestProblem(DiagnosisTestProblem::Ok);
        }
        else
        {
            if (echoToPACS.getLastError() == EchoToPACS::EchoFailed)
            {
                return DiagnosisTestProblem(DiagnosisTestProblem::Error, tr("Echo to PACS with AE Title '%1' failed").arg(pacs.getAETitle()),
                                            tr("Check PACS configuration at Tools > Configuration > PACS"));
            }
            else
            {
                return DiagnosisTestProblem(DiagnosisTestProblem::Error, tr("Unable to connect to PACS with AE Title '%1'").arg(pacs.getAETitle()),
                                            tr("Check internet connection and PACS configuration at Tools > Configuration > PACS"));
            }
        }
    }
    else if (pacs.getType() == PacsDevice::Type::Wado)
    {
        EchoToWadoPacs echoToWadoPacs;

        if (echoToWadoPacs.echo(pacs))
        {
            return DiagnosisTestProblem(DiagnosisTestProblem::Ok);
        }
        else
        {
            return DiagnosisTestProblem(DiagnosisTestProblem::Error, tr("Connection to PACS with base URI '%1' failed").arg(pacs.getBaseUri().toString()),
                                        tr("Check internet connection and PACS configuration at Tools > Configuration > PACS"));
        }
    }
    else
    {
        return DiagnosisTestProblem();
    }
}

} // end namespace udg
