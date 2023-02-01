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

#include "retrievedicomfilesfrompacs.h"

#include "directoryutilities.h"

// Make sure OS specific configuration is included first
#include <osconfig.h>
#include <diutil.h>
#include <dcfilefo.h>
// Pels tags DcmTagKey DCM_xxxx
#include <dctagkey.h>
#include <dcdeftag.h>

#include <QDir>
#include <QString>

#include "localdatabasemanager.h"
#include "dicommask.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "pacsconnection.h"
#include "pacsdevice.h"

namespace udg {

// Constant que contindrà quin Abanstract Syntax de Move utilitzem entre els diversos que hi ha utilitzem
static const char *MoveAbstractSyntax = UID_MOVEStudyRootQueryRetrieveInformationModel;

RetrieveDICOMFilesFromPACS::RetrieveDICOMFilesFromPACS(PacsDevice pacs)
 : DIMSECService()
{
    m_pacs = pacs;
    m_abortIsRequested = false;

    this->setUpAsCMove();
}

OFCondition RetrieveDICOMFilesFromPACS::acceptSubAssociation(T_ASC_Network *associationNetwork, T_ASC_Association **association)
{
    const char *knownAbstractSyntaxes[] = { UID_VerificationSOPClass };
    const char *transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    int numTransferSyntaxes;

    OFCondition condition = ASC_receiveAssociation(associationNetwork, association, ASC_DEFAULTMAXPDU);

    if (condition.good())
    {
#ifndef DISABLE_COMPRESSION_EXTENSION
        // Si disposem de compressió la demanem, i podrem accelerar el temps de descàrrega considerablement
        // De moment demanem la compressió lossless que tot PACS que suporti compressió ha
        // de proporcionar: JPEGLossless:Non-Hierarchical-1stOrderPrediction
        transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
#else
        // Defined in dcxfer.h
        if (gLocalByteOrder == EBO_LittleEndian)
        {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else
        {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
#endif

        // Accept the Verification SOP Class if presented
        condition = ASC_acceptContextsWithPreferredTransferSyntaxes((*association)->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
                                                                    transferSyntaxes, numTransferSyntaxes);

        if (condition.good())
        {
            // The array of Storage SOP Class UIDs comes from dcuid.h
            condition = ASC_acceptContextsWithPreferredTransferSyntaxes((*association)->params, dcmAllStorageSOPClassUIDs, numberOfDcmAllStorageSOPClassUIDs,
                                                                        transferSyntaxes, numTransferSyntaxes);
        }
    }

    if (condition.good())
    {
        condition = ASC_acknowledgeAssociation(*association);
    }
    else
    {
        ASC_dropAssociation(*association);
        ASC_destroyAssociation(association);
    }
    return condition;
}

void RetrieveDICOMFilesFromPACS::moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response)
{
    Q_UNUSED(responseCount);
    Q_UNUSED(response);
    Q_UNUSED(request);
    Q_UNUSED(callbackData);

    // Aquest en teoria és el codi per cancel·lar una descàrrega però el PACS del l'UDIAT no suporta les requestCancel, per tant la única manera
    // de fer-ho és com es fa en el mètode subOperationSCP que s'aborta la connexió amb el PACS.

    //MoveSCPCallbackData *moveSCPCallbackData = (MoveSCPCallbackData*) callbackData;

    //if (moveSCPCallbackData->retrieveDICOMFilesFromPACS->m_abortIsRequested)
    //{
    //    OFCondition condition = DIMSE_sendCancelRequest(moveSCPCallbackData->association, moveSCPCallbackData->presentationContextId, request->MessageID);

    //    if (condition.good())
    //    {
    //        INFO_LOG("S'ha cancel·lat la descarrega");
    //    }
    //    else
    //    {
    //        ERROR_LOG("Error al intentar cancel.lar la descarrga. Descripcio error: " + QString(condition.text()));
    //    }
    //}
}

OFCondition RetrieveDICOMFilesFromPACS::echoSCP(T_ASC_Association *association, T_DIMSE_Message *dimseMessage,
                                                T_ASC_PresentationContextID presentationContextID)
{
    // The echo succeeded
    OFCondition condition = DIMSE_sendEchoResponse(association, presentationContextID, &dimseMessage->msg.CEchoRQ, STATUS_Success, NULL);
    if (condition.bad())
    {
        ERROR_LOG("El PACS ens ha sol.licitat un echo durant la descarrega pero la resposta a aquest ha fallat");
    }

    return condition;
}

void RetrieveDICOMFilesFromPACS::storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *storeRequest, char *imageFileName,
                                                  DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *storeResponse, DcmDataset **statusDetail)
{
    // Paràmetres d'entrada: callbackData, progress, storeRequest, imageFileName, imageDataSet
    // Paràmetres de sortida: storeResponse, statusDetail
    Q_UNUSED(imageFileName);

    // Si el paquest és de finalització d'una imatge hem de guardar-la
    if (progress->state == DIMSE_StoreEnd)
    {
        // No status detail
        *statusDetail = NULL;

        if ((imageDataSet) && (*imageDataSet))
        {
            DIC_UI sopClass, sopInstance;
            OFBool correctUIDPadding = OFFalse;
            StoreSCPCallbackData *storeSCPCallbackData = (StoreSCPCallbackData*)callbackData;
            RetrieveDICOMFilesFromPACS *retrieveDICOMFilesFromPACS = storeSCPCallbackData->retrieveDICOMFilesFromPACS;
            QString dicomFileAbsolutePath = retrieveDICOMFilesFromPACS->getAbsoluteFilePathCompositeInstance(*imageDataSet, storeSCPCallbackData->fileName);

            // Guardem la imatge
            OFCondition stateSaveImage = retrieveDICOMFilesFromPACS->save(storeSCPCallbackData->dcmFileFormat, dicomFileAbsolutePath);

            if (stateSaveImage.bad())
            {
                storeResponse->DimseStatus = STATUS_STORE_Refused_OutOfResources;
                DEBUG_LOG("No s'ha pogut guardar la imatge descarregada [" + dicomFileAbsolutePath + "], error: " + stateSaveImage.text());
                ERROR_LOG("No s'ha pogut guardar la imatge descarregada [" + dicomFileAbsolutePath + "], error: " + stateSaveImage.text());
                if (!QFile::remove(dicomFileAbsolutePath))
                {
                    DEBUG_LOG("Ha fallat el voler esborrar el fitxer " + dicomFileAbsolutePath + " que havia fallat prèviament al voler guardar-se.");
                    ERROR_LOG("Ha fallat el voler esborrar el fitxer " + dicomFileAbsolutePath + " que havia fallat prèviament al voler guardar-se.");
                }
            }
            else
            {
                // Should really check the image to make sure it is consistent, that its
                // sopClass and sopInstance correspond with those in the request.
                if (storeResponse->DimseStatus == STATUS_Success)
                {
                    // Which SOP class and SOP instance?
                    if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), correctUIDPadding))
                    {
                        storeResponse->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                        ERROR_LOG(QString("No s'ha trobat la sop class i la sop instance per la imatge %1").arg(storeSCPCallbackData->fileName));
                    }
                    else if (strcmp(sopClass, storeRequest->AffectedSOPClassUID) != 0)
                    {
                        storeResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                        ERROR_LOG(QString("No concorda la sop class rebuda amb la sol.licitada per la imatge %1").arg(storeSCPCallbackData->fileName));
                    }
                    else if (strcmp(sopInstance, storeRequest->AffectedSOPInstanceUID) != 0)
                    {
                        storeResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                        ERROR_LOG(QString("No concorda sop instance rebuda amb la sol.licitada per la imatge %1").arg(storeSCPCallbackData->fileName));
                    }
                }

                // TODO:Té processar el fitxer si ha fallat alguna de les anteriors comprovacions ?
                retrieveDICOMFilesFromPACS->m_numberOfImagesRetrieved++;
                DICOMTagReader *dicomTagReader = new DICOMTagReader(dicomFileAbsolutePath, storeSCPCallbackData->dcmFileFormat->getAndRemoveDataset());
                emit retrieveDICOMFilesFromPACS->DICOMFileRetrieved(dicomTagReader, retrieveDICOMFilesFromPACS->m_numberOfImagesRetrieved);
            }
        }
    }
}

