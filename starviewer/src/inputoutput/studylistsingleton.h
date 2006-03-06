/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGSTUDYLISTSINGLETON_H
#define UDGSTUDYLISTSINGLETON_H

#include "studylist.h"

namespace udg {

/** Implamenta una classe singleton de l'studyList
@author marc
*/
class StudyListSingleton: public StudyList{
    
public:
     //this make this class a singleton class, return a static pointer to this class
    static StudyListSingleton* getStudyListSingleton();
    
private :
    static StudyListSingleton *pInstance;
    
    StudyListSingleton();
    ~StudyListSingleton();

};

};

#endif
