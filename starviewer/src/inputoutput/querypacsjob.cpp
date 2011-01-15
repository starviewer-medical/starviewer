/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "querypacsjob.h"

#include <QString>

#include "pacsconnection.h"
#include "querypacs.h"
#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "inputoutputsettings.h"
#include "errordcmtk.h"

namespace udg {

QueryPacsJob::QueryPacsJob(PacsDevice pacsDevice, DicomMask mask, QueryLevel queryLevel)
 : PACSJob(pacsDevice)
{
    //creem l'objecte fer la query
    m_queryPacs = new QueryPacs();
    m_mask = mask;
    m_queryLevel = queryLevel;
}

PACSJob::PACSJobType QueryPacsJob::getPACSJobType()
{
    return PACSJob::QueryPACS;
}

void QueryPacsJob::run()
{
    PACSConnection pacsConnection(getPacsDevice());
    Settings settings;

    INFO_LOG("Thread iniciat per cercar al PACS: AELocal= " + settings.getValue(InputOutputSettings::LocalAETitle).toString() + "; AEPACS= " + 
        getPacsDevice().getAETitle() + "; PACS Adr= " + getPacsDevice().getAddress() + "; PACS Port= " + 
        QString().setNum(getPacsDevice().getQueryRetrieveServicePort()) + ";");

    if (!pacsConnection.connectToPACS(PACSConnection::Query))
    {
        ERROR_LOG("S'ha produit un error al intentar connectar al PACS per fer query. AETitle: " + getPacsDevice().getAETitle());
        m_queryStatus = Status().setStatus(DcmtkCanNotConnectError);
    }
    else
    {
        m_queryPacs->setConnection(pacsConnection);
        //busquem els estudis
        m_queryStatus = m_queryPacs->query(m_mask);
        if (! m_queryStatus.good() && !isAbortRequested())
        {
            /*En el cas que hem abortat la query, i per abortar s'hagi abortat l'assocació, retorna el missatge DIMSE Failed to receive message, 
            per això comprovem si hem abortat, perquè no donem aquest missatge com error ja que és normal*/
            ERROR_LOG(QString("Error al fer query al PACS %1. PACS ERROR: %2").arg(getPacsDevice().getAETitle()).arg(m_queryStatus.text()));
        }

        pacsConnection.disconnect();
        INFO_LOG (QString("Consulta al PACS %1 finalitzada").arg(getPacsDevice().getAETitle()));
    }

    //TODO:Cal?
    setFinished(true);
}

DicomMask QueryPacsJob::getDicomMask()
{
    return m_mask;
}

QueryPacsJob::QueryLevel QueryPacsJob::getQueryLevel()
{
    return m_queryLevel;
}

Status QueryPacsJob::getStatus()
{
    return m_queryStatus;
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

void QueryPacsJob::requestCancelJob()
{
    INFO_LOG(QString("S'ha demanat la cancel.lacio del Job de consulta al PACS %1").arg(getPacsDevice().getAETitle()));
    m_queryPacs->cancelQuery();
}

}
