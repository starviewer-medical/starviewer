#ifndef QUERYPACS
#define QUERYPACS

#define HAVE_CONFIG_H 1
#include "querystudy.h"
//#include "struct.h"
#include <cond.h>
#include "study.h"
#include <list>
#include "studymask.h"
#include <semaphore.h>
#include "status.h"
#include "studylistsingleton.h"
#include "pacslist.h"

/** Aquesta classe permet atacar a la vegada diferents PACS, mitjançant la utilització de threads per aconseguir un rendiment optim en les queries a
  * diversos pacs a la vegada. El llistat resultant d'estudis al guardar a la classe singleton de l'studyList
*/
namespace udg{
class MulQueryStudy{

private :

    StudyMask m_searchMask;
    pthread_t m_threads[50];
    StudyListSingleton* m_studyListSingleton;
    PacsList m_PacsList;
    int m_maxThreads;
    
    void QueryStudies();
    
public:
   
    MulQueryStudy(int);
    
    void setMask(StudyMask);
    
    void setPacsList(PacsList);
    
    Status StartQueries();
    
    StudyListSingleton* getStudyList();

};
};

#endif
