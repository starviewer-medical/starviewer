#ifndef UDGSYSTEMINFORMATION_H
#define UDGSYSTEMINFORMATION_H

// Qt
#include <QStringList>
#include <QSize>

namespace udg {

class SystemInformation {
public:
    enum OperatingSystem { OSWindows, OSMac, OSLinux };
    
    /// Destructor
    virtual ~SystemInformation();

    /// Crea una nova instància d'alguna de les classes que implementa la interfície
    static SystemInformation* newInstance();

    virtual OperatingSystem getOperatingSystem();
    
    // Arquitectura de 32-bits o 64-bits
    virtual bool isOperatingSystem64BitArchitecture();
    
    virtual QString getOperatingSystemVersion();
    
    /// Retorna la versió de service pack instal·lat, només en windows
    virtual QString getOperatingSystemServicePackVersion();
    
    /// Returna el nom complert del Sistema Operatiu (Nom, arquitectura, actualitzacions...)
    virtual QString getOperatingSystemAsString();

    /// Returs the name of operating system in a short form
    virtual QString getOperatingSystemAsShortString();
    
    /// Retorna la quantitat total de memòria RAM en MegaBytes
    virtual unsigned int getRAMTotalAmount();
    
    //En MBytes
    virtual QList<unsigned int> getRAMModulesCapacity();
    
    //En MHz
    virtual QList<unsigned int> getRAMModulesFrequency();

    virtual unsigned int getCPUNumberOfCores();
    virtual QList<unsigned int> getCPUFrequencies();
    virtual unsigned int getCPUL2CacheSize(); // en KBytes

    virtual QStringList getGPUBrand();
    virtual QStringList getGPUModel();
    virtual QList<unsigned int> getGPURAM();
    virtual QStringList getGPUOpenGLCompatibilities();
    virtual QString getGPUOpenGLVersion();
    virtual QStringList getGPUDriverVersion();

    // Screen, Display, Monitor, Desktop, ...
    QList<QSize> getScreenResolutions();
    virtual QStringList getScreenVendors();

    virtual QStringList getHardDiskDevices();
    virtual unsigned int getHardDiskCapacity(const QString &device);
    virtual unsigned int getHardDiskFreeSpace(const QString &device);
    virtual bool doesOpticalDriveHaveWriteCapabilities();

    virtual unsigned int getNetworkAdapterSpeed();

    /// True if composition feature is available for the system, false otherwise
    /// For example, if system is Windows XP, this feature is not available, but it does for Windows Vista and above
    virtual bool isDesktopCompositionAvailable();

    /// For systems with desktop composition feature, tells if this feature is enabled or not
    virtual bool isDesktopCompositionEnabled();

protected:
    SystemInformation();
};

}

#endif
