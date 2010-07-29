#include "retrievedicomfilesfrompacs.h"

#include <osconfig.h> /* make sure OS specific configuration is included first */
#include <diutil.h>
#include <dcfilefo.h>
// Pels tags DcmTagKey DCM_xxxx
#include <dcmtk/dcmdata/dctagkey.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <QDir>

#include "status.h"
#include "logging.h"
#include "pacsconnection.h"
#include "localdatabasemanager.h"
#include "dicommask.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "pacsserver.h"
#include "pacsrequeststatus.h"

namespace udg{

RetrieveDICOMFilesFromPACS::RetrieveDICOMFilesFromPACS(PacsDevice pacs)
{
    m_pacs = pacs;
    m_abortIsRequested = false;
}

OFCondition RetrieveDICOMFilesFromPACS::acceptSubAssociation(T_ASC_Network * associationNetwork, T_ASC_Association ** association)
{
    const char* knownAbstractSyntaxes[] = {UID_VerificationSOPClass};

    const char* transferSyntaxes[] = {NULL, NULL, NULL, NULL};
    int numTransferSyntaxes;

    OFCondition condition = ASC_receiveAssociation(associationNetwork, association, ASC_DEFAULTMAXPDU);

    if (condition.good())
    {
#ifndef DISABLE_COMPRESSION_EXTENSION
        // Si disposem de compressio la demanem, i podrem accelerar el temps de
        // descarrega considerablement
        // de moment demanem la compressio lossless que tot PACS que suporti compressio ha
        // de proporcionar: JPEGLossless:Hierarchical-1stOrderPrediction
        transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
#else
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
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

        /* accept the Verification SOP Class if presented */
        condition = ASC_acceptContextsWithPreferredTransferSyntaxes((*association)->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), 
            transferSyntaxes, numTransferSyntaxes);

        if (condition.good())
        {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            condition = ASC_acceptContextsWithPreferredTransferSyntaxes((*association)->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,                transferSyntaxes, numTransferSyntaxes);
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
    MoveSCPCallbackData *moveSCPCallbackData = (MoveSCPCallbackData*) callbackData;

/*  Aquest en teoria és el codi per cancel·lar una descàrrega però el PACS del l'UDIAT no suporta les requestCancel, per tant la única manera
    de fer-ho és com es fa en el mètode subOperationSCP que s'aborta la connexió amb el PACS.
  
    if (moveSCPCallbackData->retrieveDICOMFilesFromPACS->m_abortIsRequested)
    {
        OFCondition condition = DIMSE_sendCancelRequest(moveSCPCallbackData->association, moveSCPCallbackData->presentationContextId, request->MessageID);
       
        if (condition.good())
        {
            INFO_LOG("S'ha cancel·lat la descarrega");
        }
        else
        {
            ERROR_LOG("Error al intentar cancel.lar la descarrga. Descripcio error: " + QString(condition.text()));
        }
    }
*/
}

OFCondition RetrieveDICOMFilesFromPACS::echoSCP(T_ASC_Association * association, T_DIMSE_Message * dimseMessage,T_ASC_PresentationContextID presentationContextID)
{
    // The echo succeeded
    OFCondition condition = DIMSE_sendEchoResponse(association, presentationContextID, &dimseMessage->msg.CEchoRQ, STATUS_Success, NULL);
    if (condition.bad())
    {
        ERROR_LOG("El PACS ens ha sol·licitat un echo durant la descàrrega però la resposta a aquest ha fallat");
    }

    return condition;
}

void RetrieveDICOMFilesFromPACS::storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *storeRequest, char *imageFileName, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *storeResponse, DcmDataset **statusDetail)
{
    // Paràmetres d'entrada: callbackData, progress, req, imageFileName, imageDataSet
    // Paràmetres de sortida: rsp, statusDetail
    Q_UNUSED(imageFileName);

    if (progress->state == DIMSE_StoreEnd) //si el paquest és de finalització d'una imatge hem de guardar-le
    {
        *statusDetail = NULL; /* no status detail */

        if ((imageDataSet) && (*imageDataSet))
        {
            DIC_UI sopClass, sopInstance;
            OFBool correctUIDPadding = OFFalse;
            StoreSCPCallbackData *storeSCPCallbackData = (StoreSCPCallbackData*) callbackData;
            RetrieveDICOMFilesFromPACS *retrieveDICOMFilesFromPACS = storeSCPCallbackData->retrieveDICOMFilesFromPACS;
            QString dicomFileAbsolutePath = retrieveDICOMFilesFromPACS ->getAbsoluteFilePathCompositeInstance(*imageDataSet, storeSCPCallbackData->fileName);

            //Guardem la imatge
            OFCondition stateSaveImage = retrieveDICOMFilesFromPACS->save(storeSCPCallbackData->dcmFileFormat, dicomFileAbsolutePath);
            
            if (stateSaveImage.bad())
            {
                storeResponse->DimseStatus = STATUS_STORE_Refused_OutOfResources;
                ERROR_LOG("No s'ha pogut guardar la imatge descarregada" + dicomFileAbsolutePath + ", error: " + stateSaveImage.text()); 
            }

            /* should really check the image to make sure it is consistent, that its sopClass and sopInstance correspond with those in
            * the request. */
            if (storeResponse->DimseStatus == STATUS_Success)
            {
                /* which SOP class and SOP instance ? */
                if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, correctUIDPadding))
                {
                    storeResponse->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                    ERROR_LOG(QString("No s'ha trobat la sop class i la sop instance per la imatge %1").arg(storeSCPCallbackData->fileName));
                }
                else if (strcmp(sopClass, storeRequest->AffectedSOPClassUID) != 0)
                {
                    storeResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    ERROR_LOG(QString("No concorda la sop class rebuda amb la sol·licitada per la imatge %1").arg(storeSCPCallbackData->fileName));
                }
                else if (strcmp(sopInstance, storeRequest->AffectedSOPInstanceUID) != 0)
                {
                    storeResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    ERROR_LOG(QString("No concorda sop instance rebuda amb la sol·licitada per la imatge %1").arg(storeSCPCallbackData->fileName));
                }
            }
            
            retrieveDICOMFilesFromPACS->m_numberOfImagesRetrieved++;
            DICOMTagReader *dicomTagReader = new DICOMTagReader(dicomFileAbsolutePath, storeSCPCallbackData->dcmFileFormat->getAndRemoveDataset());
            emit retrieveDICOMFilesFromPACS->DICOMFileRetrieved(dicomTagReader, retrieveDICOMFilesFromPACS->m_numberOfImagesRetrieved);
        }
    }
}

