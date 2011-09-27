#ifndef UDGWINDOWSFIREWALLACCESS_H
#define UDGWINDOWSFIREWALLACCESS_H

#include "firewallaccess.h"
#include <windows.h>
#include <netfw.h>
#include <QString>

namespace udg {

/**
    Classe que comprova si starviewer té accés a través del firewall de windows.
  */
class WindowsFirewallAccess : public FirewallAccess {
public:
    WindowsFirewallAccess();
    ~WindowsFirewallAccess();

    bool doesStarviewerHaveAccesThroughFirewall();

protected:
    /// Inicialitza el Firewall de windows per tal de poder-lo utilitzar.
    /// @param firewallProfile és de sortida i conté el perfil del firewall.
    virtual HRESULT initializeWindowsFirewall(INetFwProfile **firewallProfile);

    /// Comprova si el firewall està engegat.
    virtual HRESULT windowsFirewallIsOn(INetFwProfile *firewallProfile, bool *firewallOn);

    /// Comprova si el firewall permet excepcions.
    virtual HRESULT doesWindowsFirewallAllowExceptions(INetFwProfile *firewallProfile, bool *exceptionsAllowed);

    /// Donat un perfil de firewall i un path a un executable d'una aplicació, comprova si aquesta està a la llista d'excepcions del firewall.
    /// El resultat que retorna és de si ha anat bé o no.
    /// @param firewallApplicationEnabled és de sortida i guarda si l'aplicació està a la llista d'excepcions o no.
    virtual HRESULT isApplicationEnabledAtFirewall(INetFwProfile *firewallProfile, BSTR firewallProcessImageFileName, bool *firewallApplicationEnabled);

    /// Retorna el path des d'on s'executa starviewer
    virtual BSTR getStarviewerExecutablePath();

    /// Transforma un QString a una cadena de text 'Basic String' de Visual Basic. És responsabilitat de l'usuari alliberar la cadena
    /// quan ja no la necessiti més, utilitzant la funció SysFreeString.
    virtual BSTR fromQStringToBSTR(const QString &string);

    /// Inicialitza la llibreria del Firewall de windows.
    /// Retorna cert si tot ha anat bé.
    virtual HRESULT initializeWindowsFirewallLibrary(LPVOID pvReserved, DWORD dwCoInit);

    /// Finalitza la llibreria del firewall de windows.
    virtual void UninitializeWindowsFirewallLibrary();

    /// Esborra el firewall profile si no és nul
    virtual void CleanupWindowsFirewall(INetFwProfile *firewallProfile);

};

} // End namespace udg

#endif
