// Starviewer
#include "systemrequeriments.h"

namespace udg {

SystemRequeriments::SystemRequeriments()
{
    m_minimumNumberOfCores = 4;
    m_minimumCoreSpeed = 2457; //2.4GHz
    m_minimumL2CacheSize = 8192; // Kbytes
    m_minimumGPURAM = 256; // Mbytes
    m_minimumOSVersion = "5.0"; // XP
    m_minimumServicePackVersion = 3; // XP service pack 3
    m_minimumRAM = 4096; // 4Gb
    m_minimumScreenWidth = 1185; // La mínima amplada que pot tenir starviewer (si s'afageixen controls a la pantalla, s'ha de modificar)

    // Quan s'estableixin quines són les extensions d'openGL que es necessiten per cada cosa, es poden afegir aquí
    //m_minimumOpenGLExtensions << "GL_ARB_flux_capacitor";
    
    m_minimumDiskSpace = 5120; // 5 Gb (en principi, el mínim que es necessita per la cache està en un setting)

    m_doesOpticalDriveNeedsToWrite = true;
}

SystemRequeriments::~SystemRequeriments()
{
}

unsigned int SystemRequeriments::getMinimumCPUNumberOfCores()
{
    return m_minimumNumberOfCores;
}

unsigned int SystemRequeriments::getMinimumCPUFrequency()
{
    return m_minimumCoreSpeed;
}

unsigned int SystemRequeriments::getMinimumCPUL2CacheSize()
{
    return m_minimumL2CacheSize;
}

QList<QString> SystemRequeriments::getMinimumGPUOpenGLCompatibilities()
{
    return m_minimumOpenGLExtensions;
}

unsigned int SystemRequeriments::getMinimumGPURAM()
{
    return m_minimumGPURAM;
}

unsigned int SystemRequeriments::getMinimumHardDiskFreeSpace()
{
    return m_minimumDiskSpace;
}

QString SystemRequeriments::getMinimumOperatingSystemVersion()
{
    return m_minimumOSVersion;
}

unsigned int SystemRequeriments::getMinimumOperatingSystemServicePackVersion()
{
    return m_minimumServicePackVersion;
}

unsigned int SystemRequeriments::getMinimumRAMTotalAmount()
{
    return m_minimumRAM;
}

unsigned int SystemRequeriments::getMinimumScreenWidth()
{
    return m_minimumScreenWidth;
}

bool SystemRequeriments::doesOpticalDriveNeedWriteCapabilities()
{
    return m_doesOpticalDriveNeedsToWrite;
}

}
