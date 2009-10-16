#include "retrieveimages.h"

#include <osconfig.h> /* make sure OS specific configuration is included first */
#include <diutil.h>
#include <dcfilefo.h>

#include <ofconapp.h>//necessari per fer les sortides per pantalla de les dcmtkz

#include <QDir>

#include "struct.h"
#include "processimagesingleton.h"
#include "status.h"
#include "logging.h"
#include "errordcmtk.h"
#include "pacsconnection.h"
#include "localdatabasemanager.h"
#include "dicommask.h"
#include "logging.h"
#include "dicomtagreader.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */
RetrieveImages::RetrieveImages()
{
}

void RetrieveImages::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
}

void RetrieveImages::setNetwork ( T_ASC_Network * network )
{
    m_net = network;
}

void RetrieveImages:: setMask( DicomMask mask )
{
    m_mask = mask.getDicomMask();
}

OFCondition RetrieveImages::acceptSubAssoc( T_ASC_Network * aNet , T_ASC_Association ** assoc )
{
    const char* knownAbstractSyntaxes[] = {
        UID_VerificationSOPClass
    };

    //default value from movescu.cpp
    OFCmdUnsignedInt  opt_maxPDU = ASC_DEFAULTMAXPDU;

    const char* transferSyntaxes[] = { NULL , NULL , NULL , NULL };
    int numTransferSyntaxes;

    OFCondition cond = ASC_receiveAssociation( aNet , assoc , opt_maxPDU );

    if ( cond.good() )
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
        if ( gLocalByteOrder == EBO_LittleEndian )  /* defined in dcxfer.h */
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
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
            (*assoc)->params ,
            knownAbstractSyntaxes , DIM_OF(knownAbstractSyntaxes) ,
            transferSyntaxes , numTransferSyntaxes );

        if ( cond.good() )
        {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
                (*assoc)->params ,
                dcmAllStorageSOPClassUIDs , numberOfAllDcmStorageSOPClassUIDs ,
                transferSyntaxes , numTransferSyntaxes );
        }
    }

    if ( cond.good() ) 
        cond = ASC_acknowledgeAssociation( *assoc );

    if ( cond.bad() )
    {
        ASC_dropAssociation( *assoc );
        ASC_destroyAssociation( assoc );
    }
    return cond;
}

void RetrieveImages::moveCallback( void *callbackData , T_DIMSE_C_MoveRQ *req, int responseCount , T_DIMSE_C_MoveRSP *response )
{
    Q_UNUSED( req );
    Q_UNUSED( responseCount );
    Q_UNUSED( response );
    
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;

    myCallbackData = ( MyCallbackInfo* )callbackData;
}

OFCondition RetrieveImages::echoSCP(
  T_ASC_Association * assoc ,
  T_DIMSE_Message * msg ,
  T_ASC_PresentationContextID presID )
{

  /* the echo succeeded !! */
  OFCondition cond = DIMSE_sendEchoResponse( assoc , presID , &msg->msg.CEchoRQ , STATUS_Success , NULL );
  if ( cond.bad() )
  {
    DimseCondition::dump( cond );
  }

  return cond;
}

