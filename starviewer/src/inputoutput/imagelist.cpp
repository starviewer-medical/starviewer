/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "imagelist.h"
#include <list>

namespace udg {
/* AQUESTA CLASSE NOMES SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'HAN IMPLEMENTAT SEMAFORS
  */

/** Constructor
  */
ImageList::ImageList()
{
    m_iterator = m_imageList.begin();
}

/** Insereix una imatge a la llista
  *              @param  Imatge a inserir
  */
void ImageList::insert(Image image)
{
    m_imageList.push_back(image);
}

/**Aquesta accio situa l'iterador de la llista al primer element de la llista
  *abans de començar a llegir s'ha d'invocar aquest mètode
*/
void ImageList::firstImage()
{
    m_iterator=m_imageList.begin();
}

/** Indica si s'ha arribat al final de la llista
  *        @return boolea indicant si s'ha arribat al final de la llista
*/
bool ImageList::end()
{
    return m_iterator == m_imageList.end();
}

/** l'iterador passa a apuntar al següent element
*/
void ImageList:: nextImage()
{
    m_iterator++;
}


/** retorna un objecte image
 *            @return objecte image
 */
Image ImageList::getImage()
{
    return (*m_iterator);
}


/** retorna el número d'imatges de la llista
  *              @return  número d'imatges de la llista
  */
int ImageList::count()
{
    return m_imageList.size();
}

/** Aquest mètode buida la llista
  */
void ImageList::clear()
{
    if (m_imageList.size() != 0) m_imageList.clear();
}


ImageList::~ImageList()
{
}


};