OFCondition RetrieveDICOMFilesFromPACS::save(DcmFileFormat *fileRetrieved, QString dicomFileAbsolutePath)
{
    // Indiquem que no fem servir meta-header
    E_FileWriteMode writeMode = EWM_fileformat;
    E_EncodingType sequenceType = EET_ExplicitLength;
    E_GrpLenEncoding groupLength = EGL_recalcGL;
    E_PaddingEncoding paddingType = EPD_withoutPadding;
    Uint32 filePadding = 0, itemPadding = 0;
    E_TransferSyntax transferSyntaxFile = fileRetrieved->getDataset()->getOriginalXfer();

    return fileRetrieved->saveFile(qPrintable(QDir::toNativeSeparators(dicomFileAbsolutePath)), transferSyntaxFile, sequenceType, groupLength, paddingType,
                                   filePadding, itemPadding, writeMode);
}

OFCondition RetrieveDICOMFilesFromPACS::storeSCP(T_ASC_Association *association, T_DIMSE_Message *msg, T_ASC_PresentationContextID presentationContextID)
{
    T_DIMSE_C_StoreRQ *storeRequest = &msg->msg.CStoreRQ;
    OFBool useMetaheader = OFTrue;
    StoreSCPCallbackData storeSCPCallbackData;
    DcmFileFormat retrievedFile;
    DcmDataset *retrievedDataset = retrievedFile.getDataset();

    storeSCPCallbackData.dcmFileFormat = &retrievedFile;
    storeSCPCallbackData.retrieveDICOMFilesFromPACS = this;
    storeSCPCallbackData.fileName = storeRequest->AffectedSOPInstanceUID;

    OFCondition condition = DIMSE_storeProvider(association, presentationContextID, storeRequest, NULL, useMetaheader, &retrievedDataset, storeSCPCallback,
                                                (void*) &storeSCPCallbackData, DIMSE_BLOCKING, 0);

    if (condition.bad())
    {
        // Remove file
        ERROR_LOG("S'ha produit al processar una peticio de descarregar d'un fitxer, descripcio error " + QString(condition.text()));
        unlink(qPrintable(storeSCPCallbackData.fileName));
    }

    return condition;
}

