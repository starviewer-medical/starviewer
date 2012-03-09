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
    DiagnosisTestResult problems;

    Settings settings;
    QString cachePath = settings.getValue(InputOutputSettings::CachePath).toString();    
    
    /// Comprovar l'espai lliure al disc dur on hi ha la cache
    unsigned int freeSpace = getFreeSpace(cachePath);
    unsigned int minimumFreeSpace = getMinimumFreeSpace();
    if (freeSpace / 1024.0f < minimumFreeSpace)
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Error);
        problem.setDescription(tr("The free space on the local database location is below %1GB minimum requireds").arg(minimumFreeSpace));
        problem.setSolution(tr("Make some space on disk or change the local database to other location"));
        problems.addError(problem);
    }

    /// Comprovar els permisos de lectura i escriptura a la carpeta de la cache
    if (!doesCacheDirectoryHaveReadWritePermissions(cachePath))
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Error);
        problem.setDescription(tr("Invalid permissions on the local database directory"));
        problem.setSolution(tr("Fix the permissions of the directory or change the local database to other location"));
        problems.addError(problem);
    }

    /// De moment, en el cas de que no hi hagi error, mirarem lo del warning
    /// Comprovar si la caché està, o no, al path per defecte
    if (!isCacheOnDefaultPath())
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Warning);
        problem.setDescription(tr("The local database is not on the default path"));
        problem.setSolution("");
        problems.addWarning(problem);
    }
    
    return problems;
}

QString CacheTest::getDescription()
{
    return tr("%1 local database is correctly configured").arg(ApplicationNameString);
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
