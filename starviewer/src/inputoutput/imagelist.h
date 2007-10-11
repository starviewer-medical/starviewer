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
#include <list>

#include "dicomimage.h"
#include <semaphore.h>

/* AQUESTA CLASSE NOMES SERA ACCEDIDA PER MES D'UN THREAD A LA VEGADA PER AIXO NO S'HAN IMPLEMENTAT SEMAFORS
  */
namespace udg {

/** Classe per manipular una llista d'objectes image
@author marc
*/
class ImageList{
public:

    ///constructor de la classe
    ImageList();

    ///destructor de la classe
    ~ImageList();

    /** Insereix una imatge a la llista
     * @param  Imatge a inserir
     */
    void insert(DICOMImage);

     ///Aquesta accio situa l'iterador de la llista al primer element de la llista abans de començar a llegir s'ha d'invocar aquest mètode
    void firstImage();

    /// l'iterador passa a apuntar al següent element
    void nextImage();

    /** Indica si s'ha arribat al final de la llista
     * @return boolea indicant si s'ha arribat al final de la llista
     */
    bool end();

    /** retorna el número d'imatges de la llista
     * @return  número d'imatges de la llista
     */
    int count();

    /** retorna un objecte image
     * @return objecte image
     */
    DICOMImage getImage();

    /// Aquest mètode buida la llista
    void clear();

 private :

    sem_t *m_semafor;

    list<DICOMImage> m_imageList;
    list<DICOMImage>::iterator m_iterator;

};
};//end namespace udg

#endif
