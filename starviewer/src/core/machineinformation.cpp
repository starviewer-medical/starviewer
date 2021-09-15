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

#include "machineinformation.h"

#include <QNetworkInterface>
#include <QProcessEnvironment>

namespace udg {

MachineInformation::MachineInformation()
{
}

QString MachineInformation::getMACAddress()
{
    QNetworkInterface candidates[4];
    // These numbers define the priority order for the candidates
    constexpr int ConnectedEthernet = 0, ConnectedWifi = 1, DisconnectedEthernet = 2, DisconnectedWifi = 3;

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces())
    {
        if (!interface.isValid())
        {
            continue;
        }

        QNetworkInterface::InterfaceFlags flags = interface.flags();
        bool flagsOk = flags.testFlag(QNetworkInterface::IsUp) && flags.testFlag(QNetworkInterface::IsRunning) &&
                       !flags.testFlag(QNetworkInterface::IsLoopBack);
        int ips = interface.addressEntries().size();

        if (flagsOk)
        {
            int index = -1;

            if (interface.type() == QNetworkInterface::Ethernet)
            {
                if (ips > 0)
                {
                    index = ConnectedEthernet;
                }
                else
                {
                    index = DisconnectedEthernet;
                }
            }
            else if (interface.type() == QNetworkInterface::Wifi)
            {
                if (ips > 0)
                {
                    index = ConnectedWifi;
                }
                else
                {
                    index = DisconnectedWifi;
                }
            }

            if (index >= 0)
            {
                if (!candidates[index].isValid())
                {
                    candidates[index] = interface;

                    if (index == 0)
                    {
                        // This is the best possible option, no need to continue
                        break;
                    }
                }
            }
        }
    }

    QString macAdress;

    for (int i = 0; i < 4; i++)
    {
        if (candidates[i].isValid())
        {
            macAdress = candidates[i].hardwareAddress();
            break;
        }
    }

    return macAdress;
}

QString MachineInformation::getDomain()
{
    QString domain;
    // En cas que estem a windows, Busquem el groupID
#ifdef WIN32
    domain = getSystemEnvironment().value(QString("USERDOMAIN"), QString(""));
#endif
    return domain;
}

QProcessEnvironment MachineInformation::getSystemEnvironment()
{
    return QProcessEnvironment::systemEnvironment();
}

} // namespace udg
