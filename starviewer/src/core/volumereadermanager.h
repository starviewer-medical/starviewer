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

#ifndef UDG_VOLUMEREADERMANAGER_H
#define UDG_VOLUMEREADERMANAGER_H

#include <QObject>
#include <QPointer>
#include <QHash>

#include "volumereaderjob.h"

namespace udg {

class Volume;
class VolumeReaderJob;

/// Class that controls the asyncronous reading of volumes
class VolumeReaderManager : public QObject
{
    Q_OBJECT
public:
    VolumeReaderManager(QObject *parent = 0);
    ~VolumeReaderManager();

    /// Starts the reading of a volume
    void readVolume(Volume *volume);

    ///Starts the reading of n volumes
    void readVolumes(const QList<Volume *> &volumes);

    /// Cancels the reading
    void cancelReading();

    /// Returns true if a volume is being readed
    bool isReading();

    /// Returns true if the reading ended successfully
    bool readingSuccess();

    /// Returns the volume readed
    Volume *getVolume();
    QList<Volume *> getVolumes();

    /// Returns the last error messege. An empty string is retured if no error.
    QString getLastErrorMessageToUser();

signals:
    /// Signal emitted during the reading to report progress
    void progress(int progress);
    /// Signal emitted at the end of the reading
    void readingFinished();

private slots:
    /// Updates the progress of the job and emits the global progress
    void updateProgress(VolumeReaderJob*, int);
    /// Slot executed when a job finished. It emits the signal readingFinished() if no jobs are reading.
    void jobFinished(ThreadWeaver::JobPointer job);

private:
    /// Initialize internal helpers
    void initialize();

private:
    /// List of jobs to read volumes.
    QList<QWeakPointer<ThreadWeaver::JobInterface> > m_volumeReaderJobs;

    /// List to control the progress of all jobs
    QHash<VolumeReaderJob*, int> m_jobsProgress;

    /// List of readed volumes
    QList<Volume*> m_volumes;

    /// It says if the reading ended successfully
    int m_success;

    /// It contains the reading error message if any
    QString m_lastError;

    /// It counts the number of finished jobs
    int m_numberOfFinishedJobs;
};

} // namespace udg

#endif // UDG_VOLUMEREADERMANAGER_H
