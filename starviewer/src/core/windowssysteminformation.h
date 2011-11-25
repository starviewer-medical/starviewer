#ifndef UDGWINDOWSSYSTEMINFORMATION_H
#define UDGWINDOWSSYSTEMINFORMATION_H

// Starviewer
#include "systeminformation.h"
// Qt
#include <QStringList>
// Windows
#include <Wbemidl.h>

namespace udg {

class WindowsSystemInformation : public SystemInformation {
public:
    /// Constructor per defecte
    WindowsSystemInformation();
    /// Destructor
    ~WindowsSystemInformation();
    
    OperatingSystem getOperatingSystem();
    QString getOperatingSystemArchitecture();
    QString getOperatingSystemVersion();
    QString getOperatingSystemServicePackVersion();

    /// Retorna la quantitat total de memòria RAM en MegaBytes
    unsigned int getRAMTotalAmount();
    QList<unsigned int> getRAMModulesCapacity();
    QList<unsigned int> getRAMModulesFrequency();

    unsigned int getCPUNumberOfCores();
    /// Retorna una llista amb la freqüència de cada processador 
    QList<unsigned int> getCPUFrequencies();
    unsigned int getCPUL2CacheSize();

    QString getGPUBrand();
    QString getGPUModel();
    unsigned int getGPURAM();
    QList<QString> getGPUOpenGLCompatibilities();
    QString getGPUOpenGLVersion();
    QString getGPUDriverVersion();

    //Screen, Display, Monitor, Desktop, ...
    QList<QString> getScreenVendors();

    QList<QString> getHardDiskDevices();
    unsigned int getHardDiskCapacity(const QString &device); // Del disc dur que conté la carpeta de la cache de Starviewer
    unsigned int getHardDiskFreeSpace(const QString &device);
    bool doesOpticalDriveHaveWriteCapabilities();

    unsigned int getNetworkAdapterSpeed();

protected:
    virtual IWbemClassObject* getNextObject(IEnumWbemClassObject *enumerator);
    virtual IEnumWbemClassObject* executeQuery(QString query);
    virtual bool getProperty(IWbemClassObject *object, QString propertyName, VARIANT *propertyVariant);
    IWbemServices* initializeAPI();
    virtual void uninitializeAPI(IWbemServices *services);
    virtual QString createOpenGLContextAndGetExtensions();
    virtual QString createOpenGLContextAndGetVersion();

protected:
    IWbemServices* m_api;
};

}

#endif
