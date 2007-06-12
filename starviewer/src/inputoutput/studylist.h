#ifndef STUDYLIST
#define STUDYLIST

#define HAVE_CONFIG_H 1
#include <cond.h>
#include "study.h"
#include <list>
#include <semaphore.h>

/** This is a singleton class. This class implements a list of studies. Gives us the needed functions to insert studies in the list and read the studies in the list
*/

namespace udg{

class StudyList{

 public :

    StudyList();

    /** This action inserts a Study at the end of the list
     * @param  Study
     */
    void insert( Study study );

    /**This actions put the pointer of the list at the firts study.
     *It's very important before start to read, cals this action, else the pointer will be at the end of the list, and if we try to read, it will produce a segmentation fault
     */
    void firstStudy();

    /// Move the pointer to the next Study
    void nextStudy();

    /** This functions returns true if the pointer of the list is at the end
     * @return True if we have read all the studies
     */
    bool end();

    /** Match a Study with the UID parameter, If study is matched with getStudy, can be retrieved the information of the study
     * @ UID of the study to match
     * @return Return true if study is matched
     */

    /** Retorna la llista a la que esa apuntant l'iterador
     * @return estudi a la que apunta la llista
     */
    Study getStudy();

    /** Indica si un determinat estudi d'un determinat PACS està dins la llista
     * @param studyUID UID de l'estudi
     * @param AETitlePacs AETitle del Pacs on ha d'estat l'estudi
     * @return  number of studies in the list
     */
    bool exists( QString studyUID , QString AETitlePacs );

    /// neteja la llista d'estudis
    void clear();

    ///compta quants elements té la llista
    int count();

private :

    sem_t *m_semafor;

    list< Study > m_listStudy;
    list< Study >::iterator m_iterator;
};
};
#endif

