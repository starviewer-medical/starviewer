/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "systeminformation.h"

#include "harddiskinformation.h"
#include "screenmanager.h"

#if defined Q_OS_WIN32
#include "windowssysteminformation.h"
#elif defined Q_OS_LINUX
#include "linuxsysteminformation.h"
#endif

// Qt
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>
#include <QRect>
#include <QSize>
#include <QThread>

namespace {

// Auxiliar class to obtain OpenGL information. On some platforms, can only be safely used in the main thread.
class OpenGLInformation : public QOffscreenSurface, protected QOpenGLFunctions
{
public:
    OpenGLInformation()
    {
        create();
        QOpenGLContext context;
        context.create();
        context.makeCurrent(this);

        if (context.isValid())
        {
            initializeOpenGLFunctions();
            m_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
            m_extensions = QString(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
        }
    }

    const QString& getVersion() const
    {
        return m_version;
    }

    const QStringList& getExtensions() const
    {
        return m_extensions;
    }

private:
    QString m_version;
    QStringList m_extensions;
};

}

namespace udg {

SystemInformation::SystemInformation()
{
}

SystemInformation::~SystemInformation()
{
}

SystemInformation* SystemInformation::newInstance()
{
#if defined Q_OS_WIN32
    return new WindowsSystemInformation();
#elif defined Q_OS_LINUX
    return new LinuxSystemInformation();
#else
    return new SystemInformation();
#endif
}

SystemInformation::OperatingSystem SystemInformation::getOperatingSystem()
{
    return SystemInformation::OSLinux;
}

QString SystemInformation::getOperatingSystemAsString()
{
    return QString("%1 %2 (%3 %4)").arg(QSysInfo::prettyProductName()).arg(QSysInfo::currentCpuArchitecture())
                                   .arg(QSysInfo::kernelType()).arg(QSysInfo::kernelVersion());
}

QString SystemInformation::getOperatingSystemAsShortString()
{
    return QString("%1_%2_%3").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()).arg(QSysInfo::currentCpuArchitecture());
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
    return QThread::idealThreadCount();
}

QList<unsigned int> SystemInformation::getCPUFrequencies()
{
    return QList<unsigned int>();
}

unsigned int SystemInformation::getCPUL2CacheSize()
{
    return 0;
}

QStringList SystemInformation::getGPUBrand()
{
    return QStringList();
}

QStringList SystemInformation::getGPUModel()
{
    return QStringList();
}

QList<unsigned int> SystemInformation::getGPURAM()
{
    return QList<unsigned int>();
}

QStringList SystemInformation::getGPUOpenGLCompatibilities()
{
#ifdef Q_OS_WIN32
    // In Windows OpenGLInformation can only be safely created in the main thread
    if (QThread::currentThread() != qApp->thread())
    {
        return QStringList();
    }
#endif

    return OpenGLInformation().getExtensions();
}

QString SystemInformation::getGPUOpenGLVersion()
{
#ifdef Q_OS_WIN32
    // In Windows OpenGLInformation can only be safely created in the main thread
    if (QThread::currentThread() != qApp->thread())
    {
        return "0.0";
    }
#endif

    return OpenGLInformation().getVersion();
}

QStringList SystemInformation::getGPUDriverVersion()
{
    return QStringList();
}

QList<QSize> SystemInformation::getScreenResolutions()
{
    QList<QSize> screenResolutions;
    ScreenLayout layout = ScreenManager().getScreenLayout();
    for (int i = 0; i < layout.getNumberOfScreens(); i++)
    {
        screenResolutions.append(layout.getScreen(i).getGeometry().size());
    }
    return screenResolutions;
}

QStringList SystemInformation::getScreenVendors()
{
    return QStringList();
}

QStringList SystemInformation::getHardDiskDevices()
{
    return QStringList();
}

unsigned int SystemInformation::getHardDiskCapacity(const QString &device)
{
    Q_UNUSED(device);
    return 0;
}

quint64 SystemInformation::getHardDiskFreeSpace(const QString &path)
{
    HardDiskInformation hardDiskInformation;
    return hardDiskInformation.getNumberOfFreeMBytes(path);
}

bool SystemInformation::doesOpticalDriveHaveWriteCapabilities()
{
    return false;
}

unsigned int SystemInformation::getNetworkAdapterSpeed()
{
    return 0;
}

bool SystemInformation::isDesktopCompositionAvailable()
{
    return false;
}

bool SystemInformation::isDesktopCompositionEnabled()
{
    return false;
}

QString SystemInformation::getDesktopInformation() const
{
    return QObject::tr("N/A");
}

}
