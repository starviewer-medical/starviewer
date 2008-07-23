#include "querypacs.h"

#include <assoc.h>
#include <dimse.h>
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h>
#include <diutil.h>

#include "pacsconnection.h"
#include "status.h"
#include "starviewersettings.h"
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

void QueryPacs::foundMatchCallback(
        void * callbackData ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers
        )
{
    const char* text;
    StarviewerSettings settings;
    QueryPacs* queryPacsCaller = (QueryPacs*)callbackData;

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
        queryPacsCaller->addStudy( responseIdentifiers );

    } //si la query retorna un objecte sèrie
    else if ( strcmp( text, "SERIES" ) == 0 )
    {
        queryPacsCaller->addStudy( responseIdentifiers );
        queryPacsCaller->addSeries( responseIdentifiers );
    }// si la query retorna un objecte imatge
    else if ( strcmp( text , "IMAGE" ) == 0)
    {
        queryPacsCaller->addStudy( responseIdentifiers );
        queryPacsCaller->addSeries( responseIdentifiers );
        queryPacsCaller->addImage( responseIdentifiers );
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
        return state.setStatus( DcmtkNoConnectionError );
    }

    //If not mask has been setted, return error, we need a search mask
    if ( m_mask == NULL )
    {
        return state.setStatus( DcmtkNoMaskError );
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
                          foundMatchCallback , this ,
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

void QueryPacs::addStudy( DcmDataset *responsePacs )
{
    DICOMStudy dicomStudy( responsePacs );

    ///Alguns pacs no retornen a quin pacs pertany l'estudi, nosaltres per defecte hi posem el nom del pacs al que hem fer la query
    if ( dicomStudy.getPacsAETitle().isEmpty() ) dicomStudy.setPacsAETitle( m_assoc->params->DULparams.calledAPTitle );

    if ( !m_studiesList.contains( dicomStudy ) ) m_studiesList.append( dicomStudy );
}

void QueryPacs::addSeries( DcmDataset * responsePacs )
{
    DICOMSeries dicomSeries( responsePacs );

    if ( !m_seriesList.contains( dicomSeries) ) m_seriesList.append( dicomSeries );
}

void QueryPacs::addImage( DcmDataset * responsePacs )
{
    m_imageList.append( DICOMImage( responsePacs ) );
}

QList<DICOMStudy> QueryPacs::getQueryResultsAsStudyList()
{
    return m_studiesList;
}

QList<DICOMSeries> QueryPacs::getQueryResultsAsSeriesList()
{
    return m_seriesList;
}

QList<DICOMImage> QueryPacs::getQueryResultsAsImageList()
{
    return m_imageList;
}
}
