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
