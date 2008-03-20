#include "querypacs.h"

#include <assoc.h>
#include <dimse.h>
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h>
#include <diutil.h>

#include "pacsconnection.h"
#include "status.h"
#include "starviewersettings.h"
#include "studylistsingleton.h"
#include "serieslistsingleton.h"
#include "imagelistsingleton.h"
#include "pacsserver.h"
#include "dicommask.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "errordcmtk.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */

void QueryPacs::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
}

/*It's a callback function, can't own to the class, It's can be called if its belongs to the class, for this
  reason, it hasn't been declared in
the class pacsfind  */
/// This action is called for every patient that returns the find action. This is a callback action and inserts the found studies in the list study
void progressCallbackStudy(
        void * /*callbackData*/ ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers
        )
{
    const char* text;
    StarviewerSettings settings;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        /* dump response number */
        cout<< "RESPONSE: " << responseCount << "(" << DU_cfindStatusString( rsp->DimseStatus ) << ")" << endl;

        /* dump data set which was received */
        responseIdentifiers->print(COUT);

        /* dump delimiter */
        cout << "--------" << endl;
    }
#endif

    responseIdentifiers->findAndGetString( DCM_QueryRetrieveLevel , text , false );

    //Comprovem quin tipus d'objecte ens ha retorna el PACS i el transforme a un objecte del nostre tipus

    //en el cas que l'objecte que cercàvem fos un estudi afegi
    if ( strcmp( text ,"STUDY" ) == 0 )
    {
        DICOMStudy queriedStudy( responseIdentifiers );
        //gets the pointer to the study list and inserts the new study
        StudyListSingleton *studyList = StudyListSingleton::getStudyListSingleton();

        if ( !studyList->exists( queriedStudy.getStudyUID() , queriedStudy.getPacsAETitle() ) ) studyList->insert( DICOMStudy( responseIdentifiers ) );

    } //si la query retorna un objecte sèrie
    else if ( strcmp( text, "SERIES" ) == 0 )
    {
        DICOMSeries queriedSerie( responseIdentifiers );

        StudyListSingleton *studyList = StudyListSingleton::getStudyListSingleton();

        if ( !studyList->exists( queriedSerie.getStudyUID() , queriedSerie.getPacsAETitle() ) ) studyList->insert( DICOMStudy( responseIdentifiers ) );

        SeriesListSingleton *seriesList = SeriesListSingleton::getSeriesListSingleton();
        seriesList->insert( DICOMSeries( responseIdentifiers) );
    }// si la query retorna un objecte imatge
    else if ( strcmp( text , "IMAGE" ) == 0)
    {
        DICOMImage queriedImage( responseIdentifiers );

        StudyListSingleton *studyList = StudyListSingleton::getStudyListSingleton();

        if ( !studyList->exists( queriedImage.getStudyUID() , queriedImage.getPacsAETitle() ) ) studyList->insert( DICOMStudy( responseIdentifiers ) );

        SeriesListSingleton *seriesList = SeriesListSingleton::getSeriesListSingleton();
        if ( !seriesList->exists( queriedImage.getStudyUID() , queriedImage.getSeriesUID() , queriedImage.getPacsAETitle() ) )  seriesList->insert( DICOMSeries( responseIdentifiers ) );

        ImageListSingleton *imageList = ImageListSingleton::getImageListSingleton();
        imageList->insert( queriedImage );
    }
}

//Diem a quin nivell fem les cerques d'estudis! Molt important hem de fer a nivell de root
static const char *     opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

Status QueryPacs::query()
{
    DIC_US msgId = m_assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    Status state;
    StarviewerSettings settings;

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
    bzero( ( char* ) &req, sizeof( req ) );
    req.MessageID = msgId;
    strcpy( req.AffectedSOPClassUID , opt_abstractSyntax );
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "Find SCU RQ: MsgID " << msgId << endl;
        cout << "====================================== REQUEST ======================================" <<endl;
        m_mask->print( COUT );
    }
#endif

    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser( m_assoc , presId , &req , m_mask ,
                          progressCallbackStudy , NULL ,
                          DIMSE_BLOCKING , 0 ,
                          &rsp , &statusDetail );

#ifndef QT_NO_DEBUG
    if ( settings.getLogCommunicationPacsVerboseMode() )
    {
        cout << "====================================== CFIND-RSP ======================================" <<endl;
        DIMSE_printCFindRSP( stdout , &rsp );
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

    /* return */
    return state.setStatus( cond );
}

Status QueryPacs::query( DicomMask mask )
{
    m_mask = mask.getDicomMask();

    return query();
}

}
