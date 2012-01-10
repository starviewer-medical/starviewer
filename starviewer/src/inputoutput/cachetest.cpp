#include "cachetest.h"
#include "inputoutputsettings.h"
#include "systeminformation.h"
#include "starviewerapplication.h"

#include <QFileInfo>
#include <QStringList>

namespace udg {

CacheTest::CacheTest(QObject *parent)
 : DiagnosisTest(parent)
{
}

CacheTest::~CacheTest()
{
}

DiagnosisTestResult CacheTest::run()
{
    DiagnosisTestResult::DiagnosisTestResultState testResultState = DiagnosisTestResult::Ok;
    QStringList testResultDescription;
    QStringList testResultSolution;

    Settings settings;
    QString cachePath = settings.getValue(InputOutputSettings::CachePath).toString();    
    
    /// Comprovar l'espai lliure al disc dur on hi ha la cache
    unsigned int freeSpace = getFreeSpace(cachePath);
    unsigned int minimumFreeSpace = getMinimumFreeSpace();
    if (freeSpace / 1024.0f < minimumFreeSpace)
    {
        testResultState = DiagnosisTestResult::Error;
        testResultDescription << tr("The free space on the cache directory is below the minimum required");
        testResultSolution << tr("Make some space on disk");
    }

    /// Comprovar els permisos de lectura i escriptura a la carpeta de la cache
    if (!doesCacheDirectoryHaveReadWritePermissions(cachePath))
    {
        testResultState = DiagnosisTestResult::Error;
        testResultDescription << tr("Invalid permissions on the cache directory");
        testResultSolution << tr("Change the cache path or the permissions of the directory");
    }

    /// De moment, en el cas de que no hi hagi error, mirarem lo del warning
    /// Comprovar si la caché està, o no, al path per defecte
    if (testResultState != DiagnosisTestResult::Error && !isCacheOnDefaultPath())
    {
        testResultState = DiagnosisTestResult::Warning;
        testResultDescription << tr("The cache is not on the default path");
    }
    
    return DiagnosisTestResult(testResultState, testResultDescription.join("\n"), testResultSolution.join("\n"));
}

QString CacheTest::getDescription()
{
    return tr("%1 cache is correctly configured").arg(ApplicationNameString);
}

unsigned int CacheTest::getFreeSpace(const QString &cachePath)
{
    SystemInformation *system = SystemInformation::newInstance();
    unsigned int freeSpace = system->getHardDiskFreeSpace(cachePath.left(2));
    delete system;

    return freeSpace;
}

bool CacheTest::doesCacheDirectoryHaveReadWritePermissions(const QString &cachePath)
{
    QFileInfo file(cachePath);
    return file.isReadable() && file.isWritable();
}

bool CacheTest::isCacheOnDefaultPath()
{
    return true;
}

unsigned int CacheTest::getMinimumFreeSpace()
{
    Settings settings;
    return settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toUInt(); 
}

} // end namespace udg
