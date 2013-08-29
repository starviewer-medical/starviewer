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
    void updateProgress(ThreadWeaver::Job*, int);
    /// Slot executed when a job finished. It emits the signal readingFinished() if no jobs are reading.
    void jobFinished();

private:
    /// Initialize internal helpers
    void initialize();

private:
    /// List of jobs to read volumes. They don't belong to us, that is why QPointer is used.
    QList<QPointer<VolumeReaderJob> > m_volumeReaderJobs;

    /// List to control the progress of all jobs
    QHash<ThreadWeaver::Job*, int> m_jobsProgress;
};

} // namespace udg

#endif // UDG_VOLUMEREADERMANAGER_H
