#include "machineidentifier.h"

#include <QNetworkInterface>
#include <QCryptographicHash>

namespace udg {

MachineIdentifier::MachineIdentifier()
{
}

QString MachineIdentifier::getMachineID()
{
    return encryptBase64Url(getMacAddress());
}

QString MachineIdentifier::getGroupID()
{
    return encryptBase64Url(getDomain());
}

QString MachineIdentifier::encryptBase64Url(const QString &url)
{
    return QString(QCryptographicHash::hash(url.toAscii(), QCryptographicHash::Sha1).toBase64().replace("=", "").replace("+", "-").replace("/", "_"));
}

QString MachineIdentifier::getMacAddress()
{
    QString macAdress;
    // Primer de tot mirar si hi ha interficia de xarxa local
    bool found = false;
    int index = 0;
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
    while (!found && index < allInterfaces.count())
    {
        QNetworkInterface inter = allInterfaces[index++];

        bool isMainInterface = false;
#ifdef WIN32
        isMainInterface = inter.humanReadableName().contains("local");
#else
        isMainInterface = inter.humanReadableName().contains("eth");
#endif
        QNetworkInterface::InterfaceFlags f = inter.flags();
        bool flagsOk = f.testFlag(QNetworkInterface::IsUp) && f.testFlag(QNetworkInterface::IsRunning) && !f.testFlag(QNetworkInterface::IsLoopBack);

        if (isMainInterface && inter.isValid() && flagsOk)
        {
            macAdress += inter.hardwareAddress();
            found = true;
        }
    }

    // Dono prioritat a la interfície d'àrea local i després busco la primera interfície vàlida
    if (macAdress == "")
    {
        found = false;
        index = 0;
        while (!found && index < allInterfaces.count())
        {
            QNetworkInterface interface = allInterfaces[index++];

            QNetworkInterface::InterfaceFlags flags = interface.flags();
            bool flagsOk = flags.testFlag(QNetworkInterface::IsUp) && flags.testFlag(QNetworkInterface::IsRunning) &&
                           !flags.testFlag(QNetworkInterface::IsLoopBack);

            // Per si de cas el bluetooth està engegat i foncionant, fer que no l'agafi
            // Rarament trobarem una connexió de xarxa que vagi a través d'un dispositiu bluetooth
            if (interface.isValid() && flagsOk && !interface.humanReadableName().toLower().contains("bluetooth"))
            {
                macAdress += interface.hardwareAddress();
                found = true;
            }
        }
    }

    return macAdress;
}

QString MachineIdentifier::getDomain()
{
    QString domain;
    // En cas que estem a windows, Busquem el groupID
#ifdef WIN32
    domain = QProcessEnvironment::systemEnvironment().value(QString("USERDOMAIN"), QString(""));
#endif
    return domain;
}

}
