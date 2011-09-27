#include "firewallaccesstest.h"
#include "firewallaccess.h"
#include "logging.h"

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
    }
    else
    {
        result.setDescription(firewall->getErrorString());
        result.setState(DiagnosisTestResult::Error);
    }

    delete firewall;

    return result;
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
