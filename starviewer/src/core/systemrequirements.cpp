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

// Starviewer
#include "systemrequirements.h"

namespace udg {

SystemRequirements::SystemRequirements()
{
    m_minimumNumberOfCores = 4;
    m_minimumCoreSpeed = 2100; //2.1GHz
    m_minimumGPURAM = 1024; // Mbytes
    m_minimumGPUOpenGLVersion = "3.2";
    m_minimumOSVersion = "6.1"; // 7
    m_minimumServicePackVersion = 1; // 7 SP1
    m_minimumRAM = 4000; // 4Gb
    m_minimumScreenWidth = 1440;

    // Quan s'estableixin quines són les extensions d'openGL que es necessiten per cada cosa, es poden afegir aquí
    //m_minimumOpenGLExtensions << "GL_ARB_flux_capacitor";
    
    m_minimumDiskSpace = 5120; // 5 GB (en principi, el mínim que es necessita per la cache està en un setting)

    m_doesOpticalDriveNeedsToWrite = true;
}

SystemRequirements::~SystemRequirements()
{
}

unsigned int SystemRequirements::getMinimumCPUNumberOfCores()
{
    return m_minimumNumberOfCores;
}

unsigned int SystemRequirements::getMinimumCPUFrequency()
{
    return m_minimumCoreSpeed;
}

QStringList SystemRequirements::getMinimumGPUOpenGLCompatibilities()
{
    return m_minimumOpenGLExtensions;
}

QString SystemRequirements::getMinimumGPUOpenGLVersion()
{
    return m_minimumGPUOpenGLVersion;
}

unsigned int SystemRequirements::getMinimumGPURAM()
{
    return m_minimumGPURAM;
}

unsigned int SystemRequirements::getMinimumHardDiskFreeSpace()
{
    return m_minimumDiskSpace;
}

QString SystemRequirements::getMinimumOperatingSystemVersion()
{
    return m_minimumOSVersion;
}

unsigned int SystemRequirements::getMinimumOperatingSystemServicePackVersion()
{
    return m_minimumServicePackVersion;
}

unsigned int SystemRequirements::getMinimumRAMTotalAmount()
{
    return m_minimumRAM;
}

unsigned int SystemRequirements::getMinimumScreenWidth()
{
    return m_minimumScreenWidth;
}

bool SystemRequirements::doesOpticalDriveNeedWriteCapabilities()
{
    return m_doesOpticalDriveNeedsToWrite;
}

}
