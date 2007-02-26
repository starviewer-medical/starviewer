#include "queryimagenumber.h"
#include <dcdeftag.h> //provide the information for the tags
#include "serieslist.h"
#include "pacsconnection.h"
#include "imagemask.h"
#include "status.h"

namespace udg{

int imageNumberGlobal;

QueryImageNumber::QueryImageNumber( PacsConnection connection , ImageMask image )
{
    m_assoc = connection.getPacsConnection();
    m_mask = image.getImageMask();
}

void QueryImageNumber::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
}

void QueryImageNumber:: setMask( ImageMask* image )
{
    m_mask = image->getImageMask();
}

/*It's a callback function, can't own to the class, It's can be called if its belongs to the class, for this
  reason, it hasn't been declared in this class  */
/// This action is called for every image that returns the count action. Count the number of images
void progressCallbackCountImages(
        void * /*callbackData*/ ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int /*responseCount*/,
        T_DIMSE_C_FindRSP */*rsp*/,
        DcmDataset */*responseIdentifiers*/
        )
{
    imageNumberGlobal++;
}

static const char *     opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

Status QueryImageNumber::count()
{
    DIC_US msgId = m_assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
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

    /* figure out which of the accepted presentation contexts should be used */
    presId = ASC_findAcceptedPresentationContextID( m_assoc , UID_FINDStudyRootQueryRetrieveInformationModel );
    if ( presId == 0 )
    {
        return state.setStatus( DIMSE_NOVALIDPRESENTATIONCONTEXTID );
    }

    /* prepare the transmission of data */
    bzero( ( char* ) &req , sizeof( req ) );
    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , opt_abstractSyntax );
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    imageNumberGlobal = 0 ;
    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser( m_assoc , presId , &req , m_mask ,                     progressCallbackCountImages , NULL , DIMSE_BLOCKING , 0 , &rsp , &statusDetail );
    m_imageNumber = imageNumberGlobal;
    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
    {
        delete statusDetail;
    }

    /* return */
    return state.setStatus( cond );
}

int QueryImageNumber::getImageNumber()
{
    return m_imageNumber;
}

}



