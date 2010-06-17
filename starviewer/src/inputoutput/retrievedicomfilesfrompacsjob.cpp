#include "retrievedicomfilesfrompacsjob.h"

#include <QtGlobal>

#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "starviewerapplication.h"
#include "retrievedicomfilesfrompacs.h"
#include "dicommask.h"
#include "localdatabasemanager.h"
#include "patientfiller.h"
#include "qthreadrunwithexec.h"
#include "deletedirectory.h"
#include "utils.h"
#include "harddiskinformation.h"
#include "inputoutputsettings.h"
#include "dicomtagreader.h"

namespace udg
{
RetrieveDICOMFilesFromPACSJob::RetrieveDICOMFilesFromPACSJob(PacsDevice pacsDevice, Study *studyToRetrieveDICOMFiles, DicomMask dicomMaskToRetrieve, RetrievePriorityJob retrievePriorityJob):PACSJob(pacsDevice)
{
    Q_ASSERT(studyToRetrieveDICOMFiles);
    Q_ASSERT(studyToRetrieveDICOMFiles->getParentPatient());

    m_studyToRetrieveDICOMFiles = studyToRetrieveDICOMFiles;
    m_dicomMaskToRetrieve = dicomMaskToRetrieve;
    m_retrieveDICOMFilesFromPACS = new RetrieveDICOMFilesFromPACS(getPacsDevice());
    m_retrievePriorityJob = retrievePriorityJob;
}

RetrieveDICOMFilesFromPACSJob::~RetrieveDICOMFilesFromPACSJob()
{
    delete m_retrieveDICOMFilesFromPACS;
}

PACSJob::PACSJobType RetrieveDICOMFilesFromPACSJob::getPACSJobType()
{
    return PACSJob::RetrieveDICOMFilesFromPACSJobType;
}

Study* RetrieveDICOMFilesFromPACSJob::getStudyToRetrieveDICOMFiles()
{
    return m_studyToRetrieveDICOMFiles;
}

void RetrieveDICOMFilesFromPACSJob::run()
{
    INFO_LOG( QString("Iniciant la descàrrega de l'estudi %1 del pacs %2").arg(m_dicomMaskToRetrieve.getStudyUID(), getPacsDevice().getAETitle()));

    m_numberOfSeriesRetrieved = 0;
    m_lastImageSeriesInstanceUID = "";
    m_retrievedSeriesInstanceUID.clear();

    m_retrieveRequestStatus = thereIsAvailableSpaceOnHardDisk();

    if (m_retrieveRequestStatus != PACSRequestStatus::OkRetrieve)
    {
        return;
    }

    int localPort = PacsDevice::getIncomingDICOMConnectionsPort();

    if ( Utils::isPortInUse(localPort) )
    {
        m_retrieveRequestStatus = PACSRequestStatus::IncomingConnectionsPortPACSInUse;
        ERROR_LOG("El port " + QString::number(localPort) + " per a connexions entrants del PACS, està en ús, no es pot descarregar l'estudi");
    }
    else
    {
        PatientFiller patientFiller;
        QThreadRunWithExec fillersThread;
        patientFiller.moveToThread( &fillersThread );
        LocalDatabaseManager localDatabaseManager;

		/*S'ha d'especificar com a DirectConnection, perquè sinó aquest signal l'aten qui ha creat el Job, que és la interfície, per tant
		  no s'atendria fins que la interfície estigui lliure, provocant comportaments incorrectes*/
		connect(m_retrieveDICOMFilesFromPACS, SIGNAL( DICOMFileRetrieved(DICOMTagReader*, int) ), this, SLOT( DICOMFileRetrieved(DICOMTagReader*, int) ), Qt::DirectConnection);
		//Connectem amb els signals del patientFiller per processar els fitxers descarregats
		connect(this, SIGNAL(DICOMTagReaderReadyForProcess(DICOMTagReader *)), &patientFiller, SLOT(processDICOMFile(DICOMTagReader *)));
		connect(this, SIGNAL(DICOMFilesRetrieveFinished()), &patientFiller, SLOT(finishDICOMFilesProcess()));
        /*Connexió entre el processat dels fitxers DICOM i l'inserció al a BD, és important que aquest signal sigui un Qt:DirectConnection perquè així el 
          el processa els thread dels fillers, d'aquesta manera el thread de descarrega que està esperant a fillersThread.wait() quan surt 
          d'aquí perquè els fillers ja han acabat ja s'ha inserit el pacient a la base de dades.*/
		connect(&patientFiller, SIGNAL( patientProcessed(Patient *) ), &localDatabaseManager, SLOT( save(Patient *) ), Qt::DirectConnection);
		//Connexions per finalitzar els threads
		connect(&patientFiller, SIGNAL( patientProcessed(Patient *) ), &fillersThread, SLOT( quit() ), Qt::DirectConnection);

        localDatabaseManager.setStudyRetrieving(m_dicomMaskToRetrieve.getStudyUID());
        fillersThread.start();
        
        m_retrieveRequestStatus = m_retrieveDICOMFilesFromPACS->retrieve(m_dicomMaskToRetrieve);

        if (m_retrieveRequestStatus == PACSRequestStatus::OkRetrieve || m_retrieveRequestStatus == PACSRequestStatus::RetrieveWarning)
        {
            INFO_LOG("Ha finalitzat la descàrrega de l'estudi " + m_dicomMaskToRetrieve.getStudyUID() + "del pacs " + getPacsDevice().getAETitle());

            m_numberOfSeriesRetrieved++;
            emit DICOMSeriesRetrieved(this, m_numberOfSeriesRetrieved);//Indiquem que s'ha descarregat la última sèrie
            emit DICOMFilesRetrieveFinished(); 

            //Esperem que el processat i l'insersió a la base de dades acabin
            fillersThread.wait();

            if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
            {
                if (localDatabaseManager.getLastError() == LocalDatabaseManager::PatientInconsistent)
                {
                    //No s'ha pogut inserir el patient, perquè patientfiller no ha pogut emplenar l'informació de patient correctament
                    m_retrieveRequestStatus = PACSRequestStatus::PatientInconsistent;
                }
                else 
                {
                    m_retrieveRequestStatus = PACSRequestStatus::DatabaseError;
                }
            }
        }
        else
        {
            fillersThread.quit();
            deleteRetrievedDICOMFilesIfStudyNotExistInDatabase();
        }

        localDatabaseManager.setStudyRetrieveFinished();
    }
}

void RetrieveDICOMFilesFromPACSJob::requestAbort()
{
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACSJob::getStatus()
{
    return m_retrieveRequestStatus;
}

void RetrieveDICOMFilesFromPACSJob::DICOMFileRetrieved(DICOMTagReader *dicomTagReader, int numberOfImagesRetrieved)
{
    emit DICOMFileRetrieved(this, numberOfImagesRetrieved);

    QString seriesInstancedUIDRetrievedImage = dicomTagReader->getValueAttributeAsQString(DICOMSeriesInstanceUID);

    /**Comprovem si hem descarregat una nova sèrie. Degut a que pot ser que el PACS ens enviï les imatges desornades (que no estiguin agrupades per sèrie-9
       hem de comprovar que quan detectem un canvi de sèrie respecte l'anterior imatge descarregada, aquesta no s'hagi ja comptabilitzat. Som conscients
       que podem donar informació falsejada a l'usuari, però es fa per donar-lo més feedback amb la descàrrega d'imatges.*/
    if (seriesInstancedUIDRetrievedImage != m_lastImageSeriesInstanceUID && !m_lastImageSeriesInstanceUID.isEmpty())
    {
        if (!m_retrievedSeriesInstanceUID.contains(seriesInstancedUIDRetrievedImage))
        {
            m_numberOfSeriesRetrieved++;
            emit DICOMSeriesRetrieved(this, m_numberOfSeriesRetrieved);

            m_retrievedSeriesInstanceUID.append(seriesInstancedUIDRetrievedImage);
        }
    }

    
    /*Fem un emit indicat que dicomTagReader està a punt per ser processat per l'Slot processDICOMFile de PatientFiller, no podem fer un connect
      directament entre el signal de DICOMFileRetrieved de RetrieveDICOMFileFromPACS i processDICOMFile de PatientFiller, perquè ens podríem trobar
      que quan en aquest mètode comprova si hem descarregat una nova sèrie que DICOMTagReader ja no tingui valor, per això primer capturem el signal de 
      RetrieveDICOMFileFromPACS comprovem si és una sèrie nova la que es descarrega i llavors fem l'emit per que PatientFiller processi el DICOMTagReader*/

    emit DICOMTagReaderReadyForProcess(dicomTagReader);

    m_lastImageSeriesInstanceUID = seriesInstancedUIDRetrievedImage;
}

int RetrieveDICOMFilesFromPACSJob::priority() const
{
    return m_retrievePriorityJob;
}


PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACSJob::thereIsAvailableSpaceOnHardDisk()
{
    LocalDatabaseManager localDatabaseManager;

    if (!localDatabaseManager.thereIsAvailableSpaceOnHardDisk())
    {
        if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok) //si no hi ha prou espai emitim aquest signal
        {
            return PACSRequestStatus::NoEnoughSpaceToRetrieveDICOMFiles;
        }
        else
        {
            return PACSRequestStatus::ErrorFreeingSpace;
        }
    }

    return PACSRequestStatus::OkRetrieve;
}

void RetrieveDICOMFilesFromPACSJob::createRetrieveDICOMFilesConnections(LocalDatabaseManager *localDatabaseManager,LocalDatabaseManagerThreaded *localDatabaseManagerThreaded, PatientFiller *patientFiller, QThreadRunWithExec *fillersThread)
{
}

void RetrieveDICOMFilesFromPACSJob::deleteRetrievedDICOMFilesIfStudyNotExistInDatabase()
{
    /*Comprovem si l'estudi està inserit a la base de dades, si és així vol dir que anteriorment s'havia descarregat un part o tot l'estudi,
     *com que ja tenim altres elements d'aquest estudi inserits a la base de dades no esborrem el directori de l'estudi*/
    if (!existStudyInLocalDatabase(m_dicomMaskToRetrieve.getStudyUID()))
    {
        /*Si l'estudi no existeix a la base de dades esborrem el contingut del directori, en principi segons la normativa DICO; si rebem un status de 
         * tipus error per part de MoveSCP indicaria s'ha pogut descarregar cap objecte dicom amb èxit */

        INFO_LOG("L'estudi " + m_dicomMaskToRetrieve.getStudyUID() + " no existeix a la base de de dades, esborrem el contingut del seu directori.");
        DeleteDirectory().deleteDirectory(LocalDatabaseManager().getStudyPath(m_dicomMaskToRetrieve.getStudyUID()), true);
    }
    else
    {
        INFO_LOG("L'estudi " + m_dicomMaskToRetrieve.getStudyUID() + " existeix a la base de dades, no esborrem el contingut del seu directori.");
    }
}

bool RetrieveDICOMFilesFromPACSJob::existStudyInLocalDatabase(QString studyInstanceUID)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask dicomMask;

