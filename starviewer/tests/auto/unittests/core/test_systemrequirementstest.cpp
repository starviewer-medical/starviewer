/*@
    "name": "test_SystemRequerimentsTest",
    "requirements": ["other_features.diagnosis_tests"]
 */

#include "autotest.h"
#include "systemrequirementstest.h"
#include "diagnosistestresult.h"
#include "systemrequirements.h"

using namespace udg;

typedef QList<unsigned int> UnsignedIntList;
typedef QList<QSize> SizeList;

class TestingSystemRequirements : public SystemRequirements {
public:
    // Els requeriments mínims s'especifiquen aquí. S'enten que són constants, així que no s'assignen a cada test, però és necessari
    // tenir-los aquí, per si la classe pare canvia, que el test no es vegi afectat.
    void setRequirementsForTesting()
    {
        m_minimumNumberOfCores = 4;
        m_minimumCoreSpeed = 2457; //2.4GHz
        m_minimumGPURAM = 256; // Mbytes
        m_minimumGPUOpenGLVersion = "2.1";
        m_minimumOSVersion = "5.0"; // XP
        m_minimumServicePackVersion = 2; // XP service pack 2
        m_minimumRAM = 4096; // 4Gb
        m_minimumScreenWidth = 1185; // La mínima amplada que pot tenir starviewer (si s'afageixen controls a la pantalla, s'ha de modificar)

        // Quan s'estableixin quines són les extensions d'openGL que es necessiten per cada cosa, es poden afegir aquí
        m_minimumOpenGLExtensions << "GL_ARB_flux_capacitor" << "GL_ARB_half_float_pixel";
        
        m_minimumDiskSpace = 5120; // 5 GB (en principi, el mínim que es necessita per la cache està en un setting)

        m_doesOpticalDriveNeedsToWrite = true;
    }
};

