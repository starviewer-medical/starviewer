#ifndef UDGINCOMINGDICOMCONNECTIONSPORTINUSETEST_H
#define UDGINCOMINGDICOMCONNECTIONSPORTINUSETEST_H

#include "portinusetest.h"

namespace udg {

class IncomingDICOMConnectionsPortInUseTest : public PortInUseTest {
Q_OBJECT
public:
    IncomingDICOMConnectionsPortInUseTest(QObject *parent = 0);
    ~IncomingDICOMConnectionsPortInUseTest();

    ///Retorna descripcio del test
    QString getDescription();
};

static DiagnosisTestFactoryRegister<IncomingDICOMConnectionsPortInUseTest> registerIncomingDICOMConnectionsPortInUseTest("IncomingDICOMConnectionsPortInUseTest");

} // end namespace udg

#endif
