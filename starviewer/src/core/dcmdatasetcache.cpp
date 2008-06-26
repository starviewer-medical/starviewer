/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dcmdatasetcache.h"

#include <dcmtk/dcmdata/dcdatset.h>

#include <QReadLocker>
#include <QWriteLocker>

#include "logging.h"

#include <QThread>
#include <QApplication>

namespace udg {

static const int DcmDatasetCacheFilesLimit = 1050;

DcmDatasetCache::DcmDatasetCache()
    : QObject(0), m_cache(DcmDatasetCacheFilesLimit), m_autoclearTimer(0), m_autoclearHasToBeActive(false)
{
    connect(this, SIGNAL(resetTimer()), SLOT(resetAutoclearTimer()));
}

DcmDatasetCache::DcmDatasetCache(int seconds)
    : QObject(0), m_cache(DcmDatasetCacheFilesLimit), m_autoclearTimer(0), m_autoclearHasToBeActive(false)
{
    // Només es marca com a activat, el timer no s'activa perquè no cal: no hi ha elements.
    m_autoclearHasToBeActive = true;
    m_secondsForAutoclear = seconds;
    connect(this, SIGNAL(resetTimer()), SLOT(resetAutoclearTimer()));
}

DcmDatasetCache::~DcmDatasetCache()
{
}

DcmDataset* DcmDatasetCache::find(const QString &filePath)
{
    emit resetTimer();

    QReadLocker locker(&m_cacheLock);
    DcmDataset *foundDataset = m_cache[filePath];
    return foundDataset ? dynamic_cast<DcmDataset*>( foundDataset->clone() ) :  NULL;
}

bool DcmDatasetCache::insert(const QString &filePath, DcmDataset *dataSet, int cost)
{
    emit resetTimer();

    QWriteLocker locker(&m_cacheLock);
    return m_cache.insert(filePath, dataSet, cost);
}

bool DcmDatasetCache::remove(const QString &filePath)
{
    emit resetTimer();

    QWriteLocker locker(&m_cacheLock);
    return m_cache.remove(filePath);
}

void DcmDatasetCache::clear()
{
    QWriteLocker locker(&m_cacheLock);
    m_cache.clear();
}

void DcmDatasetCache::startAutoclear(int seconds)
{
    if (seconds > 0 && !isAutoclearTimerActive())
    {
        m_autoclearHasToBeActive = true;
        m_secondsForAutoclear = seconds;
        startAutoclearTimer(seconds);
    }
}

void DcmDatasetCache::stopAutoclear()
{
    stopAutoclearTimer();
    m_autoclearHasToBeActive = false;
}

void DcmDatasetCache::timerEvent(QTimerEvent *)
{
    stopAutoclearTimer();
    DEBUG_LOG("Buidem la cache DcmDataset perquè ha transcorregut el temps d'autoclear");
    this->clear();
}

void DcmDatasetCache::resetAutoclearTimer()
{
    stopAutoclearTimer();

    if (m_autoclearHasToBeActive)
    {
        startAutoclearTimer(m_secondsForAutoclear);
    }
}

bool DcmDatasetCache::isAutoclearTimerActive()
{
    QReadLocker locker(&m_timerLock);
    return m_autoclearTimer != 0;
}

void DcmDatasetCache::startAutoclearTimer(int seconds)
{
    stopAutoclearTimer();
    
    QWriteLocker locker(&m_timerLock);

    m_autoclearTimer = startTimer(seconds*1000);

    if (QThread::currentThread() != QApplication::instance()->thread())
    {
        moveToThread(QApplication::instance()->thread());
    }
}

void DcmDatasetCache::stopAutoclearTimer()
{
    if ( isAutoclearTimerActive() )
    {
        QWriteLocker locker(&m_timerLock);

        killTimer(m_autoclearTimer);
        m_autoclearTimer = 0;
    }
}

}
