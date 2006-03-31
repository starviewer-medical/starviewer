/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "starviewerprocessimage.h"
#include "image.h"
#include <iostream.h>
#include "qretrievescreen.h"
#include <string.h>

namespace udg {

StarviewerProcessImage::StarviewerProcessImage()
{
        m_downloadedImages=0;
        m_localCache = CachePacs::getCachePacs();
        m_error = false;
}


/** Enviar un signal de imatge descarregada a qretrievescreen, i quant es descarrega una sèrie nova envia signal de series descarregada
  */
void StarviewerProcessImage::process(Image *image)
{
    if (m_downloadedImages == 0)
    {   m_oldSeriesUID = image->getSeriesUID();
        //enviem un signal indicant que ha començat la descarrega de l'estudi
        emit(startRetrieving( image->getStudyUID().c_str()));
    }
    m_downloadedImages++;
    emit(imageRetrieved(image->getStudyUID().c_str(),m_downloadedImages));
    
    if (m_oldSeriesUID != image->getSeriesUID())
    {
        emit(seriesRetrieved( image->getStudyUID().c_str() ) );
        m_oldSeriesUID = image->getSeriesUID();
    }
    m_localCache->insertImage(image);
    
    m_studyUID = image->getStudyUID();
    
}

/** Indica que s'ha produit algun error intentant guardar alguna de les imatges al disc dur
  */
/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error,
  *l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem
  *el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads,
  *per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImage::setErrorRetrieving()
{
    m_error = true;
}

/** Retorna si s'ha produit algun error intentant guardar alguna de les imatges al disc dur
  */
bool StarviewerProcessImage::getErrorRetrieving()
{
    return m_error || m_downloadedImages == 0;
} 

StarviewerProcessImage::~StarviewerProcessImage()
{
}


};

