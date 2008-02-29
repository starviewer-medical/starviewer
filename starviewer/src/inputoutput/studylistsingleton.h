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

/** Implementa una classe singleton de l'studyList
@author marc
*/
class StudyListSingleton: public StudyList{

public:

    /** Retorna una instancia de l'objecte
     * @return instancia de l'objecte
     */
    static StudyListSingleton* getStudyListSingleton();

    ///Destructor de la classe
    ~StudyListSingleton();

private :
    static StudyListSingleton *pInstance;

    ///Constructor de la classe
    StudyListSingleton();


};

};

#endif
