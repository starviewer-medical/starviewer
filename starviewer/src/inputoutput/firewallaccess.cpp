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

#include "firewallaccess.h"

#if defined Q_OS_WIN32
#include "windowsfirewallaccess.h"
#elif defined Q_OS_LINUX
#include "linuxfirewallaccess.h"
#endif

namespace udg {

FirewallAccess::FirewallAccess()
{
    m_status = FirewallAccess::FirewallUnknownStatus;
}

FirewallAccess::~FirewallAccess()
{
}

FirewallAccess* FirewallAccess::newInstance()
{
#if defined Q_OS_WIN32
    return new WindowsFirewallAccess();
#elif defined Q_OS_LINUX
    return new LinuxFirewallAccess();
#else
    return new FirewallAccess();
#endif
}

bool FirewallAccess::doesStarviewerHaveAccesThroughFirewall()
{
    return true;   
}

QString FirewallAccess::getErrorString()
{
    return m_errorString;
}

FirewallAccess::FirewallStatus FirewallAccess::getStatus()
{
    return m_status;
}

}
