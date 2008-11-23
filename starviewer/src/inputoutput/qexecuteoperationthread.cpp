/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QSemaphore>
#include <QMessageBox>
#include <QList>

#include "qexecuteoperationthread.h"
#include "pacsserver.h"
#include "retrieveimages.h"
#include "operation.h"
#include "queueoperationlist.h"
#include "processimagesingleton.h"
#include "starviewerprocessimageretrieved.h"
#include "starviewerprocessimagestored.h"
#include "harddiskinformation.h"
#include "starviewersettings.h"
#include "errordcmtk.h"
#include "logging.h"
#include "status.h"
#include "storeimages.h"
#include "querypacs.h"
#include "pacsconnection.h"
#include "databaseconnection.h"
#include "dicomseries.h"
#include "patientfiller.h"
#include "dicomtagreader.h"
#include "localdatabasemanagerthreaded.h"
#include "qthreadrunwithexec.h"
#include "deletedirectory.h"

namespace udg {

QSemaphore m_semaphor(1);//controlar l'acces a la variable m_stop

//constructor
QExecuteOperationThread::QExecuteOperationThread(QObject *parent)
 : QThread(parent)
{
     m_stop = true;

    //Registrem aquest tipus per poder-ne fer signals
    qRegisterMetaType<QExecuteOperationThread::OperationError>("QExecuteOperationThread::OperationError");
}

QExecuteOperationThread::~QExecuteOperationThread()
{
    if ( isRunning() )//Si s'està executan el thread el matem, perquè estem tancant l'aplicació, sinó el thread queda per sota obert
    {
        terminate();
        wait();//Hem d'esperar que es mati el thread per poder continuar
    }
}

void QExecuteOperationThread::queueOperation(Operation operation)
{
    QueueOperationList *queueOperationList = QueueOperationList::getQueueOperationList();

    emit newOperation( &operation );//emitim una senyal per a que la qretrieveScreen sapiga que s'ha demanat una nova operació, i la mostri per pantalla

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
            case Operation::Retrieve:
            case Operation::View:
                 retrieveStudy(operation);
                 break;
            case Operation::Move:
                 moveStudy(operation);
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
    LocalDatabaseManager localDatabaseManager;
    LocalDatabaseManagerThreaded localDatabaseManagerThreaded;
    PatientFiller patientFiller;
    QThreadRunWithExec fillersThread;
    patientFiller.moveToThread( &fillersThread );
    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();

    INFO_LOG( QString("Iniciant la descàrrega de l'estudi %1 del pacs %2").arg( studyUID ).arg( operation.getPacsParameters().getAEPacs() ) );

    localDatabaseManager.setStudyRetrieving(studyUID);
    //s'indica que comença la descarrega de l'estudi al qOperationStateScreen
    emit setOperating( studyUID );

    if (!localDatabaseManager.isEnoughSpace())
    {
        if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok) //si no hi ha prou espai emitim aquest signal
            errorRetrieving(studyUID, ErrorFreeingSpace);
        else
            errorRetrieving(studyUID, NoEnoughSpace);

        localDatabaseManager.setStudyRetrieveFinished();
        return;
    }

