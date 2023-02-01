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

#ifndef UDG_WADOINSTANCEDOWNLOADER_H
#define UDG_WADOINSTANCEDOWNLOADER_H

#include <QFile>

namespace udg {

/**
 * @brief The WadoInstanceDownloader class reads the data corresponding to a single instance from a multipart/related WADO response and saves it to an
 *        appropriate file following Starviewer cache structure. The read data is the "body" of an individual part in a multipart/related response.
 */
class WadoInstanceDownloader
{
public:
    WadoInstanceDownloader();

    /// Sets the Study Instance UID of the study being downloaded.
    void setStudyInstanceUid(const QString &studyInstanceUid);
    /// Returns the Study Instance UID of the downloaded study.
    const QString& getStudyInstanceUid() const;

    /// Sets the Series Instance UID of the series being downloaded.
    void setSeriesInstanceUid(const QString &seriesInstanceUid);
    /// Returns the Series Instance UID of the downloaded study. If it was not specified before download, it is filled after download.
    const QString& getSeriesInstanceUid() const;

    /// Sets the SOP Instance UID of the instance being downloaded.
    void setSopInstanceUid(const QString &sopInstanceUid);
    /// Returns the SOP Instance UID of the downloaded study. If it was not specified before download, it is filled after download.
    const QString& getSopInstanceUid() const;

    /// Sets the instance size in bytes.
    void setInstanceSize(qint64 bytes);

    /// Sets the data source, i.e. the WADO response, where the data will be read from.
    void setDataSource(QIODevice *dataSource);

    /// Reads a chunk of data from the source and writes it to the file. On the first read creates the file.
    void read();

    /// Returns true if this has finished downloading the file and false otherwise.
    bool hasFinished() const;

    /// Returns the path of the file where the data has been saved.
    QString getPath() const;

private:
    /// Creates the file where data will we written. If the SOP Instance UID is unknown it creates a file with a random name that will be moved to its final
    /// destination when finished.
    void createFile();
    /// Moves the file saved with a random name to its final destination.
    void moveFileToFinalDestination();

private:
    /// Study Instance UID of the study being downloaded.
    QString m_studyInstanceUid;
    /// Series Instance UID of the series being downloaded, if known. If not known, it's filled after download.
    QString m_seriesInstanceUid;
    /// SOP Instance UID of the instance being downloaded, if known. If not known, it's filled after download.
    QString m_sopInstanceUid;

    /// Instance size in bytes as specified in the Content-Length of the part.
    qint64 m_instanceSize;
    /// Total number of bytes read until the moment.
    qint64 m_totalBytesRead;

    /// Data source where data will be read from.
    QIODevice *m_dataSource;

    /// File where the data is written.
    QFile m_file;

    /// Will be true when the download is finished.
    bool m_finished;
};

} // namespace udg

#endif // UDG_WADOINSTANCEDOWNLOADER_H
