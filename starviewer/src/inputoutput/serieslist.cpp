#include <string>
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
    buit = true;
    m_iterator = m_seriesList.begin();
}

void SeriesList::insert(Series series)
{
    m_seriesList.push_back( series );
    buit = false;
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

}

