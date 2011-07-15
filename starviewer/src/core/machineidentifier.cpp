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
    return QString(QCryptographicHash::hash(url.toAscii(), QCryptographicHash::Sha1).toBase64().replace("=", "").replace("+", "-").replace("/", "_"));
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
