/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGPACSLIST_H
#define UDGPACSLIST_H

#include "pacsparameters.h"
#include <list>

class string;

namespace udg {

/** Aquesta classe implementa una llistat d'objectes PacsParameters.
@author marc
*/
class PacsList{
public:
    PacsList();
    
    void insertPacs(PacsParameters);
    void nextPacs();
    void firstPacs();
    bool findPacs(std::string);
    PacsParameters getPacs();
    int size();
    void clear();   
    bool end();
    
    ~PacsList();
    
private:

    list<PacsParameters>m_listPacs;     
    list<PacsParameters>::iterator i;

};

};

#endif
