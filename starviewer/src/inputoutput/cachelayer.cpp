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
#include "starviewersettings.h"
#include <QDate>

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
    CachePool pool;
    int usedSpaceInit, deletedSpace = 0, usedSpace;
    
    pool.getPoolUsedSpace( usedSpaceInit );
    QProgressDialog *progress;
    progress = new QProgressDialog(tr("Clearing cache..."),"", 0, usedSpaceInit);
    progress->setMinimumDuration(0);

    state = localCache->queryStudy( studyMask , studyList );
    
    studyList.firstStudy();
    while ( !studyList.end() && state.good() )
    {
        study = studyList.getStudy();
        state = localCache->delStudy( study.getStudyUID() ); //indiquem l'estudi a esborrar
        
        pool.getPoolUsedSpace(usedSpace);
        deletedSpace = usedSpaceInit - usedSpace;//calculem l'espai esborrat
        progress->setValue( deletedSpace );
        progress->repaint();
        studyList.nextStudy();
    }
    
    progress->setValue (usedSpaceInit);
    
    if (!state.good())
    {
        return state;
    }
    else
    {        
        return state.setStatus( CORRECT );
    }
}

Status CacheLayer::deleteOldStudies()
{
    QDate today;
    StarviewerSettings settings;
    CachePacs *localCache = CachePacs::getCachePacs();
    StudyList studyList;
    Status state;
    Study study;
    int comptador = 0;
       
    today = today.currentDate();
    //calculem fins a quin dia conservarem els estudis
    //de la data del dia restem el paràmetre definit per l'usuari, que estableix quants dies pot estar un estudi sense ser visualitzat
    today = today.addDays( - settings.getMaximumDaysNotViewedStudy().toInt( NULL , 10 ) );
    
    //cerquem els estudis que no han estat visualitzats, en una data inferior a la passada per paràmetre
    state = localCache->queryOldStudies( today.toString("yyyyMMdd").toAscii().constData() , studyList );
    studyList.firstStudy();
    
    QProgressDialog *progress;
    progress = new QProgressDialog(tr("Clearing old studies..."), "" , 0 , studyList.count() );
    progress->setMinimumDuration(0);
    
    while ( state.good() && !studyList.end() )
    {
        study = studyList.getStudy();
        state = localCache->delStudy( study.getStudyUID() ); //indiquem l'estudi a esborrar   
        studyList.nextStudy();
        comptador++;
        progress->setValue( comptador );
        progress->repaint();
    }
    
    progress->close();
    
    if ( !state.good() )
    {
        return state;
    }
    else return state.setStatus( CORRECT );
}

CacheLayer::~CacheLayer()
{
}


}
