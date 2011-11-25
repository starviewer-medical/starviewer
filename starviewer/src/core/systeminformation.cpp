// Starviewer
#include "systeminformation.h"

#ifdef WIN32
#include "windowssysteminformation.h"
#endif

// Qt
#include <QDesktopWidget>
#include <QRect>
#include <QSize>

namespace udg {

SystemInformation::SystemInformation()
{
}

SystemInformation::~SystemInformation()
{
}

SystemInformation* SystemInformation::newInstance()
{
#ifdef WIN32
    return new WindowsSystemInformation();
#else
    return new SystemInformation();
#endif
}

SystemInformation::OperatingSystem SystemInformation::getOperatingSystem()
{
    return SystemInformation::OSLinux;
}

QString SystemInformation::getOperatingSystemArchitecture()
{
    return "";
}

QString SystemInformation::getOperatingSystemVersion()
{
    return "";
}

QString SystemInformation::getOperatingSystemServicePackVersion()
{
    return "";
}

unsigned int SystemInformation::getRAMTotalAmount()
{
    return 0;
}

QList<unsigned int> SystemInformation::getRAMModulesCapacity()
{
    return QList<unsigned int>();
}

QList<unsigned int> SystemInformation::getRAMModulesFrequency()
{
    return QList<unsigned int>();
}

unsigned int SystemInformation::getCPUNumberOfCores()
{
    return 0;
}

QList<unsigned int> SystemInformation::getCPUFrequencies()
{
    return QList<unsigned int>();
}

unsigned int SystemInformation::getCPUL2CacheSize()
{
    return 0;
}

QString SystemInformation::getGPUBrand()
{
    return "";
}

QString SystemInformation::getGPUModel()
{
    return "";
}

unsigned int SystemInformation::getGPURAM()
{
    return 0;
}

QList<QString> SystemInformation::getGPUOpenGLCompatibilities()
{
    return QList<QString>();
}

QString SystemInformation::getGPUOpenGLVersion()
{
    return "0.0";
}

QString SystemInformation::getGPUDriverVersion()
{
    return "0.0.0";
}

QList<QSize> SystemInformation::getScreenResolutions()
{
    QList<QSize> screenResolutions;
    QDesktopWidget desktop;
    for (int i = 0; i < desktop.screenCount(); i++)
    {
        screenResolutions.append(desktop.screenGeometry(i).size());
    }
    return screenResolutions;
}

QList<QString> SystemInformation::getScreenVendors()
{
    return QList<QString>();
}

QList<QString> SystemInformation::getHardDiskDevices()
{
    return QList<QString>();
}

unsigned int SystemInformation::getHardDiskCapacity(const QString &device)
{
    Q_UNUSED(device);
    return 0;
}

unsigned int SystemInformation::getHardDiskFreeSpace(const QString &device)
{
    Q_UNUSED(device);
    return 0;
}

bool SystemInformation::doesOpticalDriveHaveWriteCapabilities()
{
    return false;
}

unsigned int SystemInformation::getNetworkAdapterSpeed()
{
    return 0;
}

}
