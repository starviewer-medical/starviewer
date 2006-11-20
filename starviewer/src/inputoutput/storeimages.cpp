/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <string>
#include "status.h"
#include "storeimages.h"

namespace udg {

StoreImages::StoreImages()
{
}

void StoreImages::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
}

void StoreImages::setNetwork ( T_ASC_Network * network )
{
    m_net = network;
}

static void progressCallback( void * /*callbackData*/ , T_DIMSE_StoreProgress *progress , T_DIMSE_C_StoreRQ * /*req*/)
{
    //no duem a terme cap accio
}

static OFCondition storeSCU( T_ASC_Association * assoc , const char *fname )
    /*
     * This function will read all the information from the given file,
     * figure out a corresponding presentation context which will be used
     * to transmit the information over the network to the SCP, and it
     * will finally initiate the transmission of all data to the SCP.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed.
     */
{
    DIC_US msgId = assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_StoreRQ req;
    T_DIMSE_C_StoreRSP rsp;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    DcmDataset *statusDetail = NULL;
    static int lastStatusCode;

    OFBool unsuccessfulStoreEncountered = OFTrue; // assumption

    /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
    /* which is encapsulated in the file will be available through the DcmFileFormat object. */
    /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
    /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile( fname );

    /* figure out if an error occured while the file was read*/
    if ( cond.bad() ) return cond;

    /* figure out which SOP class and SOP instance is encapsulated in the file */
    if ( !DU_findSOPClassAndInstanceInDataSet( dcmff.getDataset() , sopClass , sopInstance , OFFalse ) ) 
    {
        return DIMSE_BADDATA;
    }

    /* figure out which of the accepted presentation contexts should be used */
    DcmXfer filexfer( dcmff.getDataset()->getOriginalXfer() );

    /* special case: if the file uses an unencapsulated transfer syntax (uncompressed
     * or deflated explicit VR) and we prefer deflated explicit VR, then try
     * to find a presentation context for deflated explicit VR first.
     */

    if ( filexfer.getXfer() != EXS_Unknown ) 
    {
        presId = ASC_findAcceptedPresentationContextID( assoc , sopClass , filexfer.getXferID() );
    }
    else presId = ASC_findAcceptedPresentationContextID( assoc , sopClass );
    
    if ( presId == 0 ) 
    {
        const char *modalityName = dcmSOPClassUIDToModality( sopClass );
        
        if ( !modalityName ) modalityName = dcmFindNameOfUID( sopClass );
        
        if ( !modalityName ) modalityName = "unknown SOP class";
        
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* prepare the transmission of data */
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , sopClass );
    strcpy( req.AffectedSOPInstanceUID , sopInstance );
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    cond = DIMSE_storeUser( assoc , presId , &req , NULL , dcmff.getDataset() , progressCallback , NULL ,  DIMSE_BLOCKING , 0 , &rsp , &statusDetail , NULL , DU_fileSize( fname ) );

    /*
     * If store command completed normally, with a status
     * of success or some warning then the image was accepted.
     */
    if ( cond == EC_Normal && ( rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS( rsp.DimseStatus ) ) ) unsuccessfulStoreEncountered = OFFalse;

    /* remember the response's status for later transmissions of data */
    lastStatusCode = rsp.DimseStatus;

    /* dump some more general information */
    if ( cond != EC_Normal ) DimseCondition::dump( cond );

    /* dump status detail information if there is some */
    if ( statusDetail != NULL ) delete statusDetail;
    
    return cond;
}
 
Status StoreImages::store( list< std::string > imagePathList )
{
    list< std::string >::iterator iterator;
    
    iterator = imagePathList.begin();
    std::string imagePath;
    OFCondition cond;
    Status state;
    
    while ( iterator != imagePathList.end() )
    {
        imagePath = *( iterator );
        cond = storeSCU( m_assoc , imagePath.c_str() );
        iterator++;
    }
    
    return state.setStatus( cond );
}

StoreImages::~StoreImages()
{
}


}
