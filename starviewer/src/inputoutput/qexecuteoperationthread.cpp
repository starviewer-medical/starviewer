/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QSemaphore>
#include <QMessageBox>
#include "qexecuteoperationthread.h"
#include "pacsserver.h"
#include "retrieveimages.h"
#include "operation.h"
#include "queueoperationlist.h"
#include "processimagesingleton.h"
#include "starviewerprocessimageretrieved.h"
#include "starviewerprocessimagestored.h"
#include "harddiskinformation.h"
#include "scalestudy.h"
#include "cachepool.h"
#include "starviewersettings.h"
#include "errordcmtk.h"
#include "cachelayer.h"
#include "logging.h"
#include "status.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "storeimages.h"
#include "serieslist.h"
#include "imagelist.h"
#include "querypacs.h"
#include "pacsconnection.h"

namespace udg {

QSemaphore m_semaphor(1);//controlar l'acces a la variable m_stop

//constructor
QExecuteOperationThread::QExecuteOperationThread(QObject *parent)
 : QThread(parent)
{
     m_stop = true;
}

QExecuteOperationThread::~QExecuteOperationThread()
{
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
            case Operation::Retrieve :
                 m_view = false;
                 retrieveStudy(operation);
                 break;
            case Operation::View :
                 m_view = true;
                 retrieveStudy(operation);
                 break;
            case Operation::Move :
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
    StarviewerProcessImageRetrieved *sProcessImg = new StarviewerProcessImageRetrieved();
    QString studyUID = operation.getStudyUID();
    Status state,retState;
    CacheStudyDAL cacheStudyDAL;

    INFO_LOG( QString("Iniciant la descàrrega de l'estudi %1 del pacs %2").arg( studyUID ).arg( operation.getPacsParameters().getAEPacs() ) );

    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    ScaleStudy scaleStudy;
    RetrieveImages retrieveImages;
    bool enoughSpace , errorRetrieving ;

    state = enoughFreeSpace( enoughSpace );

    //s'indica que comença la descarrega de l'estudi al qOperationStateScreen
    emit( setOperating( studyUID ) );

    if ( !state.good() || !enoughSpace )
    {
        QString logMessage = "La descàrrega de l'estudi " + studyUID + "del pacs " + operation.getPacsParameters().getAEPacs();

        emit( setErrorOperation( studyUID ) );

        if ( !enoughSpace ) //si no hi ha prou espai emitim aquest signal
        {
            logMessage += " al no haver suficient espai lliure al disc";
            QMessageBox::warning( 0 , tr( "Starviewer" ) , tr( "Not enough space to retrieve studies. Please free space" ) );
        }
        else
        {
            logMessage += " al intentar alliberar espai al disc ";
            QMessageBox::critical( 0 , tr( "Starviewer" ) , tr( "Error freeing space. The study couldn't be retrieved" ) );
        }
        ERROR_LOG( logMessage );

        cacheStudyDAL.delStudy( studyUID);
        return;
    }

    PacsServer pacsConnection(operation.getPacsParameters());//connemtem al pacs
    state = pacsConnection.connect(PacsServer::retrieveImages,PacsServer::studyLevel);
    if (!state.good())
    {
        ERROR_LOG( "Error al connectar al pacs " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " + state.text() );

        emit( setErrorOperation( studyUID ) );
        emit( errorConnectingPacs( operation.getPacsParameters().getPacsID() ) );
        cacheStudyDAL.delStudy( studyUID) ;
        return;
    }

    //passem els parametres a la classe retrieveImages
    retrieveImages.setConnection( pacsConnection.getConnection() );
    retrieveImages.setMask( operation.getDicomMask() );
    retrieveImages.setNetwork( pacsConnection.getNetwork() );

    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges descarregades
    piSingleton->addNewProcessImage( studyUID,sProcessImg) ;

    connect( sProcessImg , SIGNAL( imageRetrieved( QString , int ) ) , this , SLOT( imageCommitSlot( QString , int ) ) );
    connect( sProcessImg , SIGNAL( seriesRetrieved( QString ) ) , this , SLOT( seriesCommitSlot( QString ) ) );

    retState = retrieveImages.retrieve();

    pacsConnection.disconnect();

    errorRetrieving = sProcessImg->getError();

    if (!retState.good() || errorRetrieving )
    {//si s'ha produit algun error ho indiquem i esborrem l'estudi
        if ( !retState.good() )
        {
            ERROR_LOG( "S'ha produit algun error durant la descàrrega de l'estudi " + studyUID + " del pacs " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " +retState.text() );
        }

        if ( errorRetrieving )
        {
            ERROR_LOG( "S'ha produit algun error durant el processat de les imatges descarregades ( Classe StarviewerProcessImage) per l'estudi " + studyUID + " del pacs " + operation.getPacsParameters().getAEPacs() );
        }

        emit( setErrorOperation( studyUID ) );
        cacheStudyDAL.delStudy( studyUID );
    }
    else
    {
        cacheStudyDAL.setStudyRetrieved( studyUID ); //posem l'estudi com a   descarregat
        INFO_LOG( "Ha finalitzat la descàrrega de l'estudi " + studyUID + "del pacs " + operation.getPacsParameters().getAEPacs() );
        scaleStudy.scale( studyUID ); //escalem l'estudi per la previsualització de la caché
        emit( setOperationFinished( studyUID ) );// descarregat a QOperationStateScreen
        emit( setRetrieveFinished( studyUID ) );//la queryscreen l'afageix a la llista QStudyTreeView d'estudis de la cache

        if ( m_view )
            emit ( viewStudy( operation.getDicomMask().getStudyUID(), operation.getDicomMask().getSeriesUID(), operation.getDicomMask().getSOPInstanceUID() ) );
    }

    //esborrem el processImage de la llista de processImage encarregat de processar la informació per cada imatge descarregada
    piSingleton->delProcessImage( studyUID );
    delete sProcessImg; // el delete és necessari perquè al fer el delete storedProcessImage envia al signal de que l'última sèrie ha estat descarregada
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
    unsigned int freePoolSpace;
    quint64 freeSystemSpace;
    Status state;
    CacheLayer cacheLayer;

    freeSystemSpace = hardDiskInformation.getNumberOfFreeMBytes( settings.getCacheImagePath() );
    if ( freeSystemSpace == 0 )
    {
        enoughSpace = false;
        return state.setStatus( DcmtkUnknowError );
    }
    pool.getPoolFreeSpace( freePoolSpace );

    //si no hi ha suficient espai lliure a la cache o al disc dur intera esborrar dos Gb
    if ( freeSystemSpace <= CachePool::MinimumMBytesOfDiskSpaceRequired ||
         freePoolSpace <= CachePool::MinimumMBytesOfDiskSpaceRequired )
    {
        ERROR_LOG( QString("No hi ha suficient espai a la cache. Alliberant espai. Espai lliure cache %1 Mb. Espai lliure al disc %2 Mb").arg(freePoolSpace).arg(freeSystemSpace) );

        state = cacheLayer.deleteOldStudies( CachePool::MBytesToEraseWhenDiskOrCacheFull ); //esborrem els estudis

        if ( !state.good() )
        {
            enoughSpace = false;
            return state;
        }

        freeSystemSpace = hardDiskInformation.getNumberOfFreeMBytes( settings.getCacheImagePath() );
        if ( freeSystemSpace == 0 ) return state.setStatus( DcmtkUnknowError );

        pool.getPoolFreeSpace( freePoolSpace );

        //hem intentat esborrar pero no hi ha hagut suficient espai
        if ( freeSystemSpace <=  CachePool::MinimumMBytesOfDiskSpaceRequired ||
             freePoolSpace <=  CachePool::MinimumMBytesOfDiskSpaceRequired )
        {
            //si no hi ha suficient espai,significa que no hi ha prou espai al disc, perque de la cache sempre podem alliberar espai
            enoughSpace = false;
            INFO_LOG( "No hi ha suficient espai lliure al disc dur" );
        }
        else
            enoughSpace = true;
    }
    else
        enoughSpace = true;

    return state.setStatus( DcmtkNoError );
}

Status QExecuteOperationThread::moveStudy( Operation operation )
{
    ImageList imageList;
    Status state;
    PacsParameters pacs;
    StarviewerSettings settings;
    StoreImages storeImages;
    StarviewerProcessImageStored *storedProcessImage = new StarviewerProcessImageStored();
    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();

    INFO_LOG( "Preparant les dades per moure estudi " + operation.getStudyUID() + " al PACS " + operation.getPacsParameters().getAEPacs() );

    emit( setOperating( operation.getStudyUID() ) );//Indiquem al QOperationState que comença l'enviament de les imatges

    //cercquem el path de les imatges a emmagatzemar
    state = imagesPathToStore( operation.getStudyUID() , imageList );

    if ( !state.good() ) return state;

    PacsServer pacsConnection( operation.getPacsParameters() );

    state = pacsConnection.connect( PacsServer::storeImages , PacsServer::any );

    if ( !state.good() )
    {
        ERROR_LOG( " S'ha produït un error al intentar connectar al PACS " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " + state.text() );
        emit( errorConnectingPacs( operation.getPacsParameters().getPacsID() ) );
        emit( setErrorOperation( operation.getStudyUID() ) );
        return state;
    }

    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges guardades
    piSingleton->addNewProcessImage( operation.getStudyUID() , storedProcessImage ) ;

    connect( storedProcessImage , SIGNAL( imageStored( QString , int ) ) , this , SLOT( imageCommitSlot( QString , int ) ) );
    connect( storedProcessImage , SIGNAL( seriesStored( QString ) ) , this , SLOT( seriesCommitSlot( QString ) ) );

    storeImages.setConnection( pacsConnection.getConnection() );
    storeImages.setNetwork( pacsConnection.getNetwork() );
    state = storeImages.store( imageList );

    piSingleton->delProcessImage( operation.getStudyUID() );
    delete storedProcessImage; // el delete és necessari perquè al fer el delete storedProcessImage envia al signal de que l'última sèrie ha estat descarregada

    if ( state.good() )
    {
        INFO_LOG("S'ha mogut l'estudi correctament" );
        emit( setOperationFinished( operation.getStudyUID() ) );// descarregat
    }
    else
    {
        emit( setErrorOperation( operation.getStudyUID() ) );
        ERROR_LOG( "S'ha produit un error intentant guardar l'estudi : " + state.text() );
    }

    return state;
}

Status QExecuteOperationThread::imagesPathToStore( QString studyUID , ImageList &imageList )
{
    CacheSeriesDAL cacheSeriesDAL;
    CacheImageDAL cacheImageDAL;
    DicomMask mask;
    SeriesList seriesList;
    DICOMSeries series;
    ImageList imageListSeries;
    DICOMImage image;
    Status state;

    mask.setStudyUID( studyUID );
    state = cacheSeriesDAL.querySeries( mask ,seriesList );

    if ( !state.good() )
    {
        ERROR_LOG( QString("S'ha produït un error al cercar les sèries de l'estudi que s'ha de moure Error : %1 ").arg(state.code()) );

        return state;
    }

    seriesList.firstSeries();

    while ( !seriesList.end() )
    {
        series = seriesList.getSeries();

        mask.setSeriesUID( series.getSeriesUID() );

        imageList.clear();
        state = cacheImageDAL.queryImages( mask , imageListSeries );

        if ( !state.good() )
        {
            ERROR_LOG( QString("S'ha produït un error al cercar les imatges de l'estudi que s'ha de moure Error : %1").arg(state.code()) );

            return state;
        }

        imageListSeries.firstImage();
        while ( !imageListSeries.end() )
        {
            imageList.insert( imageListSeries.getImage() );
            imageListSeries.nextImage();
        }
        seriesList.nextSeries();
    }

    return state;

}

}
