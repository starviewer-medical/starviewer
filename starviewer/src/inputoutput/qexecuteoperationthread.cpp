/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QSemaphore>
#include "qexecuteoperationthread.h"
#include "studymask.h"
#include "pacsserver.h"
#include "retrieveimages.h"
#include "operation.h"
#include "queueoperationlist.h"
#include "processimagesingleton.h"
#include "starviewerprocessimage.h"
#include "cachepacs.h"
#include "harddiskinformation.h"
#include "scalestudy.h"
#include "cachepool.h"
#include "starviewersettings.h"
#include "const.h"
#include "cachelayer.h"
#include "logging.h"
#include "status.h"

namespace udg {

QSemaphore m_semaphor(1);//controlar l'acces a la variable m_stop

//constructor
QExecuteOperationThread::QExecuteOperationThread(QObject *parent)
 : QThread(parent)
{
     m_stop = true;
}

void QExecuteOperationThread::queueOperation(Operation operation)
{
    QueueOperationList *queueOperationList = QueueOperationList::getQueueOperationList();
    
    
    m_semaphor.acquire();
    
    //la variable m_stop controla si el thread està engegat o parat!
    queueOperationList->insertOperation(operation); 
    if(m_stop = true)
    {   //si parat l'engeguem
        m_stop = false;
        start();
    }
    
    m_semaphor.release();
}

//s'executa des del thread
void QExecuteOperationThread::run()
{
	INFO_LOG("Iniciant thread que executa operacions");
	
    QueueOperationList *queueOperationList = QueueOperationList::getQueueOperationList();
    
    //creem les connexions amb l'objecte QRetrieveScreen, per indicar descarregues
    
    while (!m_stop)
    {
        Operation operation;
        
        operation = queueOperationList->getMaximumPriorityOperation();
        
        switch (operation.getOperation())
        {
            case operationRetrieve : 
                 m_view = false;
                 retrieveStudy(operation);
                 break;
            case operationView : 
                 m_view = true;
                 retrieveStudy(operation);
                 break;
        }
        
        //comprovem si hem de parar
        m_semaphor.acquire();
        m_stop = queueOperationList->isEmpty();
        m_semaphor.release();
    } 
	INFO_LOG("Finalitzant thread que executa operacions");
}

//descarrega un estudi
void QExecuteOperationThread::retrieveStudy(Operation operation)
{
    StarviewerProcessImage *sProcessImg = new StarviewerProcessImage::StarviewerProcessImage();
    QString studyUID;
    Status state,retState;
	QString logMessage;

	logMessage = "Iniciant la descàrrega de l'estudi ";
	logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
	logMessage.append( "del pacs " );
	logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
	
	INFO_LOG( logMessage.toAscii().constData() );

    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    CachePacs *localCache =  CachePacs::getCachePacs();
    ScaleStudy scaleStudy;
    RetrieveImages retrieve;
    bool enoughSpace , errorRetrieving ;
    
    studyUID = operation.getStudyMask().getStudyUID().c_str();
        
    state = enoughFreeSpace( enoughSpace );    
        
    //s'indica que comença la descarreca de l'estudi al qretrieveScreen
    emit( setStudyRetrieving( studyUID.toAscii().constData() ) );   
   
    if ( !state.good() || !enoughSpace ) 
    {
		logMessage = "La descàrrega de l'estudi ";
		logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
		logMessage.append( "del pacs " );
		logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
	
        emit( setErrorRetrieving( studyUID.toAscii().constData() ) );
        
        if ( !enoughSpace ) //si no hi ha prou espai emitim aquest signal
        {
			logMessage.append (" al no haver suficient espai lliure al disc" );
            emit( notEnoughFreeSpace() );
        }
        else 
		{
			emit ( errorFreeingCacheSpace() ); //si s'ha produit algun error alliberant espai emitim aquest signal
			logMessage.append( " al intentar alliberar espai al disc " );
		}
		ERROR_LOG( logMessage.toAscii().constData() );
        
        localCache->delStudy( studyUID.toAscii().constData());
        return;
    }
    
    PacsServer pacsConnection(operation.getPacsParameters());//connemtem al pacs
    state = pacsConnection.connect(PacsServer::retrieveImages,PacsServer::studyLevel);    
    if (!state.good())
    {   
		logMessage = "Error al connectar al pacs ";
		logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
		logMessage.append( ". PACS ERROR : ");
        logMessage.append( state.text().c_str() );	
		ERROR_LOG ( logMessage.toAscii().constData() );

        emit( setErrorRetrieving( studyUID.toAscii().constData() ) );
		emit( errorConnectingPacs( operation.getPacsParameters().getPacsID() ) ); 
        localCache->delStudy( studyUID.toAscii().constData()) ;        
        return; 
    }
    
    //passem els parametres a la classe retrieveImages
    retrieve.setConnection( pacsConnection.getConnection() );  
    retrieve.setMask( operation.getStudyMask() ); 
    retrieve.setNetwork( pacsConnection.getNetwork() );
    
    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges descarregades
    piSingleton->addNewProcessImage( studyUID.toAscii().constData(),sProcessImg) ;
   
    //connectem els signals del starviewerProcessImage
    connect( sProcessImg , SIGNAL( seriesView( QString ) ) , this , SLOT( firstSeriesRetrieved( QString ) ) );
    connect( sProcessImg , SIGNAL( imageRetrieved( QString , int ) ) , this , SLOT( imageRetrievedSlot( QString , int ) ) );
    connect( sProcessImg , SIGNAL( seriesRetrieved( QString ) ) , this , SLOT( seriesRetrievedSlot( QString ) ) );

    retState = retrieve.moveSCU();
    pacsConnection.disconnect();

	errorRetrieving = sProcessImg->getErrorRetrieving();
    //esborrem el processImage de la llista de processImage encarregat de processar la informació per cada imatge descarregada
    piSingleton->delProcessImage( studyUID.toAscii().constData() );
	delete sProcessImg;    

    if (!retState.good() || errorRetrieving )
    {//si s'ha produit algun error ho indiquem i esborrem l'estudi 
		logMessage = "S'ha produit algun error durant la descàrrega de l'estudi ";
		logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
		logMessage.append( " del pacs " );
		logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
        logMessage.append( ". PACS ERROR : " );
        logMessage.append( retState.text().c_str() );
		ERROR_LOG ( logMessage.toAscii().constData() );

        emit( setErrorRetrieving( studyUID.toAscii().constData() ) );
        localCache->delStudy( studyUID.toAscii().constData() );
    }
    else 
    {    
		logMessage = "Ha finalitzat la descàrrega de l'estudi ";
		logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
		logMessage.append( "del pacs " );
		logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
		INFO_LOG( logMessage.toAscii().constData() );
        scaleStudy.scale( studyUID.toAscii().constData() ); //escalem l'estudi per la previsualització de la caché  
        emit( setStudyRetrieved( studyUID.toAscii().constData() ) );// descarregat
        localCache->setStudyRetrieved( studyUID.toAscii().constData() ); //posem l'estudi com a descarregat
    }
    
}

void QExecuteOperationThread::firstSeriesRetrieved( QString studyUID )
{
     if ( m_view ) emit( viewStudy( studyUID ) ); //signal cap a QueryScreen
}

void QExecuteOperationThread::imageRetrievedSlot( QString studyUID , int imageNumber)
{
    emit( imageRetrieved( studyUID , imageNumber ) ) ;
}

void QExecuteOperationThread::seriesRetrievedSlot( QString studyUID)
{
    emit( seriesRetrieved( studyUID ));
}

Status QExecuteOperationThread::enoughFreeSpace( bool &enoughSpace)
{
    HardDiskInformation hardDiskInformation;
    CachePool pool;
    StarviewerSettings settings;
    unsigned int freePoolSpace, freeSystemSpace;
    Status state;
    CacheLayer cacheLayer;
	QString logMessage,stringMb;    

    freeSystemSpace = hardDiskInformation.getNumberOfFreeMBytes( settings.getCacheImagePath() );
    if ( freeSystemSpace == 0 ) 
    {
        enoughSpace = false;
        return state.setStatus( ERROR );
    }
    pool.getPoolFreeSpace( freePoolSpace );  
    
    //si no hi ha suficient espai lliure a la cache o al disc dur intera esborrar dos Gb
    if ( freeSystemSpace <= CachePool::MinimumMBytesOfDiskSpaceRequired || 
         freePoolSpace <= CachePool::MinimumMBytesOfDiskSpaceRequired )
    {
		logMessage = "No hi ha suficient espai a la cache. Alliberant espai. Espai lliure cache ";
		stringMb.setNum( freePoolSpace , 10 );
		logMessage.append( stringMb );
		logMessage.append(" Mb. Espai lliure disc ");		
		stringMb.truncate(0);
		stringMb.setNum( freeSystemSpace , 10 );
		logMessage.append(" Mb");

        state = cacheLayer.deleteOldStudies( CachePool::MBytesToEraseWhenDiskOrCacheFull ); //esborrem els estudis 
        
        if ( !state.good() )
        {
            enoughSpace = false;
            return state;
        }
        
        freeSystemSpace = hardDiskInformation.getNumberOfFreeMBytes( settings.getCacheImagePath() );
        if ( freeSystemSpace == 0 ) return state.setStatus( ERROR );
        
        pool.getPoolFreeSpace( freePoolSpace );  
        
        //hem intentat esborrar pero no hi ha hagut suficient espai
        if ( freeSystemSpace <=  CachePool::MinimumMBytesOfDiskSpaceRequired || 
             freePoolSpace <=  CachePool::MinimumMBytesOfDiskSpaceRequired )
        {
            //si no hi ha suficient espai,significa que no hi ha prou espai al disc, perque de la cache sempre podem alliberar espai
            enoughSpace = false;
			INFO_LOG( "No hi ha suficient espai lliure al disc dur" );
        }
        else enoughSpace = true;
    }
    else enoughSpace = true;
    
    return state.setStatus( CORRECT );
}

QExecuteOperationThread::~QExecuteOperationThread()
{
}


}
