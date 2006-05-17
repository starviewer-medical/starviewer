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

/** Classe singletton de ImageList
@author marc
*/
class ImageListSingleton : public ImageList{

public:

    /** Retorna una instància a la llista d'imatges si existeix, sinó la crea i la retorna
     * @return instància a la ImageList
     */
    static ImageListSingleton* getImageListSingleton();
     
private:

    static ImageListSingleton *pInstance;

    ///constructor de la classe
    ImageListSingleton();
    
    ///destructor de la classe
    ~ImageListSingleton();

};

};

#endif
