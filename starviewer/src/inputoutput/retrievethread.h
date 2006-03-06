/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRETRIEVETHREAD_H
#define UDGRETRIEVETHREAD_H

#include <pthread.h>
#include <string>

namespace udg {

//class string;

/** Aquesta classe permet manipular informació d'un thread que descarrega imatges
  *
@author Grup de Gràfics de Girona  ( GGG )
*/
class RetrieveThread{

private :

    pthread_t m_Thread;
    std::string m_StudyUID;
    std::string m_DefaultSeriesUID;
    bool m_View;

public:
    
    void setThread(pthread_t );
    void setStudyUID(std::string);
    void setDefaultSeriesUID(std::string);
    void setView(bool);
        
    pthread_t getThread();
    std::string getStudyUID();
    std::string getDefaultSeriesUID();
    bool getView();
    
    RetrieveThread();

    ~RetrieveThread();

};

};

#endif
