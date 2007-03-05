#include "querystudy.h"
#include <dcdeftag.h> //provide the information for the tags
#include "studylist.h"
#include "pacsconnection.h"
#include "study.h"
#include "status.h"


#ifndef CONST
#include "const.h"
#endif

namespace udg{

QueryStudy::QueryStudy( PacsConnection connection , StudyMask study )
{
    m_assoc = connection.getPacsConnection();
    m_mask = study.getMask();
}

void QueryStudy::setConnection( PacsConnection connection )
{
    m_assoc = connection.getPacsConnection();
    //gets a pointer to list study, where the result of the search will be insert
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
}

void QueryStudy:: setMask( StudyMask study )
{
    m_mask = study.getMask();
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
    Study study;
    const char* text;
    StudyListSingleton *studyList;
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

    //set the patient Name
    responseIdentifiers->findAndGetString( DCM_PatientsName , text , false );
    if ( text != NULL ) study.setPatientName( text );

    //set Patient's Birth Date
    responseIdentifiers->findAndGetString( DCM_PatientsBirthDate , text , false );
    if ( text != NULL ) study.setPatientBirthDate( text );

    //set Patient's Id
    responseIdentifiers->findAndGetString( DCM_PatientID , text , false );
    if ( text != NULL ) study.setPatientId( text );

    //set Patient's Sex
    responseIdentifiers->findAndGetString( DCM_PatientsSex , text , false );
    if ( text != NULL ) study.setPatientSex( text );

    //set Patiens Age
    responseIdentifiers->findAndGetString( DCM_PatientsAge , text , false );
    if ( text != NULL ) study.setPatientAge( text );

    //set Study ID
    responseIdentifiers->findAndGetString( DCM_StudyID , text , false );
    if ( text != NULL ) study.setStudyId( text );

    //set Study Date
    responseIdentifiers->findAndGetString( DCM_StudyDate , text , false );
    if ( text != NULL ) study.setStudyDate( text );

     //set Study Description
    responseIdentifiers->findAndGetString( DCM_StudyDescription , text , false );
    if ( text != NULL ) study.setStudyDescription( text );

    //set Study Time
    responseIdentifiers->findAndGetString( DCM_StudyTime , text , false );
    if ( text != NULL ) study.setStudyTime( text );

    //set Institution Name
    responseIdentifiers->findAndGetString( DCM_InstitutionName , text , false );
    if ( text != NULL ) study.setInstitutionName( text );

    //set StudyUID
    responseIdentifiers->findAndGetString( DCM_StudyInstanceUID , text , false );
    if ( text != NULL ) study.setStudyUID( text );

    //set Accession Number
    responseIdentifiers->findAndGetString( DCM_AccessionNumber , text , false );
    if ( text != NULL ) study.setAccessionNumber( text );

    //set Study Modality
    responseIdentifiers->findAndGetString( DCM_ModalitiesInStudy, text , false );
    if ( text != NULL ) study.setStudyModality( text );

    //set PACS AE Title Called
    responseIdentifiers->findAndGetString( DCM_RetrieveAETitle , text , false );
    if ( text != NULL ) study.setPacsAETitle( text );

    //gets the pointer to the study list and inserts the new study
    studyList = StudyListSingleton::getStudyListSingleton();
    studyList->insert( study );

}

//Diem a quin nivell fem les cerques d'estudis! Molt important hem de fer a nivell de root
static const char *     opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

Status QueryStudy::find()
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

StudyListSingleton* QueryStudy::getStudyList()
{
    m_studyListSingleton->firstStudy();
    return m_studyListSingleton;
}

}

