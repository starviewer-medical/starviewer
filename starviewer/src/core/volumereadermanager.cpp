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

#include "volumereadermanager.h"

#include "volume.h"
#include "volumereaderjob.h"
#include "volumereaderjobfactory.h"

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
    m_volumesProgress.clear();
    m_volumes.clear();
    m_success = true;
    m_lastError = "";
    m_numberOfFinishedJobs = 0;
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
    VolumeReaderJobFactory *volumeReaderFactory = VolumeReaderJobFactory::instance();

    connect(volumeReaderFactory, &VolumeReaderJobFactory::volumeReadingProgress, this, &VolumeReaderManager::updateProgress);
    connect(volumeReaderFactory, &VolumeReaderJobFactory::volumeReadingFinished, this, &VolumeReaderManager::jobFinished);

    foreach (Volume *volume, volumes)
    {
        m_volumesProgress.insert(volume, 0);
        m_volumes.append(volume);
        volumeReaderFactory->read(this, volume);
    }
}

void VolumeReaderManager::cancelReading()
{
    VolumeReaderJobFactory *volumeReaderFactory = VolumeReaderJobFactory::instance();

    foreach (Volume *volume, m_volumes)
    {
        volumeReaderFactory->cancelRead(this, volume);
    }

    disconnect(volumeReaderFactory, nullptr, this, nullptr); // disconnect everything
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
    return m_numberOfFinishedJobs < m_volumesProgress.size();
}

void VolumeReaderManager::updateProgress(void *requester, Volume *volume, int progressValue)
{
    if (requester == this && m_volumes.contains(volume))    // check also the volume just in case this comes from a previous cancelled request
    {
        m_volumesProgress[volume] = progressValue;
        int currentProgress = 0;

        foreach (int volumeProgressValue, m_volumesProgress)
        {
            currentProgress += volumeProgressValue;
        }

        currentProgress /= m_volumesProgress.size();
        emit progress(currentProgress);
    }
}

void VolumeReaderManager::jobFinished(void *requester, VolumeReaderJob *job)
{
    if (requester == this && m_volumes.contains(job->getVolume()))  // check also the volume just in case this comes from a previous cancelled request
    {
        m_success &= job->success();

        if (!m_success)
        {
            m_lastError = job->getLastErrorMessageToUser();
        }

        m_numberOfFinishedJobs++;

        if (!isReading())
        {
            emit readingFinished();
            disconnect(VolumeReaderJobFactory::instance(), nullptr, this, nullptr); // disconnect everything
        }
    }
}

} // namespace udg
