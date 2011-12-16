#include "windowsfirewallaccess.h"
#include "logging.h"

#include <crtdbg.h>
#include <objbase.h>
#include <oleauto.h>
#include <stdio.h>

#include <QCoreApplication>

namespace udg {

WindowsFirewallAccess::WindowsFirewallAccess()
{
    m_status = FirewallAccess::FirewallUnknownStatus;
}

WindowsFirewallAccess::~WindowsFirewallAccess()
{
}

bool WindowsFirewallAccess::doesStarviewerHaveAccesThroughFirewall()
{
    HRESULT result = S_OK;
    HRESULT comInitializationResult = E_FAIL;
    INetFwProfile* firewallProfile = NULL;
    bool access = false;

    comInitializationResult = initializeWindowsFirewallLibrary(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    /// CoInitializeEx inicialitza la llibreria COM per tota la crida de threads. Pot retornar tres valors vàlids:
    /// S_OK és que la llibreria s'ha inicialitzat correctament.
    /// S_FALSE és que la llibreria ja estava inicialitzada.
    /// RPC_E_CHANGED_MODE és que s'ha canviat el model de concurrència, però no ens afecta.
    if (comInitializationResult == RPC_E_CHANGED_MODE || comInitializationResult == S_OK || comInitializationResult == S_FALSE)
    {
        /// Agafar el perfil de firewall en ús
        result = initializeWindowsFirewall(&firewallProfile);
        if (SUCCEEDED(result))
        {
            /// Comprovar si el firewall està activat
            bool enabled;
            result = windowsFirewallIsOn(firewallProfile, &enabled);
            if (SUCCEEDED(result))
            {
                if (enabled)
                {
                    /// Comprovar si el firewall permet excepcions
                    bool exceptionsAllowed;
                    result = doesWindowsFirewallAllowExceptions(firewallProfile, &exceptionsAllowed);
                    if (SUCCEEDED(result))
                    {
                        if (exceptionsAllowed)
                        {
                            /// Comprovar si Starviewer està afegit a les excepcions del firewall
                            BSTR starviewerExecutablePath = getStarviewerExecutablePath();
                            result = isApplicationEnabledAtFirewall(firewallProfile, starviewerExecutablePath, &enabled);
                            delete[] starviewerExecutablePath;
                            if (SUCCEEDED(result))
                            {
                                if (enabled)
                                {
                                    // starviewer està habilitat com a excepció a la llista del firewall
                                    access = true;
                                    m_status = FirewallAccess::FirewallIsAccessible;
                                }
                                else
                                {
                                    // el firewall no te starviewer com a excepció
                                    m_errorString = QObject::tr("Application is not in the firewall exceptions list");
                                    m_status = FirewallAccess::FirewallIsBlocking;
                                }
                            }
                            else
                            {
                                m_errorString = QObject::tr("Unable to check if application is in the firewall exceptions list");
                                m_status = FirewallAccess::FirewallError;
                            }
                        }
                        else
                        {
                            // si no es permeten excepcions, l'aplicació no té accés
                            m_errorString = QObject::tr("Windows firewall does not allow exceptions");
                            m_status = FirewallAccess::FirewallIsBlocking;
                        }
                    }
                    else
                    {
                        m_errorString = QObject::tr("Unable to check if firewall allows exceptions");
                        m_status = FirewallAccess::FirewallError;
                    }
                }
                else
                {
                    // el firewall està apagat, per tant l'aplicació té acces a través d'ell
                    m_status = FirewallAccess::FirewallIsAccessible;
                    access = true;
                }
            }
            else
            {
                m_errorString = QObject::tr("Unable to check if firewall is enabled");
                m_status = FirewallAccess::FirewallError;
            }
        }
        else
        {
            m_errorString = QObject::tr("Unable to initialize firewall");
            m_status = FirewallAccess::FirewallError;
        }
    }
    else
    {
        m_errorString = QObject::tr("Error initializing firewall library");
        m_status = FirewallAccess::FirewallError;
        ERROR_LOG(QString("Error inicialitzant la llibreria COM. CoInitializeEx ha fallat amb el codi %1\n").arg(comInitializationResult));
        CleanupWindowsFirewall(firewallProfile);
        return false;
    }

    UninitializeWindowsFirewallLibrary();
    CleanupWindowsFirewall(firewallProfile);
    return access;
}

HRESULT WindowsFirewallAccess::initializeWindowsFirewall(INetFwProfile **firewallProfile)
{
    HRESULT result = S_OK;
    INetFwMgr* firewallManager = NULL;
    INetFwPolicy* firewallPolicy = NULL;

    Q_ASSERT(firewallProfile != NULL);

    *firewallProfile = NULL;

    // Crea una instancia del firewall manager.
    result = CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), (void**)&firewallManager);
    if (FAILED(result))
    {
        ERROR_LOG(QString("CoCreateInstance ha fallat amb codi :%1\n").arg(result));
        return result;
    }

