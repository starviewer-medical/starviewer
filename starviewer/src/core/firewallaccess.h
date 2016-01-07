/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGFIREWALLACCESS_H
#define UDGFIREWALLACCESS_H

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
