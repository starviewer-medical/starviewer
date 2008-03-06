
#include <list>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "serieslist.h"

namespace udg
{

/* ATENCIO AQUESTA CLASSE NO SERA ACCEDIDA, DE MOMENT PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'IMPLEMENTEN SEMAFORS
 */
SeriesList::SeriesList()
{
    m_semaphore = new QSemaphore( 1 );//Només un thread alhora pot gravar a la llista

    buit = true;
    m_iterator = m_seriesList.begin();
}

void SeriesList::insert(DICOMSeries series)
{
    m_semaphore->acquire();;
    m_seriesList.push_back( series );
    buit = false;
    m_semaphore->release();
}

void SeriesList::firstSeries()
{
    if ( !buit ) m_iterator = m_seriesList.begin();
}

bool SeriesList::end()
{
    if ( m_seriesList.size() == 0 )
    {
        return true;
    }
    else return m_iterator == m_seriesList.end();
}

void SeriesList:: nextSeries()
{
    m_iterator++;
}

DICOMSeries SeriesList::getSeries()
{
    return ( *m_iterator );
}

int SeriesList::count()
{
    return m_seriesList.size();
}

void SeriesList::clear()
{
    if ( m_seriesList.size() != 0 ) m_seriesList.clear();
}

bool SeriesList::exists( QString studyUID , QString seriesUID , QString AETitlePacs )
{
    m_semaphore->acquire();
    m_iterator = m_seriesList.begin();

    while ( m_iterator != m_seriesList.end() )
    {
        if ( (*m_iterator).getStudyUID() == studyUID && (*m_iterator).getPacsAETitle() == AETitlePacs  && (*m_iterator).getSeriesUID() == seriesUID   ) break;
        else m_iterator++;
    }

    m_semaphore->release();
    return ( m_iterator != m_seriesList.end() );
}

}

