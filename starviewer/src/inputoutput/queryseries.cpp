#include "queryseries.h"
#include <dcdeftag.h> //provide the information for the tags
#include "serieslist.h"
#include "pacsconnection.h"
#include <string.h>
#include "series.h"

namespace udg{

QuerySeries::QuerySeries( PacsConnection connection , SeriesMask series )
{
    m_assoc = connection.getPacsConnection();
    m_mask = series.getSeriesMask();
}

void QuerySeries::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
}

void QuerySeries:: setMask( SeriesMask *series )
{
    m_mask = series->getSeriesMask();

}

/*It's a callback function, can't own to the class, It's can be called if its belongs to the class, for this
  reason, it hasn't been declared in this class  */
/// This action is called for every series that returns the find action. This is a callback action and inserts the matched series in the list series
void progressCallbackSeries(
        void * /*callbackData*/ ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int /*responseCount*/,
        T_DIMSE_C_FindRSP */*rsp*/,
        DcmDataset *responseIdentifiers
        )
{        
    Series series;
    const char* text;
    SeriesListSingleton* seriesListSingleton; 
    std::string path;
    
    //el path el construirem nosaltres, estarà format per UIDStudy/UIDSeries, aquest sempre sera el path on s'hauran de guardar les series
    
    //set the series number
    responseIdentifiers->findAndGetString( DCM_SeriesNumber , text , false );
    if ( text != NULL ) series.setSeriesNumber( text );

    //set series date
    responseIdentifiers->findAndGetString( DCM_SeriesDate , text , false );
    if ( text != NULL ) series.setSeriesDate( text );
        
    //set series description
    responseIdentifiers->findAndGetString( DCM_SeriesDescription , text , false );
    if ( text != NULL ) series.setSeriesDescription( text );
    
    //set Study UID
     responseIdentifiers->findAndGetString( DCM_StudyInstanceUID , text , false );
     if ( text != NULL ) 
     {   
         series.setStudyUID( text );     
         path.insert( 0 , text );
         path.append( "/" );
     }
     //set series modality
     responseIdentifiers->findAndGetString( DCM_Modality , text , false );
     if ( text != NULL ) series.setSeriesModality( text ); 

     //set series time    
     responseIdentifiers->findAndGetString( DCM_SeriesTime ,text , false );
     if ( text != NULL ) series.setSeriesTime( text );
     
     //set series UID
     responseIdentifiers->findAndGetString( DCM_SeriesInstanceUID , text , false );
     if ( text != NULL )
     {
         series.setSeriesUID( text );
         path.append( text );
         path.append( "/" );
     }
     
     responseIdentifiers->findAndGetString( DCM_BodyPartExamined , text , false );
     if ( text != NULL ) series.setBodyPartExaminated( text );

     responseIdentifiers->findAndGetString(DCM_ProtocolName , text , false );
     if ( text != NULL ) series.setProtocolName( text );     

     responseIdentifiers->findAndGetString( DCM_OperatorsName , text , false );
     if ( text != NULL ) series.setOperatorName( text );
     
     //inserim el path
     
     series.setSeriesPath(path.c_str());
                    
    //gets the pointer to the series list and inserts the new serie    
    seriesListSingleton = SeriesListSingleton::getSeriesListSingleton();
    seriesListSingleton->insert( series );
}

static const char *     opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

Status QuerySeries::find()

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


    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser( m_assoc , presId , &req , m_mask ,  //dcmtk lib call
                          progressCallbackSeries ,NULL ,
                          DIMSE_BLOCKING , 0,
                          &rsp , &statusDetail );
    
    /* dump status detail information if there is some */
    if ( statusDetail != NULL ) {
        delete statusDetail;
    }

    /* return */
    return state.setStatus( cond );
}

SeriesListSingleton* QuerySeries::getSeriesList()
{
    m_seriesListSingleton = SeriesListSingleton::getSeriesListSingleton();
    m_seriesListSingleton->firstSeries();
    return m_seriesListSingleton;
}

}

