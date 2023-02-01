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

#include "wadostorerequest.h"

#include "encapsulateddocument.h"
#include "image.h"
#include "logging.h"
#include "series.h"
#include "study.h"

#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUuid>

namespace udg {

WadoStoreRequest::WadoStoreRequest(const PacsDevice &pacs, const QList<Series*> &seriesList, QObject *parent)
    : WadoRequest(pacs, parent), m_seriesList(seriesList), m_totalInstancesToSend(0), m_totalSeriesToSend(0), m_currentSeriesCounted(false),
      m_totalInstancesSent(0), m_totalSeriesSent(0)
{
    for (Series *series : m_seriesList)
    {
        QSet<QString> set;
        const QList<Image*> &images = series->getImages();

        for (Image *image : images)
        {
            set.insert(image->getPath());
        }

        const QList<EncapsulatedDocument*> &encapsulatedDocuments = series->getEncapsulatedDocuments();

        for (EncapsulatedDocument *document : encapsulatedDocuments)
        {
            set.insert(document->getPath());
        }

        if (!set.isEmpty())
        {
            m_filesToSend.append(set.toList());
            m_totalInstancesToSend += set.size();
            m_totalSeriesToSend++;
        }
    }
}

const QList<Series*>& WadoStoreRequest::getSeriesList() const
{
    return m_seriesList;
}

// This is run in the WADO thread.
void WadoStoreRequest::startInternal()
{
    INFO_LOG(QString("Starting store of study %1 to WADO PACS with base URI %2.").arg(m_seriesList.first()->getParentStudy()->getInstanceUID())
             .arg(getPacsDevice().getBaseUri().toString()));
    sendNextBatchOfFiles();
}

// This is run in the WADO thread.
void WadoStoreRequest::sendNextBatchOfFiles()
{
    QString boundary(QUuid::createUuid().toString(QUuid::WithoutBraces));

    auto *multipart = new QHttpMultiPart(QHttpMultiPart::RelatedType);
    multipart->setBoundary(boundary.toLatin1());

    // Overhead independent of content: 59 bytes
    // 2 (-- before boundary) + 31 (content type header) + 16 (content length header without the number) + 2*5 (5 CRLFs total) = 59
    const qint64 ConstantOverhead = 59 + boundary.size();
    qint64 bytesToSend = 0;

    int batchSize = 0;
    constexpr int MaxBatchSize = 150;

    m_instanceBoundaries.clear();
    m_seriesBoundaries.clear();

    while (batchSize < MaxBatchSize && !m_filesToSend.isEmpty())
    {
        QList<QString> &seriesFiles = m_filesToSend.first();

        while (batchSize < MaxBatchSize && !seriesFiles.isEmpty())
        {
            const QString &path = seriesFiles.takeFirst();
            auto *file = new QFile(path, multipart);    // file will be deleted with multipart

            if (file->open(QIODevice::ReadOnly))
            {
                QHttpPart part;
                part.setHeader(QNetworkRequest::ContentTypeHeader, "application/dicom");
                part.setHeader(QNetworkRequest::ContentLengthHeader, file->size());
                part.setBodyDevice(file);
                multipart->append(part);

                bytesToSend += ConstantOverhead + QString::number(file->size()).size() + file->size();
                m_instanceBoundaries.append(bytesToSend);

                if (!m_currentSeriesCounted)
                {
                    m_seriesBoundaries.append(bytesToSend);
                    m_currentSeriesCounted = true;
                }

                batchSize++;
            }
            else
            {
                m_status = Status::Warnings;
                m_errorsDescription += file->errorString();
                WARN_LOG(file->errorString());
                delete file;
            }
        }

        if (seriesFiles.isEmpty())
        {
            m_filesToSend.removeFirst();
            m_currentSeriesCounted = false;
        }
    }

    if (bytesToSend > 0)
    {
        QString urlString = getPacsDevice().getBaseUri().toString(QUrl::StripTrailingSlash);
        urlString += "/studies/" + m_seriesList.first()->getParentStudy()->getInstanceUID();

        QNetworkRequest request(urlString);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/related; type=\"application/dicom\"; boundary=" + boundary);
        m_reply = m_networkAccessManager->post(request, multipart);
        multipart->setParent(m_reply);  // multipart will be deleted with m_reply

        emit started();

        connect(m_reply, &QNetworkReply::finished, this, &WadoStoreRequest::onReplyFinished, Qt::DirectConnection);
        connect(m_reply, &QNetworkReply::uploadProgress, this, &WadoStoreRequest::onUploadProgress, Qt::DirectConnection);
    }
    else
    {
        m_status = Status::Errored;
        m_errorsDescription = tr("Could not open any file to send.");
        emit finished();
        delete multipart;
    }
}

