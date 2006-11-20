/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QSemaphore>
#include "qexecuteoperationthread.h"
#include "studymask.h"
#include "seriesmask.h"
#include "imagemask.h"
#include "pacsserver.h"
#include "retrieveimages.h"
#include "operation.h"
#include "queueoperationlist.h"
#include "processimagesingleton.h"
#include "starviewerprocessimage.h"
#include "harddiskinformation.h"
#include "scalestudy.h"
#include "cachepool.h"
#include "starviewersettings.h"
#include "const.h"
#include "cachelayer.h"
#include "logging.h"
#include "status.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "storeimages.h"
#include "serieslist.h"
#include "imagelist.h"

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
    
    emit( newOperation( &operation ) );//emitim una senyal per a que la qretrieveScreen sapiga que s'ha demanat una nova operació, i la mostri per pantalla
    
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
            case operationMove:
                 moveStudy( operation );
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
    CacheStudyDAL cacheStudyDAL;

    logMessage = "Iniciant la descàrrega de l'estudi ";
    logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
    logMessage.append( "del pacs " );
    logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
	
    INFO_LOG( logMessage.toAscii().constData() );

    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    ScaleStudy scaleStudy;
    RetrieveImages retrieve;
    bool enoughSpace , errorRetrieving ;
    
    studyUID = operation.getStudyMask().getStudyUID().c_str();
        
    state = enoughFreeSpace( enoughSpace );    
        
    //s'indica que comença la descarreca de l'estudi al qretrieveScreen
    emit( setOperating( studyUID.toAscii().constData() ) );   
   
    if ( !state.good() || !enoughSpace ) 
    {
        logMessage = "La descàrrega de l'estudi ";
        logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
        logMessage.append( "del pacs " );
        logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
	
        emit( setErrorOperation( studyUID.toAscii().constData() ) );
        
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
        
        cacheStudyDAL.delStudy( studyUID.toAscii().constData());
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

        emit( setErrorOperation( studyUID.toAscii().constData() ) );
        emit( errorConnectingPacs( operation.getPacsParameters().getPacsID() ) ); 
        cacheStudyDAL.delStudy( studyUID.toAscii().constData()) ;        
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
    connect( sProcessImg , SIGNAL( imageRetrieved( QString , int ) ) , this , SLOT( imageCommitSlot( QString , int ) ) );
    connect( sProcessImg , SIGNAL( seriesRetrieved( QString ) ) , this , SLOT( seriesCommitSlot( QString ) ) );

    retState = retrieve.moveSCU();
    pacsConnection.disconnect();

    errorRetrieving = sProcessImg->getErrorRetrieving();
    //esborrem el processImage de la llista de processImage encarregat de processar la informació per cada imatge descarregada
    piSingleton->delProcessImage( studyUID.toAscii().constData() );
    delete sProcessImg;    

    if (!retState.good() || errorRetrieving )
    {//si s'ha produit algun error ho indiquem i esborrem l'estudi 
        if ( !retState.good() )
        {        
            logMessage = "S'ha produit algun error durant la descàrrega de l'estudi ";
            logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
            logMessage.append( " del pacs " );
            logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
            logMessage.append( ". PACS ERROR : " );
            logMessage.append( retState.text().c_str() );
            ERROR_LOG ( logMessage.toAscii().constData() );
        }
        
        if ( errorRetrieving )
        {
            logMessage = "S'ha produit algun error durant el processat de les imatges descarregades ( Classe StarviewerProcessImage) per l'estudi ";
            logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
            logMessage.append( " del pacs " );
            logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
            ERROR_LOG ( logMessage.toAscii().constData() );
        }

        emit( setErrorOperation( studyUID.toAscii().constData() ) );
        cacheStudyDAL.delStudy( studyUID.toAscii().constData() );
    }
    else 
    {    
        logMessage = "Ha finalitzat la descàrrega de l'estudi ";
        logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
        logMessage.append( "del pacs " );
        logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
        INFO_LOG( logMessage.toAscii().constData() );
        scaleStudy.scale( studyUID.toAscii().constData() ); //escalem l'estudi per la previsualització de la caché  
        emit( setOperationFinished( studyUID.toAscii().constData() ) );// descarregat
        cacheStudyDAL.setStudyRetrieved( studyUID.toAscii().constData() ); //posem l'estudi com a descarregat
    }
    
}

