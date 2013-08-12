#include "volumereadermanager.h"

#include "asynchronousvolumereader.h"
#include "volumereaderjob.h"
#include "volume.h"

namespace udg {

VolumeReaderManager::VolumeReaderManager(QObject *parent) :
    QObject(parent)
{
}

void VolumeReaderManager::readVolume(Volume *volume)
{
    QList<Volume*> volumes;
    volumes << volume;

    readVolumes(volumes);
}

void VolumeReaderManager::readVolumes(const QList<Volume*> &volumes)
{
    m_volumeReaderJobs.clear();
    m_jobsProgress.clear();

    foreach (Volume *volume, volumes)
    {
        // TODO Esborrar volumeReader!!
        AsynchronousVolumeReader *volumeReader = new AsynchronousVolumeReader();
        m_volumeReaderJobs << volumeReader->read(volume);
        m_jobsProgress.insert(m_volumeReaderJobs.last(), 0);
        connect(m_volumeReaderJobs.last(), SIGNAL(done(ThreadWeaver::Job*)), SLOT(jobFinished()));
        connect(m_volumeReaderJobs.last(), SIGNAL(progress(ThreadWeaver::Job*, int)), SLOT(updateProgress(ThreadWeaver::Job*, int)));
    }
}

void VolumeReaderManager::cancelReading()
{
    // TODO: Aquí s'hauria de cancel·lar realment el current job. De moment no podem fer-ho i simplement el desconnectem
    // Quan es faci bé, tenir en compte què passa si algun altre visor el vol continuar descarregant igualment i nosaltres aquí el cancelem?
    for (int i = 0; i < m_volumeReaderJobs.size(); ++i)
    {
        if (!m_volumeReaderJobs[i].isNull())
        {
            disconnect(m_volumeReaderJobs[i], SIGNAL(done(ThreadWeaver::Job*)), this, SIGNAL(readingFinished()));
            disconnect(m_volumeReaderJobs[i], SIGNAL(progress(ThreadWeaver::Job*, int)), this, SLOT(updateProgress(ThreadWeaver::Job*, int)));
        }
        m_volumeReaderJobs[i] = NULL;
    }
    m_volumeReaderJobs.clear();
    m_jobsProgress.clear();
}

bool VolumeReaderManager::readingSuccess()
{
    bool success = true;
    int i = 0;
    while (success && i < m_volumeReaderJobs.size())
    {
        success = m_volumeReaderJobs[i]->success();
        i++;
    }

    return success;
}

Volume* VolumeReaderManager::getVolume()
{
    return m_volumeReaderJobs.first()->getVolume();
}

QList<Volume*> VolumeReaderManager::getVolumes()
{
    QList<Volume*> volumes;
    foreach (const QPointer<VolumeReaderJob> &job, m_volumeReaderJobs)
    {
        volumes << job->getVolume();
    }

    return volumes;
}

QString VolumeReaderManager::getLastErrorMessageToUser()
{
    // TODO Ara només retorna el missatge del primer que ha fallat
    QString errorMessage;

    bool success = true;
    int i = 0;
    while (success && i < m_volumeReaderJobs.size())
    {
        success = m_volumeReaderJobs[i]->success();

        if (!success)
        {
            errorMessage = m_volumeReaderJobs[i]->getLastErrorMessageToUser();
        }

        i++;
    }
    return errorMessage;
}

bool VolumeReaderManager::isReading()
{
    bool reading = false;
    int i = 0;
    while (!reading && i < m_volumeReaderJobs.size())
    {
        reading = !m_volumeReaderJobs[i].isNull() && !m_volumeReaderJobs[i]->isFinished();
        i++;
    }

    return reading;
}

void VolumeReaderManager::updateProgress(ThreadWeaver::Job *job, int value)
{
    m_jobsProgress.insert(job, value);

    int currentProgress = 0;
    foreach (int volumeProgressValue, m_jobsProgress)
    {
        currentProgress += volumeProgressValue;
    }

    currentProgress /= m_jobsProgress.size();

    emit progress(currentProgress);
}

void VolumeReaderManager::jobFinished()
{
    if (!isReading())
    {
        emit readingFinished();
    }
}

} // namespace udg