    // Agafar la política del firewall.
    result = firewallManager->get_LocalPolicy(&firewallPolicy);
    if (FAILED(result))
    {
        ERROR_LOG(QString("get_LocalPolicy ha fallat amb codi :%1\n").arg(result));
        return result;
    }

    // Agafar el perfil actual del firewall.
    result = firewallPolicy->get_CurrentProfile(firewallProfile);
    if (FAILED(result))
    {
        ERROR_LOG(QString("get_CurrentProfile ha fallat amb codi :%1\n").arg(result));
        if (firewallManager != NULL)
        {
            firewallManager->Release();
        }
        return result;
    }


    // Esborrar la política del firewall.
    if (firewallPolicy != NULL)
    {
        firewallPolicy->Release();
    }

    // Esborrar el firewall manager.
    if (firewallManager != NULL)
    {
        firewallManager->Release();
    }

    return result;
}

HRESULT WindowsFirewallAccess::windowsFirewallIsOn(INetFwProfile *firewallProfile, bool *firewallOn)
{
    HRESULT result = S_OK;
    VARIANT_BOOL firewallEnabled;

    Q_ASSERT(firewallProfile != NULL);
    Q_ASSERT(firewallOn != NULL);

    *firewallOn = false;

    // Obtenir l'estat actual del firewall.
    // S'utilitza una variable VARIAN_BOOL. Aquestes estan definides per microsoft per tal de mantenir
    // la idea del Visual Basic on false és 0 i true és -1. Es defineix com un short (2bytes),
    // per tant no es pot passar directament un bool (1bytes) per paràmetre.
    result = firewallProfile->get_FirewallEnabled(&firewallEnabled);
    if (FAILED(result))
    {
        ERROR_LOG(QString("get_FirewallEnabled ha fallat: %1\n").arg(result));
    }

    // Comprovar si el firewall està activat.
    // VARIANT_BOOL es compara amb VARIANT_TRUE i VARIANT_FALSE, que valen 0x0000 i 0xffff
    // per tant es pot comparar directament com a bool
    if (firewallEnabled != VARIANT_FALSE)
    {
        *firewallOn = true;
    }
    else
    {
        *firewallOn = false;
    }

    return result;
}

