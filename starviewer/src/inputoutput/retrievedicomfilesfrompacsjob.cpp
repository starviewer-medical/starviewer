/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "retrievedicomfilesfrompacsjob.h"

#include <QtGlobal>
#include <QThread>

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
#include "directoryutilities.h"
#include "harddiskinformation.h"
#include "inputoutputsettings.h"
#include "dicomtagreader.h"
#include "portinuse.h"
#include "dicomsource.h"
#include "usermessage.h"
#include "studyoperationsservice.h"

namespace udg {

namespace {

// Copies basic information from the given Study to a new Study instance.
Study* copyBasicStudyInformation(const Study *studyToCopy)
{
    Study *copiedStudy = new Study();
    Patient *copiedPatient = new Patient();

    copiedPatient->setID(studyToCopy->getParentPatient()->getID());
    copiedPatient->setFullName(studyToCopy->getParentPatient()->getFullName());

    copiedStudy->setParentPatient(copiedPatient);
    copiedStudy->setInstanceUID(studyToCopy->getInstanceUID());
    copiedStudy->setID(studyToCopy->getID());
    copiedStudy->setDateTime(studyToCopy->getDateAsString(), studyToCopy->getTimeAsString());
    copiedStudy->setDescription(studyToCopy->getDescription());
    copiedStudy->setAccessionNumber(studyToCopy->getAccessionNumber());

    foreach(QString modality, studyToCopy->getModalities())
    {
        copiedStudy->addModality(modality);
    }

    copiedStudy->setDICOMSource(studyToCopy->getDICOMSource());

    return copiedStudy;
}

}

RetrieveDICOMFilesFromPACSJob::RetrieveDICOMFilesFromPACSJob(PacsDevice pacsDevice, RetrievePriorityJob retrievePriorityJob, const Study *studyToRetrieveDICOMFiles,
    const QString &seriesInstanceUIDToRetrieve, const QString &sopInstanceUIDToRetrieve)
 : PACSJob(pacsDevice)
{
    Q_ASSERT(studyToRetrieveDICOMFiles);
    Q_ASSERT(studyToRetrieveDICOMFiles->getParentPatient());

    m_retrieveDICOMFilesFromPACS = new RetrieveDICOMFilesFromPACS(getPacsDevice());
    m_studyToRetrieveDICOMFiles = copyBasicStudyInformation(studyToRetrieveDICOMFiles);
    m_seriesInstanceUIDToRetrieve = seriesInstanceUIDToRetrieve;
    m_SOPInstanceUIDToRetrieve = sopInstanceUIDToRetrieve;
    m_retrievePriorityJob = retrievePriorityJob;
}

RetrieveDICOMFilesFromPACSJob::~RetrieveDICOMFilesFromPACSJob()
{
    delete m_studyToRetrieveDICOMFiles;
    delete m_retrieveDICOMFilesFromPACS;
}

PACSJob::PACSJobType RetrieveDICOMFilesFromPACSJob::getPACSJobType()
{
    return PACSJob::RetrieveDICOMFilesFromPACSJobType;
}

const Study* RetrieveDICOMFilesFromPACSJob::getStudyToRetrieveDICOMFiles() const
{
    return m_studyToRetrieveDICOMFiles;
}

const QString& RetrieveDICOMFilesFromPACSJob::getSeriesInstanceUidToRetrieve() const
{
    return m_seriesInstanceUIDToRetrieve;
}

const QString& RetrieveDICOMFilesFromPACSJob::getSopInstanceUidToRetrieve() const
{
    return m_SOPInstanceUIDToRetrieve;
}

void RetrieveDICOMFilesFromPACSJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(self)
    Q_UNUSED(thread)

    Settings settings;
    // TODO: És aquest el lloc per aquest missatge ? no seria potser millor fer-ho a RetrieveDICOMFilesFromPACS
    INFO_LOG(QString("Iniciant descarrega del PACS %1, IP: %2, Port: %3, AE Title Local: %4 Port local: %5, "
                     "l'estudi UID: %6, series UID: %7, SOP Instance UID:%8")
        .arg(getPacsDevice().getAETitle(), getPacsDevice().getAddress(), QString::number(getPacsDevice().getQueryRetrieveServicePort()))
        .arg(settings.getValue(InputOutputSettings::LocalAETitle).toString(), settings.getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toString())
        .arg(m_studyToRetrieveDICOMFiles->getInstanceUID(), m_seriesInstanceUIDToRetrieve, m_SOPInstanceUIDToRetrieve));

    m_retrievedSeriesInstanceUIDSet.clear();

    m_retrieveRequestStatus = thereIsAvailableSpaceOnHardDisk();

