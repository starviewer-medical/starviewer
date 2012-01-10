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
    /// I comprovar si starviewer té access
    checkIfStarviewerHaveAccessThroughFirewall(firewall);
    DiagnosisTestResult result;

    if (firewall->getStatus() == FirewallAccess::FirewallIsAccessible)
    {
        result.setState(DiagnosisTestResult::Ok);
    }
    else if (firewall->getStatus() == FirewallAccess::FirewallIsBlocking)
    {
        result.setState(DiagnosisTestResult::Warning);
        result.setDescription(firewall->getErrorString());
        result.setSolution(tr("Add %1 to the list of allowed applications through firewall").arg(ApplicationNameString));
    }
    else
    {
        result.setDescription(firewall->getErrorString());
        result.setState(DiagnosisTestResult::Error);
        // TODO Proposar una solució per quan falla la comprovació del firewall
        result.setSolution(tr(""));
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
