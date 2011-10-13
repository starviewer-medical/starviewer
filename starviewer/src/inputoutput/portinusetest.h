#ifndef UDGPORTINUSETEST_H
#define UDGPORTINUSETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class PortInUse;

class PortInUseTest : public DiagnosisTest {

public:
    virtual ~PortInUseTest();

    DiagnosisTestResult run();

protected:
    PortInUseTest(QObject *parent = 0);
    virtual PortInUse* createPortInUse();
    virtual void checkIfPortIsInUse(PortInUse *portInUse);

protected:
    /// Port que es comprovarà si està en ús.
    int m_port;
};

} // end namespace udg

#endif
