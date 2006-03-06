/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "processimage.h"
#include "image.h"

namespace udg {

//aquesta classes està buida per poder ser reimplementada
ProcessImage::ProcessImage()
{
}


/** Processa la imatge
  *        @param imatge a processar
  */
void ProcessImage::process(Image* )
{    

}

/** Indica que s'ha produit algun error descarregant alguna de les imatges de l'estudi
  */
void ProcessImage::setErrorRetrieving()
{

}

/** indica si s'ha produit algun error descarregant alguna de les imatges de l'estudi
  */
bool ProcessImage::getErrorRetrieving()
{

}

ProcessImage::~ProcessImage()
{
}


};
