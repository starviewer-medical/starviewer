// Starviewer
#include "systemrequerimentstest.h"
#include "systemrequeriments.h"

// Qt
#include <QString>
#include <QList>
#include <QStringList>
#include <QSize>

namespace udg {

SystemRequerimentsTest::SystemRequerimentsTest(QObject *parent)
 : DiagnosisTest(parent)
{   
}

SystemRequerimentsTest::~SystemRequerimentsTest()
{
}

DiagnosisTestResult SystemRequerimentsTest::run()
{
    /// Requeriments mínims
    SystemRequeriments *requeriments = getSystemRequeriments();
    const unsigned int MinimumNumberOfCores = requeriments->getMinimumCPUNumberOfCores();
    const unsigned int MinimumCoreSpeed = requeriments->getMinimumCPUFrequency();
    const unsigned int MinimumL2CacheSize = requeriments->getMinimumCPUL2CacheSize();
    const unsigned int MinimumGPURAM = requeriments->getMinimumGPURAM();
    const QList<QString> MinimumGPUOpenGLExtensions = requeriments->getMinimumGPUOpenGLCompatibilities();
    const QString MinimumGPUOpenGLVersion = requeriments->getMinimumGPUOpenGLVersion();
    const QString MinimumOSVersion = requeriments->getMinimumOperatingSystemVersion();
    const unsigned int MinimumServicePackVersion = requeriments->getMinimumOperatingSystemServicePackVersion();
    const unsigned int MinimumRAM = requeriments->getMinimumRAMTotalAmount();
    const unsigned int MinimumScreenWidth = requeriments->getMinimumScreenWidth();
    const unsigned int MinimumDiskSpace = requeriments->getMinimumHardDiskFreeSpace();

    // TODO Temporal, s'ha de treure i veure com obtenir la unitat on està starviewer
    const QString whichHardDisk = "C:";


    SystemInformation *system = SystemInformation::newInstance();
    QStringList description;
    DiagnosisTestResult::DiagnosisTestResultState state = DiagnosisTestResult::Ok;
    QStringList solution;
    
    /// Requeriments mínims del sistema:
    // Per exemple: Dual core 1.5Ghz
    unsigned int numberOfCores = getCPUNumberOfCores(system);
    if (numberOfCores < MinimumNumberOfCores)
    {
        state = DiagnosisTestResult::Error;
        description << tr("The machine currently has %1 cores, and the minimum required is %2").arg(numberOfCores).arg(MinimumNumberOfCores);
        solution << tr("Update computer's hardware");
    }
    else
    {
        // Una màquina pot tenir més d'una CPU.
        // Buscar si alguna té una velocitat superior al mínim
        QList<unsigned int> cpuFrequencies = getCPUFrequencies(system);
        unsigned int maximumCPUFrequency = 0;
        for (int i = 0; i < cpuFrequencies.count(); i++)
        {
            if (cpuFrequencies.at(i) > maximumCPUFrequency)
            {
                maximumCPUFrequency = cpuFrequencies.at(i);
            }
        }
        
        if (maximumCPUFrequency < MinimumCoreSpeed)
        {
            state = DiagnosisTestResult::Error;
            description << tr("The fastest CPU runs at %1 and the minimum required is %2").arg(maximumCPUFrequency).arg(MinimumCoreSpeed);
            solution << tr("Update computer's hardware");
        }
        // Cache de nivell 2
        unsigned int cacheSize = getCPUL2CacheSize(system);
        if (cacheSize < MinimumL2CacheSize)
        {
            state = DiagnosisTestResult::Error;
            description << tr("The level 2 cache size of the CPU is %1 and the minimum required is %2").arg(cacheSize).arg(MinimumL2CacheSize);
            solution << tr("Update computer's hardware");
        }
    }

    // Comprovar si la versió d'openGL del sistema és suficient
    QString openGLVersion = getGPUOpenGLVersion(system);
    if (compareVersions(openGLVersion, MinimumGPUOpenGLVersion) == SystemRequerimentsTest::Older)
    {
        state = DiagnosisTestResult::Error;
        description << tr("Current openGL version is %1 and the minimum required is %2").arg(openGLVersion).arg(MinimumGPUOpenGLVersion);
        // Normalment la versió d'openGL s'actualitza amb els drivers de la gràfica
        solution << tr("Update your graphics card driver");
    }

    // Tenir en una llista les compatibilitats openGL que starviewer utilitza i anar-les buscant una a una al retorn del mètode
    QList<QString> openGLExtensions = getGPUOpenGLCompatibilities(system);
    bool solutionGiven = false;
    for (int i = 0; i < MinimumGPUOpenGLExtensions.count(); i++)
    {
        if (!openGLExtensions.contains(MinimumGPUOpenGLExtensions.at(i)))
        {
            state = DiagnosisTestResult::Error;
            description << tr("Current openGL version does not support %1 extension").arg(MinimumGPUOpenGLExtensions.at(i));
            if (!solutionGiven)
            {
                // En cas de que l'extensió no es suporti es pot instalar algun paquet d'extensions (GLEW, ...), o actualitzar la versió
                solution << tr("Update your graphics card driver");
                solutionGiven = true;
            }
            else
            {
                // Per intentar mantenir que cada descripció tingui una solució, i no repetir la mateixa cada vegada, ho deixem buit.
                solution << "";
            }
        }
    }
    // Memòria RAM de la GPU
    QList<unsigned int> gpuRAM = getGPURAM(system);
    QList<QString> gpuModel = getGPUModel(system);
    for (int i = 0; i < gpuRAM.count(); i++)
    {
        if (gpuRAM.at(i) < MinimumGPURAM)
        {
            state = DiagnosisTestResult::Error;
            description << tr("The graphics card %1 has %2Mb of RAM and the minimum required is %3Mb").arg(gpuModel.at(i)).arg(gpuRAM.at(i)).arg(MinimumGPURAM);
            solution << tr("Change the graphics card");
        }
    }
    
    // TODO Disc dur. S'ha de fer també del que conté el directori de la cache????????
    if (getHardDiskFreeSpace(system, whichHardDisk) < MinimumDiskSpace)
    {
        state = DiagnosisTestResult::Error;
        description << tr("There is not enough disk space to run starviewer properly.");
        solution << tr("Free some space in the hard disk");
    }

    // Arquitectura de la màquina (32 o 64 bits)
    if (requeriments->doesOperatingSystemNeedToBe64BitArchitecutre() && !isOperatingSystem64BitArchitecture(system))
    {
        state = DiagnosisTestResult::Error;
        description << tr("Operating system is not 64 bit architecture.");
        solution << tr("Update operating system to a 64 bit version");
    }

    // Versió del sistema operatiu
    QString version;
    QString servicePack;
    switch (getOperatingSystem(system))
    {
        case SystemInformation::OSWindows:
            // Si el SO és windows, quina ha de ser la mínima versió??
            version = getOperatingSystemVersion(system);
            if (compareVersions(version, MinimumOSVersion) == SystemRequerimentsTest::Older)
            {
                state = DiagnosisTestResult::Error;
                description << tr("Current Operative System version is %1 and the minimum required is %2").arg(version).arg(MinimumOSVersion);
                solution << tr("Update operating system to a newer version");
            }
            // Si és windows XP (versió 5.xx), s'ha de comprovar el service pack
            if (version.split(".").at(0).toInt() == 5)
            {
                servicePack = getOperatingSystemServicePackVersion(system);
                if (servicePack.right(servicePack.count() - 13).toUInt() < MinimumServicePackVersion)
                {
                    state = DiagnosisTestResult::Error;
                    description << tr("Current Service Pack version is %1 and the minimum required is Service Pack %2").arg(servicePack).arg(MinimumServicePackVersion);
                    solution << tr("Install a newer service pack");
                }
            }
            break;
        case SystemInformation::OSLinux:
            version = getOperatingSystemVersion(system);
            break;
        case SystemInformation::OSMac:
            version = getOperatingSystemVersion(system);
            break;
        default:
            break;
    }

    // Memòria RAM
    unsigned int RAMTotalAmount = getRAMTotalAmount(system);
    if (RAMTotalAmount < MinimumRAM)
    {
        state = DiagnosisTestResult::Error;
        description << tr("The total amount of RAM memory is %1 and the minimum required is %2").arg(RAMTotalAmount).arg(MinimumRAM);
        solution << tr("Add more RAM memory to the computer");
    }

    // Si alguna de les pantalles és menor de 1185 pixels d'amplada, poder retornar un warning, ja que starviewer no hi cap.
    QList<QSize> resolutions = getScreenResolutions(system);
    QStringList screensInWhichStarviewerWontFit;
    bool starviewerWillFitInOneScreen = false;
    for (int i = 0; i < resolutions.count(); i++)
    {
        if (resolutions.at(i).width() < (int)MinimumScreenWidth)
        {
            // i + 1, ja que les pantalles a la interfície es mostren de 1 a n
            screensInWhichStarviewerWontFit << QString::number(i + 1);
        }
        else
        {
            starviewerWillFitInOneScreen = true;
        }
    }

    if (starviewerWillFitInOneScreen)
    {
        // Només retornarem warning, si s'escau, en el cas de què la resta de requeriments es compleixin.
        if (screensInWhichStarviewerWontFit.count() > 0 && state == DiagnosisTestResult::Ok)
        {
            // Warning
            state = DiagnosisTestResult::Warning;
            description << tr("One of the screens is too small. Keep in mind that Starviewer won't fit in that screen.");
            solution << tr("Don't move Starviewer to screen/s %1, or change to a higher resolution").arg(screensInWhichStarviewerWontFit.join(", "));
        }
        // else OK
    }
    else
    {
        // ERROR
        state = DiagnosisTestResult::Error;
        solution << tr("Change to a higher resolution");
        if (resolutions.count() == 1)
        {
            description << tr("The screen is too small to fit Starviewer application.");
        }
        else
        {
            description << tr("The screens are too small to fit Starviewer application.");
        }
    }


    // Que la unitat de CD/DVD no pugui grabar, serà un warning si la resta de requeriments és correcte
    if (state != DiagnosisTestResult::Error &&
        requeriments->doesOpticalDriveNeedWriteCapabilities() &&
        !doesOpticalDriveHaveWriteCapabilities(system))
    {
        state = DiagnosisTestResult::Warning;
        description << tr("The optical drive is not capable of writing.");
        solution << tr("Change the optical drive to a CD-RW/DVD-RW");
    }

    DiagnosisTestResult result;
    result.setState(state);
    result.setDescription(description.join("\n"));
    result.setSolution(solution.join("\n"));
    delete system;
    return result;
}

SystemRequerimentsTest::VersionComparison SystemRequerimentsTest::compareVersions(QString version1, QString version2)
{
    // TODO de moment només estar pensat per windows. S'ha d'estendre a més S.O.
    
    //5.1.2600
    QStringList version1List = version1.split(".");
    QStringList version2List = version2.split(".");

    int index = 0;
    // Mentre les parts siguin iguals, anem comparant
    while (index < version1List.count() && index < version2List.count())
    {
        if (version1List.at(index).toInt() < version2List.at(index).toInt())
        {
            return SystemRequerimentsTest::Older;
        }
        else if (version1List.at(index).toInt() > version2List.at(index).toInt())
        {
            return SystemRequerimentsTest::Newer;
        }
        index++;
    }

    // Si totes les parts són iguals, la versió que en tingui més serà la major, ja que suposarem que la altra és .0
    if (version1List.count() < version2List.count())
    {
        return SystemRequerimentsTest::Older;
    }
    else if (version1List.count() > version2List.count())
    {
        return SystemRequerimentsTest::Newer;
    }

    return SystemRequerimentsTest::Same;
}

unsigned int SystemRequerimentsTest::getCPUNumberOfCores(SystemInformation *system)
{
    return system->getCPUNumberOfCores();
}

QList<unsigned int> SystemRequerimentsTest::getCPUFrequencies(SystemInformation *system)
{
    return system->getCPUFrequencies();
}

unsigned int SystemRequerimentsTest::getCPUL2CacheSize(SystemInformation *system)
{
    return system->getCPUL2CacheSize();
}

QList<QString> SystemRequerimentsTest::getGPUOpenGLCompatibilities(SystemInformation *system)
{
    return system->getGPUOpenGLCompatibilities();
}

QString SystemRequerimentsTest::getGPUOpenGLVersion(SystemInformation *system)
{
    return system->getGPUOpenGLVersion();
}

QList<unsigned int> SystemRequerimentsTest::getGPURAM(SystemInformation *system)
{
    return system->getGPURAM();
}

QList<QString> SystemRequerimentsTest::getGPUModel(SystemInformation *system)
{
    return system->getGPUModel();
}

unsigned int SystemRequerimentsTest::getHardDiskFreeSpace(SystemInformation *system, const QString &device)
{
    return system->getHardDiskFreeSpace(device);
}

SystemInformation::OperatingSystem SystemRequerimentsTest::getOperatingSystem(SystemInformation *system)
{
    return system->getOperatingSystem();
}

QString SystemRequerimentsTest::getOperatingSystemVersion(SystemInformation *system)
{
    return system->getOperatingSystemVersion();
}

QString SystemRequerimentsTest::getOperatingSystemServicePackVersion(SystemInformation *system)
{
    return system->getOperatingSystemServicePackVersion();
}

bool SystemRequerimentsTest::isOperatingSystem64BitArchitecture(SystemInformation *system)
{
    return system->isOperatingSystem64BitArchitecture();
}

unsigned int SystemRequerimentsTest::getRAMTotalAmount(SystemInformation *system)
{
    return system->getRAMTotalAmount();
}

QList<QSize> SystemRequerimentsTest::getScreenResolutions(SystemInformation *system)
{
    return system->getScreenResolutions();
}

bool SystemRequerimentsTest::doesOpticalDriveHaveWriteCapabilities(SystemInformation *system)
{
    return system->doesOpticalDriveHaveWriteCapabilities();
}

SystemRequeriments* SystemRequerimentsTest::getSystemRequeriments()
{
    return new SystemRequeriments();
}

}
