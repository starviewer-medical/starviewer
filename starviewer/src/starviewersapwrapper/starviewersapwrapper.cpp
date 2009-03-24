/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewersapwrapper.h"
#include "../inputoutput/starviewersettings.h"
#include <QTcpSocket>
#include <QSettings>

#include "logging.h"

namespace udg {

void StarviewerSapWrapper::sendRequestToLocalStarviewer(QString accessionNumber)
{
    StarviewerSettings settings;
    QTcpSocket tcpSocket;
    QString locaHostAddress = "127.0.0.1";//IP del localhost
    int starviewerRisPort = settings.getListenPortRisRequests();//Port pel que Starviewer espera peticions del RIS

    INFO_LOG(QString("Starviewer_sapwrapper::Demanare a l'Starviewer local pel port %1 la descarrega de l'estudi amb accession number %2").arg(QString().setNum(starviewerRisPort), accessionNumber));

    tcpSocket.connectToHost(locaHostAddress, starviewerRisPort);//Connectem contra el localhost

    if (!tcpSocket.waitForConnected(5000)) //Esperem que ens haguem connectat
    {
        errorConnecting(starviewerRisPort, tcpSocket.errorString());
        return;
    }

    tcpSocket.write(getXmlPier(accessionNumber).toLocal8Bit()); //Enviem la petició de descarregar del estudi

    if (tcpSocket.error() != QAbstractSocket::UnknownSocketError)
    {
        errorWriting(tcpSocket.errorString());
    }

    tcpSocket.close();//desconnectem
    tcpSocket.waitForDisconnected(5000);

    if (tcpSocket.error() != QAbstractSocket::UnknownSocketError)
    {
        errorWriting(tcpSocket.errorString());
    }
    else 
    {
        INFO_LOG("Starviewer_sapwrapper::S'ha enviat amb exit la peticio al Starviewer");
        printf(qPrintable(QString("The request to retrieve the study with accession number %1 has been sent succesfully.\n").arg(accessionNumber)));
    }
}

QString StarviewerSapWrapper::getXmlPier(QString accessionNumber)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?><Msg Name='OpenStudies'><Param Name='AccessionNumber'>" + accessionNumber + "</Param></Msg>";
    return xml;
}

void StarviewerSapWrapper::errorConnecting(int starviewerRisPort, QString errorDescription)
{
    QString messageError = QString("Can't connect with Starviewer on port %1, be sure that Starviewer is running. Error description: %2.\n").arg(QString().setNum(starviewerRisPort), errorDescription);

    ERROR_LOG(QString("Starviewer_sapwrapper::No s'ha pogut connectar amb l'Starviewer pel port %1, descripcio error: %2").arg(QString().setNum(starviewerRisPort), errorDescription));
    printf(qPrintable(messageError));
}

void StarviewerSapWrapper::errorWriting(QString errorDescription)
{
    QString messageError = QString("Error can't send the request to Starviewer. Error description:  %1.\n").arg(errorDescription);

    ERROR_LOG("Starviewer_sapwrapper::No s'ha pogut enviar la peticio al Starviewer, descripcio error: " + errorDescription);
    printf(qPrintable(messageError));
}

void StarviewerSapWrapper::errorClosing(QString errorDescription)
{
    QString messageError = QString("Error while disconnecting from host. Error description:  %1.\n").arg(errorDescription);

    ERROR_LOG("Starviewer_sapwrapper::S'ha produit un error desconnectant del host, descripcio del error: " + errorDescription);
    printf(qPrintable(messageError));
}

}
