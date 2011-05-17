#include "querypacs.h"

#include <dimse.h>
// Provide the information for the tags
#include <dcdeftag.h>
#include <ofcond.h>
#include <diutil.h>

#include "pacsconnection.h"
#include "dicommask.h"
#include "image.h"
#include "study.h"
#include "series.h"
#include "patient.h"
#include "createinformationmodelobject.h"
#include "dicomtagreader.h"
#include "logging.h"
#include "inputoutputsettings.h"

namespace udg {

//Constant que contindrà quin Abanstract Syntax de Find utilitzem entre els diversos que hi ha utilitzem
static const char *FindStudyAbstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;

QueryPacs::QueryPacs(PacsDevice pacsDevice)
{
    m_pacsDevice = pacsDevice;
}

void QueryPacs::foundMatchCallback(void *callbackData, T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp,
    DcmDataset *responseIdentifiers)
{
    Q_UNUSED(rsp);
    Q_UNUSED(responseCount);

    QueryPacs *queryPacsCaller = (QueryPacs*)callbackData;

    if (queryPacsCaller->m_cancelQuery)
    {
        // Hem de comprovar si ja haviem demanat cancel·lar la Query. És degut a que tot i que demanem cancel·lar la query actual
        // el PACS ens envia els dataset que havia posat a la pila de la xarxa just abans de rebre el cancel·lar la query, per tant
        // pot ser que tot i havent demanat cancel·lar la query rebem algun resultat més, per això comprovem si ja havíem demanat
        // cancel·lar la query per no tornar-la  demanar, quan rebem aquests resultats que ja s'havien posat a la pila de la xarxa.
        // http://forum.dcmtk.org/viewtopic.php?t=2143
        if (!queryPacsCaller->m_cancelRequestSent)
        {
            queryPacsCaller->cancelQuery(request);
            queryPacsCaller->m_cancelRequestSent = true;
        }
    }
    else
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader("", responseIdentifiers);
        QString queryRetrieveLevel = dicomTagReader->getValueAttributeAsQString(DICOMQueryRetrieveLevel);

        if (queryRetrieveLevel == "STUDY")
        {
            //en el cas que l'objecte que cercàvem fos un estudi
            queryPacsCaller->addPatientStudy(dicomTagReader);
        }
        else if (queryRetrieveLevel == "SERIES")
        {
            //si la query retorna un objecte sèrie
            queryPacsCaller->addPatientStudy(dicomTagReader);
            queryPacsCaller->addSeries(dicomTagReader);
        }
        else if (queryRetrieveLevel == "IMAGE")
        {
            // si la query retorna un objecte imatge
            queryPacsCaller->addPatientStudy(dicomTagReader);
            queryPacsCaller->addSeries(dicomTagReader);
            queryPacsCaller->addImage(dicomTagReader);
        }
    }
}

PACSRequestStatus::QueryRequestStatus QueryPacs::query()
{
    m_pacsConnection = new PACSConnection(m_pacsDevice);
    T_DIMSE_C_FindRQ findRequest;
    T_DIMSE_C_FindRSP findResponse;
    DcmDataset *statusDetail = NULL;

    if (!m_pacsConnection->connectToPACS(PACSConnection::Query))
    {
        ERROR_LOG("S'ha produit un error al intentar connectar al PACS per fer query. AETitle: " + m_pacsDevice.getAETitle());
        return PACSRequestStatus::QueryCanNotConnectToPACS;
    }

    // figure out which of the accepted presentation contexts should be used
    m_presId = ASC_findAcceptedPresentationContextID(m_pacsConnection->getConnection(), FindStudyAbstractSyntax);
    if (m_presId == 0)
    {
        ERROR_LOG("El PACS no ha acceptat el nivell de cerca d'estudis FINDStudyRootQueryRetrieveInformationModel");
        return PACSRequestStatus::QueryFailedOrRefused;
    }

    // prepare the transmission of data
    bzero((char*) &findRequest, sizeof(findRequest));
    findRequest.MessageID = m_pacsConnection->getConnection()->nextMsgID;
    strcpy(findRequest.AffectedSOPClassUID, FindStudyAbstractSyntax);
    findRequest.DataSetType = DIMSE_DATASET_PRESENT;

    // finally conduct transmission of data
    OFCondition condition = DIMSE_findUser(m_pacsConnection->getConnection(), m_presId, &findRequest, m_mask, foundMatchCallback, this, DIMSE_NONBLOCKING,
                                           Settings().getValue(InputOutputSettings::PACSConnectionTimeout).toInt(), &findResponse, &statusDetail);

    m_pacsConnection->disconnect();

    if (!condition.good())
    {
        ERROR_LOG(QString("Error al fer una consulta al PACS %1, descripcio error: %2").arg(m_pacsDevice.getAETitle(), condition.text()));
    }

    PACSRequestStatus::QueryRequestStatus queryRequestStatus = processResponseStatusFromFindUser(&findResponse, statusDetail);

    // dump status detail information if there is some
    if (statusDetail != NULL)
    {
        delete statusDetail;
    }

    return queryRequestStatus;
}

PACSRequestStatus::QueryRequestStatus QueryPacs::query(DicomMask mask)
{
    m_cancelQuery = false;
    m_cancelRequestSent = false;

    m_mask = mask.getDicomMask();

    return query();
}

