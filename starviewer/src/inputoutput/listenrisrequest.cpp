/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "listenrisrequest.h"

#include "processrisrequestthread.h"

#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMetaType>

#include "logging.h"

namespace udg
{

ListenRisRequest::ListenRisRequest(QObject *parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");
}

void ListenRisRequest::listen()
{
    m_qTcpServer = new QTcpServer();

    if (!m_qTcpServer->listen(QHostAddress::Any, 12345))
    {
        DEBUG_LOG("ERROR AL INTENTAR ESCOLTAR" + m_qTcpServer->errorString());
    }

    connect(m_qTcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
}

bool ListenRisRequest::isListen()
{
    return m_qTcpServer->isListening();
}

void ListenRisRequest::newConnection()
{
    ProcessRisRequestThread * processRisRequestThread = new ProcessRisRequestThread();

    processRisRequestThread->process(m_qTcpServer->nextPendingConnection());

    connect(processRisRequestThread,SIGNAL(requestRetrieveStudy(DicomMask)),SIGNAL(requestRetrieveStudy(DicomMask)));
}

}
