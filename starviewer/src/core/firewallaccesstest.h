#ifndef UDGFIREWALLTEST_H
#define UDGFIREWALLTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class FirewallAccess;

/**
    Test de diagnosis de l'aplicació que comprova si Starviewer té accés a través del firewall.
*/
class FirewallAccessTest : public DiagnosisTest {
Q_OBJECT
public:
    FirewallAccessTest(QObject *parent = 0);
    ~FirewallAccessTest();

    DiagnosisTestResult run();

    /// Retorna la descripció del test
    QString getDescription();

protected:
    virtual FirewallAccess* createFirewall();
    virtual void checkIfStarviewerHaveAccessThroughFirewall(FirewallAccess *firewall);

};

static DiagnosisTestFactoryRegister<FirewallAccessTest> registerFirewallTest("FirewallAccessTest");

}

#endif