void QExecuteOperationThread::firstSeriesRetrieved( QString studyUID )
{
     if ( m_view ) emit( viewStudy( studyUID ) ); //signal cap a QueryScreen
}

void QExecuteOperationThread::imageCommitSlot( QString studyUID , int imageNumber)
{
    emit( imageCommit( studyUID , imageNumber ) ) ;
}

void QExecuteOperationThread::seriesCommitSlot( QString studyUID)
{
    emit( seriesCommit( studyUID ) );
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

Status QExecuteOperationThread::moveStudy( Operation operation )
{
    QList<std::string> imagePathList;
    Status state;
    PacsParameters pacs;
    StarviewerSettings settings;
    StoreImages storeImages;
    QString logMessage , errorNumber;

    logMessage = "Preparant les dades per moure estudi ";
    logMessage.append( operation.getStudyMask().getStudyUID().c_str() );
    logMessage.append( " al PACS " );
    logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
    INFO_LOG( logMessage.toAscii().constData() );
   
    state = imagesPathToStore( operation.getStudyMask().getStudyUID().c_str() , imagePathList );
    
    if ( !state.good() ) return state;
      
    PacsServer pacsConnection( operation.getPacsParameters() );
     
    state = pacsConnection.connect( PacsServer::storeImages , PacsServer::any );
    
    if ( !state.good() )
    {
        logMessage.append(" S'ha produït un error al intentar connectar al PACS ");
        logMessage.append( operation.getPacsParameters().getAEPacs().c_str() );
        logMessage.append( ". PACS ERROR : " );
        logMessage.append( state.text().c_str() );
        emit( errorConnectingPacs( operation.getPacsParameters().getPacsID() ) ); 
        emit( setErrorOperation( operation.getStudyMask().getStudyUID().c_str() ) );
        return state;
    }
    
    storeImages.setConnection( pacsConnection.getConnection() );
    storeImages.setNetwork( pacsConnection.getNetwork() );
    state = storeImages.store( imagePathList.toStdList() );

    if ( state.good() )
    {
        INFO_LOG("S'ha mogut l'estudi correctament" );
        emit( setOperationFinished( operation.getStudyMask().getStudyUID().c_str() ) );// descarregat
    }
    else emit( setErrorOperation( operation.getStudyMask().getStudyUID().c_str() ) );
    
    cout<<state.text()<<endl;
    return state;

}

Status QExecuteOperationThread::imagesPathToStore( QString studyUID , QList<std::string> &imagePathList )
{
    CacheSeriesDAL cacheSeriesDAL;
    CacheImageDAL cacheImageDAL;
    SeriesMask mask;
    SeriesList seriesList;
    Series series;
    ImageMask imageMask;
    ImageList imageList;
    Image image;
    Status state;
    QString logMessage, errorNumber;
    
    mask.setStudyUID( studyUID.toAscii().constData() );
    state = cacheSeriesDAL.querySeries( mask ,seriesList );
    
    if ( !state.good() )
    {
        logMessage = "S'ha produït un error al cercar les sèries de l'estudi que s'ha de moure Error : ";
        errorNumber.setNum( state.code() , 10 );
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.toAscii().constData() ); 
                
        return state;
    }
        
    seriesList.firstSeries();
    
    while ( !seriesList.end() )
    {
        series = seriesList.getSeries();
        
        imageMask.setSeriesUID(series.getSeriesUID().c_str() );
        imageMask.setStudyUID( studyUID.toAscii().constData() );
        
        imageList.clear();
        state = cacheImageDAL.queryImages(imageMask , imageList);

        if ( !state.good() )
        {
            logMessage = "S'ha produït un error al cercar les imatges de l'estudi que s'ha de moure Error : ";
            errorNumber.setNum( state.code() , 10 );
            logMessage.append( errorNumber );
            ERROR_LOG( logMessage.toAscii().constData() ); 
            
            return state;
        }
        
        imageList.firstImage();
        while ( !imageList.end() )
        {
            image = imageList.getImage();
            imagePathList.push_back( image.getImagePath() );
            imageList.nextImage();
        }        
        seriesList.nextSeries();
    }
    
    return state;

}

QExecuteOperationThread::~QExecuteOperationThread()
{

}

}
