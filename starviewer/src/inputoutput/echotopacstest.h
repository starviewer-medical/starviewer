#ifndef UDGECHOTOPACSTEST_H
#define UDGECHOTOPACSTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"
#include "pacsdevice.h"
#include "echotopacs.h"

namespace udg {

class EchoToPACSTest : public DiagnosisTest {

public:
    EchoToPACSTest(QObject *parent = 0);
    ~EchoToPACSTest();

    DiagnosisTestResult run();

    ///Retorna descripcio del test
    QString getDescription();

protected:
    virtual QList<PacsDevice> getPacsDeviceList();
    virtual EchoToPACS::EchoRequestStatus echo(const PacsDevice &pacs);
};

static DiagnosisTestFactoryRegister<EchoToPACSTest> registerEchoToPACSTest("EchoToPACSTest");

} // end namespace udg

#endif
