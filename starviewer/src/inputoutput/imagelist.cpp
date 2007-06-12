/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imagelist.h"

namespace udg {
/* AQUESTA CLASSE NOMES SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'HAN IMPLEMENTAT SEMAFORS
  */

ImageList::ImageList()
{
    int init_value = 1;//Només un thread alhora pot gravar a la llista

    m_semafor = (sem_t*) malloc( sizeof( sem_t ) );
    sem_init( m_semafor , 0 , init_value );

    m_iterator = m_imageList.begin();
}

void ImageList::insert( Image image )
{
    sem_wait( m_semafor );
    m_imageList.push_back( image );
    sem_post( m_semafor );
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