OFCondition RetrieveDICOMFilesFromPACS::save(DcmFileFormat *fileRetrieved, QString dicomFileAbsolutePath)
{
    OFBool useMetaheader = OFTrue;
    E_EncodingType sequenceType = EET_ExplicitLength;
    E_GrpLenEncoding groupLength = EGL_recalcGL;
    E_PaddingEncoding paddingType = EPD_withoutPadding;
    Uint32 filePadding = 0, itemPadding = 0;
    E_TransferSyntax transferSyntaxFile = fileRetrieved->getDataset()->getOriginalXfer();

    return fileRetrieved->saveFile(qPrintable(QDir::toNativeSeparators(dicomFileAbsolutePath)), transferSyntaxFile, sequenceType, groupLength, 
        paddingType, filePadding, itemPadding, !useMetaheader);
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
        /* remove file */
        unlink(qPrintable(storeSCPCallbackData.fileName));
    }

    return condition;
}

OFCondition RetrieveDICOMFilesFromPACS::subOperationSCP(T_ASC_Association **subAssociation)
{
    //ens convertim com en un servii el PACS ens peticions que nosaltres hem de respondre, ens pot demanar descar una imatge o fer un echo
    T_DIMSE_Message dimseMessage;
    T_ASC_PresentationContextID presentationContextID;

    if (!ASC_dataWaiting(*subAssociation, 0)) 
        return DIMSE_NODATAAVAILABLE;

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
            condition = DIMSE_BADCOMMANDTYPE;
            break;
        }
    }
    /* clean up on association termination */
    if (condition == DUL_PEERREQUESTEDRELEASE)
    {
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
        condition = ASC_abortAssociation(*subAssociation);
    }
    else if (m_abortIsRequested)
    {
        INFO_LOG("Abortarem les connexions amb el PACS, perque han sol.licitant cancel.lar la descarrega");
        condition = ASC_abortAssociation(*subAssociation);
        if (!condition.good())
        {
            ERROR_LOG("Error al abortar la connexió pel qual rebem les imatges" + QString(condition.text()));
        }

        /*Tanquem la connexió amb el PACS perquè segons indica la documentació DICOM al PS 3.4 (Baseline Behavior of SCP) C.4.2.3.1 si abortem
         la connexió pel qual rebem les imatges, el comportament del PACS és desconegut, per exemple DCM4CHEE tanca la connexió amb el PACS, però
         el RAIM_Server no la tanca i la manté fent que no sortim mai d'aquesta classe. Degut a que no es pot saber en aquesta situació com actuaran 
         els PACS es tanca aquí la connexió amb el PACS.*/
        condition = ASC_abortAssociation(m_pacsServer->getConnection().getPacsConnection());
        if (!condition.good())
        {
            ERROR_LOG("Error al abortar la connexió pel amb el PACS" + QString(condition.text()));
        }
        else
        {
            INFO_LOG("Abortada la connexió amb el PACS");
        }
    }

    if (condition != EC_Normal)
    {
        ASC_dropAssociation(*subAssociation);
        ASC_destroyAssociation(subAssociation);
    }
    return condition;
}