void RetrieveImages::storeSCPCallback(
    /* in */
    void *callbackData ,
    T_DIMSE_StoreProgress *progress ,    /* progress state */
    T_DIMSE_C_StoreRQ *req ,             /* original store request */
    char *imageFileName, DcmDataset **imageDataSet , /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp ,            /* final store response */
    DcmDataset **statusDetail )
{
    Q_UNUSED( imageFileName );
    DIC_UI sopClass;
    DIC_UI sopInstance;
    /* I found their default value in movescu.cpp */
    OFBool            opt_correctUIDPadding = OFFalse;
    E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;

    if ( progress->state == DIMSE_StoreEnd ) //si el paquest és de finalització d'una imatge hem de guardar-le
    {
        *statusDetail = NULL;    /* no status detail */

        /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
        if ( (imageDataSet) && ( *imageDataSet ) )
        {
            QString imageFilenameToSave = getCompositeInstanceFileName(*imageDataSet);

            StoreCallbackData *cbdata = ( StoreCallbackData* ) callbackData;
            //procés que farà el tractament de la imatge descarregada de la nostre aplicació, 
            // en el cas de l'starviewer guardar a la cache,i augmentarà comptador de descarregats
            ProcessImageSingleton* piSingleton = ProcessImageSingleton::getProcessImageSingleton();
            // obtenim l'UID d'estudi de l'objecte descarregat que posteriorment fem servir
            const char *text;
            (*imageDataSet)->findAndGetString( DCM_StudyInstanceUID , text , false );
            QString retrievedDatasetStudyUID( text );

            E_TransferSyntax xfer = opt_writeTransferSyntax;
            if (xfer == EXS_Unknown) 
                xfer = ( *imageDataSet )->getOriginalXfer();

            //Guardem la imatge
            OFCondition stateSaveImage = save(cbdata, imageFilenameToSave);
            
            if ( stateSaveImage.bad() )
            {
                piSingleton->setError( retrievedDatasetStudyUID );
                rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
                ERROR_LOG("No s'ha pogut guardar la imatge descarregada" + imageFilenameToSave + ", error: " + stateSaveImage.text()); 
            }

            DICOMTagReader *dicomTagReader = new DICOMTagReader(imageFilenameToSave, new DcmDataset((**imageDataSet)));

            /* should really check the image to make sure it is consistent, that its sopClass and sopInstance correspond with those in
            * the request.
            */
            if ( rsp->DimseStatus == STATUS_Success )
            {
            /* which SOP class and SOP instance ? */
                if (! DU_findSOPClassAndInstanceInDataSet( *imageDataSet , sopClass , sopInstance , opt_correctUIDPadding ) )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                    piSingleton->setError( retrievedDatasetStudyUID );
                    ERROR_LOG(QString("No s'ha trobat la sop class i la sop instance per la imatge %1").arg(cbdata->imageFileName));
                }
                else if ( strcmp( sopClass , req->AffectedSOPClassUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedDatasetStudyUID );
                    ERROR_LOG(QString("No concorda la sop class rebuda amb la sol·licitada per la imatge %1").arg(cbdata->imageFileName));
                }
                else if ( strcmp( sopInstance , req->AffectedSOPInstanceUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedDatasetStudyUID );
                    ERROR_LOG(QString("No concorda sop instance rebuda amb la sol·licitada per la imatge %1").arg(cbdata->imageFileName));
                }
            }

            piSingleton->process(dicomTagReader->getAttributeByName(DCM_StudyInstanceUID), dicomTagReader);
        }
    }

    return;
}

OFCondition RetrieveImages::save(StoreCallbackData *storeCallbackData, QString imageFileNameToSave)
{
    OFBool opt_useMetaheader = OFTrue;
    E_EncodingType    opt_sequenceType = EET_ExplicitLength;
    E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
    E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
    OFCmdUnsignedInt  opt_filepad = 0;
    OFCmdUnsignedInt  opt_itempad = 0;
    E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;

    E_TransferSyntax xfer = opt_writeTransferSyntax;
    if (xfer == EXS_Unknown) 
        xfer = storeCallbackData->dcmff->getDataset()->getOriginalXfer();

    return storeCallbackData->dcmff->saveFile(
            qPrintable(QDir::toNativeSeparators(imageFileNameToSave)),
            xfer, opt_sequenceType, opt_groupLength, opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad, !opt_useMetaheader );
}

OFCondition RetrieveImages::storeSCP( T_ASC_Association *assoc , T_DIMSE_Message *msg , T_ASC_PresentationContextID presID )
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];
    req = &msg->msg.CStoreRQ;
    OFBool opt_useMetaheader = OFTrue; // I found its default value in movescu.cpp */

    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = req->AffectedSOPInstanceUID;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    DcmDataset *dset = dcmff.getDataset();

    cond = DIMSE_storeProvider( assoc , presID , req , (char *)NULL , opt_useMetaheader , &dset , storeSCPCallback , ( void* ) &callbackData , DIMSE_BLOCKING , 0 );

    if ( cond.bad() )
    {
      DimseCondition::dump( cond );
      /* remove file */
      unlink( imageFileName );
    }
    return cond;
}

