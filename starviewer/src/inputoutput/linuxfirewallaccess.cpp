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

#include "linuxfirewallaccess.h"

#include "inputoutputsettings.h"
#include "logging.h"

#include <QProcess>

namespace udg {

bool LinuxFirewallAccess::doesStarviewerHaveAccesThroughFirewall()
{
    uint port = Settings().getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toUInt();
    QString queryPort = QString("--query-port=%1/tcp").arg(port);
    QProcess firewallCmd;
    firewallCmd.start("firewall-cmd", {queryPort}, QIODevice::ReadOnly | QIODevice::Text);

    if (!firewallCmd.waitForFinished())
    {
        WARN_LOG("firewall-cmd failed");
        m_status = FirewallError;
        m_errorString = QObject::tr("Unable to check if Starviewer port is open in the firewall");
        return false;
    }

    int exitCode = firewallCmd.exitCode();

    if (exitCode == 0)
    {
        m_status = FirewallIsAccessible;
        return true;
    }
    else if (exitCode == 1)
    {
        m_status = FirewallIsBlocking;
        m_errorString = QObject::tr("Starviewer port is blocked by the firewall");
        return false;
    }
    else
    {
        WARN_LOG(QString("Unexpected exit code from firewall-cmd: %1").arg(exitCode));
        m_status = FirewallError;
        m_errorString = QObject::tr("Unable to check if Starviewer port is open in the firewall");
        return false;
    }
}

} // namespace udg