void RetrieveDICOMFilesFromPACS::subOperationCallback(void * subOperationCallbackData, T_ASC_Network *associationNetwork, T_ASC_Association **subAssociation)
{
    RetrieveDICOMFilesFromPACS *retrieveDICOMFilesFromPACS = (RetrieveDICOMFilesFromPACS*) subOperationCallbackData;
    if (associationNetwork == NULL)
    {
        return;   /* help no net ! */
    }

    if (*subAssociation == NULL)
    {
        retrieveDICOMFilesFromPACS->acceptSubAssociation(associationNetwork, subAssociation);
    }
    else
    {
        retrieveDICOMFilesFromPACS->subOperationSCP(subAssociation);
    }
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACS::retrieve(DicomMask dicomMask)
{
    T_ASC_PresentationContextID presentationContextID;
    T_DIMSE_C_MoveRSP moveResponse;
    DcmDataset *statusDetail = NULL;
    Status state;
    m_pacsServer = new PacsServer(m_pacs);
    PACSRequestStatus::RetrieveRequestStatus retrieveRequestStatus;
    MoveSCPCallbackData moveSCPCallbackData;

    m_numberOfImagesRetrieved = 0;

    //TODO: S'hauria de comprovar que es tracti d'un PACS amb el servei de retrieve configurat
    state = m_pacsServer->connect( PacsServer::retrieveImages );
    
    if ( !state.good() )
    {
        ERROR_LOG( " S'ha produit un error al intentar connectar al PACS per fer un retrieve. AETitle: " + m_pacs.getAETitle() + ", IP: " + m_pacs.getAddress() +
            ", port: " + QString().setNum(m_pacs.getQueryRetrieveServicePort()) + ", Descripcio error : " + state.text() );
        return PACSRequestStatus::CanNotConnectPACSToRetrieve;
    }

    /* which presentation context should be used, It's important that the connection has MoveStudyRoot level */
    T_ASC_Association *association = m_pacsServer->getConnection().getPacsConnection(); 
    presentationContextID = ASC_findAcceptedPresentationContextID(association, UID_MOVEStudyRootQueryRetrieveInformationModel);
    if (presentationContextID == 0) 
    {
        ERROR_LOG("No s'ha trobat cap presentation context vàlid");
        return PACSRequestStatus::RetrieveFailureOrRefused;
    }

    moveSCPCallbackData.association = association;
    moveSCPCallbackData.presentationContextId = presentationContextID;
    moveSCPCallbackData.retrieveDICOMFilesFromPACS = this;

    // set the destination of the images to us
    T_DIMSE_C_MoveRQ moveRequest = getConfiguredMoveRequest(association); 
    ASC_getAPTitles(association->params, moveRequest.MoveDestination, NULL, NULL);

    OFCondition condition = DIMSE_moveUser(association, presentationContextID, &moveRequest, dicomMask.getDicomMask(), moveCallback, &moveSCPCallbackData, 
        DIMSE_BLOCKING, 0, m_pacsServer->getNetwork(), subOperationCallback, this, &moveResponse, &statusDetail, NULL /*responseIdentifiers*/);

    m_pacsServer->disconnect();

    retrieveRequestStatus = processResponseStatusFromMoveSCP(&moveResponse, statusDetail);

    /* dump status detail information if there is some */
    if (statusDetail != NULL)
        delete statusDetail;

    return retrieveRequestStatus;
}

void RetrieveDICOMFilesFromPACS::requestCancel()
{
    m_abortIsRequested = true;
    INFO_LOG("S'ha sol·licitat cancel·lar la descàrrega");
}

T_DIMSE_C_MoveRQ RetrieveDICOMFilesFromPACS::getConfiguredMoveRequest(T_ASC_Association *association)
{
    T_DIMSE_C_MoveRQ moveRequest;
    DIC_US messageId = association->nextMsgID++;

    moveRequest.MessageID = messageId;
    strcpy(moveRequest.AffectedSOPClassUID, UID_MOVEStudyRootQueryRetrieveInformationModel);
    moveRequest.Priority = DIMSE_PRIORITY_MEDIUM;
    moveRequest.DataSetType = DIMSE_DATASET_PRESENT;

    return moveRequest;
}

PACSRequestStatus::RetrieveRequestStatus RetrieveDICOMFilesFromPACS::processResponseStatusFromMoveSCP(T_DIMSE_C_MoveRSP *moveResponse, DcmDataset *statusDetail)
{
    QList< DcmTagKey > relatedFieldsList;// Llista de camps relacionats amb l'error que poden contenir informació adicional
    QString messageErrorLog = "No s'ha pogut descarregar l'estudi, descripció error rebuda";
    PACSRequestStatus::RetrieveRequestStatus retrieveRequestStatus;

    // A la secció C.4.2.1.5, taula C.4-2 podem trobar un descripció dels errors.
    // A la secció C.4.2.3.1 es descriu els tipus generals d'error 
    //      Failure o Refused: No s'ha pogut descarregat alguna imatge
    //      Warning: S'ha pogut descarregar com a mínim una imatge
    //      Success: Totes les imatges s'han descarregat correctament

    // Per a detalls sobre els "related fields" consultar PS 3.7, Annex C - Status Type Enconding
    
    if (moveResponse->DimseStatus == STATUS_Success)
    {
        return PACSRequestStatus::OkRetrieve;
    }

    switch(moveResponse->DimseStatus)
    {
        case STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches: // 0xa701
            // Refused: Out of Resources – Unable to calculate number of matches
            // Related Fields DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            retrieveRequestStatus = PACSRequestStatus::RetrieveFailureOrRefused;
            break;

        case STATUS_MOVE_Refused_OutOfResourcesSubOperations: // 0xa702
            // Refused: Out of Resources – Unable to perform sub-operations
            // Related Fields DCM_NumberOfRemainingSuboperations (0000,1020), DCM_NumberOfCompletedSuboperations (0000,1021)
            // DCM_NumberOfFailedSuboperations (0000,1022), DCM_NumberOfWarningSuboperations (0000,1023)
            relatedFieldsList << DCM_NumberOfRemainingSuboperations << DCM_NumberOfCompletedSuboperations;
            relatedFieldsList << DCM_NumberOfFailedSuboperations << DCM_NumberOfWarningSuboperations;

            ERROR_LOG(messageErrorLog + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            retrieveRequestStatus = PACSRequestStatus::RetrieveFailureOrRefused;
            break;

        case STATUS_MOVE_Failed_MoveDestinationUnknown: // 0xa801
            // Refused: Move Destination unknown
            // Related Fields DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_ErrorComment;

            // El PACS no ens té registrat amb el nostre AETitle
            ERROR_LOG(messageErrorLog + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            retrieveRequestStatus = PACSRequestStatus::RetrieveDestinationAETileUnknown;
            break;

        case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass : //0xa900
        case STATUS_MOVE_Failed_UnableToProcess : // 0xc000 
            // Unable to Process or Identifier does not match SOP Class
            // Related fields DCM_OffendingElement (0000,0901) DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_OffendingElement << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            retrieveRequestStatus = PACSRequestStatus::RetrieveFailureOrRefused;
            break;

        case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures: // 0xb000
            // Sub-operations Complete – One or more Failures
            // Related fields DCM_NumberOfRemainingSuboperations (0000,1020), DCM_NumberOfFailedSuboperations (0000,1022), DCM_NumberOfWarningSuboperations (0000,1023)
            relatedFieldsList << DCM_NumberOfRemainingSuboperations << DCM_NumberOfFailedSuboperations  << DCM_NumberOfWarningSuboperations;

            WARN_LOG("Error no s'ha pogut descarregar tot l'estudi. Descripció rebuda: " + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            retrieveRequestStatus = PACSRequestStatus::RetrieveWarning;
            break;
        case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
            //L'usuari ha sol·licitat cancel·lar la descàrrega
            retrieveRequestStatus = PACSRequestStatus::RetrieveCancelled;
            break;
        default:
            ERROR_LOG(messageErrorLog + QString(DU_cmoveStatusString(moveResponse->DimseStatus)));
            // S'ha produït un error no contemplat. En principi no s'hauria d'arribar mai a aquesta branca
            retrieveRequestStatus = PACSRequestStatus::RetrieveUnknow;
            break;
    }

    if (statusDetail)
    {
        // Mostrem els detalls de l'status rebut, si se'ns han proporcionat
        if(!relatedFieldsList.isEmpty())
        {
            const char *text;
            INFO_LOG("Status details");
            foreach(DcmTagKey tagKey, relatedFieldsList)
            {
                // Fem un log per cada camp relacionat amb l'error amb el format 
                // NomDelTag (xxxx,xxxx): ContingutDelTag
                statusDetail->findAndGetString(tagKey, text, false);
                INFO_LOG(QString(DcmTag(tagKey).getTagName()) + " " + QString(tagKey.toString().c_str()) + ": " + QString(text));
            } 
        }
    }

    return retrieveRequestStatus;
}

QString RetrieveDICOMFilesFromPACS::getAbsoluteFilePathCompositeInstance(DcmDataset *imageDataset, QString fileName)
{
    QString studyPath, seriesPath;
    QDir directory;
    const char *text;

    imageDataset->findAndGetString(DCM_StudyInstanceUID, text, false);
    studyPath = LocalDatabaseManager::getCachePath() + text;

    //comprovem, si el directori de l'estudi ja està creat
    if (!directory.exists(studyPath )) 
        directory.mkdir(studyPath);

    imageDataset->findAndGetString(DCM_SeriesInstanceUID, text, false);
    seriesPath = studyPath + "/" + text;

    //comprovem, si el directori de la sèrie ja està creat, sinó el creem
    if (!directory.exists(seriesPath)) 
        directory.mkdir(seriesPath);

    return seriesPath + "/" + fileName;
}

}