OFCondition RetrieveImages::subOpSCP( T_ASC_Association **subAssoc )
{
    //ens convertim com en un servidor el PACS ens envai comandes que nosaltres hem de fer en aquest
    //CAS ENS POT DEMANAR UN ECHO O QUE GUARDER UNA IMATGE
    T_DIMSE_Message     msg;
    T_ASC_PresentationContextID presID;

    if ( !ASC_dataWaiting( *subAssoc , 0 ) ) 
        return DIMSE_NODATAAVAILABLE;

    OFCondition cond = DIMSE_receiveCommand( *subAssoc , DIMSE_BLOCKING , 0 , &presID , &msg , NULL );

    if ( cond == EC_Normal )
    {
        switch ( msg.CommandField )
        {
        case DIMSE_C_STORE_RQ:
            cond = storeSCP( *subAssoc , &msg , presID );
            break;
        case DIMSE_C_ECHO_RQ:
            cond = echoSCP( *subAssoc , &msg , presID );
            break;
        default:
            cond = DIMSE_BADCOMMANDTYPE;
            break;
        }
    }
    /* clean up on association termination */
    if ( cond == DUL_PEERREQUESTEDRELEASE )
    {
        cond = ASC_acknowledgeRelease( *subAssoc );
        ASC_dropSCPAssociation( *subAssoc );
        ASC_destroyAssociation( subAssoc );
        return cond;
    }
    else if ( cond == DUL_PEERABORTEDASSOCIATION )
    {
    }
    else if ( cond != EC_Normal )
    {
        DimseCondition::dump( cond );
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation( *subAssoc );
    }

    if ( cond != EC_Normal )
    {
        ASC_dropAssociation( *subAssoc );
        ASC_destroyAssociation( subAssoc );
    }
    return cond;
}

void RetrieveImages::subOpCallback(void * /*subOpCallbackData*/ , T_ASC_Network *aNet , T_ASC_Association **subAssoc )
{
    if ( aNet == NULL )
    {
        return;   /* help no net ! */
    }

    if ( *subAssoc == NULL )
    {
        acceptSubAssoc( aNet , subAssoc );
    }
    else
    {
        subOpSCP( subAssoc );
    }
}

