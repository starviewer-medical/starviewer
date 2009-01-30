/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "processrisrequestthread.h"

#include <QMetaType>

#include "parsexmlrispierrequest.h"
#include "logging.h"

namespace udg
{

ProcessRisRequestThread::ProcessRisRequestThread(QObject *parent)
 : QThread(parent)
{
    qRegisterMetaType<DicomMask>("DicomMask");
}

void ProcessRisRequestThread::process(QTcpSocket *qTcpSocket)
{
    m_qTcpSocket = qTcpSocket;

    start();//Engeguem el thread
}

void ProcessRisRequestThread::run()
{
    connect(m_qTcpSocket,SIGNAL(readyRead()),SLOT(prova()));
    if (m_qTcpSocket->waitForReadyRead(15000)) 
    {
        QString requestXML = QString(m_qTcpSocket->readAll());
        INFO_LOG("HE LLEGIT " + QString(requestXML));
        processRequest(requestXML);
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
