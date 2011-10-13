#ifndef UDGINCOMINGDICOMCONNECTIONSPORTINUSETEST_H
#define UDGINCOMINGDICOMCONNECTIONSPORTINUSETEST_H

#include "portinusetest.h"

namespace udg {

class IncomingDICOMConnectionsPortInUseTest : public PortInUseTest {

public:
    IncomingDICOMConnectionsPortInUseTest(QObject *parent = 0);
    ~IncomingDICOMConnectionsPortInUseTest();
};

static DiagnosisTestFactoryRegister<IncomingDICOMConnectionsPortInUseTest> registerIncomingDICOMConnectionsPortInUseTest("IncomingDICOMConnectionsPortInUseTest");

} // end namespace udg

#endif