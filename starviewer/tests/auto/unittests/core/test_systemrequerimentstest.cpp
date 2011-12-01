#include "autotest.h"
#include "systemrequerimentstest.h"
#include "diagnosistestresult.h"
#include "systemrequeriments.h"

using namespace udg;

typedef QList<unsigned int> UnsignedIntList;
typedef QList<QString> StringList;
typedef QList<QSize> SizeList;

class TestingSystemRequeriments : public SystemRequeriments {
public:
    // Els requeriments mínims s'especifiquen aquí. S'enten que són constants, així que no s'assignen a cada test, però és necessari
    // tenir-los aquí, per si la classe pare canvia, que el test no es vegi afectat.
    void setRequerimentsForTesting()
    {
        m_minimumNumberOfCores = 4;
        m_minimumCoreSpeed = 2457; //2.4GHz
        m_minimumL2CacheSize = 8192; // Kbytes
        m_minimumGPURAM = 256; // Mbytes
        m_minimumGPUOpenGLVersion = "2.1";
        m_minimumOSVersion = "5.0"; // XP
        m_minimumServicePackVersion = 3; // XP service pack 3
        m_minimumRAM = 4096; // 4Gb
        m_minimumScreenWidth = 1185; // La mínima amplada que pot tenir starviewer (si s'afageixen controls a la pantalla, s'ha de modificar)

        // Quan s'estableixin quines són les extensions d'openGL que es necessiten per cada cosa, es poden afegir aquí
        m_minimumOpenGLExtensions << "GL_ARB_flux_capacitor" << "GL_ARB_half_float_pixel";
        
        m_minimumDiskSpace = 5120; // 5 Gb (en principi, el mínim que es necessita per la cache està en un setting)

        m_doesOpticalDriveNeedsToWrite = true;
        m_doesOperatingSystemNeedToBe64BitArchitecutre = true;
    }
};

class TestingSystemRequerimentsTest : public SystemRequerimentsTest {
public:
    unsigned int m_testingCPUNumberOfCores;
    UnsignedIntList m_testingCPUFrequencies;
    unsigned int m_testingCPUL2CacheSize;
    StringList m_testingGPUOpenGLCompatibilities;
    QString m_testingGPUOpenGLVersion;
    unsigned int m_testingGPURAM;
    unsigned int m_testingHardDiskFreeSpace;
    SystemInformation::OperatingSystem m_testingOperatingSystem;
    QString m_testingOperatingSystemVersion;
    QString m_testingOperatingSystemServicePackVersion;
    bool m_testingIsOperatingSystem64BitArchitecutre;
    unsigned int m_testingRAMTotalAmount;
    SizeList m_testingScreenResolutions;
    bool m_testingWriteCapability;

protected:
    unsigned int getCPUNumberOfCores(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingCPUNumberOfCores;
    }
    QList<unsigned int> getCPUFrequencies(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingCPUFrequencies;
    }
    unsigned int getCPUL2CacheSize(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingCPUL2CacheSize;
    }
    QList<QString> getGPUOpenGLCompatibilities(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPUOpenGLCompatibilities;
    }
    QString getGPUOpenGLVersion(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPUOpenGLVersion;
    }
    unsigned int getGPURAM(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPURAM;
    }
    unsigned int getHardDiskFreeSpace(SystemInformation *system, const QString &device)
    {
        Q_UNUSED(system);
        Q_UNUSED(device);
        return m_testingHardDiskFreeSpace;
    }
    SystemInformation::OperatingSystem getOperatingSystem(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingOperatingSystem;
    }
    QString getOperatingSystemVersion(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingOperatingSystemVersion;
    }
    QString getOperatingSystemServicePackVersion(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingOperatingSystemServicePackVersion;
    }
    bool isOperatingSystem64BitArchitecture(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingIsOperatingSystem64BitArchitecutre;
    }
    unsigned int getRAMTotalAmount(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingRAMTotalAmount;
    }
    QList<QSize> getScreenResolutions(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingScreenResolutions;
    }
    bool doesOpticalDriveHaveWriteCapabilities(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingWriteCapability;
    }

