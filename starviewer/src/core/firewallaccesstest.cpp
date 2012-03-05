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