OFCondition RetrieveDICOMFilesFromPACS::subOperationSCP(T_ASC_Association **subAssociation)
{
    // Ens convertim com en un servei. El PACS ens fa peticions que nosaltres hem de respondre, ens pot demanar descarregar una imatge o fer un echo
    T_DIMSE_Message dimseMessage;
    T_ASC_PresentationContextID presentationContextID;

    if (!ASC_dataWaiting(*subAssociation, 0))
    {
        return DIMSE_NODATAAVAILABLE;
    }

    OFCondition condition = DIMSE_receiveCommand(*subAssociation, DIMSE_BLOCKING, 0, &presentationContextID, &dimseMessage, NULL);

    if (condition == EC_Normal)
    {
        switch (dimseMessage.CommandField)
        {
            case DIMSE_C_STORE_RQ:
                condition = storeSCP(*subAssociation, &dimseMessage, presentationContextID);
                break;

            case DIMSE_C_ECHO_RQ:
                condition = echoSCP(*subAssociation, &dimseMessage, presentationContextID);
                break;

            default:
                ERROR_LOG("El PACS ens ha sol.licitat un tipus d'operacio invalida");
                condition = DIMSE_BADCOMMANDTYPE;
                break;
        }
    }
    // Clean up on association termination
    if (condition == DUL_PEERREQUESTEDRELEASE)
    {
        INFO_LOG("El PACS sol.licita tancar la connexio per on ens ha enviat els fitxers");
        condition = ASC_acknowledgeRelease(*subAssociation);
        ASC_dropSCPAssociation(*subAssociation);
        ASC_destroyAssociation(subAssociation);
        return condition;
    }
    else if (condition == DUL_PEERABORTEDASSOCIATION)
    {
        INFO_LOG("El PACS ha abortat la connexió");
    }
    else if (condition != EC_Normal)
    {
        ERROR_LOG("S'ha produit un error reben la peticio d'una suboperacio, descripcio error: " + QString(condition.text()));
        condition = ASC_abortAssociation(*subAssociation);
    }
    else if (m_abortIsRequested)
    {
        INFO_LOG("Abortarem les connexions amb el PACS, perque han sol.licitant cancel.lar la descarrega");
        condition = ASC_abortAssociation(*subAssociation);
        if (!condition.good())
        {
            ERROR_LOG("Error al abortar la connexio pel qual rebem les imatges" + QString(condition.text()));
        }

        // Tanquem la connexió amb el PACS perquè segons indica la documentació DICOM al PS 3.4 (Baseline Behavior of SCP) C.4.2.3.1 si abortem
        // la connexió per la qual rebem les imatges, el comportament del PACS és desconegut, per exemple DCM4CHEE tanca la connexió amb el PACS, però
        // el RAIM_Server no la tanca i la manté fent que no sortim mai d'aquesta classe. Degut a que no es pot saber en aquesta situació com actuaran
        // els PACS es tanca aquí la connexió amb el PACS.
        condition = ASC_abortAssociation(m_pacsConnection->getConnection());
        if (!condition.good())
        {
            ERROR_LOG("Error al abortar la connexio pel amb el PACS" + QString(condition.text()));
        }
        else
        {
            INFO_LOG("Abortada la connexio amb el PACS");
        }
    }

    if (condition != EC_Normal)
    {
        ASC_dropAssociation(*subAssociation);
        ASC_destroyAssociation(subAssociation);
    }
    return condition;
}