    PacsServer pacsConnection(operation.getPacsParameters());//connemtem al pacs
    state = pacsConnection.connect(PacsServer::retrieveImages,PacsServer::studyLevel);
    if (!state.good())
    {
        ERROR_LOG( "Error al connectar al pacs " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " + state.text() );

        errorRetrieving(studyUID, ErrorConnectingPacs);
        localDatabaseManager.setStudyRetrieveFinished();

        return;
    }

    //passem els parametres a la classe retrieveImages
    RetrieveImages retrieveImages;
    retrieveImages.setConnection( pacsConnection.getConnection() );
    retrieveImages.setMask( operation.getDicomMask() );
    retrieveImages.setNetwork( pacsConnection.getNetwork() );

    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges descarregades
    piSingleton->addNewProcessImage(studyUID, sProcessImg);

    //creem les connexions de signals i slots per enllaçar les diferents classes que participen a la descàrrega
    createRetrieveStudyConnections(&localDatabaseManagerThreaded, &patientFiller, &fillersThread, sProcessImg);

    localDatabaseManagerThreaded.start();
    fillersThread.start();
    retState = retrieveImages.retrieve();
    pacsConnection.disconnect();

    if (!retState.good())
    {
        ERROR_LOG( "S'ha produit algun error durant la descàrrega de l'estudi " + studyUID + " del pacs " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " +retState.text() );

        errorRetrieving(studyUID, ErrorRetrieving);
    }
    else
        emit filesRetrieved();

    //Esperem que el processat i l'insersió a la base de dades acabin
    fillersThread.wait();
    localDatabaseManagerThreaded.wait();

    //Comprovem si l'estudi s'ha inserit correctament a la BD
    if (retState.good() && localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::Ok)
    {
        INFO_LOG( "Ha finalitzat la descàrrega de l'estudi " + studyUID + "del pacs " + operation.getPacsParameters().getAEPacs() );

        emit setOperationFinished( studyUID );// descarregat a QOperationStateScreen
        emit retrieveFinished( studyUID );//la queryscreen l'afageix a la llista QStudyTreeView d'estudis de la cache

        if ( operation.getOperation() == Operation::View )
            emit viewStudy( operation.getDicomMask().getStudyUID(), operation.getDicomMask().getSeriesUID(), operation.getDicomMask().getSOPInstanceUID() );
    }
    else
    {
        if (localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::PatientInconsistent)
        {
            //No s'ha pogut inserir el patient, perquè patientfiller no ha pogut emplenar l'informació de patient correctament
            errorRetrieving(studyUID, PatientInconsistent);
        }
        else
        {
            errorRetrieving(studyUID, DatabaseError);
        }
    }

    localDatabaseManager.setStudyRetrieveFinished();
    //esborrem el processImage de la llista de processImage encarregat de processar la informació per cada imatge descarregada
    piSingleton->delProcessImage( studyUID );
    delete sProcessImg; // el delete és necessari perquè al fer el delete storedProcessImage envia al signal de que l'última sèrie ha estat descarregada
}

void QExecuteOperationThread::moveStudy( Operation operation )
{
    Status state;
    PacsParameters pacs;
    StarviewerSettings settings;
    StoreImages storeImages;
    StarviewerProcessImageStored *storedProcessImage = new StarviewerProcessImageStored();
    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();

    INFO_LOG( "Preparant les dades per moure estudi " + operation.getStudyUID() + " al PACS " + operation.getPacsParameters().getAEPacs() );

    emit setOperating( operation.getStudyUID() );//Indiquem al QOperationState que comença l'enviament de les imatges

    //cerquem el path de les imatges a emmagatzemar
    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    studyMask.setStudyUID( operation.getStudyUID() );
    Patient *patient = localDatabaseManager.retrieve(studyMask);
    if(localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG(QString("Error al intentar obtenir els studys que s'han de gravar al pacs; Error: %1; StudyUID: %2")
                          .arg( localDatabaseManager.getLastError() )
                          .arg( operation.getStudyUID() ));
        return;
    }

    // \TODO Això ho hem de fer així perquè nosaltres només volem un study, l'únic que pot retornar a partir del seu UID.
    Study *study = patient->getStudies().first();
    QList<Image*> imagesList;
    foreach(Series *series, study->getSeries())
    {
        imagesList += series->getImages();
    }

    PacsServer pacsConnection( operation.getPacsParameters() );

    state = pacsConnection.connect( PacsServer::storeImages , PacsServer::any );

    if ( !state.good() )
    {
        ERROR_LOG( " S'ha produït un error al intentar connectar al PACS " + operation.getPacsParameters().getAEPacs() + ". PACS ERROR : " + state.text() );
        emit errorConnectingPacs( operation.getPacsParameters().getPacsID() );
        emit setErrorOperation( operation.getStudyUID() );
        return;
    }

    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges guardades
    piSingleton->addNewProcessImage( operation.getStudyUID(), storedProcessImage );

    connect(storedProcessImage, SIGNAL( imageStored(QString, int) ), this, SIGNAL( imageCommit(QString, int) ));
    connect(storedProcessImage, SIGNAL( seriesStored(QString) ), this, SIGNAL( seriesCommit(QString) ));

    storeImages.setConnection( pacsConnection.getConnection() );
    storeImages.setNetwork( pacsConnection.getNetwork() );

    state = storeImages.store(imagesList);

    piSingleton->delProcessImage( operation.getStudyUID() );
    delete storedProcessImage; // el delete és necessari perquè al fer el delete storedProcessImage envia al signal de que l'última sèrie ha estat descarregada

    if ( state.good() )
    {
        INFO_LOG("S'ha mogut l'estudi correctament" );
        emit setOperationFinished( operation.getStudyUID() );// descarregat
    }
    else
    {
        emit setErrorOperation( operation.getStudyUID() );
        ERROR_LOG( "S'ha produit un error intentant guardar l'estudi : " + state.text() );
    }
}

void QExecuteOperationThread::createRetrieveStudyConnections(LocalDatabaseManagerThreaded *localDatabaseManagerThreaded, PatientFiller *patientFiller, QThreadRunWithExec *fillersThread, StarviewerProcessImageRetrieved *starviewerProcessImageRetrieved)
{
    connect(patientFiller, SIGNAL( progress(int) ), this, SIGNAL( currentProcessingStudyImagesRetrievedChanged(int) ));

    //Connexions entre la descarrega i el processat dels fitxers
    connect(starviewerProcessImageRetrieved, SIGNAL( fileRetrieved(DICOMTagReader*) ), patientFiller, SLOT( processDICOMFile(DICOMTagReader*) ));
    connect(this, SIGNAL( filesRetrieved() ), patientFiller, SLOT( finishDICOMFilesProcess() ));

    connect(starviewerProcessImageRetrieved, SIGNAL(seriesRetrieved(QString)), this, SLOT (seriesRetrieved(QString)));

    //Connexió entre el processat i l'insersió al a BD
    connect(patientFiller, SIGNAL( patientProcessed(Patient *) ), localDatabaseManagerThreaded, SLOT( save(Patient *) ), Qt::DirectConnection);

    //Connexions per finalitzar els threads
    connect(patientFiller, SIGNAL( patientProcessed(Patient *) ), fillersThread, SLOT( quit() ), Qt::DirectConnection);
    connect(localDatabaseManagerThreaded, SIGNAL( operationFinished(LocalDatabaseManagerThreaded::OperationType) ), localDatabaseManagerThreaded, SLOT( quit() ), Qt::DirectConnection );

    //Connexions d'abortament
    connect(this, SIGNAL(errorInOperation(QString, QExecuteOperationThread::OperationError)), fillersThread, SLOT(quit()), Qt::DirectConnection);
    connect(this, SIGNAL(errorInOperation(QString, QExecuteOperationThread::OperationError)), localDatabaseManagerThreaded, SLOT(quit()), Qt::DirectConnection);
}

void QExecuteOperationThread::errorRetrieving(QString studyInstanceUID, QExecuteOperationThread::OperationError lastError)
{
    DeleteDirectory deleteDirectory;
    LocalDatabaseManager localDatabaseManager;

    emit errorInOperation(studyInstanceUID, lastError);

    //Com la descàrrega ha fallat esborrem el directori on s'havia de descarregar l'estudi, per si s'ha descarregat alguna imatge
    deleteDirectory.deleteDirectory(localDatabaseManager.getStudyPath(studyInstanceUID), true);
}

void QExecuteOperationThread::seriesRetrieved(QString studyInstanceUID)
{
    emit seriesCommit(studyInstanceUID);
}

}
