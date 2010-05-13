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
    m_seriesInstanceUIDLastImageRetrieved = "";
}

void StarviewerProcessImageRetrieved::process(DICOMTagReader *dicomTagReader)
{
    QString seriesInstanceUID = dicomTagReader->getValueAttributeAsQString(DICOMSeriesInstanceUID);

    if (m_seriesInstanceUIDLastImageRetrieved.isEmpty())
    {
        //Cas de la primera imatge descarregada
        m_seriesInstanceUIDLastImageRetrieved = seriesInstanceUID;
        m_studyInstanceUIDRetrieved = dicomTagReader->getValueAttributeAsQString(DICOMStudyInstanceUID);
    }
    else if (m_seriesInstanceUIDLastImageRetrieved != seriesInstanceUID)
    {
        /* No sempre tots els estudis es descarreguen les seves imatges agrupades per sèries, per tant ens podem trobar que les * imatges d'una sèrie no es descarreguin juntes una darrera l'altre, per evitar fer un emit de la mateixa sèrie, que ja * hem indicat com a descarrega, mantenim una llista amb els UID de series que hem fet l'emit, així evitem fer l'emit 
        * de la mateixa sèrie dos vegades
         */
        if (!m_seriesInstanceUIDListRetrieved.contains(m_seriesInstanceUIDLastImageRetrieved))
        {
            m_seriesInstanceUIDListRetrieved.append(m_seriesInstanceUIDLastImageRetrieved);
            emit seriesRetrieved(dicomTagReader->getValueAttributeAsQString(DICOMStudyInstanceUID));
        }
        
        //Si la imatge descarregada actual té un UID de serie diferent que l'anterior
        m_seriesInstanceUIDLastImageRetrieved = seriesInstanceUID;
    }
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
    //Com no sabem quan finalitza la descarrega, hem de compriova
    if (!m_seriesInstanceUIDListRetrieved.contains(m_seriesInstanceUIDLastImageRetrieved))
    {
        emit(seriesRetrieved(m_studyInstanceUIDRetrieved));
    }
}

};

