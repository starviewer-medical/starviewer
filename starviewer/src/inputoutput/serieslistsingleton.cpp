/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "serieslistsingleton.h"

namespace udg {


SeriesListSingleton::SeriesListSingleton()
{
}

SeriesListSingleton* SeriesListSingleton::pInstance = 0;

SeriesListSingleton * SeriesListSingleton::getSeriesListSingleton()
{
    if (pInstance == 0)
    {
        pInstance = new SeriesListSingleton;
    }
    
    return pInstance;
}

SeriesListSingleton::~SeriesListSingleton()
{
}


};
