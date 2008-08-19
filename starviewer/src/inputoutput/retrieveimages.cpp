#include "retrieveimages.h"

#include <osconfig.h> /* make sure OS specific configuration is included first */
#include <diutil.h>
#include <dcfilefo.h>

#include <ofconapp.h>//necessari per fer les sortides per pantalla de les dcmtkz

#include <QDir>
#include <QTime>

#include "struct.h"
#include "processimagesingleton.h"
#include "status.h"
#include "logging.h"
#include "errordcmtk.h"
#include "pacsconnection.h"
#include "starviewersettings.h"
#include "dicommask.h"
#include "logging.h"
#include "dicomimage.h"
#include "dicomtagreader.h"

namespace udg{

int m_timeDownloadingImages; //Comptador per saber quant de temps estem descarregant les imatges de l'estudi
int m_timeProcessingImages; //Comptador per saber quan de temps estem proce
int m_timeSaveImages;
int m_timeProcessDatabase;
QTime timer;//rellotge per comptar quan tardem a descarrega una imatge
QTime timerSaveImage;
QTime timerProcessDatabase;

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */
RetrieveImages::RetrieveImages()
{
    m_timeDownloadingImages = 0;
    m_timeProcessingImages = 0;
	m_timeSaveImages = 0;
	m_timeProcessDatabase = 0;
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

    if ( cond.good() ) cond = ASC_acknowledgeAssociation( *assoc );

    if ( cond.bad() )
    {
        ASC_dropAssociation( *assoc );
        ASC_destroyAssociation( assoc );
    }
    return cond;
}

void RetrieveImages::moveCallback( void *callbackData , T_DIMSE_C_MoveRQ *req, int responseCount , T_DIMSE_C_MoveRSP *response )
{
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;
    StarviewerSettings settings;

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
    DIC_UI sopClass;
    DIC_UI sopInstance;
    /* I found their default value in movescu.cpp */
    E_EncodingType    opt_sequenceType = EET_ExplicitLength;
    OFBool            opt_correctUIDPadding = OFFalse;
    E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
    StarviewerSettings settings;
    

    if ( progress->state == DIMSE_StoreEnd ) //si el paquest és de finalització d'una imatge hem de guardar-le
    {
        m_timeDownloadingImages += timer.elapsed();//temps que hem tardat a descarregar la imatge
        timer.restart();//reiniciem per comptar el temps que estem processant la imatge

        *statusDetail = NULL;    /* no status detail */

        /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
        if ( (imageDataSet) && ( *imageDataSet ) )
        {
            StoreCallbackData *cbdata = ( StoreCallbackData* ) callbackData;
            DICOMTagReader *dicomTagReader = new DICOMTagReader(cbdata->imageFileName, new DcmDataset((**imageDataSet)));
            ProcessImageSingleton* piSingleton = ProcessImageSingleton::getProcessImageSingleton();//proces que farà el tractament de la imatge descarregada de la nostre aplicació, en el cas de l'starviewer guardar a la cache,i augmentara comptador des descarregats
            DICOMImage retrievedImage( * imageDataSet );

            timerSaveImage.restart();

            E_TransferSyntax xfer = opt_writeTransferSyntax;
            if (xfer == EXS_Unknown) xfer = ( *imageDataSet )->getOriginalXfer();

            m_timeSaveImages += timerSaveImage.elapsed();//temps dedicat a guardar la imatge al disc dur
            //Guardem la imatge
            if ( save(cbdata, dicomTagReader).bad() )
            {
                piSingleton->setError( retrievedImage.getStudyUID() );
                rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
            }

            /* should really check the image to make sure it is consistent, that its sopClass and sopInstance correspond with those in
            * the request.
            */
            if ( rsp->DimseStatus == STATUS_Success )
            {
            /* which SOP class and SOP instance ? */
                if (! DU_findSOPClassAndInstanceInDataSet( *imageDataSet , sopClass , sopInstance , opt_correctUIDPadding ) )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                    ERROR_LOG(QString("No s'ha trobat la sop class i la sop instance per la imatge %1").arg(cbdata->imageFileName));
                }
                else if ( strcmp( sopClass , req->AffectedSOPClassUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                    ERROR_LOG(QString("No concorda la sop class rebuda amb la sol·licitada per la imatge %1").arg(cbdata->imageFileName));
                }
                else if ( strcmp( sopInstance , req->AffectedSOPInstanceUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                    ERROR_LOG(QString("No concorda sop instance rebuda amb la sol·licitada per la imatge %1").arg(cbdata->imageFileName));
                }
            }

            //TODO DESCOMENTAR PER FER FUNCIONAR EL NOU MODEL DE BD I FILLERS
            //piSingleton->process(dicomTagReader->getAttributeByName(DCM_StudyInstanceUID), dicomTagReader);

            //TODO AQUEST CODI S'HA D'ESBORRAR QUAN HI HAGI IMPLEMENTAT EL NOU MODEL DE BD I FILLERS
            //guardem la informacio que hem calculat nosaltres a l'objecte imatge
            retrievedImage.setImageName( cbdata->imageFileName );
            retrievedImage.setImagePath( settings.getCacheImagePath() + retrievedImage.getStudyUID() + "/" + retrievedImage.getSeriesUID() + "/" + cbdata->imageFileName );
            //calculem la mida de l'image TODO alerta! això ens torna un Uint32! i ho guardem en un int
            retrievedImage.setImageSize( cbdata->dcmff->calcElementLength( xfer ,opt_sequenceType ) );

            timerProcessDatabase.restart();
            piSingleton->process( retrievedImage.getStudyUID() , &retrievedImage );
            m_timeProcessDatabase += timerProcessDatabase.elapsed();//temps d'operació per processar la imatge a la caché

            m_timeProcessingImages += timer.elapsed();//temps que hem estat processant la imatge
            timer.restart();//reiniciem temporitzador per comptar quan tardem a descarregar la següent imatge
        }
    }

    return;
}

OFCondition RetrieveImages::save(StoreCallbackData *storeCallbackData, DICOMTagReader *dicomTagReader)
{
    OFBool opt_useMetaheader = OFTrue;
    E_EncodingType    opt_sequenceType = EET_ExplicitLength;
    E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
    E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
    OFCmdUnsignedInt  opt_filepad = 0;
    OFCmdUnsignedInt  opt_itempad = 0;
    E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;

    E_TransferSyntax xfer = opt_writeTransferSyntax;
    if (xfer == EXS_Unknown) xfer = storeCallbackData->dcmff->getDataset()->getOriginalXfer();

    return storeCallbackData->dcmff->saveFile( qPrintable( QDir::toNativeSeparators( getCompositeInstancePath(dicomTagReader) + "/" + storeCallbackData->imageFileName) ) , 
                                                xfer , opt_sequenceType , opt_groupLength , opt_paddingType , (Uint32)opt_filepad , (Uint32)opt_itempad , !opt_useMetaheader );
}

OFCondition RetrieveImages::storeSCP( T_ASC_Association *assoc , T_DIMSE_Message *msg , T_ASC_PresentationContextID presID )
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];
    req = &msg->msg.CStoreRQ;
    OFBool opt_useMetaheader = OFTrue; // I found its default value in movescu.cpp */
    StarviewerSettings settings;

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

