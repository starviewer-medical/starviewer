#include "retrieveimages.h"
#include "const.h"
#include "struct.h"
#include "processimagesingleton.h"
#include "status.h"

//includes per comprovar si un directori existeix
#include <sys/types.h>
#include <dirent.h>

namespace udg{

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

void RetrieveImages:: setMask( StudyMask Study )
{
    m_mask = Study.getMask();
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

void moveCallback( void *callbackData , T_DIMSE_C_MoveRQ */*request*/ , int /*responseCount*/ , T_DIMSE_C_MoveRSP */*response*/ )
{
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;

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
    std::string       seriesDir , studyDir;
    DIR *pdir;

    Image img;
    
      switch ( progress->state )
      {
        case DIMSE_StoreBegin:
          break;
        case DIMSE_StoreEnd:
          break;
        default:
          break;
      }

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
            ProcessImageSingleton* piSingleton; 
        
            //proces que farà el tractament de la imatge descarregada de la nostre aplicació, en el cas de l'starviewer guardar a la cache,i augmentara comptador des descarregats
            piSingleton=ProcessImageSingleton::getProcessImageSingleton();  
            
            const char* fileName = cbdata->imageFileName;
            const char *studyUID = NULL;
            const char *seriesUID = NULL;
            const char *SoPUID = NULL;
            const char *imageNumber = NULL;
            std::string pathfile;
            int imageSize;
        
            //Definim els TagKey per indicar que volem treure de la imatge
            DcmTagKey studyInstanceUIDTagKey( DCM_StudyInstanceUID ); //studyUID
            DcmTagKey seriesInstanceUIDTagKey( DCM_SeriesInstanceUID );//seriesUID
            DcmTagKey SOPInstanceUIDTagKey ( DCM_SOPInstanceUID );
            DcmTagKey imageNumberTagKey (DCM_InstanceNumber );    //número d'imatge    
            
            pathfile.insert( 0 , piSingleton->getPath() );//agafem el path del directori on es guarden les imatges
            
            //obtenim la informacio de la imatge
            OFCondition ec;
            ec = ( *imageDataSet )->findAndGetString( SOPInstanceUIDTagKey , SoPUID, OFFalse );
            ec = ( *imageDataSet )->findAndGetString( imageNumberTagKey , imageNumber, OFFalse );
            //recuperem l'estudi UID de la imatge, per saber el directori on l'hem de guardar
            ec = ( *imageDataSet )->findAndGetString( studyInstanceUIDTagKey , studyUID , OFFalse );
            
            studyDir = studyUID;
            
            pathfile.append( studyDir );
  
            //comprovem, si el directori de l'estudi ja està creat
            pdir = opendir( pathfile.c_str() );
            if ( !pdir )
            {
                mkdir( pathfile.c_str() , S_IRWXU | S_IRWXG | S_IRWXO );
            }
            else closedir( pdir );
            
            //obtenim a quina sèrie pertany la imatage per saber el directori on l'hem de guardar 
            ( *imageDataSet )->findAndGetString( seriesInstanceUIDTagKey , seriesUID , OFFalse );
        
            seriesDir = seriesUID;
        
            pathfile.append( "/" );
            pathfile.append( seriesDir );
            
            //comprovem, si el directori de la sèrie ja està creat
            pdir = opendir( pathfile.c_str()) ;
            if ( !pdir )
            {
                mkdir( pathfile.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
            }
            else closedir( pdir );                
        
            //acabem de concatenar el nom del fitxer
            pathfile.append("/");
            pathfile.append( fileName );
            
            E_TransferSyntax xfer = opt_writeTransferSyntax;
            if (xfer == EXS_Unknown) xfer = ( *imageDataSet )->getOriginalXfer();
        
            OFCondition cond = cbdata->dcmff->saveFile( pathfile.c_str() , xfer , opt_sequenceType , opt_groupLength ,
            opt_paddingType , (Uint32)opt_filepad , (Uint32)opt_itempad , !opt_useMetaheader );
            
            if ( cond.bad() )
            {
                piSingleton->setErrorRetrieving( studyUID );
                rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
            }         
            //calculem la mida de l'image
            imageSize = cbdata->dcmff->calcElementLength( xfer ,opt_sequenceType );
        
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
                    piSingleton->setErrorRetrieving( studyUID );
                }
                else if ( strcmp( sopClass , req->AffectedSOPClassUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setErrorRetrieving( studyUID );
                }
                else if ( strcmp( sopInstance , req->AffectedSOPInstanceUID ) != 0 )
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                    piSingleton->setErrorRetrieving( studyUID );
                }
            }   
        
            // si el numero d'imatges es null li posem 999999, per posteriorment nosaltres poder-ho saber
            if ( imageNumber == NULL ) imageNumber = "999999";
        
            //guardem la informacio a l'objecte imatge
            img.setStudyUID( studyUID );
            img.setSeriesUID( seriesUID );
            img.setSoPUID( SoPUID );
            img.setImageName( fileName );
            img.setImageNumber( atoi( imageNumber ) );
            img.setImagePath( pathfile.c_str() );
            img.setImageSize( imageSize ); 
            
            piSingleton->process( img.getStudyUID() ,&img );
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
    
    /* I found its default value in movescu.cpp */ 
    OFBool opt_useMetaheader = OFTrue;

    sprintf( imageFileName , "%s.%s" , dcmSOPClassUIDToModality( req->AffectedSOPClassUID ), req->AffectedSOPInstanceUID );

    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = imageFileName;
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

Status RetrieveImages::moveSCU()
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
        return state.setStatus( error_NoConnection );
    }
    
    //If not mask has been setted, return error, we need a search mask
    if ( m_mask == NULL )
    {
        return state.setStatus( error_NoMask );
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

    OFCondition cond = DIMSE_moveUser( m_assoc , presId , &req , m_mask ,
        moveCallback , &callbackData , DIMSE_BLOCKING , 0 ,
        m_net , subOpCallback , NULL ,
        &rsp , &statusDetail , &rspIds );

    if ( cond != EC_Normal ) 
    {
        DimseCondition::dump(cond);
    } 
    
    if ( rspIds != NULL ) delete rspIds;
        
    state.setStatus( cond );

    return state;
}
}