    if (m_retrieveRequestStatus != PACSRequestStatus::RetrieveOk)
    {
        return;
    }

    int localPort = settings.getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toInt();

    if (PortInUse().isPortInUse(localPort))
    {
        m_retrieveRequestStatus = PACSRequestStatus::RetrieveIncomingDICOMConnectionsPortInUse;
        ERROR_LOG("El port " + QString::number(localPort) + " per a connexions entrants del PACS, esta en us, no es pot descarregar l'estudi");
    }
    else
    {
        PatientFiller patientFiller(getDICOMSourceRetrieveFiles());
        QThread fillersThread;
        patientFiller.moveToThread(&fillersThread);
        LocalDatabaseManager localDatabaseManager;

        // S'ha d'especificar com a DirectConnection, perquè sinó aquest signal l'aten qui ha creat el Job, que és la interfície, per tant
        // no s'atendria fins que la interfície estigui lliure, provocant comportaments incorrectes
        connect(m_retrieveDICOMFilesFromPACS, SIGNAL(DICOMFileRetrieved(DICOMTagReader*, int)), this, SLOT(DICOMFileRetrieved(DICOMTagReader*, int)),
                Qt::DirectConnection);
        // Connectem amb els signals del patientFiller per processar els fitxers descarregats
        connect(this, &RetrieveDICOMFilesFromPACSJob::DICOMTagReaderReadyForProcess, &patientFiller, &PatientFiller::processDICOMFile);
        connect(this, &RetrieveDICOMFilesFromPACSJob::DICOMFilesRetrieveFinished, &patientFiller, &PatientFiller::finishFilesProcessing);
        // Connexió entre el processat dels fitxers DICOM i l'inserció al a BD, és important que aquest signal sigui un Qt:DirectConnection perquè així el
        // el processa els thread dels fillers, d'aquesta manera el thread de descarrega que està esperant a fillersThread.wait() quan surt
        // d'aquí perquè els fillers ja han acabat ja s'ha inserit el pacient a la base de dades.
        connect(&patientFiller, SIGNAL(patientProcessed(Patient*)), &localDatabaseManager, SLOT(save(Patient*)), Qt::DirectConnection);
        // Connexions per finalitzar els threads
        connect(&patientFiller, SIGNAL(patientProcessed(Patient*)), &fillersThread, SLOT(quit()), Qt::DirectConnection);

        StudyOperationsService::instance()->setStudyBeingRetrieved(m_studyToRetrieveDICOMFiles->getInstanceUID());
        fillersThread.start();

        m_retrieveRequestStatus = m_retrieveDICOMFilesFromPACS->retrieve(m_studyToRetrieveDICOMFiles->getInstanceUID(), m_seriesInstanceUIDToRetrieve,
            m_SOPInstanceUIDToRetrieve);

        if ((m_retrieveRequestStatus == PACSRequestStatus::RetrieveOk || m_retrieveRequestStatus == PACSRequestStatus::RetrieveSomeDICOMFilesFailed) &&
            !this->isAbortRequested())
        {
            INFO_LOG(QString("Ha finalitzat la descarrega de l'estudi %1 del PACS %2, s'han descarregat %3 fitxers")
                .arg(m_studyToRetrieveDICOMFiles->getInstanceUID(), getPacsDevice().getAETitle())
                .arg(m_retrieveDICOMFilesFromPACS->getNumberOfDICOMFilesRetrieved()));

            // Indiquem que el procés de descàrrega ha finalitzat
            emit DICOMFilesRetrieveFinished();

            // Esperem que el processat i l'insersió a la base de dades acabin
            fillersThread.wait();

            if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
            {
                if (localDatabaseManager.getLastError() == LocalDatabaseManager::PatientInconsistent)
                {
                    // No s'ha pogut inserir el patient, perquè patientfiller no ha pogut emplenar l'informació de patient correctament
                    m_retrieveRequestStatus = PACSRequestStatus::RetrievePatientInconsistent;
                }
                else
                {
                    m_retrieveRequestStatus = PACSRequestStatus::RetrieveDatabaseError;
                }
            }
        }
        else
        {
            fillersThread.quit();
            // Esperem que el thread acabi, ja que pel que s'interpreta de la documentació, sembla que el quit del thread no es fa fins que aquest retorna
            // al eventLoop, això provoca per exemple en els casos que ens han cancel·lat la descàrrega d'un estudi, si no esperem al thread que estigui mort
            // poguem esborrar imatges que els fillers estan processant en aquell moment mentre encara s'estan executant i peti l'Starviewer, perquè
            // no s'ha atés el Slot quit del thread, per això esperem que aquest estigui mort a esborrar les imatges descarregades.
            fillersThread.wait();
            deleteRetrievedDICOMFilesIfStudyNotExistInDatabase();
        }

        StudyOperationsService::instance()->setStudyNotBeingRetrieved(m_studyToRetrieveDICOMFiles->getInstanceUID());
    }
}