    if ( !ASC_dataWaiting( *subAssoc , 0 ) ) return DIMSE_NODATAAVAILABLE;

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
    StarviewerSettings settings;

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
    if ( presId == 0 ) return state.setStatus( DIMSE_NOVALIDPRESENTATIONCONTEXTID );
    callbackData.assoc = m_assoc;
    callbackData.presId = presId;

    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , UID_MOVEStudyRootQueryRetrieveInformationModel );
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    // set the destination of the images to us
    ASC_getAPTitles( m_assoc->params, req.MoveDestination , NULL , NULL );


    timer.start();//iniciem el temporitzador per saber quan tardem a descarregar la primera imatge
    OFCondition cond = DIMSE_moveUser( m_assoc , presId , &req , m_mask ,
        moveCallback , &callbackData , DIMSE_BLOCKING , 0 ,
        m_net , subOpCallback , NULL ,
        &rsp , &statusDetail , &rspIds );

    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
    {
        delete statusDetail;
    }

    DEBUG_LOG(QString( "TEMPS DESCARREGANT IMATGES : %1ms " ).arg( m_timeDownloadingImages ) );
    DEBUG_LOG(QString( "TEMPS PROCESSANT IMATGES : %1ms " ).arg( m_timeProcessingImages ) );
    DEBUG_LOG(QString( "TEMPS GUARDANT IMATGES : %1ms " ).arg( m_timeSaveImages ) );
    DEBUG_LOG(QString( "TEMPS PROCESSANT IMATGE PER LA BASE DE DADES: %1ms " ).arg( m_timeProcessDatabase ) );

    if ( rspIds != NULL ) delete rspIds;

    state.setStatus( cond );

    return state;
}

QString RetrieveImages::getCompositeInstancePath(DICOMTagReader *dicomTagReader)
{
    QString studyPath, seriesPath;
    StarviewerSettings settings;
    QDir directory;

    studyPath = settings.getCacheImagePath() + dicomTagReader->getAttributeByName(DCM_StudyInstanceUID);

    //comprovem, si el directori de l'estudi ja està creat
    if ( !directory.exists( studyPath  ) ) directory.mkdir( studyPath );

    seriesPath = studyPath + "/" + dicomTagReader->getAttributeByName(DCM_SeriesInstanceUID);

    //comprovem, si el directori de la sèrie ja està creat, sinó el creem
    if ( !directory.exists( seriesPath ) ) directory.mkdir( seriesPath );
    return seriesPath;
}

}
