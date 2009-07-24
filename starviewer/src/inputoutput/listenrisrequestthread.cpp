/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "listenrisrequestthread.h"

#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMetaType>
#include <QMessageBox>

#include "parsexmlrispierrequest.h"
#include "logging.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"

namespace udg
{

const int ListenRISRequestThread::TimeOutToReadData = 15000;

ListenRISRequestThread::ListenRISRequestThread(QObject *parent):QThread(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");//Registrem la classe DicomMask per poder-ne fer un signal
    qRegisterMetaType<ListenRISRequestThread::ListenRISRequestThreadError>("ListenRISRequestThread::ListenRISRequestThreadError");
}

void ListenRISRequestThread::listen()
{
    start(); //engeguem el thread
}

bool ListenRISRequestThread::isListen()
{
    return isRunning();
}

void ListenRISRequestThread::run()
{
    QTcpServer tcpRISServer;
    Settings settings;

    if ( !tcpRISServer.listen(QHostAddress::Any, settings.getValue( InputOutputSettings::RisRequestsPort ).toUInt() ) )
    {
        networkError(&tcpRISServer);
        return;
    }

    while (tcpRISServer.waitForNewConnection(-1)) //Esperem rebre connexions
    {
        QTcpSocket *tcpSocket = tcpRISServer.nextPendingConnection();
        QString risRequestData;

        INFO_LOG("Rebuda peticio de la IP " + tcpSocket->peerAddress().toString());
        if (tcpSocket->waitForReadyRead(TimeOutToReadData))
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

void ListenRISRequestThread::processRequest(QString risRequestData)
{
    //com ara mateix només rebrem peticions del RIS PIER del IDI, no cal esbrinar quin tipus de petició és per defecte entenem que és petició del RIS PIER
    DicomMask mask;

    INFO_LOG("S'intencarà processar la petició rebuda com a Xml");

    ParseXmlRisPIERRequest parseXml;

    mask = parseXml.parseXml(risRequestData);

    if (!parseXml.error())
    {
        INFO_LOG("Process correcte faig signal");
        emit requestRetrieveStudy(mask);
    }
}

void ListenRISRequestThread::networkError(QTcpServer *tcpServer)
{
    Settings settings;

    ERROR_LOG("No es poden escoltar les peticions del RIS pel port " + QString().setNum(settings.getValue( InputOutputSettings::RisRequestsPort ).toUInt() ) + ", error " + tcpServer->errorString());
        
    switch(tcpServer->serverError())
    {
        case QAbstractSocket::AddressInUseError :
            emit errorListening(RisPortInUse); 
            break;
        default :
            emit errorListening(UnknownNetworkError);
            break;
    }
}

ListenRISRequestThread::~ListenRISRequestThread()
{
    terminate();//Parem el thread
    wait();//Esperem que estigui parat
}

}
