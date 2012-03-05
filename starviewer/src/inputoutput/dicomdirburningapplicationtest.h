#ifndef UDGDICOMDIRBURNINGAPPLICATIONTEST_H
#define UDGDICOMDIRBURNINGAPPLICATIONTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

class QString;

namespace udg {

class DICOMDIRBurningApplicationTest : public DiagnosisTest {
Q_OBJECT
public:
    DICOMDIRBurningApplicationTest(QObject *parent = 0);
    ~DICOMDIRBurningApplicationTest();

    DiagnosisTestResult run();

    ///Retorna descripcio del test
    QString getDescription();

protected:
    virtual bool burningApplicationIsDefined(const QString &burningApplication);
    virtual bool burningApplicationIsInstalled(const QString &file);
};

static DiagnosisTestFactoryRegister<DICOMDIRBurningApplicationTest> registerDICOMDIRBurningApplicationTest("DICOMDIRBurningApplicationTest");

} // end namespace udg

#endif