    virtual SystemRequeriments* getSystemRequeriments()
    {
        TestingSystemRequeriments *requeriments = new TestingSystemRequeriments();
        requeriments->setRequerimentsForTesting();
        return requeriments;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)
Q_DECLARE_METATYPE(SystemInformation::OperatingSystem)
Q_DECLARE_METATYPE(UnsignedIntList)
Q_DECLARE_METATYPE(StringList)
Q_DECLARE_METATYPE(SizeList)

class test_SystemRequerimentsTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfSystemHasTheMinimumRequeriments_data();
    void run_ShouldTestIfSystemHasTheMinimumRequeriments();
};

void test_SystemRequerimentsTest::run_ShouldTestIfSystemHasTheMinimumRequeriments_data()
{
    // Entrada
    QTest::addColumn<unsigned int>("testingCPUNumberOfCores");
    QTest::addColumn<UnsignedIntList>("testingCPUFrequencies");
    QTest::addColumn<unsigned int>("testingCPUL2CacheSize");
    QTest::addColumn<StringList>("testingGPUOpenGLCompatibilities");
    QTest::addColumn<QString>("testingGPUOpenGLVersion");
    QTest::addColumn<unsigned int>("testingGPURAM");
    QTest::addColumn<unsigned int>("testingHardDiskFreeSpace");
    QTest::addColumn<SystemInformation::OperatingSystem>("testingOperatingSystem");
    QTest::addColumn<QString>("testingOperatingSystemVersion");
    QTest::addColumn<QString>("testingOperatingSystemServicePackVersion");
    QTest::addColumn<bool>("testingIsOperatingSystem64BitArchitecture");
    QTest::addColumn<unsigned int>("testingRAMTotalAmount");
    QTest::addColumn<SizeList>("testingScreenResolutions");
    QTest::addColumn<bool>("testingWriteCapability");
    // Sortidas
    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    // Variables per determinar que un paràmetre no s'utilitza
    QString unusedString = "";

    // Requeriments millors que els recomenats. Si canvien els recomenats, cal canviar aquests.
    unsigned int cpuNumberOfCores = 4;
    UnsignedIntList cpuFrequencies;
    unsigned int cacheSize = 10240;
    cpuFrequencies << (unsigned int)3000;
    StringList openGLExtensions;
    openGLExtensions << "GL_ARB_over_9000" << "GL_ARB_draw_buffers" << "GL_ARB_fragment_program_shadow" << "GL_ARB_half_float_pixel" << "GL_ARB_flux_capacitor";
    QString openGLVersion = "2.5";
    unsigned int gpuRAM = 500;
    unsigned int hardDiskFreeSpace = 10240;
    SystemInformation::OperatingSystem operatingSystem = SystemInformation::OSWindows;
    QString operatingSystemVersion = "5.1.2600";
    QString servicePackVersion = "Service Pack 3";
    bool isOperatingSystem64BitArchitecture = true;
    unsigned int ramTotalAmount = 80000;
    SizeList screenResolutions;
    screenResolutions << QSize(1900, 1500);
    bool writeCapability = true;

    // per testejar un requeriment unsigned int menor que els recomenats podem fer servir 0
    unsigned int zero = 0;

    // Requeriments mínims
    TestingSystemRequeriments requeriments;
    requeriments.setRequerimentsForTesting();


    QTest::newRow("ok windows") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                << DiagnosisTestResult::Ok << unusedString << unusedString;
    // TODO Si al final en linux no es comprova la versió mínim, aquest test no cal
    QTest::newRow("ok linux") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                              << SystemInformation::OSLinux << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                              << DiagnosisTestResult::Ok << unusedString << unusedString;
    // TODO Comprovar la versió de mac
    QTest::newRow("ok mac") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                            << SystemInformation::OSMac << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                            << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("not enough cores") << zero << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                      << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                      << DiagnosisTestResult::Error
                                      << QString("The machine currently has %1 cores, and the minimum required is %2").arg(zero).arg(requeriments.getMinimumCPUNumberOfCores())
                                      << unusedString;

    UnsignedIntList cpuFrequenciesTooSlow;
    cpuFrequenciesTooSlow << zero;
    QTest::newRow("CPU frequency too slow") << cpuNumberOfCores << cpuFrequenciesTooSlow << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                            << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                            << DiagnosisTestResult::Error
                                            << QString("The fastest CPU runs at %1 and the minimum required is %2").arg(zero).arg(requeriments.getMinimumCPUFrequency())
                                            << unusedString;

    unsigned int cacheTooSmall = 1024;
    QTest::newRow("CPU cache too small") << cpuNumberOfCores << cpuFrequencies << cacheTooSmall << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                         << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                         << DiagnosisTestResult::Error
                                         << QString("The level 2 cache size of the CPU is %1 and the minimum required is %2").arg(cacheTooSmall).arg(requeriments.getMinimumCPUL2CacheSize())
                                         << unusedString;

    StringList missingOpenGLExtensions;
    missingOpenGLExtensions << "GL_ARB_draw_buffers";
    // El resultat és hard coded, si canvia el requeriment, s'ha de canviar la string
    QTest::newRow("missing openGL extensions") << cpuNumberOfCores << cpuFrequencies << cacheSize << missingOpenGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                               << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                               << DiagnosisTestResult::Error
                                               << "Current openGL version does not support GL_ARB_flux_capacitor extension\nCurrent openGL version does not support GL_ARB_half_float_pixel extension"
                                               << unusedString;

    QTest::newRow("Old openGL version") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << "1.0" << gpuRAM << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Error
                                        << QString("Current openGL version is %1 and the minimum required is %2").arg("1.0").arg(requeriments.getMinimumGPUOpenGLVersion())
                                        << unusedString;

    QTest::newRow("not enough GPU RAM") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << zero << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Error
                                        << QString("The graphics card has %1Mb of RAM and the minimum required id %2Mb").arg(zero).arg(requeriments.getMinimumGPURAM())
                                        << unusedString;

    QTest::newRow("not enough space on disk") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << zero
                                              << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                              << DiagnosisTestResult::Error << "There is not enough disk space to run starviewer properly." << unusedString;

    QTest::newRow("windows is 32 bit") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                       << operatingSystem << operatingSystemVersion << servicePackVersion << false << ramTotalAmount << screenResolutions << writeCapability
                                       << DiagnosisTestResult::Error
                                       << QString("Operating system is not 64 bit architecture.")
                                       << unusedString;

    QTest::newRow("windows version error") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                           << operatingSystem << "4.1" << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                           << DiagnosisTestResult::Error
                                           << QString("Current Operative System version is %1 and the minimum required is %2").arg("4.1").arg(requeriments.getMinimumOperatingSystemVersion())
                                           << unusedString;

    QTest::newRow("windows service pack version error") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                                        << operatingSystem << operatingSystemVersion << "Service Pack 1" << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutions << writeCapability
                                                        << DiagnosisTestResult::Error
                                                        << QString("Current Service Pack version is %1 and the minimum required is Service Pack %2").arg("Service Pack 1").arg(requeriments.getMinimumOperatingSystemServicePackVersion())
                                                        << unusedString;

    QTest::newRow("not enough RAM") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                    << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << zero << screenResolutions << writeCapability
                                    << DiagnosisTestResult::Error
                                    << QString("The total amount of RAM memory is %1 and the minimum required is %2").arg(zero).arg(requeriments.getMinimumRAMTotalAmount())
                                    << unusedString;

    SizeList screenResolutionsTooSmall;
    screenResolutionsTooSmall << QSize(640, 480);
    QTest::newRow("screen too small") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                      << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << screenResolutionsTooSmall << writeCapability
                                      << DiagnosisTestResult::Error << "The screens is too small to fit Starviewer application." << unusedString;
    // Warning
    SizeList oneScreenResolutionTooSmall;
    oneScreenResolutionTooSmall << QSize(1440, 900) << QSize(640, 480);
    QTest::newRow("one screen too small") << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                                          << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << oneScreenResolutionTooSmall << writeCapability
                                          << DiagnosisTestResult::Warning << "One of the screens is too small. Keep in mind that Starviewer won't fit in that screen."
                                          << unusedString;

    QTest::newRow("one screen too small, optical drive can not write")
                         << cpuNumberOfCores << cpuFrequencies << cacheSize << openGLExtensions << openGLVersion << gpuRAM << hardDiskFreeSpace
                         << operatingSystem << operatingSystemVersion << servicePackVersion << isOperatingSystem64BitArchitecture << ramTotalAmount << oneScreenResolutionTooSmall << false
                         << DiagnosisTestResult::Warning << "One of the screens is too small. Keep in mind that Starviewer won't fit in that screen.\nThe optical drive is not capable of writing."
                         << unusedString;
}