void RetrieveDICOMFilesFromPACS::subOperationCallback(void *subOperationCallbackData, T_ASC_Network *associationNetwork, T_ASC_Association **subAssociation)
{
    RetrieveDICOMFilesFromPACS *retrieveDICOMFilesFromPACS = (RetrieveDICOMFilesFromPACS*)subOperationCallbackData;
    if (associationNetwork == NULL)
    {
        // Help no net !
        return;
    }

    if (*subAssociation == NULL)
    {
        OFCondition condition = retrieveDICOMFilesFromPACS->acceptSubAssociation(associationNetwork, subAssociation);
        if (!condition.good())
        {
            ERROR_LOG("S'ha produit un error negociant l'associacio de la connexio DICOM entrant, descripcio error: " + QString(condition.text()));
        }
        else
        {
            INFO_LOG("Rebuda solicitud de connexio pel port de connexions DICOM entrants del PACS.");
        }
    }
    else
    {
        retrieveDICOMFilesFromPACS->subOperationSCP(subAssociation);
    }
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACS::retrieve(const QString &studyInstanceUID, const QString &seriesInstanceUID, const QString &sopInstanceUID)
{
    T_ASC_PresentationContextID presentationContextID;
    T_DIMSE_C_MoveRSP moveResponse;
    DcmDataset *statusDetail = NULL;
    m_pacsConnection = new PACSConnection(m_pacs);
    PACSRequestStatus::RetrieveRequestStatus retrieveRequestStatus;
    MoveSCPCallbackData moveSCPCallbackData;
    DcmDataset *dcmDatasetToRetrieve = getDcmDatasetOfImagesToRetrieve(studyInstanceUID, seriesInstanceUID, sopInstanceUID);
    m_numberOfImagesRetrieved = 0;

    // TODO S'hauria de comprovar que es tracti d'un PACS amb el servei de retrieve configurat
    if (!m_pacsConnection->connectToPACS(PACSConnection::RetrieveDICOMFiles))
    {
        ERROR_LOG("S'ha produit un error al intentar connectar al PACS per fer un retrieve. AE Title: " + m_pacs.getAETitle());
        return PACSRequestStatus::RetrieveCanNotConnectToPACS;
    }

    // Which presentation context should be used, It's important that the connection has MoveStudyRoot level
    T_ASC_Association *association = m_pacsConnection->getConnection();
    presentationContextID = ASC_findAcceptedPresentationContextID(association, MoveAbstractSyntax);
    if (presentationContextID == 0)
    {
        ERROR_LOG("No s'ha trobat cap presentation context valid");
        return PACSRequestStatus::RetrieveFailureOrRefused;
    }

    moveSCPCallbackData.association = association;
    moveSCPCallbackData.presentationContextId = presentationContextID;
    moveSCPCallbackData.retrieveDICOMFilesFromPACS = this;

    // Set the destination of the images to us
    T_DIMSE_C_MoveRQ moveRequest = getConfiguredMoveRequest(association);
    ASC_getAPTitles(association->params, moveRequest.MoveDestination, sizeof(moveRequest.MoveDestination), NULL, 0, NULL, 0);

    OFCondition condition = DIMSE_moveUser(association, presentationContextID, &moveRequest, dcmDatasetToRetrieve, moveCallback, &moveSCPCallbackData,
                                           DIMSE_BLOCKING, 0, m_pacsConnection->getNetwork(), subOperationCallback, this, &moveResponse, &statusDetail,
                                           NULL /*responseIdentifiers*/);

    if (condition.bad())
    {
        ERROR_LOG(QString("El metode descarrega no ha finalitzat correctament. Codi error: %1, descripcio error: %2").arg(condition.code())
                     .arg(condition.text()));
    }

    m_pacsConnection->disconnect();

    retrieveRequestStatus = getDIMSEStatusCodeAsRetrieveRequestStatus(moveResponse.DimseStatus);
    processServiceClassProviderResponseStatus(moveResponse.DimseStatus, statusDetail);
    
    // Dump status detail information if there is some
    if (statusDetail != NULL)
    {
        delete statusDetail;
    }

    delete dcmDatasetToRetrieve;

    return retrieveRequestStatus;
}

void RetrieveDICOMFilesFromPACS::requestCancel()
{
    m_abortIsRequested = true;
    INFO_LOG("S'ha sol.licitat cancel.lar la descarrega");
}

int RetrieveDICOMFilesFromPACS::getNumberOfDICOMFilesRetrieved()
{
    return m_numberOfImagesRetrieved;
}

T_DIMSE_C_MoveRQ RetrieveDICOMFilesFromPACS::getConfiguredMoveRequest(T_ASC_Association *association)
{
    T_DIMSE_C_MoveRQ moveRequest;
    DIC_US messageId = association->nextMsgID++;

    moveRequest.MessageID = messageId;
    strcpy(moveRequest.AffectedSOPClassUID, MoveAbstractSyntax);
    moveRequest.Priority = DIMSE_PRIORITY_MEDIUM;
    moveRequest.DataSetType = DIMSE_DATASET_PRESENT;

    return moveRequest;
}

DcmDataset* RetrieveDICOMFilesFromPACS::getDcmDatasetOfImagesToRetrieve(const QString &studyInstanceUID, const QString &seriesInstanceUID, const QString &sopInstanceUID)
{
    DcmDataset *dcmDatasetToRetrieve = new DcmDataset();
    QString retrieveLevel = "STUDY";

    DcmElement *elemSpecificCharacterSet = DcmItem::newDicomElement(DCM_SpecificCharacterSet);
    // ISO_IR 100 és Latin1
    elemSpecificCharacterSet->putString("ISO_IR 100");
    dcmDatasetToRetrieve->insert(elemSpecificCharacterSet, OFTrue);

    DcmElement *elem = DcmItem::newDicomElement(DCM_StudyInstanceUID);
    elem->putString(qPrintable(studyInstanceUID));
    dcmDatasetToRetrieve->insert(elem, OFTrue);

    if (!seriesInstanceUID.isEmpty())
    {
        DcmElement *elem = DcmItem::newDicomElement(DCM_SeriesInstanceUID);
        elem->putString(qPrintable(seriesInstanceUID));
        dcmDatasetToRetrieve->insert(elem, OFTrue);
        retrieveLevel = "SERIES";
    }

    if (!sopInstanceUID.isEmpty())
    {
        DcmElement *elem = DcmItem::newDicomElement(DCM_SOPInstanceUID);
        elem->putString(qPrintable(sopInstanceUID));
        dcmDatasetToRetrieve->insert(elem, OFTrue);
        retrieveLevel = "IMAGE";
    }

    // Especifiquem a quin nivell es fa el QueryRetrieve
    DcmElement *elemQueryRetrieveLevel = DcmItem::newDicomElement(DCM_QueryRetrieveLevel);
    elemQueryRetrieveLevel->putString(qPrintable(retrieveLevel));
    dcmDatasetToRetrieve->insert(elemQueryRetrieveLevel, OFTrue);

    return dcmDatasetToRetrieve;
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACS::getDIMSEStatusCodeAsRetrieveRequestStatus(unsigned int dimseStatusCode)
{
    PACSRequestStatus::RetrieveRequestStatus retrieveRequestStatus;

    // Al PS 3.4, secció C.4.2.1.5, taula C.4-2 podem trobar un descripció dels errors.
    // Al PS 3.4, secció C.4.2.3.1 es descriu els tipus generals d'error
    //      Failure o Refused: No s'ha pogut descarregat alguna imatge
    //      Warning: S'ha pogut descarregar com a mínim una imatge
    //      Success: Totes les imatges s'han descarregat correctament

    // Per a detalls sobre els "related fields" consultar PS 3.7, Annex C - Status Type Enconding

    if (dimseStatusCode == STATUS_Success)
    {
        return PACSRequestStatus::RetrieveOk;
    }

    switch (dimseStatusCode)
    {
        case STATUS_MOVE_Failed_MoveDestinationUnknown:
            retrieveRequestStatus = PACSRequestStatus::RetrieveDestinationAETileUnknown;
            break;

        case STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches:
        case STATUS_MOVE_Refused_OutOfResourcesSubOperations:
        case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass:
        case STATUS_MOVE_Failed_UnableToProcess:
            retrieveRequestStatus = PACSRequestStatus::RetrieveFailureOrRefused;
            break;

        case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures:
            retrieveRequestStatus = PACSRequestStatus::RetrieveSomeDICOMFilesFailed;
            break;

        case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
            // L'usuari ha sol·licitat cancel·lar la descàrrega
            retrieveRequestStatus = PACSRequestStatus::RetrieveCancelled;
            break;

        default:
            retrieveRequestStatus = PACSRequestStatus::RetrieveUnknowStatus;
            break;
    }

    return retrieveRequestStatus;
}

QString RetrieveDICOMFilesFromPACS::getAbsoluteFilePathCompositeInstance(DcmDataset *imageDataset, QString fileName)
{
    QString absoluteFilePath = LocalDatabaseManager::getCachePath();
    QDir directory;
    const char *instanceUID;
    OFCondition dicomQueryStatus;

    // WARN In case of missing UIDs files are created in upper directories
    dicomQueryStatus = imageDataset->findAndGetString(DCM_StudyInstanceUID, instanceUID, false);
    if (dicomQueryStatus.bad())
    {
        DEBUG_LOG("Ha fallat l'obtencio de l'UID d'estudi al dataset. Rao: " + QString(dicomQueryStatus.text()));
    }
    else
    {
        QString uid(instanceUID);
        DirectoryUtilities::sanitizeFilename(uid);
        absoluteFilePath += uid + "/";
    }

    // Comprovem, si el directori de l'estudi ja està creat
    if (!directory.exists(absoluteFilePath))
    {
        directory.mkdir(absoluteFilePath);
    }

    dicomQueryStatus = imageDataset->findAndGetString(DCM_SeriesInstanceUID, instanceUID, false);
    if (dicomQueryStatus.bad())
    {
        DEBUG_LOG("Ha fallat l'obtenció de l'UID de la serie al dataset. Rao: " + QString(dicomQueryStatus.text()));
    }
    else
    {
        QString uid(instanceUID);
        DirectoryUtilities::sanitizeFilename(uid);
        absoluteFilePath += QString(uid) + "/";
    }

    // Comprovem, si el directori de la sèrie ja està creat, sinó el creem
    if (!directory.exists(absoluteFilePath))
    {
        directory.mkdir(absoluteFilePath);
    }

    DirectoryUtilities::sanitizeFilename(fileName);

    return absoluteFilePath + fileName;
}

}
