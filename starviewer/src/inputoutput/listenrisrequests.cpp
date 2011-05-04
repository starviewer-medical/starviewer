#include "listenrisrequests.h"

#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMetaType>

#include "parsexmlrispierrequest.h"
#include "logging.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"

namespace udg
{

const int ListenRISRequests::TimeOutToReadData = 15000;

ListenRISRequests::ListenRISRequests()
{
    qRegisterMetaType<DicomMask>("DicomMask");//Registrem la classe DicomMask per poder-ne fer un signal
    qRegisterMetaType<ListenRISRequests::ListenRISRequestsError>("ListenRISRequests::ListenRISRequestsError");
}

void ListenRISRequests::stopListen()
{
    if (m_tcpRISServer->isListening())
    {
        m_tcpRISServer->close();
    }
}

void ListenRISRequests::listen()
{
    m_tcpRISServer = new QTcpServer();

    if (!m_tcpRISServer->listen(QHostAddress::Any, Settings().getValue(InputOutputSettings::RISRequestsPort).toUInt()))
    {
        networkError(m_tcpRISServer);
        return;
    }

    connect(m_tcpRISServer, SIGNAL(newConnection()), SLOT(newRISRequest()));
}

void ListenRISRequests::newRISRequest()
{
    QTcpSocket *tcpSocket = m_tcpRISServer->nextPendingConnection();
    QString risRequestData;

    INFO_LOG("Rebuda peticio de la IP " + tcpSocket->peerAddress().toString());
    if (tcpSocket->waitForReadyRead(TimeOutToReadData))
    {
        risRequestData = QString(tcpSocket->readAll());
        INFO_LOG("Dades rebudes: " + risRequestData);
    }
    else
    {
        INFO_LOG("No s'ha rebut dades, error: " + tcpSocket->errorString());
    }

    INFO_LOG("Tanco socket");
    tcpSocket->disconnectFromHost();
    INFO_LOG("Faig delete del socket");
    delete tcpSocket;

    if (!risRequestData.isEmpty()) processRequest(risRequestData);
}

void ListenRISRequests::processRequest(QString risRequestData)
{
    //com ara mateix només rebrem peticions del RIS PIER del IDI, no cal esbrinar quin tipus de petició és per defecte entenem que és petició del RIS PIER
    INFO_LOG("S'intencarà processar la petició rebuda com a Xml");

    ParseXmlRisPIERRequest parseXml;

    DicomMask mask = parseXml.parseXml(risRequestData);

    if (!parseXml.error())
    {
        INFO_LOG("Process correcte faig signal");
        emit requestRetrieveStudy(mask);
    }
}

void ListenRISRequests::networkError(QTcpServer *tcpServer)
{
    ERROR_LOG("No es poden escoltar les peticions del RIS pel port " + QString().setNum(Settings().getValue(InputOutputSettings::RISRequestsPort).toUInt()) + ", error " + tcpServer->errorString());

    switch (tcpServer->serverError())
    {
        case QAbstractSocket::AddressInUseError:
            emit errorListening(RisPortInUse);
            break;
        default:
            emit errorListening(UnknownNetworkError);
            break;
    }
}

}