Status RetrieveImages::retrieve()
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US              msgId = m_assoc->nextMsgID++;
    DcmDataset          *rspIds = NULL;
    DcmDataset          *statusDetail = NULL;
    MyCallbackInfo      callbackData;
    Status state;

    //If not connection has been setted, return error because we need a PACS connection
    if ( m_assoc == NULL )
    {
        return state.setStatus( DcmtkNoConnectionError );
    }

    //If not mask has been setted, return error, we need a search mask
    if ( m_mask == NULL )
    {
        return state.setStatus( DcmtkNoMaskError );
    }

    /* which presentation context should be used, It's important that the connection has MoveStudyRoot level */
    presId = ASC_findAcceptedPresentationContextID( m_assoc , UID_MOVEStudyRootQueryRetrieveInformationModel );
    if ( presId == 0 ) 
        return state.setStatus( DIMSE_NOVALIDPRESENTATIONCONTEXTID );
    
    callbackData.assoc = m_assoc;
    callbackData.presId = presId;

    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , UID_MOVEStudyRootQueryRetrieveInformationModel );
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    // set the destination of the images to us
    ASC_getAPTitles( m_assoc->params, req.MoveDestination , NULL , NULL );

    OFCondition cond = DIMSE_moveUser( m_assoc , presId , &req , m_mask ,
        moveCallback , &callbackData , DIMSE_BLOCKING , 0 ,
        m_net , subOpCallback , NULL ,
        &rsp , &statusDetail , &rspIds );

    //el rsp.DimseStatus conté el valor del tag 0000,0900 que indica l'status de les operacions amb el Pacs
    if (rsp.DimseStatus != STATUS_Success)
    {
        // Aquests són els codis que podem rebre. Extrets de dimse.h (dcmtk)
        /*
        // Move Specific Codes 
        STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches                   0xa701
        STATUS_MOVE_Refused_OutOfResourcesSubOperations                     0xa702
        STATUS_MOVE_Failed_SOPClassNotSupported                             0xa800
        STATUS_MOVE_Failed_MoveDestinationUnknown                           0xa801
        STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass                   0xa900
        STATUS_MOVE_Failed_UnableToProcess                                  0xc000
        STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication     0xfe00
        STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures          0xb000        
        */
        // Per a una informació més detallada, consultar PS 3.4, secció C.4.2.1.5, taula C.4-2
        // Per a detalls sobre els "related fields" consultar PS 3.7, Annex C - Status Type Enconding
        
        QString errorMessage;
        QString warnMessage;
        QString infoMessage;
        // Llista de camps relacionats amb l'error que poden contenir informació adicional
        QList< DcmTagKey > relatedFieldsList;
        switch(rsp.DimseStatus)
        {
            case STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches: // 0xa701
                // Refused: Out of Resources – Unable to calculate number of matches
                // Related Fields
                // DCM_ErrorComment (0000,0902)
                relatedFieldsList << DCM_ErrorComment;
                errorMessage = "(0xa701) MOVE REFUSED. Sense recursos. Incapaç de calcular el nombre de correspondències.";  
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;

            case STATUS_MOVE_Refused_OutOfResourcesSubOperations: // 0xa702
                // Refused: Out of Resources – Unable to perform sub-operations
                // Related Fields 
                // DCM_NumberOfRemainingSuboperations (0000,1020) 
                // DCM_NumberOfCompletedSuboperations (0000,1021)
                // DCM_NumberOfFailedSuboperations (0000,1022)
                // DCM_NumberOfWarningSuboperations (0000,1023)
                relatedFieldsList << DCM_NumberOfRemainingSuboperations;
                relatedFieldsList << DCM_NumberOfCompletedSuboperations;
                relatedFieldsList << DCM_NumberOfFailedSuboperations;
                relatedFieldsList << DCM_NumberOfWarningSuboperations;
                // No es poden descarregar les imatges
                errorMessage = "(0xa702) MOVE REFUSED. Sense recursos. Han fallat algunes sub-operacions.";  
                state.setStatus(DcmtkMoveRefusedOutOfResources);
                // TODO Provoca aquest error que s'esborri tot el que s'ha descarregat? Potser no és la millor solució
                break;

            case STATUS_MOVE_Failed_MoveDestinationUnknown: // 0xa801
                // Refused: Move Destination unknown
                // Related Fields 
                // DCM_ErrorComment (0000,0902)
                relatedFieldsList << DCM_ErrorComment;
                // El PACS no ens té registrat amb el nostre AETitle
                errorMessage = "(0xa801) MOVE FAILED. No s'han pogut descarregar els objectes DICOM, el PACS no ens té registrat amb l'AETITLE donat.";
                state.setStatus(DcmtkMoveDestionationUnknown);
                break;

            case STATUS_MOVE_Failed_SOPClassNotSupported: // 0xa800
                // ?
                // En principi aquest error no ens hauria d'arribar per aquesta operació
                errorMessage = "(0xa800) MOVE FAILED. SOP Class not Supported. En principi aquest error no s'hauria de donar aquí!.";
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;
            
            case STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass: // 0xa900
                // Identifier does not match SOP Class
                // Related fields 
                // DCM_OffendingElement (0000,0901)
                // DCM_ErrorComment (0000,0902)
                relatedFieldsList << DCM_OffendingElement;
                relatedFieldsList << DCM_ErrorComment;
                // En principi aquest error no ens hauria d'arribar per aquesta operació
                errorMessage = "(0xa900) MOVE FAILED. Identifier does not match SOP Class. En principi aquest error no s'hauria de donar aquí!.";
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;

            case STATUS_MOVE_Failed_UnableToProcess: // 0xc000
                // Unable to Process
                // Related fields 
                // DCM_OffendingElement (0000,0901)
                // DCM_ErrorComment (0000,0902)
                relatedFieldsList << DCM_OffendingElement;
                relatedFieldsList << DCM_ErrorComment;
                errorMessage = "(0xc000) MOVE FAILED. Incapaç de processar.";
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;

            case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication: // 0xfe00
                // Sub-operations terminated due to Cancel Indication
                // Related fields 
                // DCM_NumberOfRemainingSuboperations (0000,1020) 
                // DCM_NumberOfCompletedSuboperations (0000,1021)
                // DCM_NumberOfFailedSuboperations (0000,1022)
                // DCM_NumberOfWarningSuboperations (0000,1023)
                relatedFieldsList << DCM_NumberOfRemainingSuboperations;
                relatedFieldsList << DCM_NumberOfCompletedSuboperations;
                relatedFieldsList << DCM_NumberOfFailedSuboperations;
                relatedFieldsList << DCM_NumberOfWarningSuboperations;
                // Això representa que el cancel l'ha fet l'usuari o la cancel·lació pot provenir des d'un altre entitat?
                infoMessage = "(0xfe00) MOVE CANCEL.(Cancel·lació demanada per l'usuari?).";
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;

            case STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures: // 0xb000
                // Sub-operations Complete – One or more Failures
                // Related fields 
                // DCM_NumberOfRemainingSuboperations (0000,1020) 
                // DCM_NumberOfFailedSuboperations (0000,1022)
                // DCM_NumberOfWarningSuboperations (0000,1023)
                relatedFieldsList << DCM_NumberOfRemainingSuboperations;
                relatedFieldsList << DCM_NumberOfFailedSuboperations;
                relatedFieldsList << DCM_NumberOfWarningSuboperations;
                warnMessage = "(0xb000) MOVE WARNING. Sub-operacions completades però hi ha hagut un o més errors en alguna de les sub-operacions.";
                // TODO cal especificar un state.setStatus()? En quins casos s'ens pot donar aquest error?
                break;
            
            default:
                // S'ha produït un error no contemplat. En principi no s'hauria d'arribar mai a aquesta branca
                errorMessage = "S'ha produit un error desconegut a l'intentar descarregar l'estudi.";
                state.setStatus(DcmtkMovescuUnknownError);
                break;
        }

        // Fem logs de la informació relacionada amb els errors
        if( !errorMessage.isEmpty() )
        {
            // DU_cmoveStatusString és una funció dcmtk que tradueix el codi d'error a un string comprensible
            ERROR_LOG( errorMessage + " Descripció rebuda: " + QString(DU_cmoveStatusString(rsp.DimseStatus)) );
        }
        if( !warnMessage.isEmpty() )
        {
            WARN_LOG( warnMessage + " Descripció rebuda: " + QString(DU_cmoveStatusString(rsp.DimseStatus)) );
        }
        if( !infoMessage.isEmpty() )
        {
            INFO_LOG( warnMessage + " Descripció rebuda: " + QString(DU_cmoveStatusString(rsp.DimseStatus)) );
        }

        if (statusDetail)
        {
            // Mostrem els detalls de l'status rebut, si se'ns han proporcionat
            if( !relatedFieldsList.isEmpty() )
            {
                const char *text;
                INFO_LOG("Status details");
                foreach( DcmTagKey tagKey, relatedFieldsList )
                {
                    // Fem un log per cada camp relacionat amb l'error amb el format 
                    // NomDelTag (xxxx,xxxx): ContingutDelTag
                    statusDetail->findAndGetString(tagKey, text, false);
                    INFO_LOG( QString( DcmTag(tagKey).getTagName() ) + " " + QString( tagKey.toString().c_str() ) + ": " + QString(text) );
                } 
            }
        }
    }
    else state.setStatus(cond);

    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
        delete statusDetail;

    if ( rspIds != NULL ) 
        delete rspIds;

    return state;
}

QString RetrieveImages::getCompositeInstanceFileName(DcmDataset *imageDataset)
{
    QString studyPath, seriesPath;
    QDir directory;
    const char *text;

    imageDataset->findAndGetString(DCM_StudyInstanceUID, text, false);
    studyPath = LocalDatabaseManager::getCachePath() + text;

    //comprovem, si el directori de l'estudi ja està creat
    if ( !directory.exists( studyPath  ) ) 
        directory.mkdir( studyPath );

    imageDataset->findAndGetString( DCM_SeriesInstanceUID , text , false );
    seriesPath = studyPath + "/" + text;

    //comprovem, si el directori de la sèrie ja està creat, sinó el creem
    if ( !directory.exists( seriesPath ) ) 
        directory.mkdir( seriesPath );

    imageDataset->findAndGetString( DCM_SOPInstanceUID , text , false );

    return seriesPath + "/" + text;
}

}
