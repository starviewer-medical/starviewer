/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "starviewerprocessimagestored.h"

#include "image.h"
#include "series.h"
#include "study.h"

namespace udg {

StarviewerProcessImageStored::StarviewerProcessImageStored(QObject *parent)
 : QObject(parent)
{
    m_imagesStored = 0;
}

void StarviewerProcessImageStored::process(Image *image)
{
    QString seriesUID = image->getParentSeries()->getInstanceUID();
    QString studyUID = image->getParentSeries()->getParentStudy()->getInstanceUID();

    if ( m_imagesStored == 0 ) // és la primera imatge que guardem
    {
        m_oldSeriesUID = seriesUID;
        m_studyUID = studyUID;
    }

    if ( m_oldSeriesUID != seriesUID ) // canviem de serie d'imatges guardades
    {
        emit( seriesStored(studyUID) );
        m_oldSeriesUID = seriesUID;
    }

    m_imagesStored++;
    emit( imageStored(studyUID, m_imagesStored) );
}

StarviewerProcessImageStored::~StarviewerProcessImageStored()
{
    //com no sabem quant s'acaba de guardar l'última sèrie, fem que s'indiqui que ha finalitzat el procés de moure quan es destrueix l'objecte StarviewerProcessImageStored, que és destruït just finalitzar el procés d'emmagatzematge de tot l'estudi
    emit( seriesStored( m_studyUID ) );
}


}
