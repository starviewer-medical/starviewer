
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
    int init_value = 1;//Només un thread alhora pot gravar a la llista

    m_semafor = (sem_t*) malloc( sizeof( sem_t ) );
    sem_init( m_semafor , 0 , init_value );

    buit = true;
    m_iterator = m_seriesList.begin();
}

void SeriesList::insert(Series series)
{
    sem_wait( m_semafor );
    m_seriesList.push_back( series );
    buit = false;
    sem_post( m_semafor );
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

Series SeriesList::getSeries()
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
    sem_wait( m_semafor );
    m_iterator = m_seriesList.begin();

    while ( m_iterator != m_seriesList.end() )
    {
        if ( (*m_iterator).getStudyUID() == studyUID && (*m_iterator).getPacsAETitle() == AETitlePacs  && (*m_iterator).getSeriesUID() == seriesUID   ) break;
        else m_iterator++;
    }

    sem_post( m_semafor );
    return ( m_iterator != m_seriesList.end() );
}

}

