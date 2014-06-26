/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "querypacsjob.h"

#include <QString>

#include "querypacs.h"
#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "inputoutputsettings.h"
#include "usermessage.h"

namespace udg {

QueryPacsJob::QueryPacsJob(PacsDevice pacsDevice, DicomMask mask, QueryLevel queryLevel)
 : PACSJob(pacsDevice)
{
    // Creem l'objecte fer la query
    m_queryPacs = new QueryPacs(pacsDevice);
    m_mask = mask;
    m_queryLevel = queryLevel;
}

QueryPacsJob::~QueryPacsJob()
{
    delete m_queryPacs;
}

PACSJob::PACSJobType QueryPacsJob::getPACSJobType()
{
    return PACSJob::QueryPACS;
}

void QueryPacsJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(self)
    Q_UNUSED(thread)

    Settings settings;

    INFO_LOG("Thread iniciat per cercar al PACS: AELocal= " + settings.getValue(InputOutputSettings::LocalAETitle).toString() + "; AEPACS= " +
        getPacsDevice().getAETitle() + "; PACS Adr= " + getPacsDevice().getAddress() + "; PACS Port= " +
        QString().setNum(getPacsDevice().getQueryRetrieveServicePort()) + ";");

    // Busquem els estudis
    m_queryRequestStatus = m_queryPacs->query(m_mask);

    INFO_LOG(QString("Consulta al PACS %1 finalitzada").arg(getPacsDevice().getAETitle()));
}

DicomMask QueryPacsJob::getDicomMask()
{
    return m_mask;
}

QueryPacsJob::QueryLevel QueryPacsJob::getQueryLevel()
{
    return m_queryLevel;
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

void QueryPacsJob::requestCancelJob()
{
    INFO_LOG(QString("S'ha demanat la cancel.lacio del Job de consulta al PACS %1").arg(getPacsDevice().getAETitle()));
    m_queryPacs->cancelQuery();
}

PACSRequestStatus::QueryRequestStatus QueryPacsJob::getStatus()
{
    return m_queryRequestStatus;
}

// TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors
// duplicat ni traduccions, però és el millor lloc aquí posar aquest codi?
QString QueryPacsJob::getStatusDescription()
{
    QString message;
    QString errorDetails = "\n\n" + tr("Error details:") + "\n" + m_queryPacs->getResponseStatus().toString();
    QString pacsAETitle = getPacsDevice().getAETitle();

    switch (getStatus())
    {
        case PACSRequestStatus::QueryOk:
            message = tr("Query to PACS %2 for %1 has been successful.").arg(getQueryLevelAsQString(), pacsAETitle);
            break;
        case PACSRequestStatus::QueryCancelled:
            message = tr("Query to PACS %2 for %1 has been cancelled.").arg(getQueryLevelAsQString(), pacsAETitle);
            break;
        case PACSRequestStatus::QueryCanNotConnectToPACS:
            message = tr("Query failed: Unable to connect to PACS %1.").arg(pacsAETitle);
            message += "\n\n";
            message += tr("Make sure your computer is connected to the network and the PACS configuration is correct.");
            message += UserMessage::getProblemPersistsAdvice();
            break;
        case PACSRequestStatus::QueryFailedOrRefused:
            message = tr("PACS %1 could not process the query.").arg(pacsAETitle);
            message += "\n\n";
            message += tr("Try with a different query or wait a few minutes. If the problem persists contact with a PACS administrator.");
            message += errorDetails;
            break;
        case PACSRequestStatus::QueryUnknowStatus:
            message = tr("PACS %1 could not process the query and returned an unknown error.").arg(pacsAETitle);
            message += "\n\n";
            message += tr("Please contact with a PACS administrator to report the issue.");
            message += errorDetails;
            break;
        default:
            message = tr("An unknown error has occurred while querying PACS %2 for %1.").arg(getQueryLevelAsQString(), pacsAETitle);
            message += "\n\n";
            message += UserMessage::getProblemPersistsAdvice();
            message += errorDetails;
            break;
    }

    return message;
}

QString QueryPacsJob::getQueryLevelAsQString()
{
    switch (m_queryLevel)
    {
        case study:
            return tr("studies");
        case series:
            return tr("series");
        case image:
            return tr("images");
        default:
            return tr("unknown query level");
    }
}
}
