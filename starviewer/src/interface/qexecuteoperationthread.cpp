/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qexecuteoperationthread.h"
#include "studymask.h"
#include "pacsserver.h"
#include "retrieveimages.h"
#include "operation.h"
#include "queueoperationlist.h"
#include "processimagesingleton.h"
#include "starviewerprocessimage.h"
#include "cachepacs.h"
#include "scalestudy.h"
#include "status.h"
#include <QSemaphore>


namespace udg {

QSemaphore m_semaphor(1);//controlar l'acces a la variable m_stop

//constructor
QExecuteOperationThread::QExecuteOperationThread(QObject *parent)
 : QThread(parent)
{
     m_qretrieveScreen  = QRetrieveScreen::getQRetrieveScreen();
     m_stop = true;
}

void QExecuteOperationThread::createConnections()
{
    connect(this , SIGNAL( setErrorRetrieving( QString ) ) , m_qretrieveScreen, SLOT( setErrorRetrieving( QString ) ), Qt::QueuedConnection);
    connect(this , SIGNAL( setStudyRetrieved( QString ) ) , m_qretrieveScreen, SLOT( setRetrievedFinished( QString ) ), Qt::QueuedConnection); 
    connect(this , SIGNAL( setStudyRetrieving( QString ) ) , m_qretrieveScreen, SLOT( setRetrieving( QString ) ), Qt::QueuedConnection);
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
    QueueOperationList *queueOperationList = QueueOperationList::getQueueOperationList();
    
    //creem les connexions amb l'objecte QRetrieveScreen, per indicar descarregues
    createConnections();
    
    while (!m_stop)
    {
        Operation operation;
        
        operation = queueOperationList->getMaximumPriorityOperation();
        
        switch (operation.getOperation())
        {
            case operationRetrieve : 
                 retrieveStudy(operation,false);
                 break;
            case operationView : 
                 retrieveStudy(operation,true);
                 break;
        }
        
        //comprovem si hem de parar
        m_semaphor.acquire();
        m_stop = queueOperationList->isEmpty();
        m_semaphor.release();
    } 
}

//descarrega un estudi
void QExecuteOperationThread::retrieveStudy(Operation operation,bool view)
{
    StarviewerProcessImage *sProcessImg = new StarviewerProcessImage::StarviewerProcessImage();
    QString studyUID;
    Status state,retState;

    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    CachePacs *localCache =  CachePacs::getCachePacs();
    ScaleStudy scaleStudy;
    RetrieveImages retrieve;
    
    studyUID = operation.getStudyMask().getStudyUID().c_str();
        
    //s'indica que comença la descarreca de l'estudi al qretrieveScreen
    emit( setStudyRetrieving( studyUID.toAscii().constData() ) );
   
   
    PacsServer pacsConnection(operation.getPacsParameters());//connemtem al pacs
    state = pacsConnection.Connect(PacsServer::retrieveImages,PacsServer::studyLevel);    
    if (!state.good())
    {   
        emit( setErrorRetrieving( studyUID.toAscii().constData() ) );
        localCache->delStudy( studyUID.toAscii().constData()) ;        
    }
    
    //passem els parametres a la classe retrieveImages
    retrieve.setConnection( pacsConnection.getConnection() );  
    retrieve.setMask( operation.getStudyMask() ); 
    retrieve.setNetwork( pacsConnection.getNetwork() );
    
    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges descarregades
    piSingleton->addNewProcessImage( studyUID.toAscii().constData(),sProcessImg) ;
    //indiquem al qretrievescreen de quina instancia ha d'esperar el signal per controlar la descarrega   
    m_qretrieveScreen->setConnectSignal( sProcessImg ); 
   
    if ( view )
    {
        connect( sProcessImg , SIGNAL( seriesView( QString ) ) , this , SLOT( seriesRetrieved( QString ) ) );
    }
   
    retState = retrieve.moveSCU();
    if (!retState.good() || sProcessImg->getErrorRetrieving() )
    {//si s'ha produit algun error ho indiquem i esborrem l'estudi 
         emit( setErrorRetrieving( studyUID.toAscii().constData() ) );
        localCache->delStudy( studyUID.toAscii().constData() );
    }
    else 
    {    
        scaleStudy.scale( studyUID.toAscii().constData() ); //escalem l'estudi per la previsualització de la caché  
        emit( setStudyRetrieved( studyUID.toAscii().constData() ) );// descarregat
        localCache->setStudyRetrieved( studyUID.toAscii().constData() ); //posem l'estudi com a descarregat
    }
    
    pacsConnection.Disconnect();

   //esborrem les senyals del thread amb la interficeiq QRetrieveScreen
    m_qretrieveScreen->delConnectSignal( sProcessImg) ;    
    //esborrem el processImage de la llista de processImage encarregat de processar la informació per cada imatge descarregada
    piSingleton->delProcessImage( studyUID.toAscii().constData() );
    
}

void QExecuteOperationThread::seriesRetrieved( QString studyUID )
{
    emit( viewStudy( studyUID ) ); //signal cap a QueryScreen
}

QExecuteOperationThread::~QExecuteOperationThread()
{
}


}
