#ifndef UDGSYSTEMREQUERIMENTSTEST_H
#define UDGSYSTEMREQUERIMENTSTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"
#include "systeminformation.h"

class QString;
class QSize;

namespace udg {

class SystemRequeriments;

/**
    Test de diagnosis de l'aplicació que comprova si el sistema compleix els requeriments mínims per que Starviewer funcioni correctament.
*/
class SystemRequerimentsTest : public DiagnosisTest {
Q_OBJECT
public:
    SystemRequerimentsTest(QObject *parent = 0);
    ~SystemRequerimentsTest();

    DiagnosisTestResult run();

    /// Retorna la descripció del test
    QString getDescription();

protected:
    enum VersionComparison { Older, Same, Newer };

    /// Retorna 0 sí son iguals, 1 sí la version1 és major que la version2 i -1 si és menor.
    VersionComparison compareVersions(QString version1, QString version2);

    virtual unsigned int getCPUNumberOfCores(SystemInformation *system);
    virtual QList<unsigned int> getCPUFrequencies(SystemInformation *system);
    virtual QStringList getGPUOpenGLCompatibilities(SystemInformation *system);
    virtual QString getGPUOpenGLVersion(SystemInformation *system);
    virtual QList<unsigned int> getGPURAM(SystemInformation *system);
    virtual QStringList getGPUModel(SystemInformation *system);
    virtual unsigned int getHardDiskFreeSpace(SystemInformation *system, const QString &device);
    virtual SystemInformation::OperatingSystem getOperatingSystem(SystemInformation *system);
    virtual QString getOperatingSystemVersion(SystemInformation *system);
    virtual QString getOperatingSystemServicePackVersion(SystemInformation *system);
    virtual bool isOperatingSystem64BitArchitecture(SystemInformation *system);
    virtual unsigned int getRAMTotalAmount(SystemInformation *system);
    virtual QList<QSize> getScreenResolutions(SystemInformation *system);
    virtual bool doesOpticalDriveHaveWriteCapabilities(SystemInformation *system);

    virtual SystemRequeriments* getSystemRequeriments();
};

static DiagnosisTestFactoryRegister<SystemRequerimentsTest> registerSystemRequerimentsTest("SystemRequerimentsTest");

}

#endif
