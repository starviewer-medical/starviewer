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
#include "processimagesingleton.h"
#include "starviewerprocessimageretrieved.h"
#include "starviewerprocessimagestored.h"
#include "harddiskinformation.h"
#include "errordcmtk.h"
#include "logging.h"
#include "status.h"
#include "storeimages.h"
#include "querypacs.h"
#include "pacsconnection.h"
#include "databaseconnection.h"
#include "patientfiller.h"
#include "localdatabasemanagerthreaded.h"
#include "qthreadrunwithexec.h"
#include "deletedirectory.h"
#include "utils.h"

#include <QMetaType> // pel qRegisterMetaType

namespace udg {

QSemaphore m_semaphor(1);//controlar l'acces a la variable m_stoppedThread

//constructor
QExecuteOperationThread::QExecuteOperationThread(QObject *parent)
 : QThread(parent)
{
    m_stoppedThread = true;
    m_qsemaphoreQueueOperationList = new QSemaphore(1);

    //Registrem aquest tipus per poder-ne fer signals
    qRegisterMetaType<QExecuteOperationThread::RetrieveError>("QExecuteOperationThread::RetrieveError");
    qRegisterMetaType<QExecuteOperationThread::RetrieveWarning>("QExecuteOperationThread::RetrieveWarning");
    qRegisterMetaType<QExecuteOperationThread::StoreError>("QExecuteOperationThread::StoreError");
    qRegisterMetaType<QExecuteOperationThread::StoreWarning>("QExecuteOperationThread::StoreWarning");
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
    m_qsemaphoreQueueOperationList->acquire();

    if (!m_queueOperationList.contains(operation))//Comprovem que la mateixa operació no estigui pendent ja de ser executada
    {
        m_queueOperationList << operation;
    }

    m_qsemaphoreQueueOperationList->release();

    emit newOperation( &operation );//emitim una senyal per a que la qretrieveScreen sapiga que s'ha demanat una nova operació, i la mostri per pantalla

    m_semaphor.acquire();

    //la variable m_stoppedThread controla si el thread està engegat o parat!
    if(m_stoppedThread = true)
    {   //si parat l'engeguem
        m_stoppedThread = false;
        start();
    }

    m_semaphor.release();
}

//s'executa des del thread
void QExecuteOperationThread::run()
{
    INFO_LOG("Iniciant thread que executa operacions");

    while (!m_stoppedThread)
    {
        Operation operation;

        operation = takeMaximumPriorityOperation();

        switch (operation.getOperation())
        {
            case Operation::Retrieve:
            case Operation::View:
            case Operation::Load:
                 retrieveStudy(operation);
                 break;
            case Operation::Move:
                 moveStudy(operation);
                 break;
        }

        //comprovem si hem de parar
        m_semaphor.acquire();
        m_stoppedThread = m_queueOperationList.isEmpty();
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

    INFO_LOG( QString("Iniciant la descàrrega de l'estudi %1 del pacs %2").arg( studyUID ).arg( operation.getPacsDevice().getAETitle() ) );

    //creem les connexions de signals i slots per enllaçar les diferents classes que participen a la descàrrega
    createRetrieveStudyConnections(&localDatabaseManager, &localDatabaseManagerThreaded, &patientFiller, &fillersThread, sProcessImg);

    localDatabaseManager.setStudyRetrieving(studyUID);
    //s'indica que comença la descarrega de l'estudi al qOperationStateScreen
    emit setOperating( studyUID );
    emit retrieveStarted( studyUID );

    if (!localDatabaseManager.thereIsAvailableSpaceOnHardDisk())
    {
        if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok) //si no hi ha prou espai emitim aquest signal
        {
            //si no hi ha prou espai cancel·lem les operacions de descàrrega
            errorRetrieving(studyUID, operation.getPacsDevice().getID(), NoEnoughSpace);
            cancelAllPendingOperations(Operation::Retrieve);
            cancelAllPendingOperations(Operation::View);
            cancelAllPendingOperations(Operation::Load);
        }
        else errorRetrieving(studyUID, operation.getPacsDevice().getID(), ErrorFreeingSpace);

        localDatabaseManager.setStudyRetrieveFinished();
        return;
    }

    int localPort = PacsDevice::getIncomingDICOMConnectionsPort();
    if ( Utils::isPortInUse(localPort) )
    {
        errorRetrieving(studyUID, operation.getPacsDevice().getID(), IncomingConnectionsPortPacsInUse);
        cancelAllPendingOperations(Operation::Retrieve);
        cancelAllPendingOperations(Operation::View);
        cancelAllPendingOperations(Operation::Load);
        localDatabaseManager.setStudyRetrieveFinished();
        
        ERROR_LOG("El port " + QString::number(localPort) + " per a connexions entrants del PACS, està en ús, no es pot descarregar l'estudi");
        return;
    }


    PacsServer pacsConnection(operation.getPacsDevice());//connemtem al pacs
    state = pacsConnection.connect(PacsServer::retrieveImages);
    if (!state.good())
    {
        ERROR_LOG( "Error al connectar al pacs " + operation.getPacsDevice().getAETitle() + ". PACS ERROR : " + state.text() );

        errorRetrieving(studyUID, operation.getPacsDevice().getID(), CanNotConnectPacsToMove);
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

    localDatabaseManagerThreaded.start();
    fillersThread.start();
    retState = retrieveImages.retrieve();
    pacsConnection.disconnect();

    if (!retState.good())
    {
        switch(retState.code())
        {
            case 1300://Move Destination Unknow
                errorRetrieving(studyUID, operation.getPacsDevice().getID(), MoveDestinationAETileUnknownStatus);
                break;
            case 1301://Move Status unknow
                errorRetrieving(studyUID, operation.getPacsDevice().getID(), MoveFailureOrRefusedStatus);
                break;
            case 1302://Warning Status una part de l'estudi no s'ha descarregat
                emit warningInRetrieve(studyUID, operation.getPacsDevice().getID(), MoveWarningStatus);

                emit filesRetrieved();//Si l'error és un warning vol dir que com a mínim hem rebut un objecte dicom, per tant el processem
                break;
            default:
                errorRetrieving(studyUID, operation.getPacsDevice().getID(), MoveUnknowStatus);
                break;
		}
    }
    else emit filesRetrieved();

    //Esperem que el processat i l'insersió a la base de dades acabin
    fillersThread.wait();
    localDatabaseManagerThreaded.wait();

    /*Si l'estudi s'ha descarregat comprovem que no s'hagi produït cap error al inserir-lo a la base de dades
     *El codi d'error 1302 és un MoveWarningStatus, indica que alguna de les imatges de l'estudi no s'ha descarregat, tot i així processem les imatges
     *que si s'han descarregat correctament*/
    if (retState.good() || retState.code() == 1302) 
    {
        if ( localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::Ok) //Comprovem si l'estudi s'ha inserit correctament a la BD
        {
            INFO_LOG( "Ha finalitzat la descàrrega de l'estudi " + studyUID + "del pacs " + operation.getPacsDevice().getAETitle() );

            emit setOperationFinished( studyUID );// descarregat a QOperationStateScreen
            emit retrieveFinished( studyUID );//la queryscreen l'afageix a la llista QStudyTreeView d'estudis de la cache

            if ( operation.getOperation() == Operation::View )
                emit viewStudy( operation.getDicomMask().getStudyUID(), operation.getDicomMask().getSeriesUID(), operation.getDicomMask().getSOPInstanceUID() );
            else if ( operation.getOperation() == Operation::Load )
                emit loadStudy( operation.getDicomMask().getStudyUID(), operation.getDicomMask().getSeriesUID(), operation.getDicomMask().getSOPInstanceUID() );
        }
        else
        {
            if (localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::PatientInconsistent)
            {
                //No s'ha pogut inserir el patient, perquè patientfiller no ha pogut emplenar l'informació de patient correctament
                errorRetrieving(studyUID, operation.getPacsDevice().getID(), PatientInconsistent);
            }
            else errorRetrieving(studyUID, operation.getPacsDevice().getID(), DatabaseError);
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
    PacsDevice pacs = operation.getPacsDevice();
    StoreImages storeImages;
    StarviewerProcessImageStored *storedProcessImage = new StarviewerProcessImageStored();
    ProcessImageSingleton *piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    QList<Image*> imagesToStoreList ;

    INFO_LOG( "Preparant les dades per guardar l' estudi " + operation.getStudyUID() + " al PACS " + pacs.getAETitle() );

    emit setOperating( operation.getStudyUID() );//Indiquem al QOperationState que comença l'enviament de les imatges

    imagesToStoreList = getImagesToStoreList(operation.getDicomMask());

    PacsServer pacsConnection( pacs );

    state = pacsConnection.connect( PacsServer::storeImages );

    if ( !state.good() )
    {
        ERROR_LOG( " S'ha produït un error al intentar connectar al PACS " + pacs.getAETitle() + ". PACS ERROR : " + state.text() );
        emit errorInStore(operation.getStudyUID(), pacs.getID(), CanNotConnectPacsToStore);
        return;
    }

    //afegim a la ProcssesImageSingletton quin objecte s'encarregarrà de processar les imatges guardades
    piSingleton->addNewProcessImage( operation.getStudyUID(), storedProcessImage );

    connect(storedProcessImage, SIGNAL( imageStored(QString, int) ), this, SIGNAL( imageCommit(QString, int) ));
    connect(storedProcessImage, SIGNAL( seriesStored(QString) ), this, SIGNAL( seriesCommit(QString) ));

    storeImages.setConnection( pacsConnection );

    state = storeImages.store(imagesToStoreList);

    piSingleton->delProcessImage( operation.getStudyUID() );
    delete storedProcessImage; // el delete és necessari perquè al fer el delete storedProcessImage envia al signal de que l'última sèrie ha estat descarregada

    if ( state.good() )
    {
        emit setOperationFinished( operation.getStudyUID() );// descarregat
    }
    else
    {
        switch(state.code())
        {
            case 1400:
                emit errorInStore( operation.getStudyUID() , pacs.getID() , StoreFailureStatus );
                break;
            case 1401:
                emit warningInStore( operation.getStudyUID(), pacs.getID() , StoreSomeImagesFailureStatus );
                emit setOperationFinished( operation.getStudyUID() );
                break;
            case 1402:
                emit warningInStore( operation.getStudyUID(), pacs.getID() , StoreWarningStatus );
                emit setOperationFinished( operation.getStudyUID() );
                break;
            default:
                emit errorInStore( operation.getStudyUID(), pacs.getID() , StoreUnknowStatus );
                break;
        }
    }
}

Operation QExecuteOperationThread::takeMaximumPriorityOperation()
{
    Operation operationMaxPriority, operationAtIndex;
    int positionMaxPriorityOperation = 0;

    m_qsemaphoreQueueOperationList->acquire();

    if (!m_queueOperationList.isEmpty())
    {
        operationMaxPriority = m_queueOperationList.at(positionMaxPriorityOperation);

        for (int index = 1; index < m_queueOperationList.count(); index++)
        {
            operationAtIndex = m_queueOperationList.at(index);

            if (operationAtIndex.getPriority() < operationMaxPriority.getPriority())
            {
                //la operació és més prioritària
                operationMaxPriority = operationAtIndex;
                positionMaxPriorityOperation = index;
            }
        }

        m_queueOperationList.removeAt(positionMaxPriorityOperation);//treiem l'operació de màxim prioritat de la llista
    }

    m_qsemaphoreQueueOperationList->release();

    return operationMaxPriority;
}

void QExecuteOperationThread::cancelAllPendingOperations(Operation::OperationAction operationActionToCancel)
{
    Operation operationAtIndex;
    QList<Operation> operationsNotCancelled;

    m_qsemaphoreQueueOperationList->acquire();

    for (int index = 0; index < m_queueOperationList.count(); index++)
    {
        operationAtIndex = m_queueOperationList.at(index);

        if (operationActionToCancel == operationAtIndex.getOperation())
        {
            emit setCancelledOperation(operationAtIndex.getStudyUID());
        }
        else
        {
            //L'operació no queda cancel·lada l'afegim a la llista de no cancel·lades
            operationsNotCancelled.append(operationAtIndex);
        }
    }

    m_queueOperationList.clear();
    m_queueOperationList = operationsNotCancelled;

    m_qsemaphoreQueueOperationList->release();
}

void QExecuteOperationThread::createRetrieveStudyConnections(LocalDatabaseManager *localDatabaseManager,LocalDatabaseManagerThreaded *localDatabaseManagerThreaded, PatientFiller *patientFiller, QThreadRunWithExec *fillersThread, StarviewerProcessImageRetrieved *starviewerProcessImageRetrieved)
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
    connect(this, SIGNAL(errorInRetrieve(QString, QString, QExecuteOperationThread::RetrieveError)), fillersThread, SLOT(quit()), Qt::DirectConnection);
    connect(this, SIGNAL(errorInRetrieve(QString, QString, QExecuteOperationThread::RetrieveError)), localDatabaseManagerThreaded, SLOT(quit()), Qt::DirectConnection);

	//Connexió que s'esborrarà un estudi per alliberar espai
	connect(localDatabaseManager, SIGNAL(studyWillBeDeleted(QString)), this, SLOT(studyWillBeDeletedSlot(QString)));
}

void QExecuteOperationThread::errorRetrieving(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::RetrieveError lastError)
{
    DeleteDirectory deleteDirectory;
    LocalDatabaseManager localDatabaseManager;

    emit errorInRetrieve(studyInstanceUID, pacsID, lastError);

    if (lastError == QExecuteOperationThread::MoveFailureOrRefusedStatus || lastError == QExecuteOperationThread::MoveUnknowStatus ||
        lastError == QExecuteOperationThread::DatabaseError || lastError == QExecuteOperationThread::PatientInconsistent)
    {
        /*Comprovem si l'estudi està inserit a la base de dades, si és així vol dir que anteriorment s'havia descarregat un part o tot l'estudi,
         *com que ja tenim altres elements d'aquest estudi inserits a la base de dades no esborrem el directori de l'estudi*/
        if (!existStudyInLocalDatabase(studyInstanceUID))
        {
            /*Si l'estudi no existeix a la base de dades esborrem el contingut del directori, en principi segons la normativa DICO; si rebem un status de 
             * tipus error per part de MoveSCP indicaria s'ha pogut descarregar cap objecte dicom amb èxit */

            INFO_LOG("L'estudi " + studyInstanceUID + " no existeix a la base de de dades, esborrem el contingut del seu directori.");
            deleteDirectory.deleteDirectory(localDatabaseManager.getStudyPath(studyInstanceUID), true);
        }
        else
        {
            INFO_LOG("L'estudi " + studyInstanceUID + " existeix a la base de dades, no esborrem el contingut del seu directori.");
        }
    }
}

bool QExecuteOperationThread::existStudyInLocalDatabase(QString studyInstanceUID)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask dicomMask;

    dicomMask.setStudyUID(studyInstanceUID);

    return localDatabaseManager.queryStudy(dicomMask).count() > 0;
}

QList<Image*> QExecuteOperationThread::getImagesToStoreList(DicomMask dicomMask)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    QList<Image*> imagesToStoreList;

    studyMask.setStudyUID( dicomMask.getStudyUID() );

    /*Hem d'agafar tot l'estructura patient,study,series,image perquè la classe StarviewerProcessImageStored que se li passa un objecte Image, per indicar-li
     que ja s'ha fet l'stored d'aquella imatge, necessita el SeriesUID i el StudyUID, l'objecte image si no té l'estructura Patient,Study,Series,Image per si sol 
     no té aquesta informació, per això no podem utilitzar el mètode queryImage de LocalDatabaseManager*/
    Patient *patient = localDatabaseManager.retrieve(studyMask);

    if(localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG(QString("Error al intentar obtenir els studys que s'han de gravar al pacs; Error: %1; StudyUID: %2")
                          .arg( localDatabaseManager.getLastError() )
                          .arg( dicomMask.getStudyUID() ));

        return imagesToStoreList;
    }

    foreach(Series *series, patient->getStudies().first()->getSeries())
    {
        QString seriesInstanceUIDMask = dicomMask.getSeriesUID();

        if (seriesInstanceUIDMask.isEmpty() || seriesInstanceUIDMask == series->getInstanceUID())
        {
            ///Si no tenim màscara de sèrie per aplicar o la màscara de la sèrie coincideix amb la sèrie actual, explorem les imatges
            foreach(Image* image,series->getImages())
            {
                QString imageSOPInstanceUIDMask = dicomMask.getSOPInstanceUID();

                if (imageSOPInstanceUIDMask.isEmpty() || imageSOPInstanceUIDMask == image->getSOPInstanceUID())
                {
                    //Si no tenim màscara d'imatge per aplicar o la màscara de la iamtge coincideix amb la imatge actual l'afegim a la llista
                    imagesToStoreList.append(image);
                }
            }
        }
    }

    return imagesToStoreList;
}

void QExecuteOperationThread::seriesRetrieved(QString studyInstanceUID)
{
    emit seriesCommit(studyInstanceUID);
}

void QExecuteOperationThread::studyWillBeDeletedSlot(QString studyInstanceUID)
{
    emit studyWillBeDeleted(studyInstanceUID);
}

}
