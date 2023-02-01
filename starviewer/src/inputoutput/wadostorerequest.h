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

#ifndef UDG_WADOSTOREREQUEST_H
#define UDG_WADOSTOREREQUEST_H

#include "wadorequest.h"

namespace udg {

class Series;

/**
 * @brief The WadoStoreRequest class implements the WADO Store transaction (PS3.18§10.5).
 */
class WadoStoreRequest : public WadoRequest
{
    Q_OBJECT

public:
    /// Initializes the object and gathers the lists of files to send.
    explicit WadoStoreRequest(const PacsDevice &pacs, const QList<Series*> &seriesList, QObject *parent = nullptr);

    /// Returns the list of series to store.
    const QList<Series*>& getSeriesList() const;

signals:
    /// Emitted each time an instance is sent, with the total number of instances sent until now.
    void instanceSent(int totalInstancesSent);
    /// Emitted each time a new series is sent, with the total number of different series sent until now.
    void seriesSent(int totalSeriesSent);

private:
    /// Starts the store transaction, sending the first batch of files.
    void startInternal() override;
    /// Sends the next batch of files to the PACS. Files are sent in batches because all files to be sent must be open at the same time; thus all the files in a
    /// batch are open, a request is started, and when finished the request is deleted and the files closed, and a new batch of files is sent. This way the
    /// probability of hitting the limit of open files is reduced.
    void sendNextBatchOfFiles();

private slots:
    /// Called when the current request finishes for any reason, including cancellation. If it has finished without errors and there are still files pending to
    /// be sent, it will send the next batch. Otherwise, it sets the appropriate status and errors description and emits the appropriate signal.
    void onReplyFinished();
    /// Called to track upload progress. It will emit instanceSent() and seriesSent() at certain points.
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    /// List of series to send.
    QList<Series*> m_seriesList;

    /// Files that will be sent. One list of files for each series.
    QList<QList<QString>> m_filesToSend;
    /// Total number of instances (files) that will be sent.
    int m_totalInstancesToSend;
    /// Total number of series that will be sent.
    int m_totalSeriesToSend;

    /// Used when filling m_seriesBoundaries to know when the currently handled series has already been counted, i.e. its boundary added to m_seriesBoundaries.
    bool m_currentSeriesCounted;
    /// Contains the points (byte quantities) at which each instance can be considered sent and the corresponding signal emitted.
    QList<qint64> m_instanceBoundaries;
    /// Contains the points (byte quantities) at which the first instance of each series can be considered sent and the corresponding signal emitted.
    QList<qint64> m_seriesBoundaries;

    /// Total number of instances (files) already sent.
    int m_totalInstancesSent;
    /// Total number of different series already sent.
    int m_totalSeriesSent;
};

} // namespace udg

#endif // UDG_WADOSTOREREQUEST_H
