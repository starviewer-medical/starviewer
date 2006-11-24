/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <string.h>

#include "starviewerprocessimage.h"
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

StarviewerProcessImage::StarviewerProcessImage() : ProcessImage()
{
    m_downloadedImages = 0;
    m_error = false;
    m_downloadedSeries = 0;
}

void StarviewerProcessImage::process( Image *image )
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheSeriesDAL cacheSeriesDAL;
    CacheImageDAL cacheImageDAL;

    /*si es la primera imatge que es descarrega fem un signal indicant que comença la descarrega, inserim la primera
    serie, i ara que tenim la informació de la sèrie update la modalitat de l'estudi*/
    if ( m_downloadedImages == 0 )
    {  
        Series serie;
        
        //canviem l'estat de l'estudi de PENDING A RETRIEVING
        state = cacheStudyDAL.setStudyRetrieving( image->getStudyUID().c_str() );
        if ( !state.good() ) m_error = true;
        
        //inserim serie
        state = getSeriesInformation ( createImagePath( image  ), serie );
        
        if ( state.good() )
        {
            state = cacheSeriesDAL.insertSeries( &serie );
        }
        //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error
        if ( !state.good() ) m_error = true;
        
        state = cacheStudyDAL.setStudyModality( serie.getStudyUID() , serie.getSeriesModality() );
        
        //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error
        if ( !state.good() ) 
        {
            m_error = true;
        }
        else m_oldSeriesUID = image->getSeriesUID().c_str();
        
        m_studyUID = image->getStudyUID().c_str();
    }
    
    //inserim la nova sèrie que es descarrega
    if ( m_oldSeriesUID != image->getSeriesUID().c_str() )
    {
        Series serie;
    
        //inserim serie
        state = getSeriesInformation ( createImagePath( image ) , serie );
        
        if ( state.good() )
        {
            state = cacheSeriesDAL.insertSeries( &serie );
        }
        //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error
        //l'error 2019 no el tractem, perquè indica serie duplicada, alguns pacs envien les imatges desornades per tant pot ser que les imatges d'una mateixa serie arribin separades, i intentem inserir altre vegada la serie
        if ( !state.good() && state.code() != 2019 )
        {
            m_error = true;
        }
        //Nomes emitirem el signal quant la operacio s'hagi realitzat amb exit, pq hi ha alguns PACS que retornen les
        //imatges desordenades, i no venen agrupades per sèries
        else  
        {
            emit( seriesRetrieved( image->getStudyUID().c_str() ) );
            
            if ( m_downloadedSeries == 0 )
            {
                emit( seriesView( image->getStudyUID().c_str() ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
            }
            m_downloadedSeries++;
        }
        m_oldSeriesUID = image->getSeriesUID().c_str();
    }
    
    //inserim imatge
    state = cacheImageDAL.insertImage( image );
    //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error
    if ( !state.good() ) m_error = true;
   
    m_downloadedImages++;
    emit( imageRetrieved( image->getStudyUID().c_str(),m_downloadedImages ) );
}

/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error, l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads, per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImage::setError()
{
    std::string logMessage;
    m_error = true;
    logMessage = "Error descarregant l'estudi";
    ERROR_LOG( logMessage.c_str() );  
}

bool StarviewerProcessImage::getError()
{
    std::string logMessage;
    
    if ( m_downloadedImages == 0)
    {
        logMessage = "Error s'han descarregat 0 imatges de l'estudi";
        ERROR_LOG( logMessage.c_str() );          
    }
    return m_error || m_downloadedImages == 0;
} 

Status StarviewerProcessImage::getSeriesInformation( QString imagePath , Series &serie )
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

QString StarviewerProcessImage::createImagePath( Image *image )
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

StarviewerProcessImage::~StarviewerProcessImage()
{
    //com no sabem quant s'acaba la descàrrega de l'última sèrie, fem que s'indiqui que ha finalitzat la seva descàrrega quan es destrueix l'objecte StarViewerProcessImage, que és destruït just finalitzar la descarrega de tot l'estudi
	emit( seriesRetrieved( m_studyUID ) );

    // si les series està a 0 vol dir que l'estudi només tenia una sèrie, per tant si l'usuari ha demanat visualitzar-lo no s'haurà emés el signal seriesView, perquè no sabrem que ha finalitzat la descarrega de la sèrie, fins que es destrueixi l'objecte StarviewerProcessImage, el qual no es destrueix just quan finalitza la descàrrega de l'estudi
    if ( m_downloadedSeries == 0 ) 
    {
    	emit( seriesView( m_studyUID ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
    }
}

};

