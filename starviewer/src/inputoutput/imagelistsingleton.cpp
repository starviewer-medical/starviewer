/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "imagelistsingleton.h"

namespace udg {

ImageListSingleton::ImageListSingleton()
{
}

ImageListSingleton* ImageListSingleton::pInstance = 0;

ImageListSingleton * ImageListSingleton::getImageListSingleton()
{
    if ( pInstance == 0 )
    {
        pInstance = new ImageListSingleton;
    }
    
    return pInstance;
}

ImageListSingleton::~ImageListSingleton()
{
}

};
