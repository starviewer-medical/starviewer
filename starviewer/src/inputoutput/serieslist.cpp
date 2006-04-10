#include "serieslist.h"
#include <list>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

namespace udg
{

/* ATENCIO AQUESTA CLASSE NO SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'IMPLEMENTEN SEMAFORS
 */
SeriesList::SeriesList()
{
    buit = true;
    m_iterator = m_seriesList.begin();
}    

/** This action inserts a Study at the end of the list
  *              @param  Study
  */
void SeriesList::insert(Series series)
{
    m_seriesList.push_back(series);
    buit = false;
}

/**This actions put the pointer of the list at the firts study. 
  *It's very important before start to read, cals this action, else the pointer will be at the end of the list, and if we try to read,
  *we will produce a segmentation fault 
  */
void SeriesList::firstSeries()
{
    if (!buit) m_iterator = m_seriesList.begin();
}

/** This functions returns true if the pointer of the list is at the end
  *        @return True if we have read all the studies
  */
bool SeriesList::end()
{
    if (m_seriesList.size() == 0)
    {
        return true;
    }   
    else return m_iterator == m_seriesList.end();
}

/** Move the pointer to the next Study
*/
void SeriesList:: nextSeries()
{
    m_iterator++;
}

/** Returns a Series
 *            @return return a series
 */
Series SeriesList::getSeries()
{
    return (*m_iterator);
}

/** This function gets the number of series in the list
  *              @return  number of series that match the search
  */
int SeriesList::count()
{
    return m_seriesList.size();
}

/** Aquests mètode buida la llista
  */
void SeriesList::clear()
{
    if (m_seriesList.size() != 0) m_seriesList.clear();
}

}

