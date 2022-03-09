/*@
    "name": "test_WindowsSystemInformation",
    "requirements": ["other_features.diagnosis_tests", "technical.platforms.windows", "archive.ris", "archive.dimse"]
 */

//Starviewer
#include "autotest.h"
#include "windowssysteminformation.h"

//Qt
#include <QString>
#include <QStringList>
// Windows
#include <windows.h>
// Windows API
#include <Wbemidl.h>

// Per bstr_t
#include <comdef.h>

using namespace udg;

typedef QList<unsigned int> UnsignedIntList;

class TestingIWBemClassObjectTest : public IWbemClassObject {
    ULONG STDMETHODCALLTYPE AddRef(void){return 1;}
    ULONG STDMETHODCALLTYPE Release(void){return 0;}

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID &,void **){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetQualifierSet(IWbemQualifierSet **){return S_OK;}
    HRESULT STDMETHODCALLTYPE Get(LPCWSTR,long,VARIANT *,CIMTYPE *,long *){return S_OK;}
    HRESULT STDMETHODCALLTYPE Put(LPCWSTR,long,VARIANT *,CIMTYPE){return S_OK;}
    HRESULT STDMETHODCALLTYPE Delete(LPCWSTR){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetNames(LPCWSTR,long,VARIANT *,SAFEARRAY **){return S_OK;}
    HRESULT STDMETHODCALLTYPE BeginEnumeration(long){return S_OK;}
    HRESULT STDMETHODCALLTYPE Next(long,BSTR *,VARIANT *,CIMTYPE *,long *){return S_OK;}
    HRESULT STDMETHODCALLTYPE EndEnumeration(void){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetPropertyQualifierSet(LPCWSTR,IWbemQualifierSet **){return S_OK;}
    HRESULT STDMETHODCALLTYPE Clone(IWbemClassObject **){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetObjectText(long,BSTR *){return S_OK;}
    HRESULT STDMETHODCALLTYPE SpawnDerivedClass(long,IWbemClassObject **){return S_OK;}
    HRESULT STDMETHODCALLTYPE SpawnInstance(long,IWbemClassObject **){return S_OK;}
    HRESULT STDMETHODCALLTYPE CompareTo(long,IWbemClassObject *){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetPropertyOrigin(LPCWSTR,BSTR *){return S_OK;}
    HRESULT STDMETHODCALLTYPE InheritsFrom(LPCWSTR){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetMethod(LPCWSTR,long,IWbemClassObject **,IWbemClassObject **){return S_OK;}
    HRESULT STDMETHODCALLTYPE PutMethod(LPCWSTR,long,IWbemClassObject *,IWbemClassObject *){return S_OK;}
    HRESULT STDMETHODCALLTYPE DeleteMethod(LPCWSTR){return S_OK;}
    HRESULT STDMETHODCALLTYPE BeginMethodEnumeration(long){return S_OK;}
    HRESULT STDMETHODCALLTYPE NextMethod(long,BSTR *,IWbemClassObject **,IWbemClassObject **){return S_OK;}
    HRESULT STDMETHODCALLTYPE EndMethodEnumeration(void){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetMethodQualifierSet(LPCWSTR,IWbemQualifierSet **){return S_OK;}
    HRESULT STDMETHODCALLTYPE GetMethodOrigin(LPCWSTR,BSTR *){return S_OK;}
};

class TestingWindowsSystemInformation : public WindowsSystemInformation {
public:

    // GetNextObject retornarà un dels objectes del vector, i per tant necessita un index per anar-lo recorrent. Quan arribi al final
    // retornarà NULL. En cas de atributs que no siguin vectors, després de retornar el seu valor, retornarà NULL.
    int m_getNextObjectIndex;
    // Per retornar un vector de forma correcta, necessitem un index de 0 a n
    int m_nextObjectIndex;
    bool m_getPropertyWorksProperly;

    QString m_testingAPIRAMTotalAmount;
    QStringList m_testingAPIRAMModulesCapacity;

    UnsignedIntList m_testingAPICPUNumberOfCores;
    unsigned int m_testingEnvironmentCPUNumberOfCores;

    UnsignedIntList m_testingAPIGPURAM;
    QString m_testingAPIGPUOpenGLCompatibilities;

    QString m_testingAPIGPUDriverVersion;
    
    QString m_testingAPIHardDiskCapacity;
    QString m_testingAPIHardDiskFreeSpace;

protected:
    IWbemClassObject* getNextObject(IEnumWbemClassObject *enumerator) override
    {
        Q_UNUSED(enumerator);
        if (m_getNextObjectIndex <= 0)
        {
            m_nextObjectIndex = 0;
            return NULL;
        }
        else
        {
            return new TestingIWBemClassObjectTest();
        }
    }

    IEnumWbemClassObject* executeQuery(QString query) override
    {
        Q_UNUSED(query);
        return NULL;
    }

    BSTR fromQStringToBSTR(const QString &string)
    {
        BSTR resultBSTR = new WCHAR[string.size() + 1];
        int size = string.toWCharArray(resultBSTR);
        resultBSTR[size] = '\0';
        return resultBSTR;
    }

    bool getProperty(IWbemClassObject *object, QString propertyName, VARIANT *propertyVariant) override
    {
        Q_UNUSED(object);
        if (propertyName == "TotalPhysicalMemory")
        {
            propertyVariant->bstrVal = fromQStringToBSTR(m_testingAPIRAMTotalAmount);
            m_getNextObjectIndex--;
        }
        else if (propertyName == "Capacity")
        {
            propertyVariant->bstrVal = fromQStringToBSTR(m_testingAPIRAMModulesCapacity.at(m_nextObjectIndex++));
            m_getNextObjectIndex--;
        }
        else if (propertyName == "NumberOfCores")
        {
            propertyVariant->uintVal = m_testingAPICPUNumberOfCores.at(m_nextObjectIndex++);
            m_getNextObjectIndex--;
        }
        else if (propertyName == "AdapterRAM")
        {
            propertyVariant->uintVal = m_testingAPIGPURAM.at(m_nextObjectIndex++);
            m_getNextObjectIndex--;
        }

        // Del mètode getGPUDriverVersion
        else if (propertyName == "DriverVersion")
        {
            propertyVariant->bstrVal = fromQStringToBSTR(m_testingAPIGPUDriverVersion);
            m_getNextObjectIndex--;
        }
        // FI ------------------------------

        else if (propertyName == "Size")
        {
            propertyVariant->bstrVal = fromQStringToBSTR(m_testingAPIHardDiskCapacity);
            m_getNextObjectIndex--;
        }
        else if (propertyName == "FreeSpace")
        {
            propertyVariant->bstrVal = fromQStringToBSTR(m_testingAPIHardDiskFreeSpace);
            m_getNextObjectIndex--;
        }

        return m_getPropertyWorksProperly;
    }

    void uninitializeAPI(IWbemServices *services) override
    {
        Q_UNUSED(services);
    }

    QString createOpenGLContextAndGetExtensions()
    {
        return m_testingAPIGPUOpenGLCompatibilities.toLocal8Bit();
    }
    
    unsigned int getCPUNumberOfCoresFromEnvironmentVar() override
    {
        return m_testingEnvironmentCPUNumberOfCores;
    }
};

Q_DECLARE_METATYPE(QStringList)
Q_DECLARE_METATYPE(UnsignedIntList)

class test_WindowsSystemInformation : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfWindowsSystemInformationRunsAsExpected_data();
    void run_ShouldTestIfWindowsSystemInformationRunsAsExpected();
};

void test_WindowsSystemInformation::run_ShouldTestIfWindowsSystemInformationRunsAsExpected_data()
{
    /// Es comproven els mètodes de la classe windowsSystemInformation que no són un símple get (o un get de la API)
    // Entrada
    QTest::addColumn<QString>("testingAPIRAMTotalAmount");
    QTest::addColumn<QStringList>("testingAPIRAMModulesCapacity");
    QTest::addColumn<UnsignedIntList>("testingAPICPUNumberOfCores");
    QTest::addColumn<unsigned int>("testingEnvironmentCPUNumberOfCores");
    QTest::addColumn<UnsignedIntList>("testingAPIGPURAM");
    QTest::addColumn<QString>("testingAPIGPUOpenGLCompatibilities");

    QTest::addColumn<QString>("testingAPIGPUDriverVersion");
    
    QTest::addColumn<QString>("testingAPIHardDiskCapacity");
    QTest::addColumn<QString>("testingAPIHardDiskFreeSpace");

    // Enabler
    QTest::addColumn<bool>("getPropertyWorksProperly");

    // Sortida
    QTest::addColumn<unsigned int>("testingRAMTotalAmount");
    QTest::addColumn<UnsignedIntList>("testingRAMModulesCapacity");
    QTest::addColumn<unsigned int>("testingCPUNumberOfCores");
    QTest::addColumn<UnsignedIntList>("testingGPURAM");
    QTest::addColumn<QStringList>("testingGPUOpenGLCompatibilities");
    QTest::addColumn<QStringList>("testingGPUDriverVersion");
    QTest::addColumn<unsigned int>("testingHardDiskCapacity");
    QTest::addColumn<unsigned int>("testingHardDiskFreeSpace");

    QString APIRAMTotalAmount = "1073741824"; // 1024 MB
    unsigned int RAMTotalAmount = 1024;

    QStringList APIRAMModulesCapacity;
    APIRAMModulesCapacity << "2147483648" << "1073741824";
    UnsignedIntList RAMModulesCapacity;
    RAMModulesCapacity << 2048 << 1024;

    UnsignedIntList APICPUNumberOfCores;
    APICPUNumberOfCores << (unsigned int)2 << (unsigned int)6;
    unsigned int environmentCPUNumberOfCores = 8;
    unsigned int CPUNumberOfCores = 8;

    UnsignedIntList APIGPURAM;
    APIGPURAM << 1073741824;
    UnsignedIntList GPURAM;
    GPURAM << 1024;

    QString APIGPUOpenGLCompatibilities = "GL_ARB_shading_language_100 GL_ARB_shader_objects GL_ARB_fragment_shader GL_ARB_vertex_shader";
    QStringList GPUOpenGLCompatibilities;
    GPUOpenGLCompatibilities << "GL_ARB_shading_language_100" << "GL_ARB_shader_objects" << "GL_ARB_fragment_shader" << "GL_ARB_vertex_shader";

    QString APIGPUDriverVersion = "8.17.12.6890";
    QStringList GPUDriverVersion;
    GPUDriverVersion << "8.17.12.6890";

    QString APIHardDiskCapacity = "1073741824";
    unsigned int hardDiskCapacity = 1024;

    QString APIHardDiskFreeSpace = "1073741824";
    unsigned int hardDiskFreeSpace = 1024;

    QTest::newRow("ok") << APIRAMTotalAmount << APIRAMModulesCapacity << APICPUNumberOfCores << environmentCPUNumberOfCores << APIGPURAM 
                        << APIGPUOpenGLCompatibilities << APIGPUDriverVersion << APIHardDiskCapacity << APIHardDiskFreeSpace
                        << true
                        << RAMTotalAmount << RAMModulesCapacity << CPUNumberOfCores << GPURAM
                        << GPUOpenGLCompatibilities << GPUDriverVersion << hardDiskCapacity << hardDiskFreeSpace;

    unsigned int RAMTotalAmount2 = 0;
    UnsignedIntList RAMModulesCapacity2;
    RAMModulesCapacity2 << 0 << 0;
    UnsignedIntList GPURAM2;
    GPURAM2 << 0;
    QStringList GPUDriverVersion2;
    GPUDriverVersion2 << "";
    unsigned int hardDiskCapacity2 = 0;
    unsigned int hardDiskFreeSpace2 = 0;

    QTest::newRow("API not getting properties")
                    << APIRAMTotalAmount << APIRAMModulesCapacity << APICPUNumberOfCores << environmentCPUNumberOfCores << APIGPURAM
                    << APIGPUOpenGLCompatibilities << APIGPUDriverVersion << APIHardDiskCapacity << APIHardDiskFreeSpace
                    << false
                    << RAMTotalAmount2 << RAMModulesCapacity2 << environmentCPUNumberOfCores << GPURAM2
                    << GPUOpenGLCompatibilities << GPUDriverVersion2 << hardDiskCapacity2 << hardDiskFreeSpace2;
}

void test_WindowsSystemInformation::run_ShouldTestIfWindowsSystemInformationRunsAsExpected()
{
    QFETCH(QString, testingAPIRAMTotalAmount);
    QFETCH(unsigned int, testingRAMTotalAmount);

    QFETCH(QStringList, testingAPIRAMModulesCapacity);
    QFETCH(UnsignedIntList, testingRAMModulesCapacity);

    QFETCH(UnsignedIntList, testingAPICPUNumberOfCores);
    QFETCH(unsigned int, testingEnvironmentCPUNumberOfCores);
    QFETCH(unsigned int, testingCPUNumberOfCores);

    QFETCH(UnsignedIntList, testingAPIGPURAM);
    QFETCH(UnsignedIntList, testingGPURAM);

    QFETCH(QString, testingAPIGPUOpenGLCompatibilities);
    QFETCH(QStringList, testingGPUOpenGLCompatibilities);

    QFETCH(QString, testingAPIGPUDriverVersion);
    QFETCH(QStringList, testingGPUDriverVersion);

    QFETCH(QString, testingAPIHardDiskCapacity);
    QFETCH(unsigned int, testingHardDiskCapacity);

    QFETCH(QString, testingAPIHardDiskFreeSpace);
    QFETCH(unsigned int, testingHardDiskFreeSpace);

    QFETCH(bool, getPropertyWorksProperly);

    TestingWindowsSystemInformation windowsSystemInformation;
    // Necessari per retornar llistes de forma correcte.
    windowsSystemInformation.m_nextObjectIndex = 0;
    windowsSystemInformation.m_getPropertyWorksProperly = getPropertyWorksProperly;
    windowsSystemInformation.m_testingAPIRAMTotalAmount = testingAPIRAMTotalAmount;
    windowsSystemInformation.m_testingAPIRAMModulesCapacity = testingAPIRAMModulesCapacity;
    windowsSystemInformation.m_testingAPICPUNumberOfCores = testingAPICPUNumberOfCores;
    windowsSystemInformation.m_testingEnvironmentCPUNumberOfCores = testingEnvironmentCPUNumberOfCores;
    windowsSystemInformation.m_testingAPIGPURAM = testingAPIGPURAM;
    windowsSystemInformation.m_testingAPIGPUOpenGLCompatibilities = testingAPIGPUOpenGLCompatibilities;
    windowsSystemInformation.m_testingAPIHardDiskCapacity = testingAPIHardDiskCapacity;
    windowsSystemInformation.m_testingAPIHardDiskFreeSpace = testingAPIHardDiskFreeSpace;
    windowsSystemInformation.m_testingAPIGPUDriverVersion = testingAPIGPUDriverVersion;
    
    windowsSystemInformation.m_getNextObjectIndex = 1;
    unsigned int RAMTotalAmount = windowsSystemInformation.getRAMTotalAmount();

    windowsSystemInformation.m_getNextObjectIndex = testingAPIRAMModulesCapacity.count();
    UnsignedIntList RAMModulesCapacity = windowsSystemInformation.getRAMModulesCapacity();

    windowsSystemInformation.m_getNextObjectIndex = testingAPICPUNumberOfCores.count();
    unsigned int CPUNumberOfCores = windowsSystemInformation.getCPUNumberOfCores();

    windowsSystemInformation.m_getNextObjectIndex = testingGPURAM.count();
    UnsignedIntList GPURAM = windowsSystemInformation.getGPURAM();

    windowsSystemInformation.m_getNextObjectIndex = 1;
    QStringList OpenGLCompatibilities = windowsSystemInformation.getGPUOpenGLCompatibilities();

    windowsSystemInformation.m_getNextObjectIndex = 1;
    unsigned int hardDiskCapacity = windowsSystemInformation.getHardDiskCapacity("C:");

    windowsSystemInformation.m_getNextObjectIndex = 1;
    unsigned int hardDiskFreeSpace = windowsSystemInformation.getHardDiskFreeSpace("C:");

    windowsSystemInformation.m_getNextObjectIndex = 1;
    QStringList GPUDriverVersion = windowsSystemInformation.getGPUDriverVersion();


    QCOMPARE(RAMTotalAmount, testingRAMTotalAmount);
    if (!getPropertyWorksProperly)
    {
        QCOMPARE(RAMModulesCapacity.count(), 0);
    }
    else
    {
        for (int i = 0; i < testingRAMModulesCapacity.count(); i++)
        {
            QCOMPARE(RAMModulesCapacity.at(i), testingRAMModulesCapacity.at(i));
        }
    }

    QCOMPARE(CPUNumberOfCores, testingCPUNumberOfCores);

    for (int i = 0; i < GPURAM.count(); i++)
    {
        QCOMPARE(GPURAM.at(i), testingGPURAM.at(i));
    }
    for (int i = 0; i < OpenGLCompatibilities.count(); i++)
    {
        QCOMPARE(OpenGLCompatibilities.at(i), testingGPUOpenGLCompatibilities.at(i));
    }

    QCOMPARE(hardDiskCapacity, testingHardDiskCapacity);
    QCOMPARE(hardDiskFreeSpace, testingHardDiskFreeSpace);

    for (int i = 0; i < GPUDriverVersion.count(); i++)
    {
        QCOMPARE(GPUDriverVersion.at(i), testingGPUDriverVersion.at(i));
    }
}

DECLARE_TEST(test_WindowsSystemInformation)

#include "test_windowssysteminformation.moc"
