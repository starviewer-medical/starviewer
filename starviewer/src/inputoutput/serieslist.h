#ifndef SERIESLIST
#define SERIESLIST

#define HAVE_CONFIG_H 1

#include "series.h"
#include <semaphore.h>
class string;

/* ATENCIO AQUESTA CLASSE NO SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'IMPLEMENTEN SEMAFORS
 */
/** This class implements a list of series. Gives us the needed functions to read the information
  * of the series that match the queries and to insert series in the list
*/

namespace udg{
class SeriesList
{

 public :

    ///Constructor
    SeriesList();

    /** This action inserts a Study at the end of the list
     *              @param  Study
     */
    void insert(Series series);

    /**This actions put the pointer of the list at the firts study.
     *It's very important before start to read, cals this action, else the pointer will be at the end of the list, and if we try to read, it will produce a segmentation fault
    */
    void firstSeries();

    /// Move the pointer to the next Study
    void nextSeries();

    /** This functions returns true if the pointer of the list is at the end
     * @return True if we have read all the studies
     */
    bool end();

    /** This function gets the number of series in the list
     * @return  number of series that match the search
     */
    int count();

    /** Returns a Series
     * @return return a series
     */
    Series getSeries();

    /// Aquests mètode buida la llista
    void clear();

    /** Indica si una sèrie d'un determinat PACS està dins la llista
     * @param studyUID UID de l'estudi
     * @param AETitlePacs AETitle del Pacs on ha d'estat l'estudi
     * @return  number of studies in the list
     */
    bool exists( std::string studyUID , std::string seriesUID , std::string AETitlePacs );

 private :

    bool buit;
    sem_t *m_semafor;

    std::list<Series> m_seriesList;
    std::list<Series>::iterator m_iterator;
};
}; //end name space
#endif