void QueryPacs::cancelQuery()
{
    // Indiquem que s'ha de cancel·lar la query, el mètode foundMatchCallback, comprova el flag cada vegada que rep un resultat DICOM
    // que compleix amb la màscara de cerca
    m_cancelQuery = true;
}

void QueryPacs::cancelQuery(T_DIMSE_C_FindRQ *request)
{
    INFO_LOG(QString("Demanem cancel.lar al PACS %1 l'actual query").arg(m_pacsDevice.getAETitle()));

    //Tots els PACS està obligats pel DICOM Conformance a implementar la cancel·lació
    OFCondition cond = DIMSE_sendCancelRequest(m_pacsConnection->getConnection(), m_presId, request->MessageID);
    if (cond.bad())
    {
        ERROR_LOG("S'ha produit el seguent error al cancel.lar la query: " + QString(cond.text()));
        INFO_LOG(QString("Aborto la connexio amb el PACS %1").arg(m_pacsDevice.getAETitle()));

        //Si hi hagut un error demanant el cancel·lar, abortem l'associació, d'aquesta manera segur que cancel·lem la query
        ASC_abortAssociation(m_pacsConnection->getConnection());
    }
}

void QueryPacs::addPatientStudy(DICOMTagReader *dicomTagReader)
{
    if (!m_hashPacsIDOfStudyInstanceUID.contains(dicomTagReader->getValueAttributeAsQString(DICOMStudyInstanceUID)))
    {
        Patient *patient = CreateInformationModelObject::createPatient(dicomTagReader);
        Study *study = CreateInformationModelObject::createStudy(dicomTagReader);
        study->setInstitutionName(m_pacsDevice.getInstitution());

        patient->addStudy(study);
        m_patientStudyList.append(patient);
        // Afegim a la taula de QHash de quin pacs és l'estudi
        m_hashPacsIDOfStudyInstanceUID[study->getInstanceUID()] = m_pacsDevice.getID();
    }
}

void QueryPacs::addSeries(DICOMTagReader *dicomTagReader)
{
    Series *series = CreateInformationModelObject::createSeries(dicomTagReader);

    //TODO: Si ens fan una cerca a nivell d'imatge inserirem la mateixa serie tantes vegades com images tenim, s'hauria de comprovar si ja conté
    //la sèrie la llista abans d'afegir-la
    m_seriesList.append(series);
}

void QueryPacs::addImage(DICOMTagReader *dicomTagReader)
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

QHash<QString, QString> QueryPacs::getHashTablePacsIDOfStudyInstanceUID()
{
    return m_hashPacsIDOfStudyInstanceUID;
}

PACSRequestStatus::QueryRequestStatus QueryPacs::processResponseStatusFromFindUser(T_DIMSE_C_FindRSP *findResponse, DcmDataset *statusDetail)
{
    // Al PS 3.4, secció C.4.1.1.4, taula C.4-1 podem trobar un descripció dels errors.
    // Per a detalls sobre els "related fields" consultar PS 3.7, Annex C - Status Type Enconding

    if (findResponse->DimseStatus == STATUS_Success)
    {
        return PACSRequestStatus::QueryOk;
    }

    // Llista de camps relacionats amb l'error que poden contenir informació adicional
    QList<DcmTagKey> relatedFieldsList;
    QString messageErrorLog = "No s'ha pogut fer la cerca, descripció error rebuda: ";
    PACSRequestStatus::QueryRequestStatus queryRequestStatus;

    switch (findResponse->DimseStatus)
    {
        case STATUS_FIND_Refused_OutOfResources:
            // 0xa700
            // Refused: Out of Resources
            // Related Fields DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cfindStatusString(findResponse->DimseStatus)));
            queryRequestStatus = PACSRequestStatus::QueryFailedOrRefused;
            break;

        case STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass:
            // 0xa900
        case STATUS_FIND_Failed_UnableToProcess:
            // 0xc000
            // Identifier does not match SOP Class or Unable To Process
            // Related fields DCM_OffendingElement (0000,0901) DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_OffendingElement << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cfindStatusString(findResponse->DimseStatus)));
            queryRequestStatus = PACSRequestStatus::QueryFailedOrRefused;
            break;

        case STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest:
            // L'usuari ha sol·licitat cancel·lar la descàrrega
            queryRequestStatus = PACSRequestStatus::QueryCancelled;
            break;

        default:
            ERROR_LOG(messageErrorLog + QString(DU_cfindStatusString(findResponse->DimseStatus)));
            // S'ha produït un error no contemplat. En principi no s'hauria d'arribar mai a aquesta branca
            queryRequestStatus = PACSRequestStatus::QueryUnknowStatus;
            break;
    }

    if (statusDetail)
    {
        // Mostrem els detalls de l'status rebut, si se'ns han proporcionat
        if (!relatedFieldsList.isEmpty())
        {
            const char *text;
            INFO_LOG("Status details");
            foreach (DcmTagKey tagKey, relatedFieldsList)
            {
                // Fem un log per cada camp relacionat amb l'error amb el format
                // NomDelTag (xxxx,xxxx): ContingutDelTag
                statusDetail->findAndGetString(tagKey, text, false);
                INFO_LOG(QString(DcmTag(tagKey).getTagName()) + " " + QString(tagKey.toString().c_str()) + ": " + QString(text));
            }
        }
    }

    return queryRequestStatus;
}

}
