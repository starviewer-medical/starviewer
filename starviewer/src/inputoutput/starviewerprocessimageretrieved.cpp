/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <string.h>

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

    /*si es la primera imatge que es descarrega fem un signal indicant que comença la descarrega, inserim la primera serie, i ara que tenim la informació de la sèrie update la modalitat de l'estudi*/
    if ( m_downloadedImages == 0 )
    {  
        Series serie;
        
        //canviem l'estat de l'estudi de PENDING A RETRIEVING
        state = cacheStudyDAL.setStudyRetrieving( image->getStudyUID().c_str() );
        if ( !state.good() ) m_error = true;
        
        //inserim serie
        insertSerie( image );
        
        state = getSeriesInformation ( createImagePath( image  ), serie );
        if ( !state.good() ) m_error = true;
           
        state = cacheStudyDAL.setStudyModality( serie.getStudyUID() , serie.getSeriesModality() );
        if ( !state.good() ) m_error = true;
               
        m_studyUID = image->getStudyUID().c_str();
    }
    
    //inserim la nova sèrie que es descarrega
    if ( !m_addedSeriesList.contains( image->getSeriesUID().c_str() ) )
    {
        if ( insertSerie( image ).good() )
        {
            emit( seriesRetrieved( image->getStudyUID().c_str() ) );
            
            if ( m_downloadedSeries == 0 )
            {
                emit( seriesView( image->getStudyUID().c_str() ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
            }
            m_downloadedSeries++;
        }
    }
    
    //inserim imatge
    state = cacheImageDAL.insertImage( image );
    //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error
    if ( !state.good() ) m_error = true;
   
    m_downloadedImages++;
    emit( imageRetrieved( image->getStudyUID().c_str(),m_downloadedImages ) );
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
        if ( !state.good())
        {
            m_error = true;
        }
        else m_addedSeriesList.push_back( serie.getSeriesUID().c_str() );
    }
    
    return state;
}

/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error, l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads, per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImageRetrieved::setError()
{
    std::string logMessage;
    m_error = true;
    logMessage = "Error descarregant l'estudi";
    ERROR_LOG( logMessage.c_str() );  
}

bool StarviewerProcessImageRetrieved::getError()
{
    std::string logMessage;
    
    if ( m_downloadedImages == 0)
    {
        logMessage = "Error s'han descarregat 0 imatges de l'estudi";
        ERROR_LOG( logMessage.c_str() );          
    }
    return m_error || m_downloadedImages == 0;
} 

Status StarviewerProcessImageRetrieved::getSeriesInformation( QString imagePath , Series &serie )
{
    Status state;
    QString path;
    std::string logMessage;
    char errorNumber[5];
    
    ImageDicomInformation dInfo;
    
    state = dInfo.openDicomFile( imagePath.toAscii().constData() );
    
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
    path = dInfo.getStudyUID().c_str();
    path.append( "/" );
    path.append( dInfo.getSeriesUID().c_str() );
    path.append( "/" );
    
    serie.setSeriesPath( path.toAscii().constData());
    
        
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error obtenint informació de la sèrie. Número d'error";
        logMessage.append( errorNumber );
        logMessage.append( " ERROR : " );
        logMessage.append( state.text() );
        ERROR_LOG( logMessage.c_str() );  
    }    
    return state; 
}

QString StarviewerProcessImageRetrieved::createImagePath( Image *image )
{
    StarviewerSettings settings;
    Series serie;
    QString imagePath;   
    
    imagePath.insert( 0 , settings.getCacheImagePath() );
    imagePath.append( image->getStudyUID().c_str() );
    imagePath.append( "/" );
    imagePath.append( image->getSeriesUID().c_str() );
    imagePath.append("/");
    imagePath.append( image->getImageName().c_str() );

    return imagePath;
}

StarviewerProcessImageRetrieved::~StarviewerProcessImageRetrieved()
{
    //com no sabem quant s'acaba la descàrrega de l'última sèrie, fem que s'indiqui que ha finalitzat la seva descàrrega quan es destrueix l'objecte StarviewerProcessImageRetrieved, que és destruït just finalitzar la descarrega de tot l'estudi
	emit( seriesRetrieved( m_studyUID ) );

    // si les series està a 0 vol dir que l'estudi només tenia una sèrie, per tant si l'usuari ha demanat visualitzar-lo no s'haurà emés el signal seriesView, perquè no sabrem que ha finalitzat la descarrega de la sèrie, fins que es destrueixi l'objecte StarviewerProcessImageRetrieved, el qual no es destrueix just quan finalitza la descàrrega de l'estudi
    if ( m_downloadedSeries == 0 ) 
    {
    	emit( seriesView( m_studyUID ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
    }
}

};

