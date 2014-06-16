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

#include "machineidentifier.h"
#include "machineinformation.h"

#include <QCryptographicHash>

namespace udg {

MachineIdentifier::MachineIdentifier()
{
}

QString MachineIdentifier::getMachineID()
{
    return encryptBase64Url(getMACAddress());
}

QString MachineIdentifier::getGroupID()
{
    return encryptBase64Url(getDomain());
}

QString MachineIdentifier::encryptBase64Url(const QString &url)
{
    return QString(QCryptographicHash::hash(url.toLatin1(), QCryptographicHash::Sha1).toBase64().replace("=", "").replace("+", "-").replace("/", "_"));
}

QString MachineIdentifier::getMACAddress()
{
    return MachineInformation().getMACAddress();
}

QString MachineIdentifier::getDomain()
{
    return MachineInformation().getDomain();
}

}
