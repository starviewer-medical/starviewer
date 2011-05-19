#include "querypacsjob.h"

#include <QString>

#include "querypacs.h"
#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"

namespace udg {

QueryPacsJob::QueryPacsJob(PacsDevice pacsDevice, DicomMask mask, QueryLevel queryLevel)
 : PACSJob(pacsDevice)
{
    //creem l'objecte fer la query
    m_queryPacs = new QueryPacs(pacsDevice);
    m_mask = mask;
    m_queryLevel = queryLevel;
}

PACSJob::PACSJobType QueryPacsJob::getPACSJobType()
{
    return PACSJob::QueryPACS;
}

void QueryPacsJob::run()
{
    Settings settings;

    INFO_LOG("Thread iniciat per cercar al PACS: AELocal= " + settings.getValue(InputOutputSettings::LocalAETitle).toString() + "; AEPACS= " +
        getPacsDevice().getAETitle() + "; PACS Adr= " + getPacsDevice().getAddress() + "; PACS Port= " +
        QString().setNum(getPacsDevice().getQueryRetrieveServicePort()) + ";");

    //busquem els estudis
    m_queryRequestStatus = m_queryPacs->query(m_mask);

    INFO_LOG (QString("Consulta al PACS %1 finalitzada").arg(getPacsDevice().getAETitle()));
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

QHash<QString, QString> QueryPacsJob::getHashTablePacsIDOfStudyInstanceUID()
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

PACSRequestStatus::QueryRequestStatus QueryPacsJob::getStatus()
{
    return m_queryRequestStatus;
}

// TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors
// duplicat ni traduccions, però és el millor lloc aquí posar aquest codi?
QString QueryPacsJob::getStatusDescription()
{
    QString message;
    QString pacsAETitle = getPacsDevice().getAETitle();

    switch (getStatus())
    {
        case PACSRequestStatus::QueryOk:
            message = tr("Query %1 to PACS %2 has been successful.").arg(getQueryLevelAsQString(), pacsAETitle);
            break;
        case PACSRequestStatus::QueryCancelled:
            message = tr("Query %1 to PACS %2 has been cancelled.").arg(getQueryLevelAsQString(), pacsAETitle);
            break;
        case PACSRequestStatus::QueryCanNotConnectToPACS:
            message = tr("%1 can't connect to PACS %2 trying to query %3.\n").arg(ApplicationNameString, pacsAETitle, getQueryLevelAsQString());
            message += tr("\nBe sure that your computer is connected on network and the PACS parameters are correct.");
            message += tr("If the problem persists contact with an administrator.");
            break;
        case PACSRequestStatus::QueryFailedOrRefused:
            message = tr("PACS %1 could not process the query.\n\n").arg(pacsAETitle);
            message += tr("Try with a different query or wait a few minutes, if the problem persists contact with PACS administrator.");
            break;
        case PACSRequestStatus::QueryUnknowStatus:
            message = tr("PACS %1 could not process the query returning an unknown error.\n\n").arg(pacsAETitle);
            message += tr("Please contact with PACS administrator to report the issue.");
            break;
        default:
            message = tr("An unknown error has occurred querying %1 to PACS %2.").arg(getQueryLevelAsQString(), pacsAETitle);
            message += tr("\nIf the problem persists contact with an administrator.");
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
