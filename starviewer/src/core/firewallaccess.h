#ifndef UDGFIREWALLACCESS_H
#define UDGFIREWALLACCESS_H

#include <windows.h>
#include <netfw.h>
#include <QString>

namespace udg {

/**
    Interfície per comprovar si starviewer té access a través del firewall.
    El mètode \sa newInstance és l'encarregat de crear una instància de les classes d'implementació que es decideix
    segons el sistema operatiu, windows o altres. 
  */
class FirewallAccess {
public:
    enum FirewallStatus { FirewallIsAccessible, FirewallIsBlocking, FirewallError, FirewallUnknownStatus };

    virtual ~FirewallAccess();

    /// Crea una nova instància d'alguna de les classes que implementa la interfície
    static FirewallAccess* newInstance();

    virtual bool doesStarviewerHaveAccesThroughFirewall();

    QString getErrorString();
    FirewallStatus getStatus();

protected:
    /// Per instanciar nous objectes s'ha de fer ús del mètode \sa newInstance
    FirewallAccess();

protected:
    /// Guarda l'estat del firewall
    FirewallStatus m_status;
    /// Si starviewer no té accés a través del firewall, m_errorString guarda la causa
    QString m_errorString;
};

} // End namespace udg

#endif
