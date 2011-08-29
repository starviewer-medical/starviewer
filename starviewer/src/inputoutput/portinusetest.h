#ifndef UDGPORTINUSETEST_H
#define UDGPORTINUSETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"

namespace udg {

class PortInUseTest : public DiagnosisTest {

public:
    PortInUseTest(int port);
    ~PortInUseTest();

    DiagnosisTestResult run();

private:
    /// Port que es comprovarà si està en ús.
    int m_port;
};

} // end namespace udg

#endif
