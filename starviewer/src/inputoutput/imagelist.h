/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGIMAGELIST_H
#define UDGIMAGELIST_H
#define HAVE_CONFIG_H 1

#include <cond.h>
#include "image.h"
#include <string>
#include <list>

/* AQUESTA CLASSE NOMES SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'HAN IMPLEMENTAT SEMAFORS
  */
namespace udg {

/** Classe per manipular una llista d'objectes image
@author marc
*/
class ImageList{
public:
    ImageList();

    ~ImageList();
    
    void insert(Image);
        
    void firstImage();
    void nextImage();
    bool end();    

    int getNumberOfImages();
  
    Image getImage();
      
    void clear();
         
 private :
 
    list<Image> m_imageList;     
    list<Image>::iterator m_iterator;
    
};

};

#endif
