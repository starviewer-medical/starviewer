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
#include "errordcmtk.h"
#include "image.h"
#include "study.h"
#include "series.h"
#include "patient.h"
#include "createinformationmodelobject.h"
#include "dicomtagreader.h"
#include "pacsmanager.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */

void QueryPacs::setConnection(QString pacsID, PacsConnection connection)
{
    m_assoc = connection.getPacsConnection();
    m_pacsID = pacsID;

    m_institutionPacs = PacsManager().queryPacs(pacsID).getInstitution();//Cerquem el nom de la Institució a la qual pertany el PACS
}

void QueryPacs::foundMatchCallback(
        void * callbackData ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers
        )
{
    QueryPacs* queryPacsCaller = (QueryPacs*)callbackData;
    DICOMTagReader *dicomTagReader = new DICOMTagReader("", responseIdentifiers);
    QString queryRetrieveLevel = dicomTagReader->getAttributeByName( DCM_QueryRetrieveLevel );

    //en el cas que l'objecte que cercàvem fos un estudi afegi
    if ( queryRetrieveLevel == "STUDY" )
    {
        queryPacsCaller->addPatientStudy( dicomTagReader );

    } //si la query retorna un objecte sèrie
    else if ( queryRetrieveLevel == "SERIES" )
    {
        queryPacsCaller->addPatientStudy( dicomTagReader );
        queryPacsCaller->addSeries( dicomTagReader );
    }// si la query retorna un objecte imatge
    else if ( queryRetrieveLevel == "IMAGE" )
    {
        queryPacsCaller->addPatientStudy( dicomTagReader );
        queryPacsCaller->addSeries( dicomTagReader );
        queryPacsCaller->addImage( dicomTagReader );
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

    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser( m_assoc , presId , &req , m_mask ,
                          foundMatchCallback , this ,
                          DIMSE_BLOCKING , 0 ,
                          &rsp , &statusDetail );

    /* dump status detail information if there is some */
    if ( statusDetail != NULL )
    {
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

void QueryPacs::addPatientStudy( DICOMTagReader *dicomTagReader )
{
    //Copiem a un altre dataset response pacs perquè dicomTagReader de DcmtkDatsetToStarviewerObject elimina l'objecte després de llegir-lo

    Patient *patient = CreateInformationModelObject::createPatient(dicomTagReader);
    Study *study = CreateInformationModelObject::createStudy(dicomTagReader);
    study->setInstitution(m_institutionPacs);

    patient->addStudy(study);

    m_patientStudyList.append(patient);
    m_hashPacsIDOfStudyInstanceUID[study->getInstanceUID()] = m_pacsID;//Afegim a la taula de QHash de quin pacs és l'estudi
}

void QueryPacs::addSeries( DICOMTagReader *dicomTagReader )
{
    Series *series = CreateInformationModelObject::createSeries(dicomTagReader);

    m_seriesList.append(series);
}

void QueryPacs::addImage( DICOMTagReader *dicomTagReader )
{
    Image *image = CreateInformationModelObject::createImage(dicomTagReader);

    m_imageList.append(image);
}

QList<Patient*> QueryPacs::getQueryResultsAsPatientStudyList()
{
    return m_patientStudyList;
}

QList<Series*> QueryPacs::getQueryResultsAsSeriesList()
{
    return m_seriesList;
}

QList<Image*> QueryPacs::getQueryResultsAsImageList()
{
    return m_imageList;
}

QHash<QString,QString> QueryPacs::getHashTablePacsIDOfStudyInstanceUID()
{
    return m_hashPacsIDOfStudyInstanceUID;
}

}
