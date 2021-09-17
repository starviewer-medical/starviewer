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

#include "linuxsysteminformation.h"

#include "logging.h"

#include <QFile>
#include <QProcess>
#include <QRegularExpression>

#include <sys/sysinfo.h>

namespace udg {

namespace {

QString parseGlxinfo(const QString &pattern)
{
    // We could use inxi but it resorts to glxinfo, so better just use that directly
    // TODO probably this information could be extracted directly from OpenGL
    //      https://www.khronos.org/registry/OpenGL/extensions/MESA/GLX_MESA_query_renderer.txt
    //      https://cgit.freedesktop.org/mesa/demos/tree/src/xdemos/glxinfo.c
    //      GLX_RENDERER_DEVICE_ID_MESA --(string)-> Radeon RX 570 Series (POLARIS10, DRM 3.41.0, 5.13.14-200.fc34.x86_64, LLVM 12.0.1)
    //      GLX_RENDERER_VERSION_MESA ~~(int)~> 21.1.7 [array of 3 ints]
    //      GLX_RENDERER_VIDEO_MEMORY_MESA --(int)-> 4096
    //      GLX_RENDERER_OPENGL_CORE_PROFILE_VERSION_MESA ~~(int)~> 4.6 [array of 2 ints]
    // TODO what happens when there is more than one GPU?
    QProcess glxinfo;
    glxinfo.start("glxinfo", {"-B"}, QIODevice::ReadOnly | QIODevice::Text);

    if (!glxinfo.waitForFinished())
    {
        WARN_LOG("glxinfo failed");
        return {};
    }

    QString glxinfoOutput = glxinfo.readAll();
    QRegularExpression regexp(pattern);
    QRegularExpressionMatch match = regexp.match(glxinfoOutput);

    if (match.hasMatch())
    {
        return match.captured(1);
    }
    else
    {
        WARN_LOG(QString("Regular expression \"%1\" does not match. glxinfo output: %2").arg(regexp.pattern()).arg(glxinfoOutput));
        return QString();
    }
}

}

unsigned int LinuxSystemInformation::getRAMTotalAmount()
{
    struct sysinfo systemInformation;

    if (sysinfo(&systemInformation) == 0)
    {
        unsigned long totalRam = systemInformation.totalram;    // RAM size as a multiple of unit (i.e. totalRam * unit = size in bytes)
        unsigned int unit = systemInformation.mem_unit;         // unit size in bytes
        double factor = unit / (1024.0 * 1024.0);
        return totalRam * factor;
    }
    else
    {
        WARN_LOG(QString("Error in sysinfo: %1").arg(strerror(errno)));
        return 0;
    }
}

QList<unsigned int> LinuxSystemInformation::getCPUFrequencies()
{
    QList<unsigned int> frequencies;
    QString frequencyPath("/sys/devices/system/cpu/cpu%1/cpufreq/scaling_max_freq");
    QString cpusListPath("/sys/devices/system/cpu/cpu%1/topology/package_cpus_list");
    uint cpu = 0;

    QFile frequencyFile(frequencyPath.arg(cpu));

    while (frequencyFile.exists())
    {
        if (frequencyFile.open(QFile::ReadOnly | QFile::Text))
        {
            QString frequencyInKHz = frequencyFile.readAll();   // example: 3400000
            frequencyFile.close();
            unsigned int frequencyInMHz = frequencyInKHz.toUInt() / 1000;
            frequencies.append(frequencyInMHz);

            QFile cpusListFile(cpusListPath.arg(cpu));

            if (cpusListFile.exists())
            {
                if (cpusListFile.open(QFile::ReadOnly | QFile::Text))
                {
                    QString cpusList = cpusListFile.readAll();  // example: 0-31
                    cpusListFile.close();

                    if (cpusList.contains(','))
                    {
                        WARN_LOG(QString("Unexpected CPU topology. %1 -> %2").arg(cpusListFile.fileName()).arg(cpusList));
                        break;
                    }

                    uint lastCpuInPackage = cpusList.split('-').last().toUInt();
                    frequencyFile.setFileName(frequencyPath.arg(lastCpuInPackage + 1)); // the next call to exists will determine if there are more CPUs
                }
                else
                {
                    WARN_LOG(QString("Can't read file %1").arg(cpusListFile.fileName()));
                    break;
                }
            }
        }
        else
        {
            WARN_LOG(QString("Can't read file %1").arg(frequencyFile.fileName()));
            break;
        }
    }

    if (frequencies.isEmpty())
    {
        WARN_LOG("Could not detect any CPU frequency");
    }

    return frequencies;
}

QStringList LinuxSystemInformation::getGPUModel()
{
    return {parseGlxinfo(R"(Device: (.*) \(0x[0-9a-f]+\)\n)")};
}

QList<unsigned int> LinuxSystemInformation::getGPURAM()
{
    return {parseGlxinfo(R"(Video memory: (\d+)MB\n)").toUInt()};
}

QStringList LinuxSystemInformation::getGPUDriverVersion()
{
    return {parseGlxinfo(R"(Version: (.*)\n)")};
}

bool LinuxSystemInformation::doesOpticalDriveHaveWriteCapabilities()
{
    QFile cdromInfoFile("/proc/sys/dev/cdrom/info");

    if (!cdromInfoFile.exists())
    {
        return false;
    }

    if (cdromInfoFile.open(QFile::ReadOnly | QFile::Text))
    {
        QString cdromInfo = cdromInfoFile.readAll();
        cdromInfoFile.close();
        return cdromInfo.contains(QRegularExpression("Can write.*1"));
    }
    else
    {
        WARN_LOG(QString("Can't read file %1").arg(cdromInfoFile.fileName()));
        return false;
    }
}

QString LinuxSystemInformation::getDesktopInformation() const
{
    QString desktop, display;
    bool found = false;

    // First, try if the inxi tool is available
    QProcess inxi;
    inxi.start("inxi", {"-G", "-S", "-y", "1", "-c", "0"}, QIODevice::ReadOnly | QIODevice::Text);

    if (inxi.waitForFinished())
    {
        QString inxiOutput = inxi.readAll();
        QRegularExpression regexp("Desktop: ([^\n]*)\n.*Display: ([^\n]*)\n", QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch match = regexp.match(inxiOutput);

        if (match.hasMatch())
        {
            desktop = match.captured(1);
            display = match.captured(2);
            found = true;
        }
        else
        {
            WARN_LOG(QString("Regular expression does not match. inxi output: %1").arg(inxiOutput));
        }
    }

    // If inxi fails, try to read environment variables
    if (!found)
    {
        if (qEnvironmentVariableIsSet("XDG_CURRENT_DESKTOP"))
        {
            desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
        }
        else if (qEnvironmentVariableIsSet("XDG_SESSION_DESKTOP"))
        {
            desktop = qEnvironmentVariable("XDG_SESSION_DESKTOP");
        }
        else
        {
            desktop = QObject::tr("desktop environment not detected");
        }

        if (qEnvironmentVariableIsSet("XDG_SESSION_TYPE"))
        {
            display = qEnvironmentVariable("XDG_SESSION_TYPE");
        }
        else
        {
            if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY"))
            {
                display = "Wayland";
            }
            else if (qEnvironmentVariableIsSet("DISPLAY"))
            {
                display = "X11";
            }
            else
            {
                display = QObject::tr("display type not detected");
            }
        }
    }

    return QString("%1 (%2)").arg(desktop).arg(display);
}

} // namespace udg
