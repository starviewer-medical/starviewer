/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <string>
#include "status.h"
#include "imagelist.h"
#include "storeimages.h"
#include "processimagesingleton.h"

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
    StarviewerSettings settings;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        switch ( progress->state )
        {
            case DIMSE_StoreBegin:
                cout << "TRANSMITTING:";
                break;
            case DIMSE_StoreEnd:
                cout << endl;
                break;
            default:
                cout << "." ;
                break;
            }
        fflush(stdout);
    }
#endif
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
    StarviewerSettings settings;

    OFBool unsuccessfulStoreEncountered = OFTrue; // assumption

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "--------------------------\n";
        cout << "Sending file: " << fname << endl;
    }
#endif

    /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
    /* which is encapsulated in the file will be available through the DcmFileFormat object. */
    /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
    /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile( fname );

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "====================================== DICOM DATA OBJECT TO STORE ======================================" << endl;
        dcmff.print( COUT );
        cout << "-----------" << endl;
    }
#endif

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

    /* if required, dump general information concerning transfer syntaxes */
#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
        T_ASC_PresentationContext pc;
        ASC_findAcceptedPresentationContext(assoc->params, presId, &pc);
        DcmXfer netTransfer(pc.acceptedTransferSyntax);
        cout << "Transfer: " << dcmFindNameOfUID(fileTransfer.getXferID()) << " -> " <<  dcmFindNameOfUID(netTransfer.getXferID()) << endl;
    }
#endif

    /* prepare the transmission of data */
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , sopClass );
    strcpy( req.AffectedSOPInstanceUID , sopInstance );
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "Store SCU RQ: MsgID " << msgId << ", (" << dcmSOPClassUIDToModality(sopClass) << ")" << endl;
    }
#endif

    cond = DIMSE_storeUser( assoc , presId , &req , NULL , dcmff.getDataset() , progressCallback , NULL ,  DIMSE_BLOCKING , 0 , &rsp , &statusDetail , NULL , DU_fileSize( fname ) );

    /*
     * If store command completed normally, with a status
     * of success or some warning then the image was accepted.
     */
    if ( cond == EC_Normal && ( rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS( rsp.DimseStatus ) ) ) unsuccessfulStoreEncountered = OFFalse;

    m_lastStatusCode = rsp.DimseStatus;

#ifndef QT_NO_DEBUG
    if ( cond == EC_Normal && settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "====================================== CSTORE-RSP ======================================" <<endl;
        DIMSE_printCStoreRSP( stdout , &rsp );
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
    return cond;
}

Status StoreImages::store( ImageList imageList )
{
    OFCondition cond = EC_Normal;
    Status state;
    ProcessImageSingleton* piSingleton;
    char hexadecimalCodeError[6];
    std::string statusMessage;

    //proces que farà el tractament de la imatge enviada des de la nostra aplicació, en el cas de l'starviewer informar a QOperationStateScreen que s'ha guardar una imatge més
    piSingleton=ProcessImageSingleton::getProcessImageSingleton();

    imageList.firstImage();
    while ( !imageList.end() &&  !cond.bad() && m_lastStatusCode == STATUS_Success )
    {
        cond = storeSCU( m_assoc , imageList.getImage().getImagePath().c_str() );
        piSingleton->process( imageList.getImage().getStudyUID() , &imageList.getImage() );
        imageList.nextImage();
    }

    /*aquest codi és un altre que s'ha de comprovar que no s'hi hagi produït cap error, el retorna
    el cstore, aquest codi està codificat en format hexadecimal a dcmtkxxx/dcmnet/include/dcmtk/dcmnet/dimse.h
    en allà es pot descodificar i saber quin error, per això si es produeix aquest error hem d'anar
    a dimse.h i mirar quina codifació té*/

    if ( m_lastStatusCode != STATUS_Success )
    {
        sprintf(hexadecimalCodeError, "%x" , m_lastStatusCode);
        statusMessage = "Error ";
        statusMessage.append( hexadecimalCodeError );
        statusMessage += " al fer el store de la imatge ";
        statusMessage += " per coneixer el significat de l'error consultar el fitxer ";
        statusMessage += "dcmtkxxx/dcmnet/include/dcmtk/dcmnet/dimse.h";
        state.setStatus( statusMessage, false , 1400 );

        return state;
    }
    else return state.setStatus( cond );
}

StoreImages::~StoreImages()
{

}

}
