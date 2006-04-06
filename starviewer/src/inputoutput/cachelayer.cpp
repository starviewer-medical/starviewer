/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cachelayer.h"
#include "cachepacs.h"
#include "status.h"
#include "studymask.h"
#include <iostream.h>
#include <cachepool.h>
#include <QProgressDialog>

namespace udg {

CacheLayer::CacheLayer(QObject *parent)
 : QObject(parent)
{
}

Status CacheLayer::clearCache()
{
    CachePacs *localCache = CachePacs::getCachePacs();
    StudyMask studyMask;
    StudyList studyList;
    Study study;
    Status state;
    CachePool *pool = CachePool::getCachePool();
    int usedSpaceInit, deletedSpace = 0, usedSpace;
    
    pool->getPoolUsedSpace( usedSpaceInit );
    QProgressDialog *progress;
    progress = new QProgressDialog(tr("Clearing cache..."), tr("Cancel"), 0, usedSpaceInit);
    progress->setMinimumDuration(0);

    state = localCache->queryStudy( studyMask , studyList );
    
    studyList.firstStudy();
    while ( !studyList.end() && state.good() )
    {
        study = studyList.getStudy();
        state = localCache->delStudy( study.getStudyUID() ); //indiquem l'estudi a esborrar
        
        pool->getPoolUsedSpace(usedSpace);
        deletedSpace = usedSpaceInit - usedSpace;//calculem l'espai esborrat
        progress->setValue( deletedSpace );
        progress->repaint();
        studyList.nextStudy();
    }
    
    progress->setValue (usedSpaceInit);
    
    pool->resetPoolSpace();
    
    if (!state.good())
    {
        return state;
    }
    else
    {        
        return state.setStatus( CORRECT );
    }
}

CacheLayer::~CacheLayer()
{
}


}
