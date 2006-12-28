/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "imagelist.h"
#include <string>

namespace udg {
/* AQUESTA CLASSE NOMES SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'HAN IMPLEMENTAT SEMAFORS
  */

ImageList::ImageList()
{
    m_iterator = m_imageList.begin();
}

void ImageList::insert( Image image )
{
    m_imageList.push_back( image );
}

void ImageList::firstImage()
{
    m_iterator = m_imageList.begin();
}

bool ImageList::end()
{
    return m_iterator == m_imageList.end();
}

void ImageList:: nextImage()
{
    m_iterator++;
}

Image ImageList::getImage()
{
    return ( *m_iterator );
}

int ImageList::count()
{
    return m_imageList.size();
}

void ImageList::clear()
{
    if ( m_imageList.size() != 0 ) m_imageList.clear();
}

ImageList::~ImageList()
{
}

};
