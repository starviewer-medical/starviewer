/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "storeimages.h"

#include <diutil.h>
#include <dcfilefo.h>
#include <ofconapp.h>
#include <assoc.h>

#include <QDir>

#include "status.h"
#include "processimagesingleton.h"
#include "logging.h"
#include "pacsconnection.h"
#include "starviewersettings.h"
#include "errordcmtk.h"
#include "image.h"
#include "series.h"
#include "study.h"

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */
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

    /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
    /* which is encapsulated in the file will be available through the DcmFileFormat object. */
    /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
    /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile( qPrintable( QDir::toNativeSeparators( fname ) ) );

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

    m_lastStatusCode = rsp.DimseStatus;

    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
    {
        delete statusDetail;
    }
    return cond;
}

Status StoreImages::store( QList<Image*> imageListToStore )
{
    OFCondition cond = EC_Normal;
    Status state;
    ProcessImageSingleton* piSingleton;
    QString statusMessage;

    //If not connection has been setted, return error because we need a PACS connection
    if ( m_assoc == NULL )
    {
        return state.setStatus( DcmtkNoConnectionError );
    }

    //proces que farà el tractament de la imatge enviada des de la nostra aplicació, en el cas de l'starviewer informar a QOperationStateScreen que s'ha guardar una imatge més
    piSingleton=ProcessImageSingleton::getProcessImageSingleton();

    foreach(Image *imageToStore, imageListToStore)
    {
        cond = storeSCU( m_assoc, qPrintable(imageToStore->getPath()) );
        piSingleton->process(imageToStore->getParentSeries()->getParentStudy()->getInstanceUID(), imageToStore);

        if ( m_lastStatusCode != STATUS_Success ) 
        {
            ERROR_LOG( QString("Error %1 al fer el store de la imatge " ).arg( m_lastStatusCode ) + imageToStore->getPath() );
            break;
        }
    }

    /*aquest codi és un altre que s'ha de comprovar que no s'hi hagi produït cap error, el retorna
    el cstore, aquest codi està codificat en format hexadecimal a dcmtkxxx/dcmnet/include/dcmtk/dcmnet/dimse.h
    en allà es pot descodificar i saber quin error, per això si es produeix aquest error hem d'anar
    a dimse.h i mirar quina codifació té*/

    if ( m_lastStatusCode != STATUS_Success )
    {
        state.setStatus( QString("Error %1 al fer el store de les imatges " ).arg( m_lastStatusCode ), false, 1400 );
        return state;
    }
    else return state.setStatus( cond );
}

StoreImages::~StoreImages()
{

}

}
