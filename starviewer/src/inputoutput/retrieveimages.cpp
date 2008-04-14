#include "retrieveimages.h"

#include <osconfig.h> /* make sure OS specific configuration is included first */
#include <diutil.h>
#include <dcfilefo.h>
#include <assoc.h>
#include <ofconapp.h>//necessari per fer les sortides per pantalla de les dcmtkz
#include <QDir>

#include "struct.h"
#include "processimagesingleton.h"
#include "status.h"
#include "logging.h"
#include "errordcmtk.h"
#include "pacsconnection.h"
#include "starviewersettings.h"
#include "imagelistsingleton.h"
#include "dicommask.h"

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

/// En aquesta funció acceptem la connexió que se'ns sol·licita per transmetre'ns imatges
OFCondition
acceptSubAssoc( T_ASC_Network * aNet , T_ASC_Association ** assoc )
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

void moveCallback( void *callbackData , T_DIMSE_C_MoveRQ */*request*/ , int responseCount , T_DIMSE_C_MoveRSP *response )
{
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;
    StarviewerSettings settings;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "Move Response " << responseCount << endl;
        DIMSE_printCMoveRSP( stdout , response );
    }
#endif

    myCallbackData = ( MyCallbackInfo* )callbackData;
}

OFCondition echoSCP(
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

/// Aquesta funció s'encarrega de guardar cada paquet que rebem
/*It's a callback function, can't own to the class */
 /**
  *
  * @param callbackData
  * @param progress
  * @param req
  * @param
  * @param imageDataSet
  * @param rsp
  * @param statusDetail
  */
 void storeSCPCallback(
    /* in */
    void *callbackData ,
    T_DIMSE_StoreProgress *progress ,    /* progress state */
    T_DIMSE_C_StoreRQ *req ,             /* original store request */
    char */*imageFileName*/, DcmDataset **imageDataSet , /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp ,            /* final store response */
    DcmDataset **statusDetail )
{
    DIC_UI sopClass;
    DIC_UI sopInstance;
    /* I found their default value in movescu.cpp */
    OFBool opt_useMetaheader = OFTrue;
    E_EncodingType    opt_sequenceType = EET_ExplicitLength;
    E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
    E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
    OFCmdUnsignedInt  opt_filepad = 0;
    OFCmdUnsignedInt  opt_itempad = 0;
    OFBool            opt_correctUIDPadding = OFFalse;
    E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
    StarviewerSettings settings;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
      switch (progress->state)
      {
        case DIMSE_StoreBegin:
          cout<<"====================================== REICIVING DICOM DATA ======================================" << endl;
          cout << "RECEIVING:" ;
          break;
        case DIMSE_StoreEnd:
          cout << endl;
          break;
        default:
          cout << ".";
          break;
      }
      fflush(stdout);
    }
#endif

    if ( progress->state == DIMSE_StoreEnd ) //si el paquest és de finalització d'una imatge hem de guardar-le
    {
        *statusDetail = NULL;    /* no status detail */

        /* could save the image somewhere else, put it in database, etc */
        /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
        // rsp->DimseStatus = STATUS_Success;
        if ( (imageDataSet) && ( *imageDataSet ) )
        {
            StoreCallbackData *cbdata = ( StoreCallbackData* ) callbackData;
            ProcessImageSingleton* piSingleton = ProcessImageSingleton::getProcessImageSingleton();//proces que farà el tractament de la imatge descarregada de la nostre aplicació, en el cas de l'starviewer guardar a la cache,i augmentara comptador des descarregats
            QString studyPath, seriesPath, imagePath;
            int imageSize;
            DICOMImage retrievedImage( * imageDataSet );

            studyPath = QDir::toNativeSeparators( piSingleton->getPath() + retrievedImage.getStudyUID() );//agafem el path del directori on es guarden les imatges
            QDir directory;
        
            //comprovem, si el directori de l'estudi ja està creat
            if ( !directory.exists( studyPath  ) ) directory.mkdir( studyPath );

            seriesPath = QDir::toNativeSeparators( studyPath + "/" + retrievedImage.getSeriesUID() );

            //comprovem, si el directori de la sèrie ja està creat, sinó el creem
            if ( !directory.exists( seriesPath ) ) directory.mkdir( seriesPath );

            //acabem de concatenar el nom del fitxer
            imagePath = seriesPath;
            imagePath.append("/");
            imagePath.append( cbdata->imageFileName );
            
            imagePath = QDir::toNativeSeparators( imagePath );

            E_TransferSyntax xfer = opt_writeTransferSyntax;
            if (xfer == EXS_Unknown) xfer = ( *imageDataSet )->getOriginalXfer();

            //Guardem la imatge
            OFCondition cond = cbdata->dcmff->saveFile( qPrintable( imagePath ) , xfer , opt_sequenceType , opt_groupLength ,
            opt_paddingType , (Uint32)opt_filepad , (Uint32)opt_itempad , !opt_useMetaheader );

            if ( cond.bad() )
            {
                piSingleton->setError( retrievedImage.getStudyUID() );
                rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
            }

#ifndef QT_NO_DEBUG
            if ( settings.getLogCommunicationPacsVerboseMode() )
            {
                cout << "====================================== DICOM DATA OBJECT RETRIEVED ======================================" << endl;
               cbdata->dcmff->print( COUT );
            }
#endif
            //calculem la mida de l'image TODO alerta! això ens torna un Uint32! i ho guardem en un int
            imageSize = cbdata->dcmff->calcElementLength( xfer ,opt_sequenceType );

            DEBUG_LOG( QString("Image Size rebut: %1\n i 'stored' a la variable: %2 ").arg(cbdata->dcmff->calcElementLength( xfer ,opt_sequenceType ) ).arg(imageSize) );
            if( imageSize < 0 )
            {
                DEBUG_LOG( "La imatge amb: \nStudyUID:[" + retrievedImage.getStudyUID() +
                "],\nSeriesUID:["+ retrievedImage.getSeriesUID() +
                "] i\nImageUID:["+ retrievedImage.getSOPInstanceUID() +"]\n dóna mida negativa" );
                ERROR_LOG( "La imatge amb: \nStudyUID:[" + retrievedImage.getStudyUID() +
                "],\nSeriesUID:["+ retrievedImage.getSeriesUID() +
                "] i\nImageUID:["+ retrievedImage.getSOPInstanceUID() +"]\n dóna mida negativa" );
            }
            /* should really check the image to make sure it is consistent,
            * that its sopClass and sopInstance correspond with those in
            * the request.
            */
            if ( rsp->DimseStatus == STATUS_Success )
            {
            /* which SOP class and SOP instance ? */
                if (! DU_findSOPClassAndInstanceInDataSet( *imageDataSet , sopClass , sopInstance , opt_correctUIDPadding ) )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                }
                else if ( strcmp( sopClass , req->AffectedSOPClassUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                }
                else if ( strcmp( sopInstance , req->AffectedSOPInstanceUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setError( retrievedImage.getStudyUID() );
                }
            }

            //guardem la informacio que hem calculat nosaltres a l'objecte imatge
            retrievedImage.setImageName( cbdata->imageFileName );
            retrievedImage.setImagePath( qPrintable( imagePath ) );
            retrievedImage.setImageSize( imageSize );

            piSingleton->process( retrievedImage.getStudyUID() , &retrievedImage );
        }
    }

    return;
}

OFCondition storeSCP( T_ASC_Association *assoc , T_DIMSE_Message *msg , T_ASC_PresentationContextID presID )
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];
    req = &msg->msg.CStoreRQ;
    OFBool opt_useMetaheader = OFTrue; // I found its default value in movescu.cpp */
    StarviewerSettings settings;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
      cout << "Received " <<endl ;
      DIMSE_printCStoreRQ( stdout , req );
    }
#endif

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

/// Accepta la connexió que ens fa el PACS, per convertir-nos en un scp
OFCondition subOpSCP( T_ASC_Association **subAssoc )
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


void subOpCallback(void * /*subOpCallbackData*/ , T_ASC_Network *aNet , T_ASC_Association **subAssoc )
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

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "Move SCU RQ: MsgID " << msgId << endl;
        cout << "====================================== REQUEST ======================================" <<endl;
        m_mask->print( COUT );
    }
#endif

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

#ifndef QT_NO_DEBUG
    if ( cond == EC_Normal && settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "====================================== CMOVE-RSP ======================================" <<endl;
        DIMSE_printCMoveRSP( stdout , &rsp );
        if ( rspIds != NULL )
        {
            cout << "Response Identifiers:" << endl;
            rspIds->print(COUT);
        }
    }
#endif

    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
    {
#ifndef QT_NO_DEBUG
        if ( settings.getLogCommunicationPacsVerboseMode() )
        {
            cout << "====================================== STATUS-DETAIL ======================================" <<endl;
            statusDetail->print( COUT );
        }
#endif
        delete statusDetail;
    }

    if ( rspIds != NULL ) delete rspIds;

    state.setStatus( cond );

    return state;
}
}
