/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dcmdatasetcache.h"

//\TODO trobar perquè això és necessari amb les dcmtk
#define HAVE_CONFIG_H 1
#include "dcmtk/dcmdata/dcdatset.h"

#include "logging.h"

namespace udg {

static const int DcmDatasetCacheFilesLimit = 1050;

DcmDatasetCache::DcmDatasetCache()
    : QObject(0), m_cache(DcmDatasetCacheFilesLimit), m_autoclearTimer(0), m_autoclearIsActive(false)
{
}

DcmDatasetCache::DcmDatasetCache(int seconds)
    : QObject(0), m_cache(DcmDatasetCacheFilesLimit), m_autoclearTimer(0), m_autoclearIsActive(false)
{
    // Només es marca com a activat, però el timer no s'activa perquè no cal: no hi ha elements.
    m_autoclearIsActive = true;
    m_secondsForAutoclear = seconds;
}

DcmDatasetCache::~DcmDatasetCache()
{
}

DcmDataset* DcmDatasetCache::find(const QString &filePath)
{
    resetAutoclearTimer();
    DcmDataset *foundDataset = m_cache[filePath];
    return foundDataset ? dynamic_cast<DcmDataset*>( foundDataset->clone() ) :  NULL;
}

bool DcmDatasetCache::insert(const QString &filePath, DcmDataset *dataSet, int cost)
{
    resetAutoclearTimer();
    return m_cache.insert(filePath, dataSet, cost);
}

bool DcmDatasetCache::remove(const QString &filePath)
{
    resetAutoclearTimer();
    return m_cache.remove(filePath);
}

void DcmDatasetCache::clear()
{
    m_cache.clear();
}

void DcmDatasetCache::activateAutoclear(int seconds)
{
    if (seconds > 0)
    {
        m_autoclearIsActive = true;
        m_secondsForAutoclear = seconds;
        activateAutoclearTimer(seconds);
    }
}

void DcmDatasetCache::deactivateAutoclear()
{
    killAutoclearTimer();
}

void DcmDatasetCache::timerEvent(QTimerEvent *)
{
    killAutoclearTimer();
    DEBUG_LOG("Buidem la cache DcmDataset perquè ha transcorregut el temps d'autoclear");
    m_cache.clear();
}

void DcmDatasetCache::resetAutoclearTimer()
{
    if ( isAutoclearTimerActive() )
    {
        killAutoclearTimer();
    }

    if (m_autoclearIsActive)
    {
        activateAutoclearTimer(m_secondsForAutoclear);
    }
}

bool DcmDatasetCache::isAutoclearTimerActive()
{
    return m_autoclearTimer != 0;
}

void DcmDatasetCache::activateAutoclearTimer(int seconds)
{
    if ( isAutoclearTimerActive() )
    {
        killAutoclearTimer();
    }
    m_autoclearTimer = startTimer(seconds);
}

void DcmDatasetCache::killAutoclearTimer()
{
    if ( isAutoclearTimerActive() )
    {
        killTimer(m_autoclearTimer);
        m_autoclearTimer = 0;
    }
}

}
