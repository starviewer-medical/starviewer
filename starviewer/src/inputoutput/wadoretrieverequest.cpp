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

#include "wadoretrieverequest.h"

#include "localdatabasemanager.h"
#include "logging.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace udg {

WadoRetrieveRequest::WadoRetrieveRequest(const PacsDevice &pacs, const QString &studyInstanceUid, const QString &seriesInstanceUid,
                                         const QString &sopInstanceUid, QObject *parent)
    : WadoRequest(pacs, parent), m_studyInstanceUid(studyInstanceUid), m_seriesInstanceUid(seriesInstanceUid), m_sopInstanceUid(sopInstanceUid),
      m_numberOfInstancesDownloaded(0)
{
}

const QString& WadoRetrieveRequest::getStudyInstanceUid() const
{
    return m_studyInstanceUid;
}

const QString& WadoRetrieveRequest::getSeriesInstanceUid() const
{
    return m_seriesInstanceUid;
}

const QString& WadoRetrieveRequest::getSopInstanceUid() const
{
    return m_sopInstanceUid;
}

// This is run in the WADO thread.
void WadoRetrieveRequest::startInternal()
{
    if (!ensureEnoughHardDiskSpace())
    {
        return;
    }

    QString urlString = getPacsDevice().getBaseUri().toString(QUrl::StripTrailingSlash);
    urlString.append("/studies/").append(m_studyInstanceUid);

    if (!m_seriesInstanceUid.isEmpty())
    {
        urlString.append("/series/").append(m_seriesInstanceUid);

        if (!m_sopInstanceUid.isEmpty())
        {
            urlString.append("/instances/").append(m_sopInstanceUid);
        }
    }

    INFO_LOG(QString("Starting retrieve from WADO PACS with URI %1.").arg(urlString));

    QUrl url(urlString);
    QNetworkRequest request(url);
    m_reply = m_networkAccessManager->get(request);

    emit started();

    connect(m_reply, &QNetworkReply::readyRead, this, &WadoRetrieveRequest::onReadyRead, Qt::DirectConnection);
    connect(m_reply, &QNetworkReply::finished, this, &WadoRetrieveRequest::onReplyFinished, Qt::DirectConnection);
    connect(m_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &WadoRetrieveRequest::onReplyError,
            Qt::DirectConnection);
}

