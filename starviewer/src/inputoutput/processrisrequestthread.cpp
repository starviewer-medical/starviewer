/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "processrisrequestthread.h"

#include <QMetaType>
#include <QHostAddress>

#include "parsexmlrispierrequest.h"
#include "logging.h"

namespace udg
{

const int ProcessRisRequestThread::msTimeOutToReadData = 15000;

ProcessRisRequestThread::ProcessRisRequestThread(QObject *parent)
 : QThread(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask"); //Registrem la classe DicomMask per poder-ne fer un signal
}

void ProcessRisRequestThread::process(QTcpSocket *qTcpSocket)
{
    m_qTcpSocket = qTcpSocket;

    start();//Engeguem el thread
}

void ProcessRisRequestThread::run()
{
    if (m_qTcpSocket->waitForReadyRead(msTimeOutToReadData)) 
    {
        QString requestXML = QString(m_qTcpSocket->readAll());

        INFO_LOG("He rebut de la IP " + m_qTcpSocket->peerAddress().toString() + " la cadena " + QString(requestXML));
        processRequest(requestXML);
    }
    else INFO_LOG("S'ha produït timeout esperant llegir dades de la IP " + m_qTcpSocket->peerAddress().toString());

    if ( m_qTcpSocket->error() != QAbstractSocket::UnknownSocketError)
    {
        ERROR_LOG("S'ha produït un error m'entre s'esperava rebre dades del RIS" + m_qTcpSocket->errorString());
    }
    m_qTcpSocket->close();
}

void ProcessRisRequestThread::processRequest(QString request)
{
    //com ara mateix només rebrem peticions del RIS PIER del IDI, no cal esbrinar quin tipus de petició és per defecte entenem que és petició del RIS PIER
    DicomMask mask;

    #if QT_VERSION >= 0x040300

    ParseXmlRisPIERRequest parseXml;

    mask = parseXml.parseXml(request);

    if (!parseXml.error())
    {
        emit requestRetrieveStudy(mask);
    }

    #endif
}

}
