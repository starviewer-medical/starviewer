#include "firewallaccess.h"

#ifdef WIN32
#include "windowsfirewallaccess.h"
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
#ifdef WIN32
    return new WindowsFirewallAccess();
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