HRESULT WindowsFirewallAccess::doesWindowsFirewallAllowExceptions(INetFwProfile *firewallProfile, bool *exceptionsAllowed)
{
    HRESULT result = S_OK;
    VARIANT_BOOL exceptionsNotAllowed;

    Q_ASSERT(firewallProfile != NULL);
    Q_ASSERT(exceptionsAllowed != NULL);

    *exceptionsAllowed = false;

    // Comprovar si el firewall permet excepcions
    // S'utilitza una variable VARIAN_BOOL. Aquestes estan definides per microsoft per tal de mantenir
    // la idea del Visual Basic on false és 0 i true és -1. Es defineix com un short (2bytes),
    // per tant no es pot passar directament un bool (1bytes) per paràmetre.
    result = firewallProfile->get_ExceptionsNotAllowed(&exceptionsNotAllowed);
    if (FAILED(result))
    {
        ERROR_LOG(QString("get_ExceptionsNotAllowed ha fallat: %1\n").arg(result));
    }
    
    // VARIANT_BOOL es compara amb VARIANT_TRUE i VARIANT_FALSE, que valen 0x0000 i 0xffff
    // per tant es pot comparar directament com a bool
    if (exceptionsNotAllowed != VARIANT_FALSE)
    {
        *exceptionsAllowed = false;
    }
    else
    {
        *exceptionsAllowed = true;
    }

    return result;
}

HRESULT WindowsFirewallAccess::isApplicationEnabledAtFirewall(INetFwProfile *firewallProfile,
                                                              BSTR          firewallProcessImageFileName,
                                                              bool          *firewallApplicationEnabled)
{
    /// BSTR = OLECHAR*. OLECHAR = WCHAR. WCHAR = wchar_t --> BSTR = wchar_t*

    HRESULT result = S_OK;
    VARIANT_BOOL firewallEnabled;
    INetFwAuthorizedApplication *firewallApplication = NULL;
    INetFwAuthorizedApplications *firewallApplications = NULL;

    Q_ASSERT(firewallProfile != NULL);
    Q_ASSERT(firewallProcessImageFileName != NULL);
    Q_ASSERT(firewallApplicationEnabled != NULL);

    *firewallApplicationEnabled = false;

    // Agafar la llista d'aplicacions autoritzades
    result = firewallProfile->get_AuthorizedApplications(&firewallApplications);
    if (FAILED(result))
    {
        ERROR_LOG(QString("get_AuthorizedApplications ha fallat: %1\n").arg(result));
    }
    else
    {
        // Intentar extreure l'applicació de la llista
        result = firewallApplications->Item(firewallProcessImageFileName, &firewallApplication);
        if (SUCCEEDED(result))
        {
            // Comprovar si l'aplicació està habilitada com a excepció del firewall
            result = firewallApplication->get_Enabled(&firewallEnabled);
            if (FAILED(result))
            {
                ERROR_LOG(QString("get_Enabled failed: %1\n").arg(result));
            }
            else if (firewallEnabled != VARIANT_FALSE)
            {
                // L'applicació està habilitada
                *firewallApplicationEnabled = true;
            }
        }
        else
        {
            // Si el resultat no és success, vol dir que l'aplicació no està a la llista d'excepcions, i per tant no té access.
            result = S_OK;
        }
    }

    // Esborrar la instància de l'aplicació
    if (firewallApplication != NULL)
    {
        firewallApplication->Release();
    }

    // Esborrar la llista d'aplicacions
    if (firewallApplications != NULL)
    {
        firewallApplications->Release();
    }

    return result;
}

BSTR WindowsFirewallAccess::getStarviewerExecutablePath()
{
    QString path;
    path = QCoreApplication::applicationDirPath().replace("/","\\") + "\\starviewer.exe";

    return fromQStringToBSTR(path); 
}

BSTR WindowsFirewallAccess::fromQStringToBSTR(const QString &string)
{
    BSTR result = new WCHAR[string.size() + 1];
    int size = string.toWCharArray(result);
    result[size] = '\0';
    return result;
}

HRESULT WindowsFirewallAccess::initializeWindowsFirewallLibrary(LPVOID pvReserved, DWORD dwCoInit)
{
    return CoInitializeEx(pvReserved, dwCoInit);
}

void WindowsFirewallAccess::UninitializeWindowsFirewallLibrary()
{
    CoUninitialize();
}

void WindowsFirewallAccess::CleanupWindowsFirewall(INetFwProfile *firewallProfile)
{
    // Esborrar el perfil del firewall
    if (firewallProfile != NULL)
    {
        firewallProfile->Release();
    }
}

}