    dicomMask.setStudyUID(studyInstanceUID);

    return localDatabaseManager.queryStudy(dicomMask).count() > 0;
}

/*TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors duplicat
       ni traduccions, però és el millor lloc aquí posar aquest codi? */
QString RetrieveDICOMFilesFromPACSJob::getStatusDescription()
{
    QString message;
    QString studyID = getStudyToRetrieveDICOMFiles()->getID();
    QString patientName = getStudyToRetrieveDICOMFiles()->getParentPatient()->getFullName();
    QString pacsAETitle = getPacsDevice().getAETitle();

    switch (getStatus())
    {
        case PACSRequestStatus::OkRetrieve:
            message = tr("Study %1 of patient %2 has been retrieved succesfully from PACS %3.").arg(studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::CanNotConnectPACSToRetrieve :
            message += tr("%1 can't connect to PACS %2 trying to retrieve study %3 from patient %4.\n").arg(ApplicationNameString, pacsAETitle,
                studyID,patientName);
            message += tr("\nBe sure that your computer is connected on network and the PACS parameters are correct.");
            message += tr("\nIf the problem persists contact with an administrator.");
            break;
        case PACSRequestStatus::NoEnoughSpaceToRetrieveDICOMFiles:
            {
                Settings settings;
                HardDiskInformation hardDiskInformation;
                quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath());
                quint64 minimumSpaceRequired = quint64( settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache ).toULongLong() * 1024 );
                message = tr("There is not enough space to retrieve studiy %1 from patient %2, please free space or change your Local Database settings.").arg(
                    studyID, patientName);
                message += tr("\n\nAvailable space in Disk: %1 Mb").arg(freeSpaceInHardDisk);
                message += tr("\nMinimum space required in Disk to retrieve studies: %1 Mb").arg(minimumSpaceRequired);
            }
            break;
        case PACSRequestStatus::ErrorFreeingSpace :
            message += tr("An error ocurred freeing space the study %1 from patient %2 won't be retrieved.").arg(studyID, patientName);
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case PACSRequestStatus::DatabaseError :
            message += tr("Study %1 from patient %2 can't be retreived because a database error ocurred.").arg(studyID, patientName);
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case PACSRequestStatus::PatientInconsistent :
            message += tr("\n%1 can't retrieve the study %2 from patient %3 becuase has not be capable of read correctly DICOM information of the study.").arg(
                ApplicationNameString,  studyID, patientName);
            message += tr("\n\nThe study may be corrupted, if It is not corrupted please contact with %1 team.").arg(ApplicationNameString);
            break;
        case PACSRequestStatus::RetrieveDestinationAETileUnknown:
            message += tr("The study %1 from patient %2 can't be retrieved because PACS %3 doesn't recognize your computer's AETitle %4.").arg(
                    studyID, patientName, pacsAETitle, PacsDevice::getLocalAETitle() );
            message += tr("\n\nContact with an administrador to register your computer to the PACS.");
            break;
        case PACSRequestStatus::RetrieveUnknow:
        case PACSRequestStatus::RetrieveFailureOrRefused:
            message = tr("Please review the operation list screen, ");
            message += tr("%1 can't retrieve study %2 from patient %3 because PACS %4 doesn't respond as expected.\n\n").arg(ApplicationNameString, studyID,
                patientName, pacsAETitle);
            message += tr("The cause of the error can be that the requested images are corrupted or the incoming connections port in PACS configuration is not correct.");
            break;
        case PACSRequestStatus::IncomingConnectionsPortPACSInUse :
            message = tr("%1 can't retrieve study %2 from patient %3 because port %4 for incoming connections from PACS is already in use by another application.").arg( 
                ApplicationNameString, studyID, patientName, QString().setNum(PacsDevice::getIncomingDICOMConnectionsPort()));
            break;
        case PACSRequestStatus::RetrieveWarning:
            message = tr("There were problems to retrieve some images of study %1 from patient %2 from PACS %3. Those images may be missing in the local database.").arg(
                studyID, patientName, pacsAETitle);
            message += "\n";
            message += tr("Try again later. If the problem persists, contact your system administrator.");
        default:
            message = tr("An unknown error has ocurred and %1 can't retrieve study %2 from patient %3").arg(ApplicationNameString, studyID, patientName);
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
    }

    return message;
}

};