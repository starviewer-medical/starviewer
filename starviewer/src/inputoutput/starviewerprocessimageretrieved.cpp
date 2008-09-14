/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewerprocessimageretrieved.h"

#include "dicomtagreader.h"

namespace udg {

StarviewerProcessImageRetrieved::StarviewerProcessImageRetrieved() : ProcessImage()
{
    m_error = false;
}

void StarviewerProcessImageRetrieved::process(DICOMTagReader *dicomTagReader)
{
    emit fileRetrieved(dicomTagReader);
}

/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error, l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads, per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImageRetrieved::setError()
{
    m_error = true;
}

bool StarviewerProcessImageRetrieved::getError()
{
    return m_error;
}

StarviewerProcessImageRetrieved::~StarviewerProcessImageRetrieved()
{
}

};