class TestingSystemRequirementsTest : public SystemRequirementsTest {
public:
    unsigned int m_testingCPUNumberOfCores;
    UnsignedIntList m_testingCPUFrequencies;
    QStringList m_testingGPUOpenGLCompatibilities;
    QString m_testingGPUOpenGLVersion;
    UnsignedIntList m_testingGPURAM;
    QStringList m_testingGPUModel;
    unsigned int m_testingHardDiskFreeSpace;
    SystemInformation::OperatingSystem m_testingOperatingSystem;
    QString m_testingOperatingSystemVersion;
    QString m_testingOperatingSystemServicePackVersion;
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
    QStringList getGPUOpenGLCompatibilities(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPUOpenGLCompatibilities;
    }
    QString getGPUOpenGLVersion(SystemRequirements *requirements)
    {
        Q_UNUSED(requirements);
        return m_testingGPUOpenGLVersion;
    }
    QList<unsigned int> getGPURAM(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPURAM;
    }
    QStringList getGPUModel(SystemInformation *system)
    {
        Q_UNUSED(system);
        return m_testingGPUModel;
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

    virtual SystemRequirements* getSystemRequirements()
    {
        TestingSystemRequirements *requirements = new TestingSystemRequirements();
        requirements->setRequirementsForTesting();
        return requirements;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)
Q_DECLARE_METATYPE(SystemInformation::OperatingSystem)
Q_DECLARE_METATYPE(UnsignedIntList)
Q_DECLARE_METATYPE(SizeList)

class test_SystemRequirementsTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfSystemHasTheMinimumRequirements_data();
    void run_ShouldTestIfSystemHasTheMinimumRequirements();
};

void test_SystemRequirementsTest::run_ShouldTestIfSystemHasTheMinimumRequirements_data()
{
    // Entrada
    QTest::addColumn<unsigned int>("testingCPUNumberOfCores");
    QTest::addColumn<UnsignedIntList>("testingCPUFrequencies");
    QTest::addColumn<QStringList>("testingGPUOpenGLCompatibilities");
    QTest::addColumn<QString>("testingGPUOpenGLVersion");
    QTest::addColumn<UnsignedIntList>("testingGPURAM");
    QTest::addColumn<QStringList>("testingGPUModel");
    QTest::addColumn<unsigned int>("testingHardDiskFreeSpace");
    QTest::addColumn<SystemInformation::OperatingSystem>("testingOperatingSystem");
    QTest::addColumn<QString>("testingOperatingSystemVersion");
    QTest::addColumn<QString>("testingOperatingSystemServicePackVersion");
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
    cpuFrequencies << (unsigned int)3000;
    QStringList openGLExtensions;
    openGLExtensions << "GL_ARB_over_9000" << "GL_ARB_draw_buffers" << "GL_ARB_fragment_program_shadow" << "GL_ARB_half_float_pixel" << "GL_ARB_flux_capacitor";
    QString openGLVersion = "2.5";
    UnsignedIntList gpuRAM;
    gpuRAM << (unsigned int)500;
    QStringList gpuModel;
    gpuModel << "NVidia GeForce GTX 470";
    unsigned int hardDiskFreeSpace = 10240;
    SystemInformation::OperatingSystem operatingSystem = SystemInformation::OSWindows;
    QString operatingSystemVersion = "6.1.2600";
    QString servicePackVersion = "Service Pack 3";
    unsigned int ramTotalAmount = 80000;
    SizeList screenResolutions;
    screenResolutions << QSize(1900, 1500);
    bool writeCapability = true;

    // per testejar un requeriment unsigned int menor que els recomenats podem fer servir 0
    unsigned int zero = 0;

    // Requeriments mínims
    TestingSystemRequirements requirements;
    requirements.setRequirementsForTesting();


    QTest::newRow("ok windows") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                << DiagnosisTestResult::Ok << unusedString << unusedString;
    // TODO Si al final en linux no es comprova la versió mínim, aquest test no cal
    QTest::newRow("ok linux") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                              << SystemInformation::OSLinux << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                              << DiagnosisTestResult::Ok << unusedString << unusedString;
    // TODO Comprovar la versió de mac
    QTest::newRow("ok mac") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                            << SystemInformation::OSMac << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                            << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("not enough cores") << zero << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                      << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                      << DiagnosisTestResult::Error
                                      << QString("The computer has %1 cores, and the minimum required is %2").arg(zero).arg(requirements.getMinimumCPUNumberOfCores())
                                      << "Update computer's hardware";

    UnsignedIntList cpuFrequenciesTooSlow;
    cpuFrequenciesTooSlow << zero;
    QTest::newRow("CPU frequency too slow") << cpuNumberOfCores << cpuFrequenciesTooSlow << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                            << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                            << DiagnosisTestResult::Error
                                            << QString("The fastest CPU runs at %1 MHz, and the minimum required is %2 MHz").arg(zero).arg(requirements.getMinimumCPUFrequency())
                                            << "Update computer's hardware";

    QStringList missingOpenGLExtensions;
    missingOpenGLExtensions << "GL_ARB_draw_buffers";
    // El resultat és hard coded, si canvia el requeriment, s'ha de canviar la string
    QTest::newRow("missing openGL extensions") << cpuNumberOfCores << cpuFrequencies << missingOpenGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                               << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                               << DiagnosisTestResult::Error
                                               << "Current OpenGL version does not support these extensions: GL_ARB_flux_capacitor, GL_ARB_half_float_pixel"
                                               << "Update your graphics card driver";

    QTest::newRow("Old openGL version") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << "1.0" << gpuRAM << gpuModel << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Error
                                        << QString("Current OpenGL version is %1, and the minimum required is %2").arg("1.0").arg(requirements.getMinimumGPUOpenGLVersion())
                                        << "Update your graphics card driver";

    QTest::newRow("OK openGL version with vendor info") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion+" ATI-1.0" << gpuRAM << gpuModel << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("OK openGL version format X.X.X") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << "2.1.2" << gpuRAM << gpuModel << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Ok << unusedString << unusedString;

    UnsignedIntList notEnoughRAM;
    notEnoughRAM << zero;
    QTest::newRow("not enough GPU RAM") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << notEnoughRAM << gpuModel << hardDiskFreeSpace
                                        << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                        << DiagnosisTestResult::Error
                                        << QString("The graphics card %1 has %2 MB of RAM, and the minimum required is %3 MB").arg(gpuModel.at(0)).arg(zero).arg(requirements.getMinimumGPURAM())
                                        << "Change the graphics card";

    QTest::newRow("not enough space on disk") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << zero
                                              << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                              << DiagnosisTestResult::Error
                                              << "There is not enough disk space to run Starviewer properly"
                                              << "Free some space in the hard disk";

    QTest::newRow("windows version error") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                           << operatingSystem << "4.1" << servicePackVersion << ramTotalAmount << screenResolutions << writeCapability
                                           << DiagnosisTestResult::Error
                                           << QString("Current operating system version is %1, and the minimum required is %2").arg("4.1").arg(requirements.getMinimumOperatingSystemVersion())
                                           << "Update operating system to a newer version";

    QTest::newRow("windows service pack version error") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                                        << operatingSystem << operatingSystemVersion << "Service Pack 1" << ramTotalAmount << screenResolutions << writeCapability
                                                        << DiagnosisTestResult::Error
                                                        << QString("Current Service Pack version is %1, and the minimum required is Service Pack %2").arg("Service Pack 1").arg(requirements.getMinimumOperatingSystemServicePackVersion())
                                                        << "Install a newer Service Pack";

    QTest::newRow("not enough RAM") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                    << operatingSystem << operatingSystemVersion << servicePackVersion << zero << screenResolutions << writeCapability
                                    << DiagnosisTestResult::Error
                                    << QString("The total amount of RAM memory is %1 MB, and the minimum required is %2 MB").arg(zero).arg(requirements.getMinimumRAMTotalAmount())
                                    << "Upgrade computer's RAM memory";

    SizeList screenResolutionsTooSmall;
    screenResolutionsTooSmall << QSize(640, 480);
    QTest::newRow("screen too small") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                      << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutionsTooSmall << writeCapability
                                      << DiagnosisTestResult::Error
                                      << "The screen is too small to fit Starviewer application"
                                      << "Increase screen resolution";

    SizeList screensResolutionsTooSmall;
    screensResolutionsTooSmall << QSize(640, 480) << QSize(640, 480);
    QTest::newRow("screens too small") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                      << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screensResolutionsTooSmall << writeCapability
                                      << DiagnosisTestResult::Error
                                      << "The screens are too small to fit Starviewer application"
                                      << "Increase screen resolution";

    // Warning
    SizeList oneScreenResolutionTooSmall;
    oneScreenResolutionTooSmall << QSize(640, 480) << QSize(1440, 900) << QSize(640, 480);
    QTest::newRow("one screen too small") << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                                          << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << oneScreenResolutionTooSmall << writeCapability
                                          << DiagnosisTestResult::Warning
                                          << "One of the screens is too small. Keep in mind that Starviewer won't fit in that screen"
                                          << QString("Avoid placing Starviewer on the screen or screens 1, 3, or increase their resolution if possible");

    QTest::newRow("one screen too small, optical drive can not write")
                         << cpuNumberOfCores << cpuFrequencies << openGLExtensions << openGLVersion << gpuRAM << gpuModel << hardDiskFreeSpace
                         << operatingSystem << operatingSystemVersion << servicePackVersion << ramTotalAmount << screenResolutions << false
                         << DiagnosisTestResult::Warning
                         << "The optical drive does not have burning capabilities"
                         << "Change the optical drive to a CD-RW/DVD-RW";
}

void test_SystemRequirementsTest::run_ShouldTestIfSystemHasTheMinimumRequirements()
{
    QFETCH(unsigned int, testingCPUNumberOfCores);
    QFETCH(UnsignedIntList, testingCPUFrequencies);
    QFETCH(QStringList, testingGPUOpenGLCompatibilities);
    QFETCH(QString, testingGPUOpenGLVersion);
    QFETCH(UnsignedIntList, testingGPURAM);
    QFETCH(QStringList, testingGPUModel);
    QFETCH(unsigned int, testingHardDiskFreeSpace);
    QFETCH(SystemInformation::OperatingSystem, testingOperatingSystem);
    QFETCH(QString, testingOperatingSystemVersion);
    QFETCH(QString, testingOperatingSystemServicePackVersion);
    QFETCH(unsigned int, testingRAMTotalAmount);
    QFETCH(SizeList, testingScreenResolutions);
    QFETCH(bool, testingWriteCapability);

    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingSystemRequirementsTest systemRequirementsTest;
    systemRequirementsTest.m_testingCPUNumberOfCores = testingCPUNumberOfCores;
    systemRequirementsTest.m_testingCPUFrequencies = testingCPUFrequencies;
    systemRequirementsTest.m_testingGPUOpenGLCompatibilities = testingGPUOpenGLCompatibilities;
    systemRequirementsTest.m_testingGPUOpenGLVersion = testingGPUOpenGLVersion;
    systemRequirementsTest.m_testingGPURAM = testingGPURAM;
    systemRequirementsTest.m_testingGPUModel = testingGPUModel;
    systemRequirementsTest.m_testingHardDiskFreeSpace = testingHardDiskFreeSpace;
    systemRequirementsTest.m_testingOperatingSystem = testingOperatingSystem;
    systemRequirementsTest.m_testingOperatingSystemVersion = testingOperatingSystemVersion;
    systemRequirementsTest.m_testingOperatingSystemServicePackVersion = testingOperatingSystemServicePackVersion;
    systemRequirementsTest.m_testingRAMTotalAmount = testingRAMTotalAmount;
    systemRequirementsTest.m_testingScreenResolutions = testingScreenResolutions;
    systemRequirementsTest.m_testingWriteCapability = testingWriteCapability;

    DiagnosisTestResult result = systemRequirementsTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QList<DiagnosisTestProblem> problems = result.getErrors() + result.getWarnings();
        QCOMPARE(problems.count(), 1);
        QCOMPARE(problems.at(0).getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(problems.at(0).getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_SystemRequirementsTest)

#include "test_systemrequirementstest.moc"