bool WadoRetrieveRequest::ensureEnoughHardDiskSpace()
{
    // Copied and adapted from RetrieveDICOMFilesFromPACSJob::thereIsAvailableSpaceOnHardDisk
    LocalDatabaseManager localDatabaseManager;
    connect(&localDatabaseManager, &LocalDatabaseManager::studyWillBeDeleted, this, &WadoRetrieveRequest::studyFromCacheWillBeDeleted);

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

void WadoRetrieveRequest::initializeReading()
{
    QVariant contentTypeVariant = m_reply->header(QNetworkRequest::ContentTypeHeader);

    if (!contentTypeVariant.isValid())
    {
        WARN_LOG("No content type.");
        return;
    }

    QString contentType = contentTypeVariant.toString();

    if (!contentType.startsWith("multipart/related"))
    {
        WARN_LOG(QString("Unsupported content type: %1. Only multipart/related is supported.").arg(contentType));
        return;
    }

    QStringList contentTypeParts = contentType.split("; ");

    if (contentTypeParts.size() != 3)
    {
        WARN_LOG(QString("Unexpected content type format: %1.").arg(contentType));
        return;
    }

    if (contentTypeParts[1] != "type=\"application/dicom\"")
    {
        WARN_LOG(QString("Unsupported type: %1. Must be \"application/dicom\".").arg(contentTypeParts[1]));
        return;
    }

    if (!contentTypeParts[2].startsWith("boundary="))
    {
        WARN_LOG(QString("Boundary not found where expected: %1.").arg(contentTypeParts[2]));
        return;
    }

    m_boundary = contentTypeParts[2].remove(0, 9);

    if (m_boundary.isEmpty())
    {
        WARN_LOG("Empty boundary.");
    }
}

bool WadoRetrieveRequest::readPartBoundary()
{
    if (m_reply->canReadLine())
    {
        QString line = m_reply->readLine();
        Q_ASSERT(line.endsWith("\r\n"));
        line.chop(2);

        if (line == "--" + m_boundary)
        {
            m_readingState = ReadingState::ReadingPartHeader;
            m_partContentTypeOk = false;
            return true;
        }
        else if (line == "--" + m_boundary + "--")
        {
            m_readingState = ReadingState::Finished;
            return true;
        }
        else
        {
            error();
            ERROR_LOG(QString("Expected boundary but found another thing instead: \"%1\".").arg(line));
        }
    }

    return false;
}

bool WadoRetrieveRequest::readPartHeader()
{
    static const QString ContentType("Content-Type: application/dicom");
    static const QString ContentLength("Content-Length: ");

    while (m_reply->canReadLine())
    {
        QString line = m_reply->readLine();
        Q_ASSERT(line.endsWith("\r\n"));
        line.chop(2);

        if (line == ContentType)
        {
            m_partContentTypeOk = true;
        }
        else if (line.startsWith(ContentLength))
        {
            qint64 length = line.remove(0, ContentLength.size()).toLongLong();
            m_wadoInstanceDownloader.reset(new WadoInstanceDownloader());
            m_wadoInstanceDownloader->setStudyInstanceUid(m_studyInstanceUid);
            m_wadoInstanceDownloader->setSeriesInstanceUid(m_seriesInstanceUid);
            m_wadoInstanceDownloader->setSopInstanceUid(m_sopInstanceUid);
            m_wadoInstanceDownloader->setInstanceSize(length);
            m_wadoInstanceDownloader->setDataSource(m_reply);
        }
        else if (line.isEmpty())
        {
            if (m_partContentTypeOk && m_wadoInstanceDownloader)
            {
                m_readingState = ReadingState::ReadingPartBody;
                return true;
            }
            else
            {
                error();
                ERROR_LOG(QString("Error when reading part header. Found expected content type? %1. Found content length? %2.")
                          .arg(m_partContentTypeOk).arg(!!m_wadoInstanceDownloader));
                m_wadoInstanceDownloader.reset();
            }

            break;
        }
    }

    return false;
}

bool WadoRetrieveRequest::readPartBody()
{
    if (!m_wadoInstanceDownloader->hasFinished())
    {
        try
        {
            m_wadoInstanceDownloader->read();
        }
        catch (const std::runtime_error &error)
        {
            this->error();
            m_errorsDescription = error.what();
            ERROR_LOG(m_errorsDescription);
            return false;
        }
    }

    if (m_wadoInstanceDownloader->hasFinished())
    {
        if (m_reply->canReadLine())
        {
            m_reply->readLine();
            m_readingState = ReadingState::Empty;

            emit instanceDownloaded(++m_numberOfInstancesDownloaded);

            if (!m_downloadedSeriesUids.contains(m_wadoInstanceDownloader->getSeriesInstanceUid()))
            {
                m_downloadedSeriesUids.insert(m_wadoInstanceDownloader->getSeriesInstanceUid());
                emit seriesDownloaded(m_downloadedSeriesUids.size());
            }

            m_downloadedFilesProcessor->processFile(m_wadoInstanceDownloader->getPath());

            m_wadoInstanceDownloader.reset();

            return true;
        }
    }

    return false;
}

void WadoRetrieveRequest::error()
{
    m_reply->abort();
    m_status = Status::Errored;
    m_errorsDescription = tr("Invalid or unsupported server response.");
    disconnect(m_reply, &QNetworkReply::readyRead, this, &WadoRetrieveRequest::onReadyRead);
}

// This is run in the WADO thread.
void WadoRetrieveRequest::onReadyRead()
{
    if (m_boundary.isEmpty() && m_status == Status::Clean)
    {
        initializeReading();

        if (m_boundary.isEmpty())
        {
            error();
            return;
        }

        m_readingState = ReadingState::Empty;
        m_downloadedFilesProcessor.reset(new DownloadedFilesProcessor(getPacsDevice()));
        m_downloadedFilesProcessor->beginDownloadStudy(m_studyInstanceUid);
    }

    bool continueReading = true;

    while (continueReading && (m_status == Status::Clean || m_status == Status::Warnings))
    {
        if (m_readingState == ReadingState::Empty)
        {
            continueReading = readPartBoundary();
        }
        else if (m_readingState == ReadingState::ReadingPartHeader)
        {
            continueReading = readPartHeader();
        }
        else if (m_readingState == ReadingState::ReadingPartBody)
        {
            continueReading = readPartBody();
        }
        else if (m_readingState == ReadingState::Finished)
        {
            continueReading = false;
        }
    }
}

// This is run in the WADO thread.
void WadoRetrieveRequest::onReplyFinished()
{
    bool errors = false;

    if (m_reply->error() == QNetworkReply::OperationCanceledError)
    {
        if (m_status == Status::Clean)  // external (user or program closed) cancellation
        {
            INFO_LOG(QString("Cancelled retrieve of study %1 (series %2, instance %3).")
                     .arg(m_studyInstanceUid).arg(m_seriesInstanceUid).arg(m_sopInstanceUid));

            if (m_downloadedFilesProcessor)
            {
                m_downloadedFilesProcessor->cancelDownloadStudy();
            }

            m_status = Status::Cancelled;
            emit cancelled();

            return;
        }
        else    // internal cancellation due to error
        {
            errors = true;
        }
    }
    else if (m_reply->error() != QNetworkReply::NoError)
    {
        errors = true;
        m_errorsDescription = m_reply->errorString();
        WARN_LOG(m_errorsDescription);
    }
    else
    {
        if (m_readingState != ReadingState::Finished)
        {
            errors = true;
            m_errorsDescription = tr("Download ended prematurely for an unknown reason.");
            WARN_LOG(m_errorsDescription);
        }
    }

    if (m_numberOfInstancesDownloaded > 0)
    {
        INFO_LOG(QString("Finished retrieve of study %1 (series %2, instance %3). %4 instance(s) downloaded.")
                 .arg(m_studyInstanceUid).arg(m_seriesInstanceUid).arg(m_sopInstanceUid).arg(m_numberOfInstancesDownloaded));
        LocalDatabaseManager::LastError error = m_downloadedFilesProcessor->finishDownloadStudy();

        if (error == LocalDatabaseManager::Ok)
        {
            if (errors)
            {
                m_status = Status::Warnings;
            }
        }
        else
        {
            m_status = Status::Errored;

            if (error == LocalDatabaseManager::PatientInconsistent)
            {
                m_errorsDescription = tr("Can't read correctly data from downloaded images. They may be corrupt.");
            }
            else
            {
                m_errorsDescription = tr("Database error when saving downloaded images.");
            }

            ERROR_LOG(m_errorsDescription);
        }
    }
    else
    {
        ERROR_LOG(QString("Finished retrieve of study %1 (series %2, instance %3) without downloading anything.")
                  .arg(m_studyInstanceUid).arg(m_seriesInstanceUid).arg(m_sopInstanceUid));
        m_status = Status::Errored;

        if (m_downloadedFilesProcessor)
        {
            m_downloadedFilesProcessor->cancelDownloadStudy();
        }
    }

    emit finished();
}

void WadoRetrieveRequest::onReplyError()
{
    WARN_LOG(QString("Network error: %1. %2.").arg(m_reply->error()).arg(m_reply->errorString()));
}

} // namespace udg
