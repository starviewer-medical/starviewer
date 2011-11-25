// Starviewer
#include "windowssysteminformation.h"
#include "systeminformation.h"
#include "logging.h"

// Qt
#include <QList>
#include <QByteArray>
// Qt openGL
#include <QtOpenGL/QGLWidget>

// Windows
#include <windows.h>

/// Per bstr_t
#include <comdef.h>

namespace udg {

WindowsSystemInformation::WindowsSystemInformation()
{
    m_api = NULL;
}

WindowsSystemInformation::~WindowsSystemInformation()
{
    uninitializeAPI(m_api);
}

WindowsSystemInformation::OperatingSystem WindowsSystemInformation::getOperatingSystem()
{
    return SystemInformation::OSWindows;
}

QString WindowsSystemInformation::getOperatingSystemArchitecture()
{
    QString operatingSystemArchitecture = "";
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_OperatingSystem");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        if (getProperty(object, "OSArchitecture", &variantProperty))
        {
            operatingSystemArchitecture = QString().fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return operatingSystemArchitecture;
}

QString WindowsSystemInformation::getOperatingSystemVersion()
{
    QString operatingSystemVersion = "";
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_OperatingSystem");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        if (getProperty(object, "Version", &variantProperty))
        {
            operatingSystemVersion = QString().fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return operatingSystemVersion;
}

QString WindowsSystemInformation::getOperatingSystemServicePackVersion()
{
    QString servicePackVersion = "";
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_OperatingSystem");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        if (getProperty(object, "CSDVersion", &variantProperty))
        {
            servicePackVersion = QString().fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return servicePackVersion;
}

unsigned int WindowsSystemInformation::getRAMTotalAmount()
{
    unsigned int RAMTotalAmount = 0;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_ComputerSystem");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar la memòria total del sistema en Bytes
        if (getProperty(object, "TotalPhysicalMemory", &variantProperty))
        {
            BSTR TotalPhysicalMemory = variantProperty.bstrVal;
            long long LRAMTotalAmount = QString().fromWCharArray(TotalPhysicalMemory).toLongLong();
            RAMTotalAmount = LRAMTotalAmount / (1024.0 * 1024.0);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return RAMTotalAmount;
}

QList<unsigned int> WindowsSystemInformation::getRAMModulesCapacity()
{
    QList<unsigned int> RAMModulesCapacity;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_PhysicalMemory");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar la memòria de cada mòdul de RAM en Bytes
        if (getProperty(object, "Capacity", &variantProperty))
        {
            long long LRAMTotalModuleAmount = QString().fromWCharArray(variantProperty.bstrVal).toLongLong();
            RAMModulesCapacity.append(LRAMTotalModuleAmount / (1024.0 * 1024.0));
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return RAMModulesCapacity;
}

QList<unsigned int> WindowsSystemInformation::getRAMModulesFrequency()
{
    QList<unsigned int> RAMModulesFrequency;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_PhysicalMemory");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar la frequència de cada mòdul de RAM
        if (getProperty(object, "Speed", &variantProperty))
        {
            RAMModulesFrequency.append(variantProperty.uintVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return RAMModulesFrequency;
}

unsigned int WindowsSystemInformation::getCPUNumberOfCores()
{
    // Portser caldria canviar el tipus de retorn per una llista d'unsigned ints amb el nombre de cores de cada CPU de la màquina
    unsigned int CPUNumberOfCores = 0;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_Processor");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar el nombre de cores del processador
        if (getProperty(object, "NumberOfCores", &variantProperty))
        {
            CPUNumberOfCores += variantProperty.uintVal;
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return CPUNumberOfCores;
}

QList<unsigned int> WindowsSystemInformation::getCPUFrequencies()
{
    QList<unsigned int> CPUFrequencies;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_Processor");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar la frequència del processador.
        // S'utilitza el MaxClockSpeed ja que el CurrentClockSpeed retorna la càrrega de la CPU actual.
        if (getProperty(object, "MaxClockSpeed", &variantProperty))
        {
            CPUFrequencies.append(variantProperty.uintVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return CPUFrequencies;
}

unsigned int WindowsSystemInformation::getCPUL2CacheSize()
{
    unsigned int cacheSize = 0;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_Processor");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        // Recuperar la frequència del processador.
        if (getProperty(object, "L2CacheSize", &variantProperty))
        {
            cacheSize = variantProperty.uintVal;
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return cacheSize;
}

QString WindowsSystemInformation::getGPUBrand()
{
    QString GPUBrand = "";
    /// Si el hardware és incompatible amb WDDM, Win32_VideoController pot no retornar els valors esperats.
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_VideoController");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        if (getProperty(object, "AdapterCompatibility", &variantProperty))
        {
            GPUBrand = QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return GPUBrand;
}

QString WindowsSystemInformation::getGPUModel()
{
    QString GPUBrand = "";
    /// Si el hardware és incompatible amb WDDM, Win32_VideoController pot no retornar els valors esperats.
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_VideoController");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "VideoProcessor", &variantProperty))
        {
            GPUBrand = QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return GPUBrand;
}

unsigned int WindowsSystemInformation::getGPURAM()
{
    unsigned int GPURAM = 0;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_VideoController");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;
        if (getProperty(object, "AdapterRAM", &variantProperty))
        {
            GPURAM = variantProperty.uintVal / (1024 * 1024);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return GPURAM;
}

QList<QString> WindowsSystemInformation::getGPUOpenGLCompatibilities()
{
    QString extensions = createOpenGLContextAndGetExtensions();
    return extensions.split(" ");
}

QString WindowsSystemInformation::getGPUOpenGLVersion()
{
    return createOpenGLContextAndGetVersion();
}

QString WindowsSystemInformation::getGPUDriverVersion()
{
    QString driverVersion = "";
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_VideoController");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "DriverVersion", &variantProperty))
        {
            driverVersion += "Version: ";
            driverVersion += QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }
        if (getProperty(object, "DriverDate", &variantProperty))
        {
            driverVersion += ", Date: ";
            driverVersion += QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }
        if (getProperty(object, "infFilename", &variantProperty))
        {
            driverVersion += ", Information file: ";
            driverVersion += QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }
        if (getProperty(object, "InstalledDisplayDrivers", &variantProperty))
        {
            driverVersion += ", Display drivers: ";
            driverVersion += QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }
        if (getProperty(object, "infSection", &variantProperty))
        {
            driverVersion += ", Section: ";
            driverVersion += QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return driverVersion;
}

QList<QString> WindowsSystemInformation::getScreenVendors()
{
    QList<QString> screenVendors;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_DesktopMonitor");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "MonitorManufacturer", &variantProperty))
        {
            screenVendors.append(QString::fromWCharArray(variantProperty.bstrVal));
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return screenVendors;
}

QList<QString> WindowsSystemInformation::getHardDiskDevices()
{
    QList<QString> devices;
    IEnumWbemClassObject* enumerator = executeQuery("SELECT * FROM Win32_LogicalDisk");

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "DeviceID", &variantProperty))
        {
            QString device = QString::fromWCharArray(variantProperty.bstrVal);
            VariantClear(&variantProperty);
            devices.append(device);
        }
        
        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return devices;
}

unsigned int WindowsSystemInformation::getHardDiskCapacity(const QString &device)
{
    unsigned int hardDiskCapacity = 0;
    IEnumWbemClassObject* enumerator = executeQuery(QString("SELECT * FROM Win32_LogicalDisk WHERE DeviceID LIKE '%1'").arg(device));

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "Size", &variantProperty))
        {
            unsigned long long size = QString::fromWCharArray(variantProperty.bstrVal).toULongLong();
            VariantClear(&variantProperty);
            hardDiskCapacity = size / (1024.0 * 1024.0);
        }
        
        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return hardDiskCapacity;
}

unsigned int WindowsSystemInformation::getHardDiskFreeSpace(const QString &device)
{
    unsigned int hardDiskFreeSpace = 0;
    IEnumWbemClassObject* enumerator = executeQuery(QString("SELECT * FROM Win32_LogicalDisk WHERE DeviceID LIKE '%1'").arg(device));
    IWbemClassObject* object = getNextObject(enumerator);

    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "FreeSpace", &variantProperty))
        {
            QString string = QString::fromWCharArray(variantProperty.bstrVal);
            unsigned long long size = string.toULongLong();
            VariantClear(&variantProperty);
            hardDiskFreeSpace = size / (1024.0 * 1024.0);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return hardDiskFreeSpace;
}

bool WindowsSystemInformation::doesOpticalDriveHaveWriteCapabilities()
{
    bool canWrite = false;

    IEnumWbemClassObject* enumerator = executeQuery(QString("SELECT * FROM Win32_CDROMDrive"));
    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "Capabilities", &variantProperty))
        {
            SAFEARRAY *capabilities = variantProperty.parray;
            // Obtenir el nombre de capabilities
            unsigned long elements = capabilities->rgsabound[0].cElements;

            unsigned int index = 0;
            while (!canWrite && index < elements)
            {
                unsigned int capability = ((unsigned int*)capabilities->pvData)[index++];
                // Segons la documentació, 4, és la capacitat de grabar CDs o DVDs
                if (capability == 4)
                {
                    canWrite = true;
                }
            }
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }
    return canWrite;
}

unsigned int WindowsSystemInformation::getNetworkAdapterSpeed()
{
    unsigned int networkAdapterSpeed = 0;
    // Els valors 2 i 9 signifiquen 'Connected' i 'Authentication succeeded' respectivament.
    // I s'utilitzen per la compatibilitat amb els diferents service pack de l'XP
    IEnumWbemClassObject* enumerator = executeQuery(QString("SELECT * FROM Win32_NetworkAdapter WHERE NetConnectionStatus = 2 OR NetConnectionStatus = 9"));

    // TODO Faltarà determinar si quan hi ha més d'un adaptador connectat, realment s'agafa el que toca

    IWbemClassObject* object = getNextObject(enumerator);
    while (object)
    {
        VARIANT variantProperty;

        if (getProperty(object, "Speed", &variantProperty))
        {
            QString speed = QString::fromWCharArray(variantProperty.bstrVal);
            networkAdapterSpeed = speed.toUInt() / 1000000;
            VariantClear(&variantProperty);
        }

        object->Release();
        object = getNextObject(enumerator);
    }

    if (enumerator)
    {
        enumerator->Release();
    }

    return networkAdapterSpeed;
}

IWbemClassObject* WindowsSystemInformation::getNextObject(IEnumWbemClassObject *enumerator)
{
    // Precondició: Si l'enumerador és nul, retornem nul
    if (!enumerator)
    {
        return NULL;
    }

    ULONG returned = 0;
    IWbemClassObject *object;
    HRESULT result = enumerator->Next(WBEM_INFINITE, 1, &object, &returned);
    if (FAILED(result) || returned == 0)
    {
        /// Si no hi ha cap més objecte a l'enumerador, pot retornar brossa, per tant, el posem manualment a nul
        object = NULL;
    }
    return object;
}

IEnumWbemClassObject* WindowsSystemInformation::executeQuery(QString query)
{
    if (!m_api)
    {
        m_api = initializeAPI();
    }

    HRESULT result;
    IEnumWbemClassObject* enumerator = NULL;
    QByteArray byteArrayQuery = query.toLocal8Bit();

    result = m_api->ExecQuery(bstr_t("WQL"), bstr_t(byteArrayQuery.data()),
                              WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumerator);
    
    if (FAILED(result))
    {
        enumerator = NULL;
    }

    return enumerator;
}

bool WindowsSystemInformation::getProperty(IWbemClassObject *object, QString propertyName, VARIANT *propertyVariant)
{
    QByteArray byteArrayQuery = propertyName.toLocal8Bit();
    HRESULT result = object->Get(bstr_t(byteArrayQuery.data()), 0, propertyVariant, 0, 0);
    return  propertyVariant->vt != VT_NULL && SUCCEEDED(result);
}

IWbemServices* WindowsSystemInformation::initializeAPI()
{
    HRESULT result;
    IWbemLocator *locator = NULL;
    IWbemServices *services = NULL;

    // Pas 1: Inicialitzar la llibreria COM.
    result =  CoInitializeEx(0, COINIT_APARTMENTTHREADED);//COINIT_MULTITHREADED); 
    if (FAILED(result))
    {
        ERROR_LOG(QString("Failed to initialize COM library. Error code = %1").arg(result));
        return NULL;
    }
    
    // Pas 2: Obtenir el locator a WMI
    // Quan es fa aquesta crida es carrega la CLR, cosa que fa augmentar la memòria 1Mb (+ o -). Aquesta memòria no es pot alliberar
    // de cap manera, ja que windows deixa la llibreria carregada per futures consultes. No és un memory leak, ja que si es crida 
    // n vegades, només es carregarà la llibreria un cop (1Mb)
    // Font: http://stackoverflow.com/questions/4736788/memory-leak-when-using-cocreateinstance
    result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &locator);
 
    if (FAILED(result))
    {
        ERROR_LOG(QString("Failed to create IWbemLocator object. Err code = %1").arg(result));
        CoUninitialize();
        return services;
    }

    // Pas 3: Connectar a WMI a través del mètode IWbemLocator::ConnectServer

    // Connectar al namespace root\cimv2 amb l'usuari actuali obtenir un punter 'services' per fer crides IWbemServices.
    // Els paràmetres són: el namespace, nom d'usuari (NULL = actual), password (NULL = actual), idioma (locale) (NULL = actual),
    // flags de seguretat, authority (p.e. Kerberos), objecte de context, (sortida) punter al proxy de IWbemServices.
    result = locator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, NULL, NULL, NULL, NULL, &services); 
    
    if (FAILED(result))
    {
        ERROR_LOG(QString("Could not connect. Error code = %1").arg(result));
        locator->Release(); 
        locator = NULL;
        services = NULL;
        CoUninitialize();
        return services;
    }

    DEBUG_LOG("Connected to ROOT\\CIMV2 WMI namespace");

    // Pas 4: Establir els nivells de seguretat del proxy de IWbemServices

    // Els paràmetres són: el proxy de IWbemServices, RPC_C_AUTHN_xxx, RPC_C_AUTHZ_xxx, nom del servidor, RPC_C_AUTHN_LEVEL_xxx,
    // RPC_C_IMP_LEVEL_xxx, identitat del client, capacitats del proxy de IWbemServices
    result = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    locator->Release();
    locator = NULL;

    if (FAILED(result))
    {
        ERROR_LOG(QString("Could not set proxy blanket. Error code = %1").arg(result));
        services->Release();
        services = NULL;
        CoUninitialize();
    }

    return services;
}

void WindowsSystemInformation::uninitializeAPI(IWbemServices *services)
{
    if (services)
    {
        services->Release();
        services = NULL;
    }
    CoUninitialize();
}

QString WindowsSystemInformation::createOpenGLContextAndGetExtensions()
{
    /// Per tal de fer servir el QGLWidget, cal afegir en el core.pro QT += opengl
    /// TODO: Cal comprovar si això afecte al rendiment, compilació, etc.
    /// La llibreria QtOpenGL semble que quan es carrega, deixa en memòria uns 4Mb, per poder-la segir utilitzant.
    QGLWidget window;
    /// Forçar a carregar el context openGL
    window.updateGL();

    return QString((const char*)glGetString(GL_EXTENSIONS));
}

QString WindowsSystemInformation::createOpenGLContextAndGetVersion()
{
    /// Per tal de fer servir el QGLWidget, cal afegir en el core.pro QT += opengl
    /// TODO: Cal comprovar si això afecte al rendiment, compilació, etc.
    /// La llibreria QtOpenGL semble que quan es carrega, deixa en memòria uns 4Mb, per poder-la segir utilitzant.
    QGLWidget window;
    /// Forçar a carregar el context openGL
    window.updateGL();

    return QString((const char*)glGetString(GL_VERSION));
}

}
