#include "autotest.h"
#include "cachetest.h"
#include "diagnosistestresult.h"

#include <QString>

using namespace udg;

class TestingCacheTest : public CacheTest {
public:
    unsigned int m_freeSpace;
    unsigned int m_minimumFreeSpace;
    bool m_permissions;
    bool m_cacheIsOnDefaultPath;

protected:
    virtual unsigned int getFreeSpace(const QString &cachePath)
    {
        Q_UNUSED(cachePath);
        return m_freeSpace;
    }

    virtual bool doesCacheDirectoryHaveReadWritePermissions(const QString &cachePath)
    {
        Q_UNUSED(cachePath);
        return m_permissions;
    }

    virtual bool isCacheOnDefaultPath()
    {
        return m_cacheIsOnDefaultPath;
    }

    virtual unsigned int getMinimumFreeSpace()
    {
        return m_minimumFreeSpace;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResult)

class test_CacheTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfCacheIsCorrectlyConfigured_data();
    void run_ShouldTestIfCacheIsCorrectlyConfigured();
};

void test_CacheTest::run_ShouldTestIfCacheIsCorrectlyConfigured_data()
{
    /// Entrada
    QTest::addColumn<unsigned int>("testingFreeSpace");
    QTest::addColumn<unsigned int>("testingMinimumFreeSpace");
    QTest::addColumn<bool>("testingPermissions");
    QTest::addColumn<bool>("testingCacheIsOnDefaultPath");

    /// Sortida
    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    /// Variables unused
    QString unusedString = "";
    bool unusedBool = true;

    DiagnosisTestProblem permissionsProblem(DiagnosisTestProblem::Error, "Invalid permissions on the local database directory", "Fix the permissions of the directory or change the local database to other location");
    DiagnosisTestProblem noSpaceProblem(DiagnosisTestProblem::Error, "The free space on the local database location is below the minimum required", "Make some space on disk or change the local database to other location");
    DiagnosisTestProblem defaultPathProblem(DiagnosisTestProblem::Warning, "The local database is not on the default path", "");

    DiagnosisTestResult permissionsResult;
    permissionsResult.addError(permissionsProblem);

    DiagnosisTestResult noSpaceAndPermissionsResult;
    noSpaceAndPermissionsResult.addError(noSpaceProblem);
    noSpaceAndPermissionsResult.addError(permissionsProblem);

    DiagnosisTestResult defaultPathResult;
    defaultPathResult.addWarning(defaultPathProblem);

    /// Tests
    QTest::newRow("ok") << (unsigned int)1 << (unsigned int)0 << true << true << DiagnosisTestResult();
    QTest::newRow("invalid permissions") << (unsigned int)1 << (unsigned int)0 << false << unusedBool << permissionsResult;
    QTest::newRow("not enough free space and invalid permissions") << (unsigned int)0 << (unsigned int)1 << false << unusedBool << noSpaceAndPermissionsResult;
    QTest::newRow("cache not on default path") << (unsigned int)1 << (unsigned int)0 << true << false << defaultPathResult;
}

void test_CacheTest::run_ShouldTestIfCacheIsCorrectlyConfigured()
{
    QFETCH(unsigned int, testingFreeSpace);
    QFETCH(unsigned int, testingMinimumFreeSpace);
    QFETCH(bool, testingPermissions);
    QFETCH(bool, testingCacheIsOnDefaultPath);
 
    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    TestingCacheTest cacheTest;
    
    cacheTest.m_freeSpace = testingFreeSpace;
    cacheTest.m_minimumFreeSpace = testingMinimumFreeSpace;
    cacheTest.m_permissions = testingPermissions;
    cacheTest.m_cacheIsOnDefaultPath = testingCacheIsOnDefaultPath;

    DiagnosisTestResult result = cacheTest.run();

    QCOMPARE(result.getState(), testingDiagnosisTestResult.getState());
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getErrors().size(), testingDiagnosisTestResult.getErrors().size());
        QCOMPARE(result.getWarnings().size(), testingDiagnosisTestResult.getWarnings().size());

        QListIterator<DiagnosisTestProblem> resultIterator(result.getErrors() + result.getWarnings());
        QListIterator<DiagnosisTestProblem> testingResultIterator(testingDiagnosisTestResult.getErrors() + testingDiagnosisTestResult.getWarnings());
        while (resultIterator.hasNext() && testingResultIterator.hasNext())
        {
            DiagnosisTestProblem problem = resultIterator.next();
            DiagnosisTestProblem testingProblem = testingResultIterator.next();

            QCOMPARE(problem.getDescription(), testingProblem.getDescription());
            QCOMPARE(problem.getSolution(), testingProblem.getSolution());
        }
    }
}

DECLARE_TEST(test_CacheTest)

#include "test_cachetest.moc"
