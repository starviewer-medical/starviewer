#include "volumereadermanager.h"

#include "asynchronousvolumereader.h"
#include "volumereaderjob.h"
#include "volume.h"

namespace udg {

VolumeReaderManager::VolumeReaderManager(QObject *parent) :
    QObject(parent)
{
}

VolumeReaderManager::~VolumeReaderManager()
{
    cancelReading();
}

void VolumeReaderManager::initialize()
{
    m_volumeReaderJobs.clear();
    m_jobsProgress.clear();
    m_volumes.clear();
    m_success = true;
    m_lastError = "";
}

void VolumeReaderManager::readVolume(Volume *volume)
{
    QList<Volume*> volumes;
    volumes << volume;

    readVolumes(volumes);
}

void VolumeReaderManager::readVolumes(const QList<Volume*> &volumes)
{
    initialize();

    foreach (Volume *volume, volumes)
    {
        // TODO Esborrar volumeReader!!
        AsynchronousVolumeReader *volumeReader = new AsynchronousVolumeReader();
        m_volumeReaderJobs << volumeReader->read(volume);
        m_jobsProgress.insert(m_volumeReaderJobs.last(), 0);
        m_volumes << NULL;
        connect(m_volumeReaderJobs.last(), SIGNAL(done(ThreadWeaver::Job*)), SLOT(jobFinished(ThreadWeaver::Job*)));
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
            disconnect(m_volumeReaderJobs[i], SIGNAL(done(ThreadWeaver::Job*)), this, SIGNAL(jobFinished(ThreadWeaver::Job*)));
            disconnect(m_volumeReaderJobs[i], SIGNAL(progress(ThreadWeaver::Job*, int)), this, SLOT(updateProgress(ThreadWeaver::Job*, int)));
        }
        m_volumeReaderJobs[i] = NULL;
    }
    initialize();
}

bool VolumeReaderManager::readingSuccess()
{
    return m_success;
}

Volume* VolumeReaderManager::getVolume()
{
    return m_volumes.first();
}

QList<Volume*> VolumeReaderManager::getVolumes()
{
    return m_volumes;
}

QString VolumeReaderManager::getLastErrorMessageToUser()
{
    // TODO Ara només retorna el missatge del primer que ha fallat
    return m_lastError;
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

void VolumeReaderManager::jobFinished(ThreadWeaver::Job *job)
{
    VolumeReaderJob *readerJob = qobject_cast<VolumeReaderJob*>(job);

    m_volumes[m_volumeReaderJobs.indexOf(readerJob)] = readerJob->getVolume();

    if (m_success)
    {
        m_success = readerJob->success();

        if (!m_success)
        {
            m_lastError = readerJob->getLastErrorMessageToUser();
        }
    }

    if (!isReading())
    {
        emit readingFinished();
    }
}

} // namespace udg