void test_SystemRequerimentsTest::run_ShouldTestIfSystemHasTheMinimumRequeriments()
{
    QFETCH(unsigned int, testingCPUNumberOfCores);
    QFETCH(UnsignedIntList, testingCPUFrequencies);
    QFETCH(unsigned int, testingCPUL2CacheSize);
    QFETCH(StringList, testingGPUOpenGLCompatibilities);
    QFETCH(QString, testingGPUOpenGLVersion);
    QFETCH(unsigned int, testingGPURAM);
    QFETCH(unsigned int, testingHardDiskFreeSpace);
    QFETCH(SystemInformation::OperatingSystem, testingOperatingSystem);
    QFETCH(QString, testingOperatingSystemVersion);
    QFETCH(QString, testingOperatingSystemServicePackVersion);
    QFETCH(bool, testingIsOperatingSystem64BitArchitecture);
    QFETCH(unsigned int, testingRAMTotalAmount);
    QFETCH(SizeList, testingScreenResolutions);
    QFETCH(bool, testingWriteCapability);

    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingSystemRequerimentsTest systemRequerimentsTest;
    systemRequerimentsTest.m_testingCPUNumberOfCores = testingCPUNumberOfCores;
    systemRequerimentsTest.m_testingCPUFrequencies = testingCPUFrequencies;
    systemRequerimentsTest.m_testingCPUL2CacheSize = testingCPUL2CacheSize;
    systemRequerimentsTest.m_testingGPUOpenGLCompatibilities = testingGPUOpenGLCompatibilities;
    systemRequerimentsTest.m_testingGPUOpenGLVersion = testingGPUOpenGLVersion;
    systemRequerimentsTest.m_testingGPURAM = testingGPURAM;
    systemRequerimentsTest.m_testingHardDiskFreeSpace = testingHardDiskFreeSpace;
    systemRequerimentsTest.m_testingOperatingSystem = testingOperatingSystem;
    systemRequerimentsTest.m_testingOperatingSystemVersion = testingOperatingSystemVersion;
    systemRequerimentsTest.m_testingOperatingSystemServicePackVersion = testingOperatingSystemServicePackVersion;
    systemRequerimentsTest.m_testingIsOperatingSystem64BitArchitecutre = testingIsOperatingSystem64BitArchitecture;
    systemRequerimentsTest.m_testingRAMTotalAmount = testingRAMTotalAmount;
    systemRequerimentsTest.m_testingScreenResolutions = testingScreenResolutions;
    systemRequerimentsTest.m_testingWriteCapability = testingWriteCapability;

    DiagnosisTestResult result = systemRequerimentsTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_SystemRequerimentsTest)

#include "test_systemrequerimentstest.moc"
