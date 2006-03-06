/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "studylistsingleton.h"

namespace udg {

StudyListSingleton::StudyListSingleton()
{
}

StudyListSingleton* StudyListSingleton::pInstance = 0;

/** Retorna una instancia de l'objecte
  *        @return instancia de l'objecte
  */
StudyListSingleton * StudyListSingleton::getStudyListSingleton()
{
    if (pInstance == 0)
    {
        pInstance = new StudyListSingleton;
    }
    
    return pInstance;
}

StudyListSingleton::~StudyListSingleton()
{
}


};
