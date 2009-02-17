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
#include "qpopuprisrequestsscreen.h"

namespace udg
{

ListenRisRequest::ListenRisRequest(QObject *parent):QObject(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");//Registrem la classe DicomMask per poder-ne fer un signal

	m_popUp = new QPopUpRisRequestsScreen();
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
	QTcpSocket *tcpSocket = m_qTcpServer->nextPendingConnection();

	INFO_LOG("Rebuda petició de connexió per la IP " + tcpSocket->peerAddress().toString());
	processRisRequestThread->process(tcpSocket->socketDescriptor());
    connect(processRisRequestThread, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(requestRetrieveStudySlot(DicomMask)));

	//Quan ha acabat el thread de processament fa delete del processRisRequestThread i del QTcpSocket
	connect(processRisRequestThread, SIGNAL(finished()), processRisRequestThread, SLOT(deleteLater()));
	connect(processRisRequestThread, SIGNAL(finished()), tcpSocket, SLOT(deleteLater()));
}

void ListenRisRequest::requestRetrieveStudySlot(DicomMask mask)
{
    m_popUp->setAccessionNumber(mask.getAccessionNumber());
    m_popUp->show();
    emit requestRetrieveStudy(mask);
}

void ListenRisRequest::showNetworkError()
{
    StarviewerSettings settings;
    QString message;

    switch(m_qTcpServer->serverError())
    {
        case QAbstractSocket::AddressInUseError :
            message = tr("Can't listen RIS requests on port %1, the port is used for another application.").arg(settings.getListenPortRisRequests());
            message += tr("\n\nIf the error has produced when openned new %1's windows, close that window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);
            ERROR_LOG(QString("No es poden escoltar les peticions del RIS pel port %1, perquè una altra aplicació ja l'esta utilitzant").arg(settings.getListenPortRisRequests()));
            break;
        default :
            message = tr("Can't listen RIS requests on port %1, an unknow network error has produced.").arg(settings.getListenPortRisRequests());
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            ERROR_LOG(QString("No es poden escoltar les peticions del RIS pel port %1, s' ha produït un error no controlat : " + m_qTcpServer->errorString()).arg(settings.getListenPortRisRequests()));
            break;
    }

    QMessageBox::critical((QWidget* )this->parent(), ApplicationNameString, message);
}

}
