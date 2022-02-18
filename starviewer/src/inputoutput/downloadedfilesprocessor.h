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

#ifndef UDG_DOWNLOADEDFILESPROCESSOR_H
#define UDG_DOWNLOADEDFILESPROCESSOR_H

#include "localdatabasemanager.h"

#include <QThread>

namespace udg {

class DICOMTagReader;
class PacsDevice;
class PatientFiller;

/**
 * @brief The DownloadedFilesProcessor class does all the processing needed with each file of a study that is downloaded. It runs each file through the
 *        PatientFiller and saves the study in the database at the end. Both things are done in a separate thread.
 */
class DownloadedFilesProcessor : public QObject
{
    Q_OBJECT

public:
    /// Creates the worker thread and the necessary connections.
    explicit DownloadedFilesProcessor(const PacsDevice &pacsDevice, QObject *parent = nullptr);
    ~DownloadedFilesProcessor() override;

    /// Notifies to this that the study with the given Study Instance UID is being downloaded.
    void beginDownloadStudy(const QString &studyInstanceUid);

public slots:
    /// Processes the DICOM file in the given path, running it through the PatientFiller.
    void processFile(const QString &path);
    /// Processes the DICOM file open on the given DICOMTagReader, running it through the PatientFiller.
    void processFile(const DICOMTagReader *dicomTagReader);
    /// Notifies to this that the downloaded study has finished downloading. The PatientFiller is told to finish processing and the study is saved (inserted or
    /// updated) in the database. The study is marked as not downloading. The last error of the database operation is returned.
    LocalDatabaseManager::LastError finishDownloadStudy();
    /// Stops all processing, marks the downloading study as not downloading and removes its files if it's not in the database.
    void cancelDownloadStudy();

signals:
    /// For internal use to execute a slot of the PatientFiller in another thread.
    void processFileCalled(const DICOMTagReader *dicomTagReader);
    /// For internal use to execute a slot of the PatientFiller in another thread.
    void finishDownloadStudyCalled();

private:
    /// Deletes the cache directory corresponding to the downloaded study if the study is not found in the database.
    void deletePartiallyDownloadedStudy();

private:
    PatientFiller *m_patientFiller;
    LocalDatabaseManager m_localDatabaseManager;
    /// The worker thread where the PatientFiller and the database operations are run.
    QThread m_thread;
    /// Study Instance UID of the study being downloaded.
    QString m_studyInstanceUid;
};

} // namespace udg

#endif // UDG_DOWNLOADEDFILESPROCESSOR_H
