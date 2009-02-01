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

#include "processrisrequestthread.h"
#include "logging.h"
#include "starviewersettings.h"
#include "starviewerapplication.h"

namespace udg
{

ListenRisRequest::ListenRisRequest(QObject *parent):QObject(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");//Registrem la classe DicomMask per poder-ne fer un signal
}

void ListenRisRequest::listen()
{
    StarviewerSettings settings;
    m_qTcpServer = new QTcpServer();

    if (!m_qTcpServer->listen(QHostAddress::Any, settings.getListenPortRisRequests()))
    {
        showNetworkError();
    }
    else INFO_LOG(QString("Iniciada l'escolta de peticions del RIS pel port %1").arg(settings.getListenPortRisRequests()));

    connect(m_qTcpServer, SIGNAL(newConnection()), SLOT(newConnection()));//Connectem a l'slot per quan arribin noves connexions
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

void ListenRisRequest::showNetworkError()
{
    StarviewerSettings settings;
    QString message;

    switch(m_qTcpServer->serverError())
    {
        case QAbstractSocket::AddressInUseError :
            message = tr("Can't listen RIS requests on port %1, the port is used for another application").arg(settings.getListenPortRisRequests());
            message += tr("\n\nIf you want to open different Starviewer's windows always choose the 'New' option from the File menu.");
            ERROR_LOG(QString("No es poden escoltar les peticions del RIS pel port %1, perquè una altra aplicació ja l'esta utilitzant").arg(settings.getListenPortRisRequests()));
            break;
        default :
            message = tr("Can't listen Ris requests on port %1, an unknow network error has produced").arg(settings.getListenPortRisRequests());
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            ERROR_LOG(QString("No es poden escoltar les peticions del RIS pel port %1, s' ha produït un error no controlat : " + m_qTcpServer->errorString()).arg(settings.getListenPortRisRequests()));
            break;
    }

    QMessageBox::critical((QWidget* )this->parent(), ApplicationNameString, message);
}

}
