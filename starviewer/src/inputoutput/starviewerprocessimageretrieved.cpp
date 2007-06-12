/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewerprocessimageretrieved.h"
#include "imagedicominformation.h"
#include "series.h"
#include "starviewersettings.h"
#include "status.h"
#include "image.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "logging.h"

namespace udg {

StarviewerProcessImageRetrieved::StarviewerProcessImageRetrieved() : ProcessImage()
{
    m_downloadedImages = 0;
    m_error = false;
    m_downloadedSeries = 0;
}

void StarviewerProcessImageRetrieved::process( Image *image )
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheImageDAL cacheImageDAL;
    Series serie;

    /*si es la primera imatge que es descarrega fem un signal indicant que comença la descarrega, inserim la primera serie, i ara que tenim la informació de la sèrie update la modalitat de l'estudi*/
    if ( m_downloadedImages == 0 )
    {
        Series serie;

        //canviem l'estat de l'estudi de PENDING A RETRIEVING
        state = cacheStudyDAL.setStudyRetrieving( image->getStudyUID() );
        if ( !state.good() ) m_error = true;

        //inserim serie
        insertSerie( image );

        state = getSeriesInformation ( createImagePath( image  ), serie );
        if ( !state.good() ) m_error = true;

        m_studyUID = image->getStudyUID();
    }

    //inserim la nova sèrie que es descarrega
    if ( !m_addedSeriesList.contains( image->getSeriesUID() ) )
    {
        state = insertSerie( image );
        if ( state.good() || ( !state.good() && state.code() == 2019 ) ) // 2019, cas en que ja existia la serie a la base de dades, en aquest cas s'esta tornant a baixar la sèrie
        {
            emit( seriesRetrieved( image->getStudyUID() ) );

            if ( m_downloadedSeries == 0 )
            {
                emit( seriesView( image->getStudyUID() ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
            }
            m_downloadedSeries++;
        }
    }

    //inserim imatge
    state = cacheImageDAL.insertImage( image );
    //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error, en cas de l'error 2019 és que la imatge ja existia a la cache
    if ( !state.good() && state.code() != 2019 ) m_error = true;

    m_downloadedImages++;
    emit( imageRetrieved( image->getStudyUID(),m_downloadedImages ) );
}

Status StarviewerProcessImageRetrieved::insertSerie(Image *newImage)
{
    Series serie;
    Status state;
    CacheSeriesDAL cacheSeriesDAL;

    //inserim serie
    state = getSeriesInformation ( createImagePath( newImage ) , serie );

    if ( state.good() )
    {
        state = cacheSeriesDAL.insertSeries( &serie );
        //Podria ser que la sèrie estigués parcialment baixada, per tant si ja existeix ignorem l'errorProcessImage() de que ja existeix la sèrie a la base de dades
        if ( !state.good() && state.code() != 2019)
        {
            m_error = true;
        }
        else m_addedSeriesList.push_back( serie.getSeriesUID() );
    }

    return state;
}

/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error, l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads, per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImageRetrieved::setError()
{
    m_error = true;
    ERROR_LOG( "Error descarregant l'estudi" );
}

bool StarviewerProcessImageRetrieved::getError()
{
    if ( m_downloadedImages == 0)
    {
        ERROR_LOG( "Error s'han descarregat 0 imatges de l'estudi" );
    }
    return m_error || m_downloadedImages == 0;
}

Status StarviewerProcessImageRetrieved::getSeriesInformation( QString imagePath , Series &serie )
{
    Status state;
    QString path;

    ImageDicomInformation dInfo;

    state = dInfo.openDicomFile( imagePath );

    serie.setStudyUID( dInfo.getStudyUID() );
    serie.setSeriesUID( dInfo.getSeriesUID() );
    serie.setSeriesNumber( dInfo.getSeriesNumber() );
    serie.setSeriesModality( dInfo.getSeriesModality() );
    serie.setSeriesDescription( dInfo.getSeriesDescription() );
    serie.setBodyPartExaminated( dInfo.getSeriesBodyPartExamined() );
    serie.setProtocolName( dInfo.getSeriesProtocolName() );
    serie.setSeriesTime( dInfo.getSeriesTime() );
    serie.setSeriesDate( dInfo.getSeriesDate() );

    //calculem el path de la serie
    path = dInfo.getStudyUID();
    path.append( "/" );
    path.append( dInfo.getSeriesUID() );
    path.append( "/" );

    serie.setSeriesPath( path );

    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString( "Error obtenint informació de la sèrie. Número d'error %1 ERROR: %2" ).arg( state.code() ).arg( state.text() ) ) );
    }
    return state;
}

QString StarviewerProcessImageRetrieved::createImagePath( Image *image )
{
    StarviewerSettings settings;
    Series serie;
    QString imagePath;

    imagePath.insert( 0 , settings.getCacheImagePath() );
    imagePath.append( image->getStudyUID() );
    imagePath.append( "/" );
    imagePath.append( image->getSeriesUID() );
    imagePath.append("/");
    imagePath.append( image->getImageName() );

    return imagePath;
}

StarviewerProcessImageRetrieved::~StarviewerProcessImageRetrieved()
{
    //com no sabem quant s'acaba la descàrrega de l'última sèrie, fem que s'indiqui que ha finalitzat la seva descàrrega quan es destrueix l'objecte StarviewerProcessImageRetrieved, que és destruït just finalitzar la descarrega de tot l'estudi
	emit( seriesRetrieved( m_studyUID ) );

}

};

