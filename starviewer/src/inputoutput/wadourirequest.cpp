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

#include "wadourirequest.h"

#include "dicommask.h"
#include "image.h"
#include "localdatabasemanager.h"
#include "series.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"
#include "wadoinstancedownloader.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace udg {

WadoUriRequest::WadoUriRequest(const PacsDevice &pacs, const QString &studyInstanceUid, const QString &seriesInstanceUid, const QString &sopInstanceUid,
                               QObject *parent)
    : WadoRequest(pacs, parent), m_studyInstanceUid(studyInstanceUid), m_seriesInstanceUid(seriesInstanceUid), m_sopInstanceUid(sopInstanceUid),
      m_numberOfInstancesDownloaded(0)
{
}

const QString& WadoUriRequest::getStudyInstanceUid() const
{
    return m_studyInstanceUid;
}

const QString& WadoUriRequest::getSeriesInstanceUid() const
{
    return m_seriesInstanceUid;
}

const QString& WadoUriRequest::getSopInstanceUid() const
{
    return m_sopInstanceUid;
}

void WadoUriRequest::cancel()
{
    for (QNetworkReply *reply : qAsConst(m_pendingReplies))
    {
        reply->abort();
    }
}

// This is run in the WADO thread.
void WadoUriRequest::startInternal()
{
    if (!ensureEnoughHardDiskSpace())
    {
        return;
    }

    emit started();

    if (!obtainSopInstanceUids())
    {
        emit finished();
        return;
    }

    QUrl baseUri = getPacsDevice().getBaseUri();
    INFO_LOG(QString("Starting retrieve of %1 instances from WADO-URI PACS with base URI %2.").arg(m_instancesToDownload.size()).arg(baseUri.toString()));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("requestType", "WADO");
    urlQuery.addQueryItem("contentType", "application/dicom");
    urlQuery.addQueryItem("studyUID", m_studyInstanceUid);

    m_downloadedFilesProcessor.reset(new DownloadedFilesProcessor(getPacsDevice()));
    m_downloadedFilesProcessor->beginDownloadStudy(m_studyInstanceUid);

    for (const auto &pair : qAsConst(m_instancesToDownload))
    {
        QUrlQuery specificQuery(urlQuery);
        specificQuery.addQueryItem("seriesUID", pair.first);
        specificQuery.addQueryItem("objectUID", pair.second);

        QUrl url(baseUri);
        url.setQuery(specificQuery);

        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

        QNetworkReply *reply = m_networkAccessManager->get(request);
        m_pendingReplies.insert(reply);

        connect(reply, &QNetworkReply::finished, this, [=] {
            onReplyFinished(reply);
        }, Qt::DirectConnection);
    }
}

// This is run in the WADO thread.
bool WadoUriRequest::ensureEnoughHardDiskSpace()
{
    // Copied and adapted from RetrieveDICOMFilesFromPACSJob::thereIsAvailableSpaceOnHardDisk
    LocalDatabaseManager localDatabaseManager;
    connect(&localDatabaseManager, &LocalDatabaseManager::studyWillBeDeleted, this, &WadoUriRequest::studyFromCacheWillBeDeleted);

    // This method will delete studies if configured in settings if there is not enough space. It returns whether there is enough space after that.
    if (!localDatabaseManager.thereIsAvailableSpaceOnHardDisk())
    {
        m_status = Status::Errored;

        if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok)
        {
            m_errorsDescription = tr("Not enough space to download studies.");  // TODO unify messages with the ones for DIMSE
        }
        else
        {
            m_errorsDescription = tr("Error when trying to delete studies to free up space in disk.");
        }

        return false;
    }

    return true;
}

// This is run in the WADO thread.
bool WadoUriRequest::obtainSopInstanceUids()
{
    if (!m_sopInstanceUid.isEmpty())
    {
        m_instancesToDownload.insert({m_seriesInstanceUid, m_sopInstanceUid});
        return true;
    }

    INFO_LOG(QString("Asking PACS %1 about all instances to download.").arg(getPacsDevice().getAETitle()));

    DicomMask mask;
    mask.setStudyInstanceUID(m_studyInstanceUid);
    mask.setSeriesInstanceUID(m_seriesInstanceUid.isNull() ? "" : m_seriesInstanceUid);
    mask.setSOPInstanceUID("");
    StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(getPacsDevice(), mask, StudyOperations::TargetResource::Instances);

    const QList<Series*> &series = result->getSeries();
    const QList<Image*> &instances = result->getInstances();

    for (int i = 0; i < instances.size(); i++)
    {
        m_instancesToDownload.insert({series[i]->getInstanceUID(), instances[i]->getSOPInstanceUID()});
    }

    qDeleteAll(series);
    qDeleteAll(instances);

    bool ok = result->getResultType() != StudyOperationResult::ResultType::Error;

    if (!ok)
    {
        m_status = Status::Errored;
        m_errorsDescription = result->getErrorText();
        ERROR_LOG(m_errorsDescription);
    }
    else if (!result->getErrorText().isEmpty())
    {
        m_status = Status::Warnings;
        m_errorsDescription = result->getErrorText();
        WARN_LOG(m_errorsDescription);
    }

    delete result;

    // With Raïmserver in some cases we can receive an empty response without error because of a 10 minute limit (see #2982).
    if (m_instancesToDownload.isEmpty())
    {
        m_status = Status::Errored;
        m_errorsDescription = tr("Could not obtain the list of files in the study.");
        ERROR_LOG(m_errorsDescription);
        ok = false;
    }

    return ok;
}

