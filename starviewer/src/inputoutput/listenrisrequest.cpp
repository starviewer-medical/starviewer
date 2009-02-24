/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "listenrisrequest.h"

#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMetaType>
#include <QMessageBox>

#include "parsexmlrispierrequest.h"
#include "logging.h"
#include "starviewersettings.h"
#include "starviewerapplication.h"

namespace udg
{

const int ListenRisRequest::msTimeOutToReadData = 15000;

ListenRisRequest::ListenRisRequest(QObject *parent):QThread(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");//Registrem la classe DicomMask per poder-ne fer un signal
    qRegisterMetaType<ListenRisRequest::ListenRisRequestError>("ListenRisRequest::ListenRisRequestError");
}

void ListenRisRequest::listen()
{
    start(); //engeguem el thread
}

bool ListenRisRequest::isListen()
{
    return isRunning();
}

void ListenRisRequest::run()
{
    QTcpServer tcpRISServer;
    StarviewerSettings settings;

    if (!tcpRISServer.listen(QHostAddress::Any, settings.getListenPortRisRequests()))
    {
        networkError(&tcpRISServer);
        return;
    }

    while (tcpRISServer.waitForNewConnection(-1)) //Esperem rebre connexions
    {
        QTcpSocket *tcpSocket = tcpRISServer.nextPendingConnection();
        QString risRequestData;

        INFO_LOG("Rebuda peticio de la IP " + tcpSocket->peerAddress().toString());
        if (tcpSocket->waitForReadyRead(msTimeOutToReadData))
        {
            risRequestData= QString(tcpSocket->readAll());
            INFO_LOG("Dades rebudes: " + risRequestData);
        }
        else INFO_LOG("No s'ha rebut dades, error: " + tcpSocket->errorString());

        INFO_LOG("Tanco socket");
        tcpSocket->close();
        INFO_LOG("Faig delete del socket");
        delete tcpSocket;

        if (!risRequestData.isEmpty()) processRequest(risRequestData);
    }

    //Si sortim del bucle és que s'ha produït un error
    ERROR_LOG("S'ha produït un error esperant peticions del RIS, error: " + tcpRISServer.errorString());
    networkError(&tcpRISServer);
}

void ListenRisRequest::processRequest(QString risRequestData)
{
    //com ara mateix només rebrem peticions del RIS PIER del IDI, no cal esbrinar quin tipus de petició és per defecte entenem que és petició del RIS PIER
    DicomMask mask;

    INFO_LOG("S'intencarà processar la petició rebuda com a Xml");

    #if QT_VERSION >= 0x040300

    ParseXmlRisPIERRequest parseXml;

    mask = parseXml.parseXml(risRequestData);

    if (!parseXml.error())
    {
        INFO_LOG("Process correcte faig signal");
        emit requestRetrieveStudy(mask);
    }

    #endif
}

void ListenRisRequest::networkError(QTcpServer *tcpServer)
{
    StarviewerSettings settings;

    ERROR_LOG("No es poden escoltar les peticions del RIS pel port " + QString().setNum(settings.getListenPortRisRequests()) + ", error " + tcpServer->errorString());
        
    switch(tcpServer->serverError())
    {
        case QAbstractSocket::AddressInUseError :
            emit errorListening(risPortInUse); 
            break;
        default :
            emit errorListening(unknowNetworkError);
            break;
    }
}

ListenRisRequest::~ListenRisRequest()
{
    terminate();//Parem el thread
    wait();//Esperem que estigui parat
}

}