void RetrieveDICOMFilesFromPACSJob::requestCancelJob()
{
    INFO_LOG(QString("S'ha demanat la cancel.lacio del Job de descarrega d'imatges de l'estudi %1 del PACS %2")
                .arg(getStudyToRetrieveDICOMFiles()->getInstanceUID(),
             getPacsDevice().getAETitle()));
    m_retrieveDICOMFilesFromPACS->requestCancel();
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACSJob::getStatus()
{
    return m_retrieveRequestStatus;
}

void RetrieveDICOMFilesFromPACSJob::DICOMFileRetrieved(DICOMTagReader *dicomTagReader, int numberOfImagesRetrieved)
{
    emit DICOMFileRetrieved(m_selfPointer.toStrongRef(), numberOfImagesRetrieved);

    /// Actualitzem el número de sèries processades si ens arriba una nova imatge que pertanyi a una sèrie no descarregada fins al moment
    QString seriesInstancedUIDRetrievedImage = dicomTagReader->getValueAttributeAsQString(DICOMSeriesInstanceUID);
    if (!m_retrievedSeriesInstanceUIDSet.contains(seriesInstancedUIDRetrievedImage))
    {
        m_retrievedSeriesInstanceUIDSet.insert(seriesInstancedUIDRetrievedImage);
        emit DICOMSeriesRetrieved(m_selfPointer.toStrongRef(), m_retrievedSeriesInstanceUIDSet.count());
    }

    // Fem un emit indicat que dicomTagReader està a punt per ser processat per l'Slot processDICOMFile de PatientFiller, no podem fer un connect
    // directament entre el signal de DICOMFileRetrieved de RetrieveDICOMFileFromPACS i processDICOMFile de PatientFiller, perquè ens podríem trobar
    // que quan en aquest mètode comprova si hem descarregat una nova sèrie que DICOMTagReader ja no tingui valor, per això primer capturem el signal de
    // RetrieveDICOMFileFromPACS comprovem si és una sèrie nova la que es descarrega i llavors fem l'emit per que PatientFiller processi el DICOMTagReader

    emit DICOMTagReaderReadyForProcess(dicomTagReader);
}

int RetrieveDICOMFilesFromPACSJob::priority() const
{
    return m_retrievePriorityJob;
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACSJob::thereIsAvailableSpaceOnHardDisk()
{
    LocalDatabaseManager localDatabaseManager;
    // TODO: Aquest signal no s'hauria de fer des d'aquesta classe sinó des d'una CacheManager, però com de moment encara no està implementada
    //       temporalment emetem el signal des d'aquí*/
    connect(&localDatabaseManager, SIGNAL(studyWillBeDeleted(QString)), SIGNAL(studyFromCacheWillBeDeleted(QString)));

    if (!localDatabaseManager.thereIsAvailableSpaceOnHardDisk())
    {
        // Si no hi ha prou espai emitim aquest signal
        if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok)
        {
            return PACSRequestStatus::RetrieveNoEnoughSpace;
        }
        else
        {
            return PACSRequestStatus::RetrieveErrorFreeingSpace;
        }
    }

    return PACSRequestStatus::RetrieveOk;
}

void RetrieveDICOMFilesFromPACSJob::deleteRetrievedDICOMFilesIfStudyNotExistInDatabase()
{
    // Comprovem si l'estudi està inserit a la base de dades, si és així vol dir que anteriorment s'havia descarregat un part o tot l'estudi,
    // com que ja tenim altres elements d'aquest estudi inserits a la base de dades no esborrem el directori de l'estudi
    if (!LocalDatabaseManager().studyExists(m_studyToRetrieveDICOMFiles->getInstanceUID()))
    {
        // Si l'estudi no existeix a la base de dades esborrem el contingut del directori, en principi segons la normativa DICO; si rebem un status de
        // tipus error per part de MoveSCP indicaria s'ha pogut descarregar cap objecte dicom amb èxit

        INFO_LOG("L'estudi " + m_studyToRetrieveDICOMFiles->getInstanceUID() + " no existeix a la base de de dades, esborrem el contingut del seu directori.");
        DirectoryUtilities().deleteDirectory(LocalDatabaseManager().getStudyPath(m_studyToRetrieveDICOMFiles->getInstanceUID()), true);
    }
    else
    {
        INFO_LOG("L'estudi " + m_studyToRetrieveDICOMFiles->getInstanceUID() + " existeix a la base de dades, no esborrem el contingut del seu directori.");
    }
}

// TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors
// duplicat ni traduccions, però és el millor lloc aquí posar aquest codi?
QString RetrieveDICOMFilesFromPACSJob::getStatusDescription()
{
    QString message;
    QString errorDetails = "\n\n" + tr("Details:") + "\n" + m_retrieveDICOMFilesFromPACS->getResponseStatus().toString();
    QString studyID = getStudyToRetrieveDICOMFiles()->getID();
    QString patientName = getStudyToRetrieveDICOMFiles()->getParentPatient()->getFullName();
    QString pacsAETitle = getPacsDevice().getAETitle();
    Settings settings;

    switch (getStatus())
    {
        case PACSRequestStatus::RetrieveOk:
            message = tr("Images from study %1 of patient %2 have been successfully retrieved from PACS %3.").arg(studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::RetrieveCancelled:
            message = tr("Retrieval of the images from study %1 of patient %2 from PACS %3 has been canceled.").arg(studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::RetrieveCanNotConnectToPACS:
            message = tr("Unable to connect to PACS %1 to retrieve images from study %2 of patient %3.")
                .arg(pacsAETitle, studyID, patientName);
            message += "\n\n";
            message += tr("Make sure your computer is connected to the network and the PACS parameters are correct.");
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            break;
        case PACSRequestStatus::RetrieveNoEnoughSpace:
            {
                Settings settings;
                HardDiskInformation hardDiskInformation;
                quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath());
                quint64 minimumSpaceRequired = quint64(settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toULongLong() * 1024);
                message = tr("There is not enough space to retrieve images from study %1 of patient %2, please free space or change your local "
                             "database settings.")
                        .arg(studyID, patientName);
                message += "\n\n";
                message += tr("- Available disk space: %1 MB.").arg(freeSpaceInHardDisk);
                message += "\n";
                message += tr("- Minimum disk space required to retrieve studies: %1 MB.").arg(minimumSpaceRequired);
            }
            break;
        case PACSRequestStatus::RetrieveErrorFreeingSpace:
            message = tr("An error occurred while freeing space on hard disk, images from study %1 of patient %2 won't be retrieved.").arg(studyID, patientName);
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            break;
        case PACSRequestStatus::RetrieveDatabaseError:
            message = tr("Cannot retrieve images from study %1 of patient %2 because a database error occurred.").arg(studyID, patientName);
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            break;
        case PACSRequestStatus::RetrievePatientInconsistent:
            message = tr("Cannot retrieve images from study %1 of patient %2 from PACS %3. Unable to correctly read data from images.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("The study may be corrupted, if it is not corrupted please contact with %1 team.").arg(ApplicationNameString);
            break;
        case PACSRequestStatus::RetrieveDestinationAETileUnknown:
            message = tr("Cannot retrieve images from study %1 of patient %2 because PACS %3 does not recognize your computer's AE Title %4.")
                    .arg(studyID, patientName, pacsAETitle, settings.getValue(InputOutputSettings::LocalAETitle).toString());
            message += "\n\n";
            message += tr("Contact with an administrator to register your computer to the PACS.");
            message += errorDetails;
            break;
        case PACSRequestStatus::RetrieveUnknowStatus:
            message = tr("Cannot retrieve images from study %1 of patient %2 due to an unknown error of PACS %3.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("The cause of the error may be that the requested images are corrupted. Please contact with a PACS administrator.");
            message += errorDetails;
            break;
        case PACSRequestStatus::RetrieveFailureOrRefused:
            message = tr("Cannot retrieve images from study %1 of patient %2 due to an error of PACS %3.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("The cause of the error may be that the requested images are corrupted or the incoming connections port in PACS configuration "
                          "is not correct.");
            message += errorDetails;
            break;
        case PACSRequestStatus::RetrieveIncomingDICOMConnectionsPortInUse:
            message = tr("Cannot retrieve images from study %1 of patient %2 because port %3 for incoming connections from PACS is already in use "
                         "by another application.")
                .arg(studyID, patientName, settings.getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toString());
            break;
        case PACSRequestStatus::RetrieveSomeDICOMFilesFailed:
            message = tr("Unable to retrieve some images from study %1 of patient %2 from PACS %3. Maybe those images are missing or corrupted in PACS.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n";
            message += errorDetails;
            break;
        default:
            message = tr("Cannot retrieve images from study %1 of patient %2 from PACS %3 due to an unknown error.")
                .arg(ApplicationNameString, studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            message += errorDetails;
            break;
    }

    return message;
}

DICOMSource RetrieveDICOMFilesFromPACSJob::getDICOMSourceRetrieveFiles()
{
    DICOMSource filesDICOMSource;
    filesDICOMSource.addRetrievePACS(getPacsDevice());

    return filesDICOMSource;
}

};
