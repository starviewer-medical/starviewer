#ifndef SERIESLIST
#define SERIESLIST

#define HAVE_CONFIG_H 1

#include "series.h"
#include <string>
#include <iostream>

/* ATENCIO AQUESTA CLASSE NO SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'IMPLEMENTEN SEMAFORS
 */
/** This class implements a list of series. Gives us the needed functions to read the information
  * of the series that match the queries and to insert series in the list
*/

namespace udg{
class SeriesList{

 public :
    
    SeriesList();
    
    void insert(Series series);
    
    void firstSeries();
    void nextSeries();
    bool end();    

    int getNumberOfSeries();
  
    Series getSeries();
      
    void clear();
         
 private :
 
    bool buit;
    
    std::list<Series> m_seriesList;     
    std::list<Series>::iterator m_iterator;
};
};
#endif

