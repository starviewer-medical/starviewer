/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "querypacsjob.h"

#include <QString>

#include "pacsconnection.h"
#include "pacsserver.h"
#include "querypacs.h"
#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

namespace udg {

QueryPacsJob::QueryPacsJob(PacsDevice pacsDevice , DicomMask mask, QueryLevel queryLevel, QObject *parent)
 : Job(parent)
{
    //creem l'objecte fer la query
    m_queryPacs = new QueryPacs();
    m_mask = mask;
    m_pacsDevice = pacsDevice; 
    m_queryLevel = queryLevel;

    m_isAbortRequested = false;
}

void QueryPacsJob::run()
{
    Status state;
    QString missatgeLog;
    //creem la connexió
    PacsServer pacsServer(m_pacsDevice);

    INFO_LOG( "Thread iniciat per cercar al PACS: AELocal= " + PacsDevice::getLocalAETitle() + "; Port Local= " + QString::number(PacsDevice::getQueryRetrievePort()) + "; AEPACS= " + m_pacsDevice.getAETitle() + "; PACS Adr= " + m_pacsDevice.getAddress() + "; PACS Port= " + m_pacsDevice.getPort() + ";" );

    //Establim a quin nivell farem la cerca en funció del Query Level que ens han especificat
    switch (getQueryLevel())
    {
        case study:
            m_queryStatus = pacsServer.connect(PacsServer::query, PacsServer::studyLevel);
            break;
        case series:
            m_queryStatus = pacsServer.connect(PacsServer::query, PacsServer::seriesLevel);
            break;
        case image:
            m_queryStatus = pacsServer.connect(PacsServer::query, PacsServer::imageLevel);
            break;
    }

    if ( !m_queryStatus.good() )
    {
        ERROR_LOG( QString("Error al connectar al PACS %1. PACS ERROR: %2").arg( m_pacsDevice.getAETitle() ).arg( state.text() ) );
    }
    else
    {
        m_queryPacs->setConnection(pacsServer);
        //busquem els estudis
        m_queryStatus = m_queryPacs->query( m_mask );
        if (! m_queryStatus.good() && !isAbortRequested() )
        {
            //En el cas que hem abortat la query, i per abortar s'hagi abortat l'assocació, retorna el missatge DIMSE Failed to receive message, per això comprovem si hem abortat, perquè no donem aquest missatge com error ja que és normal
            ERROR_LOG( QString("Error al connectar al PACS %1. PACS ERROR: %2").arg( m_pacsDevice.getAETitle() ).arg( m_queryStatus.text() ) );
        }

        INFO_LOG ( QString("Thread del PACS %1 finalitzant").arg( m_pacsDevice.getAETitle() ) );

        //desconnectem
        pacsServer.disconnect();
    }
    setFinished(true);
}

void QueryPacsJob::requestAbort()
{
    m_isAbortRequested = true;
    m_queryPacs->cancelQuery();
}

DicomMask QueryPacsJob::getDicomMask()
{
    return m_mask;
}

QueryPacsJob::QueryLevel QueryPacsJob::getQueryLevel()
{
    return m_queryLevel;
}

PacsDevice QueryPacsJob::getPacsDevice()
{
    return m_pacsDevice;
}

Status QueryPacsJob::getStatus()
{
    return m_queryStatus;
}

bool QueryPacsJob::isAbortRequested()
{
    return m_isAbortRequested;
}

QList<Patient*> QueryPacsJob::getPatientStudyList()
{
    Q_ASSERT (isFinished());
    return m_queryPacs->getQueryResultsAsPatientStudyList();
}

QList<Series*> QueryPacsJob::getSeriesList()
{
    Q_ASSERT (isFinished());
    return m_queryPacs->getQueryResultsAsSeriesList();
}

QList<Image*> QueryPacsJob::getImageList()
{
    Q_ASSERT (isFinished());
    return m_queryPacs->getQueryResultsAsImageList();
}

QHash<QString,QString> QueryPacsJob::getHashTablePacsIDOfStudyInstanceUID()
{
    Q_ASSERT (isFinished());
    return m_queryPacs->getHashTablePacsIDOfStudyInstanceUID();
}

QueryPacsJob::~QueryPacsJob()
{
    delete m_queryPacs;
}

}