// This is run in the WADO thread.
void WadoUriRequest::onReplyFinished(QNetworkReply *reply)
{
    m_pendingReplies.remove(reply);

    if (reply->error() == QNetworkReply::NoError)
    {
        QUrlQuery query(reply->url().query());
        WadoInstanceDownloader wadoInstanceDownloader;
        wadoInstanceDownloader.setStudyInstanceUid(m_studyInstanceUid);
        wadoInstanceDownloader.setSeriesInstanceUid(query.queryItemValue("seriesUID"));
        wadoInstanceDownloader.setSopInstanceUid(query.queryItemValue("objectUID"));
        wadoInstanceDownloader.setInstanceSize(reply->bytesAvailable());
        wadoInstanceDownloader.setDataSource(reply);
        wadoInstanceDownloader.read();

        emit instanceDownloaded(++m_numberOfInstancesDownloaded);

        if (!m_downloadedSeriesUids.contains(wadoInstanceDownloader.getSeriesInstanceUid()))
        {
            m_downloadedSeriesUids.insert(wadoInstanceDownloader.getSeriesInstanceUid());
            emit seriesDownloaded(m_downloadedSeriesUids.size());
        }

        m_downloadedFilesProcessor->processFile(wadoInstanceDownloader.getPath());
    }
    else if (reply->error() == QNetworkReply::OperationCanceledError)
    {
        if (m_status != Status::Cancelled)  // first cancelled reply
        {
            INFO_LOG(QString("Cancelled retrieve of study %1 (series %2, instance %3).").arg(m_studyInstanceUid, m_seriesInstanceUid, m_sopInstanceUid));
            m_downloadedFilesProcessor->cancelDownloadStudy();
            m_status = Status::Cancelled;
        }
    }
    else
    {
        m_status = Status::Warnings;

        if (m_errorsDescription.size() < 1000)  // avoid excessive errors accumulation
        {
            m_errorsDescription += '\n' + reply->errorString();
        }
        else if (!m_errorsDescription.endsWith("…"))
        {
            m_errorsDescription += "\n…";
        }

        ERROR_LOG(QString("QNetworkReply::NetworkError %1: %2. (HTTP %3: %4)")
                  .arg(reply->error()).arg(reply->errorString())
                  .arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())
                  .arg(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()));
    }

    reply->deleteLater();

    if (m_pendingReplies.isEmpty())
    {
        if (m_numberOfInstancesDownloaded > 0)
        {
            INFO_LOG(QString("Finished retrieve of study %1 (series %2, instance %3). %4 instance(s) downloaded.")
                     .arg(m_studyInstanceUid, m_seriesInstanceUid, m_sopInstanceUid).arg(m_numberOfInstancesDownloaded));
            LocalDatabaseManager::LastError error = m_downloadedFilesProcessor->finishDownloadStudy();

            if (error != LocalDatabaseManager::Ok)
            {
                m_status = Status::Errored;

                if (error == LocalDatabaseManager::PatientInconsistent)
                {
                    m_errorsDescription += '\n' + tr("Can't read correctly data from downloaded images. They may be corrupt.");
                }
                else
                {
                    m_errorsDescription += '\n' + tr("Database error when saving downloaded images.");
                }

                ERROR_LOG(m_errorsDescription);
            }
            else
            {
                if (!m_errorsDescription.isEmpty())
                {
                    m_errorsDescription = tr("Some images have been downloaded. However there have been some errors:\n\n") + m_errorsDescription;
                    // TODO unify error messages with WADO-RS and DIMSE
                }
            }
        }
        else
        {
            m_downloadedFilesProcessor->cancelDownloadStudy();

            if (m_status != Status::Cancelled)
            {
                m_status = Status::Errored;
                m_errorsDescription = tr("Could not download any image. Errors:\n\n") + m_errorsDescription;
            }
        }

        m_errorsDescription = m_errorsDescription.trimmed();    // trim possible extra \n

        if (m_status == Status::Cancelled)
        {
            emit cancelled();
        }
        else
        {
            emit finished();
        }
    }
}

} // namespace udg