// This is run in the WADO thread.
void WadoStoreRequest::onReplyFinished()
{
    if (m_reply->error() == QNetworkReply::OperationCanceledError)
    {
        INFO_LOG(QString("Cancelled store of study %1 to WADO PACS with base URI %2.").arg(m_seriesList.first()->getParentStudy()->getInstanceUID())
                 .arg(getPacsDevice().getBaseUri().toString()));
        m_status = Status::Cancelled;
        emit cancelled();
    }
    else if (m_reply->error() != QNetworkReply::NoError)
    {
        ERROR_LOG(QString("Error during store of study %1 to WADO PACS with base URI %2.").arg(m_seriesList.first()->getParentStudy()->getInstanceUID())
                 .arg(getPacsDevice().getBaseUri().toString()));
        m_status = Status::Errored;
        m_errorsDescription = m_reply->errorString();
        ERROR_LOG(m_errorsDescription);

        QVariant httpStatusCodeVariant = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

        if (httpStatusCodeVariant.isValid())
        {
            ERROR_LOG(QString("HTTP error %1: %2.").arg(httpStatusCodeVariant.toInt())
                      .arg(m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()));
            ERROR_LOG(m_reply->readAll());
        }

        emit finished();
    }
    else
    {
        if (m_filesToSend.isEmpty())    // all files have already been sent
        {
            INFO_LOG(QString("Finished store of study %1 to WADO PACS with base URI %2.").arg(m_seriesList.first()->getParentStudy()->getInstanceUID())
                     .arg(getPacsDevice().getBaseUri().toString()));

            if (m_totalInstancesSent < m_totalInstancesToSend)
            {
                emit instanceSent(m_totalInstancesToSend);
            }

            if (m_totalSeriesSent < m_totalSeriesToSend)
            {
                emit seriesSent(m_totalSeriesToSend);
            }

            int httpStatusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            if (httpStatusCode == 202)
            {
                m_status = Status::Warnings;
                m_errorsDescription += m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                WARN_LOG(QString("HTTP status %1: %2.").arg(httpStatusCode).arg(m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()));
                WARN_LOG(m_reply->readAll());
            }

            emit finished();
        }
        else    // this batch of files has finished but there are still more to send
        {
            int httpStatusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            if (httpStatusCode == 202)
            {
                m_status = Status::Warnings;
                m_errorsDescription += m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                WARN_LOG(QString("HTTP status %1: %2.").arg(httpStatusCode).arg(m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()));
                WARN_LOG(m_reply->readAll());
            }

            m_reply->deleteLater();
            sendNextBatchOfFiles();
        }
    }

    /*
     * Response status codes according to DICOM PS3.18§10.5.3.1:
     *
     * Success:
     *
     * 200 (OK)
     * The origin server successfully stored all Instances.
     *
     * 202 (Accepted)
     * The origin server stored some of the Instances but warnings or failures exist for others. Additional information regarding this error may be found in the
     * response message body.
     *
     * Failure:
     *
     * 400 (Bad Request)
     * The origin server was unable to store any instances due to bad syntax.
     *
     * 409 (Conflict)
     * The request was formed correctly but the origin server was unable to store any instances due to a conflict in the request (e.g., unsupported SOP Class or
     * Study Instance UID mismatch). This may also be used to indicate that the origin server was unable to store any instances for a mixture of reasons.
     * Additional information regarding the instance errors may be found in the payload.
     *
     * 415 (Unsupported Media Type)
     * The origin server does not support the media type specified in the Content-Type header field of the request
     */
}

void WadoStoreRequest::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal)

    if (!m_instanceBoundaries.isEmpty() && bytesSent >= m_instanceBoundaries.first())
    {
        while (!m_instanceBoundaries.isEmpty() && bytesSent >= m_instanceBoundaries.first())
        {
            m_instanceBoundaries.removeFirst();
            ++m_totalInstancesSent;
        }

        emit instanceSent(m_totalInstancesSent);
    }

    if (!m_seriesBoundaries.isEmpty() && bytesSent >= m_seriesBoundaries.first())
    {
        while (!m_seriesBoundaries.isEmpty() && bytesSent >= m_seriesBoundaries.first())
        {
            m_seriesBoundaries.removeFirst();
            ++m_totalSeriesSent;
        }

        emit seriesSent(m_totalSeriesSent);
    }
}

} // namespace udg
