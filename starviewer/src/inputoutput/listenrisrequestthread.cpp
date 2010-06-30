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

/* El motiu que el disseny d'escolta peticions del RIS no segueixi el model tradicional de serveis de xarxa, que consisteix en tenir un thread que s'encarrega
 * d'esperar connexions entrants i una vegada les té, crear un thread fill per atendre aquest nova connexió, mentre el thread pare pot tornar a esperar noves 
 * connexions entrants, és per què amb el RIS PIER aquest model donava problemes, ens trobàvem moltes vegades que amb el temps que transcurria en que s'engegava 
 * el thread i cridàvem el mètode readAll de QTcpServer ens donava un error de que el client remot havia tancat la connexió. Hem de tenir en compte que no teníem 
 * accés al codi del RIS PIER que s'utilitzava per fer la petició, per saber com funcionava i quins timeouts tenia, ni teníem accés a una màquina amb el RIS PIER 
 * per fer proves. Finalment es va optar per provar de processar les peticions del RIS amb aquest model, en que hi ha un mateix thread que accepta les connexions 
 * i les processa, amb aquest model es van acabar els problemes amb les connexions del RIS PIER.
    
 *  Degut aquest mateix fet per això tampoc s'ha implamentat aquesta classe seguint el model recomenat per Qt, en el qual no tenim un loop pendent sempre 
 *  d'acceptar noves connexions, sinó que conectem el signal incomingConnection() amb un slot de la nostra classe que s'activa cada vegada que hi ha una 
 *  nova connexió.

 */
void ListenRISRequestThread::run()
{
    QTcpServer tcpRISServer;
    Settings settings;

    if ( !tcpRISServer.listen(QHostAddress::Any, settings.getValue( InputOutputSettings::RISRequestsPort ).toUInt() ) )
    {
        networkError(&tcpRISServer);
        return;
    }

    while (tcpRISServer.waitForNewConnection(-1)) //Esperem rebre connexions
    {
        INFO_LOG("S'ha rebut una nova connexió de RIS per atendre.");
        while (tcpRISServer.hasPendingConnections())
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

            if (tcpRISServer.hasPendingConnections()) 
            {
                INFO_LOG("Hi ha connexions de RIS pendents per atendre.");
            }
            else INFO_LOG("No hi ha connexions de RIS pendents per atendre.");
        }
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

    ERROR_LOG("No es poden escoltar les peticions del RIS pel port " + QString().setNum(settings.getValue( InputOutputSettings::RISRequestsPort ).toUInt() ) + ", error " + tcpServer->errorString());
        
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
