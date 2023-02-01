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

#ifndef UDG_WADORETRIEVEREQUEST_H
#define UDG_WADORETRIEVEREQUEST_H

#include "wadorequest.h"

#include "downloadedfilesprocessor.h"
#include "wadoinstancedownloader.h"

#include <memory>

#include <QSet>

namespace udg {

/**
 * @brief The WadoRetrieveRequest class implements the WADO Retrieve transaction (PS3.18§10.4).
 */
class WadoRetrieveRequest : public WadoRequest
{
    Q_OBJECT

public:
    WadoRetrieveRequest(const PacsDevice &pacs, const QString &studyInstanceUid, const QString &seriesInstanceUid, const QString &sopInstanceUid,
                        QObject *parent = nullptr);

    /// Returns the Study Instance UID of the requested study, series or instance.
    const QString& getStudyInstanceUid() const;
    /// Returns the Series Instance UID of the requested series or instance.
    const QString& getSeriesInstanceUid() const;
    /// Returns the SOP Instance UID of the requested instance.
    const QString& getSopInstanceUid() const;

signals:
    /// Emitted when a local study is deleted to free up space. The end goal is to update the local database view.
    void studyFromCacheWillBeDeleted(const QString &studyInstanceUid);
    /// Emitted each time an instance is downloaded, with the total number of instances downloaded until now.
    void instanceDownloaded(int totalInstancesDownloaded);
    /// Emitted each time a new series is downloaded, with the total number of different series downloaded until now.
    void seriesDownloaded(int totalSeriesDownloaded);

private:
    /// Starts the retrieve transaction.
    void startInternal() override;

    /// Checks available space in the local cache and if needed deletes studies until there is enough.
    /// Returns true if at the end is enough space and false otherwise.
    bool ensureEnoughHardDiskSpace();

    /// Reads the response headers and checks that they have the expected values. Reads the boundary that will be used to separate parts.
    void initializeReading();
    /// Reads the boundary before the next part or the last boundary, including the CRLF. After this, the part "header" is to be read, or the data is finished.
    /// Returns true if it has successfully finished its task and false otherwise.
    bool readPartBoundary();
    /// Reads the part "header", including the empty line at the end and the CRLF. Checks that the Content-Type is as expected and parses the Content-Length.
    /// After this, the part "body" is to be read, with the exact size in bytes specified in the Content-Length and followed by a CRLF. Returns true if it has
    /// successfully finished its task and false otherwise.
    bool readPartHeader();
    /// Reads the part "body", including the CRLF after it. The number of bytes specified in the Content-Length is read and saved to a file on disk using a
    /// WadoInstanceDownloader and later processed with a DownloadedFilesProcessor to insert or update the study in the database. After this, the next boundary
    /// is to be read. Returns true if it has successfully finished its task and false otherwise.
    bool readPartBody();

    /// Used to stop everything when a major error is found. Aborts the transfer, sets the status as Errored and a generic error description.
    void error();

private slots:
    /// Called when there is new data available in the response. Uses the read* methods above to read all the data that can be read each time.
    void onReadyRead();
    /// Called when the download finishes for any reason, including cancellation. If the download has finished successfully, finishes processing with
    /// DownloadedFilesProcessor. In any case it sets the appropriate status and errors description and emits the appropriate signal.
    void onReplyFinished();

private:
    /// Study Instance UID of the requested study, series or instance.
    QString m_studyInstanceUid;
    /// Series Instance UID of the requested series or instance.
    QString m_seriesInstanceUid;
    /// SOP Instance UID of the requested instance.
    QString m_sopInstanceUid;

    /// Boundary used to separate parts in the multipart/related response.
    QString m_boundary;

    /// Used to track which of the read* methods has to be called next.
    enum class ReadingState { Empty, ReadingPartHeader, ReadingPartBody, Finished };
    ReadingState m_readingState;

    /// True if the Content-Type of the part being read is as expected, and false otherwise.
    bool m_partContentTypeOk;
    /// Helper object to save files to their expected location. A new instance of this class is created for each downloaded DICOM instance.
    std::unique_ptr<WadoInstanceDownloader> m_wadoInstanceDownloader;
    /// Helper object to process files after they are downloaded.
    std::unique_ptr<DownloadedFilesProcessor> m_downloadedFilesProcessor;

    /// Counter of the number of instances downloaded until now.
    uint m_numberOfInstancesDownloaded;
    /// Set with the distinct Series Instance UIDs found until now. Used to know the number of different series received.
    QSet<QString> m_downloadedSeriesUids;
};

} // namespace udg

#endif // UDG_WADORETRIEVEREQUEST_H
