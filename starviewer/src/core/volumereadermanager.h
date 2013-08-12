#ifndef UDG_VOLUMEREADERMANAGER_H
#define UDG_VOLUMEREADERMANAGER_H

#include <QObject>
#include <QPointer>

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

    /// Cancels the reading
    void cancelReading();

    /// Returns true if a volume is being readed
    bool isReading();

    /// Returns true if the reading ended successfully
    bool readingSuccess();

    /// Returns the volume readed
    Volume *getVolume();

    /// Returns the last error messege. An empty string is retured if no error.
    QString getLastErrorMessageToUser();

signals:
    /// Signal emitted during the reading to report progress
    void progress(int progress);
    /// Signal emitted at the end of the reading
    void readingFinished();

private:
    /// Job to read the volume. It doesn't belong to us, that is why QPointer is used.
    QPointer<VolumeReaderJob> m_volumeReaderJob;
};

} // namespace udg

#endif // UDG_VOLUMEREADERMANAGER_H
