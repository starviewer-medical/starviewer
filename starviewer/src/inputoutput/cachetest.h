#ifndef UDGCACHETEST_H
#define UDGCACHETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class CacheTest : public DiagnosisTest {

public:
    CacheTest(QObject *parent = 0);
    ~CacheTest();

    DiagnosisTestResult run();

    ///Retorna descripcio del test
    QString getDescription();

protected:
    virtual unsigned int getFreeSpace(const QString &cachePath);
    virtual unsigned int getMinimumFreeSpace();
    virtual bool doesCacheDirectoryHaveReadWritePermissions(const QString &cachePath);
    virtual bool isCacheOnDefaultPath();
};

static DiagnosisTestFactoryRegister<CacheTest> registerCacheTest("CacheTest");

} // end namespace udg

#endif
