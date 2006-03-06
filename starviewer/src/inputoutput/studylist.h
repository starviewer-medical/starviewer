#ifndef STUDYLIST
#define STUDYLIST

#define HAVE_CONFIG_H 1
#include <cond.h>
#include "study.h"
#include <list>
#include <semaphore.h>

/** This is a singleton class. This class implements a list of studies. Gives us the needed functions to insert studies in the list and
read the studies in the list
*/

namespace udg{
class StudyList{

 public :
    
     StudyList();
     
    void insert(Study study);
    
    void firstStudy();
    void nextStudy();
    bool end();  
    Study getStudy();
    bool findStudy(std::string);
    void clear();
    int getNumberOfStudies();
    
 private :

    sem_t *m_semafor;
    
    list<Study>m_listStudy;     
    list<Study>::iterator m_iterator;
};
};
#endif

