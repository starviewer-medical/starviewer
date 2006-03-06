/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGIMAGELISTSINGLETON_H
#define UDGIMAGELISTSINGLETON_H
#include "imagelist.h"

namespace udg {

/**
@author marc
*/
class ImageListSingleton : public ImageList{

public:

     static ImageListSingleton* getImageListSingleton();
     
private:

    static ImageListSingleton *pInstance;

    ImageListSingleton();
    ~ImageListSingleton();

};

};

#endif
