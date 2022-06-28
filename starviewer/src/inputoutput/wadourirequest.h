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

#ifndef UDG_WADOURIREQUEST_H
#define UDG_WADOURIREQUEST_H

#include "wadorequest.h"

#include "downloadedfilesprocessor.h"

#include <QSet>

namespace udg {

/**
 * @brief The WadoUriRequest class implements a WADO-URI download (PS3.18§9) of a study, series or instance.
 *
 * In case of studies and series, several requests are made.
 */
class WadoUriRequest : public WadoRequest
{
    Q_OBJECT

public:
    explicit WadoUriRequest(const PacsDevice &pacs, const QString &studyInstanceUid, const QString &seriesInstanceUid, const QString &sopInstanceUid,
                            QObject *parent = nullptr);

    /// Returns the Study Instance UID of the requested study, series or instance.
    const QString& getStudyInstanceUid() const;
    /// Returns the Series Instance UID of the requested series or instance.
    const QString& getSeriesInstanceUid() const;
    /// Returns the SOP Instance UID of the requested instance.
    const QString& getSopInstanceUid() const;

    /// Cancels the request.
    void cancel() override;

signals:
    /// Emitted when a local study is deleted to free up space. The end goal is to update the local database view.
    void studyFromCacheWillBeDeleted(const QString &studyInstanceUid);
    /// Emitted each time an instance is downloaded, with the total number of instances downloaded until now.
    void instanceDownloaded(int totalInstancesDownloaded);
    /// Emitted each time a new series is downloaded, with the total number of different series downloaded until now.
    void seriesDownloaded(int totalSeriesDownloaded);

private:
    /// Starts the download.
    void startInternal() override;

    /// Checks available space in the local cache and if needed deletes studies until there is enough.
    /// Returns true if at the end is enough space and false otherwise.
    bool ensureEnoughHardDiskSpace();

    /// Performs a query on the PACS to obtain the UIDs of all the instances in the requested study or series.
    bool obtainSopInstanceUids();

private slots:
    /// Called when a QNetworkReply finishes.
    void onReplyFinished(QNetworkReply *reply);

    /// Called when a QNetworkReply has an error.
    void onReplyError(QNetworkReply *reply);

private:
    /// Study Instance UID of the requested study, series or instance.
    QString m_studyInstanceUid;
    /// Series Instance UID of the requested series or instance.
    QString m_seriesInstanceUid;
    /// SOP Instance UID of the requested instance.
    QString m_sopInstanceUid;

    /// Set of pairs of SeriesInstanceUID and SOPInstanceUID of each instance that is to be downloaded.
    QSet<QPair<QString, QString>> m_instancesToDownload;

    /// Contains all the replies returned by the QNetworkAccessManager that have not yet been dealt with.
    QSet<QNetworkReply*> m_pendingReplies;

    /// Helper object to process files after they are downloaded.
    std::unique_ptr<DownloadedFilesProcessor> m_downloadedFilesProcessor;

    /// Counter of the number of instances downloaded until now.
    uint m_numberOfInstancesDownloaded;
    /// Set with the distinct Series Instance UIDs found until now. Used to know the number of different series received.
    QSet<QString> m_downloadedSeriesUids;
};

} // namespace udg

#endif // UDG_WADOURIREQUEST_H
