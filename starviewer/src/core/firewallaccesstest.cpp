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

#include "firewallaccesstest.h"
#include "firewallaccess.h"
#include "logging.h"
#include "starviewerapplication.h"

namespace udg {

FirewallAccessTest::FirewallAccessTest(QObject *parent)
 : DiagnosisTest(parent)
{
}

FirewallAccessTest::~FirewallAccessTest()
{
}

DiagnosisTestResult FirewallAccessTest::run()
{
    /// Crear el firewall
    FirewallAccess *firewall = createFirewall();
    /// I comprovar si starviewer t access
    checkIfStarviewerHaveAccessThroughFirewall(firewall);
    DiagnosisTestResult result;

    if (firewall->getStatus() == FirewallAccess::FirewallIsBlocking)
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Warning);
        problem.setDescription(firewall->getErrorString());
        problem.setSolution(tr("Add %1 to the list of applications that have access through firewall").arg(ApplicationNameString));
        result.addWarning(problem);
    }
    else if (firewall->getStatus() != FirewallAccess::FirewallIsAccessible)
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Error);
        problem.setDescription(firewall->getErrorString());
        problem.setSolution(tr("Contact technical service to evaluate the problem"));
        result.addError(problem);
    }

    delete firewall;

    return result;
}

QString FirewallAccessTest::getDescription()
{
    return tr("%1 has access through firewall").arg(ApplicationNameString);
}

FirewallAccess* FirewallAccessTest::createFirewall()
{
    FirewallAccess *firewall = FirewallAccess::newInstance();

    return firewall;
}

void FirewallAccessTest::checkIfStarviewerHaveAccessThroughFirewall(FirewallAccess *firewall)
{
    firewall->doesStarviewerHaveAccesThroughFirewall();
}

}
