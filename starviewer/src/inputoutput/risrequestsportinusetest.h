#ifndef UDGRISREQUESTSPORTINUSETEST_H
#define UDGRISREQUESTSPORTINUSETEST_H

#include "portinusetest.h"

namespace udg {

class RISRequestsPortInUseTest : public PortInUseTest {

public:
    RISRequestsPortInUseTest(QObject *parent = 0);
    ~RISRequestsPortInUseTest();
};

static DiagnosisTestFactoryRegister<RISRequestsPortInUseTest> registerRISRequestsPortInUseTest("RISRequestsPortInUseTest");

} // end namespace udg

#endif