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

#include "volumereaderjob.h"

#include "volumereader.h"
#include "volume.h"
#include "logging.h"

namespace udg {

VolumeReaderJob::VolumeReaderJob(Volume *volume, QObject *parent)
    : QObject(parent)
{
    m_volumeToRead = volume;
    m_volumeIdentifier = volume->getIdentifier();
    m_volumeReadSuccessfully = false;
    m_lastErrorMessageToUser = "";
    m_abortRequested = false;
}

VolumeReaderJob::~VolumeReaderJob()
{
    DEBUG_LOG(QString("Destructor ~VolumeReaderJob pel Volume: %1").arg(m_volumeIdentifier.getValue()));
}

void VolumeReaderJob::requestAbort()
{
    // Hem de protegir l'accés a m_volumeReaderToAbort amb un lock ja que si no, es podria donar el cas que es comprovés si és null, donés que no
    // i, jut abans de cridar els seus mètodes, aquest es posés a null, fent petar l'aplicació.
    QMutexLocker locker(&m_volumeReaderToAbortMutex);

    m_abortRequested = true;
    if (!m_volumeReaderToAbort.isNull())
    {
        m_volumeReaderToAbort.data()->requestAbort();
        DEBUG_LOG(QString("requestAbort to Volume: %1 done").arg(m_volumeIdentifier.getValue()));
    }
}

bool VolumeReaderJob::success() const
{
    return m_volumeReadSuccessfully && !m_abortRequested;
}

QString VolumeReaderJob::getLastErrorMessageToUser() const
{
    return m_lastErrorMessageToUser;
}

Volume* VolumeReaderJob::getVolume() const
{
    return m_volumeToRead;
}

const Identifier& VolumeReaderJob::getVolumeIdentifier() const
{
    return m_volumeIdentifier;
}

void VolumeReaderJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(self)
    Q_UNUSED(thread)

    Q_ASSERT(m_volumeToRead);

    DEBUG_LOG(QString("VolumeReaderJob::run() with Volume: %1").arg(m_volumeIdentifier.getValue()));

    VolumeReader *volumeReader = new VolumeReader();

    {
        // El locker només serà vàlid dintre de l'scope. Es fa així en comptes de fer-ho amb un .lock o .unlock per
        // assegurar-nos que si salta una excepció s'alliberarà el lock.
        QMutexLocker locker(&m_volumeReaderToAbortMutex);
        m_volumeReaderToAbort = volumeReader;
    }

    connect(volumeReader, SIGNAL(progress(int)), SLOT(updateProgress(int)));
    m_volumeReadSuccessfully = volumeReader->readWithoutShowingError(m_volumeToRead);
    m_lastErrorMessageToUser = volumeReader->getLastErrorMessageToUser();

    {
        QMutexLocker locker(&m_volumeReaderToAbortMutex);

        m_volumeReaderToAbort.clear();
        delete volumeReader;
    }

    DEBUG_LOG(QString("End VolumeReaderJob::run() with Volume: %1 and result %2").arg(m_volumeIdentifier.getValue()).arg(m_volumeReadSuccessfully));
    if (!m_volumeReadSuccessfully)
    {
        DEBUG_LOG(QString("                          Error Volume: %1: %2").arg(m_volumeIdentifier.getValue()).arg(m_lastErrorMessageToUser));
    }
}

void VolumeReaderJob::defaultEnd(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread)
{
    emit done(job);

    Job::defaultEnd(job, thread);
}

void VolumeReaderJob::updateProgress(int value)
{
    emit progress(this, value);
}

} // End namespace udg
