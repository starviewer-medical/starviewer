#include "querypacs.h"

#include <assoc.h>
#include <dimse.h>
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h>
#include <diutil.h>

#include "pacsconnection.h"
#include "status.h"
#include "pacsserver.h"
#include "dicommask.h"
#include "errordcmtk.h"
#include "image.h"
#include "study.h"
#include "series.h"
#include "patient.h"
#include "createinformationmodelobject.h"
#include "dicomtagreader.h"
#include "dicomdictionary.h"
#include "pacsdevicemanager.h"
#include "logging.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */

void QueryPacs::setConnection(PacsServer pacsServer)
{
    m_assoc = pacsServer.getConnection().getPacsConnection();
    m_pacs = pacsServer.getPacs();
}

void QueryPacs::foundMatchCallback(
        void * callbackData ,
        T_DIMSE_C_FindRQ * request,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers
        )
{
    QueryPacs* queryPacsCaller = (QueryPacs*)callbackData;

    if (queryPacsCaller->m_cancelQuery)
    {
        /*Hem de comprovar si ja haviem demanat cancel·lar la Query. És degut a que tot i que demanem cancel·lar la query actual
          el PACS ens envia els dataset que havia posat a la pila de la xarxa just abans de rebre el cancel·lar la query, per això 
         pot ser que tot i havent demanat cancel·lar la query rebem algun resultat més, per això comprovem si ja havíem demant
         cancel·lar la query per tornar-la  demanar, quan rebem aquests resultats que ja s'havien posat a la pila de la xarxa.
        http://forum.dcmtk.org/viewtopic.php?t=2143
        */
        if (!queryPacsCaller->m_cancelRequestSent)
        {
            queryPacsCaller->cancelQuery(request);
            queryPacsCaller->m_cancelRequestSent = true;
        }
    }
    else
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader("", responseIdentifiers);
        QString queryRetrieveLevel = dicomTagReader->getAttributeByName( DICOMQueryRetrieveLevel );

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
}

//Diem a quin nivell fem les cerques d'estudis! Molt important hem de fer a nivell de root
static const char *     opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

Status QueryPacs::query()
{
    DIC_US msgId = m_assoc->nextMsgID++;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    Status state;

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
    m_presId = ASC_findAcceptedPresentationContextID( m_assoc , UID_FINDStudyRootQueryRetrieveInformationModel );
    if ( m_presId == 0 )
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
    OFCondition cond = DIMSE_findUser( m_assoc , m_presId , &req , m_mask ,
                          foundMatchCallback , this ,
                          DIMSE_NONBLOCKING , PacsDevice::getConnectionTimeout() ,
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
    m_cancelQuery = false;

    m_mask = mask.getDicomMask();

    return query();
}

void QueryPacs::cancelQuery()
{
    /*Indiquem que s'ha de cancel·lar la query, el mètode foundMatchCallback, comprova el flag cada vegada que rep un resultat DICOM
     *que compleix amb la màscara de cerca*/
    m_cancelQuery = true;
}

void QueryPacs::cancelQuery(T_DIMSE_C_FindRQ *request)
{
    INFO_LOG(QString("Demanem cancel·lar al PACS %1 l'actual query").arg(m_pacs.getAETitle()));

    //Tots els PACS està obligats pel DICOM Conformance a implementar la cancel·lació
    OFCondition cond = DIMSE_sendCancelRequest(m_assoc, m_presId, request->MessageID);
    if (cond.bad())
    {
        ERROR_LOG("S'ha produït el següent error al cancel·lar la query: " + QString(cond.text()));
        INFO_LOG(QString("Aborto la connexió amb el PACS %1").arg(m_pacs.getAETitle()));

        //Si hi hagut un error demanant el cancel·lar, abortem l'associació, d'aquesta manera segur que cancel·lem la query
        ASC_abortAssociation(m_assoc);
    }
}

void QueryPacs::addPatientStudy( DICOMTagReader *dicomTagReader )
{
    if (!m_hashPacsIDOfStudyInstanceUID.contains(dicomTagReader->getAttributeByName(DICOMStudyInstanceUID)))
    {
        Patient *patient = CreateInformationModelObject::createPatient(dicomTagReader);
        Study *study = CreateInformationModelObject::createStudy(dicomTagReader);
        study->setInstitutionName(m_pacs.getInstitution());

        patient->addStudy(study);

        m_patientStudyList.append(patient);
        m_hashPacsIDOfStudyInstanceUID[study->getInstanceUID()] = m_pacs.getID();//Afegim a la taula de QHash de quin pacs és l'estudi
    }
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
