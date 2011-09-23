#ifndef UDGPORTINUSETEST_H
#define UDGPORTINUSETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class PortInUse;

class PortInUseTest : public DiagnosisTest {

public:
    PortInUseTest(QObject *parent = 0);
    ~PortInUseTest();

    DiagnosisTestResult run();

protected:
    virtual PortInUse* createAndCheckPortInUse();

private:
    /// Port que es comprovarà si està en ús.
    int m_port;
};

static DiagnosisTestFactoryRegister<PortInUseTest> registerPortInUseTest("PortInUseTest");

} // end namespace udg

#endif
