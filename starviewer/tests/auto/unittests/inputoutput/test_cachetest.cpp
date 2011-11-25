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

Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)

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
    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    /// Variables unused
    QString unusedString = "";
    bool unusedBool = true;

    /// Tests
    QTest::newRow("ok") << (unsigned int)1 << (unsigned int)0 << true << true
                        << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("invalid permissions") << (unsigned int)1 << (unsigned int)0 << false << unusedBool
                                         << DiagnosisTestResult::Error 
                                         << "Invalid permissions on the cache directory"
                                         << "Change the cache path or the permissions of the directory";

    QTest::newRow("not enough free space and invalid permissions")
        << (unsigned int)0 << (unsigned int)1 << false << unusedBool
        << DiagnosisTestResult::Error
        << "The free space on the cache directory is below the minimum required\nInvalid permissions on the cache directory" 
        << "Make some space on disk\nChange the cache path or the permissions of the directory";

    QTest::newRow("cache not on default path") << (unsigned int)1 << (unsigned int)0 << true << false
                                               << DiagnosisTestResult::Warning << "The cache is not on the default path" << unusedString;
}

void test_CacheTest::run_ShouldTestIfCacheIsCorrectlyConfigured()
{
    QFETCH(unsigned int, testingFreeSpace);
    QFETCH(unsigned int, testingMinimumFreeSpace);
    QFETCH(bool, testingPermissions);
    QFETCH(bool, testingCacheIsOnDefaultPath);
 
    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingCacheTest cacheTest;
    
    cacheTest.m_freeSpace = testingFreeSpace;
    cacheTest.m_minimumFreeSpace = testingMinimumFreeSpace;
    cacheTest.m_permissions = testingPermissions;
    cacheTest.m_cacheIsOnDefaultPath = testingCacheIsOnDefaultPath;

    DiagnosisTestResult result = cacheTest.run();

    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState()!= DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_CacheTest)

#include "test_cachetest.moc"
