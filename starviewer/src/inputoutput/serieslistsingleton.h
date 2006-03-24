/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSERIESLISTSINGLETON_H
#define UDGSERIESLISTSINGLETON_H

#include "serieslist.h"

namespace udg {

/** Implementa una classe singletton de la classe SeriesList
@author marc
*/
class SeriesListSingleton: public SeriesList{

public :

     //this make this class a singleton class, return a static pointer to this class
     static SeriesListSingleton* getSeriesListSingleton();
      ~SeriesListSingleton();
     
private:
    SeriesListSingleton();
    static SeriesListSingleton *pInstance;
};

};

#endif
