#include "querypacs.h"

#include <dimse.h>
#include <ofcond.h>
#include <diutil.h>
#include <dcsequen.h>

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

// Constant que contindrà quin Abanstract Syntax de Find utilitzem entre els diversos que hi ha utilitzem
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

    QueryPacs *queryPacsCaller = (QueryPacs *)callbackData;

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
            // En el cas que l'objecte que cercàvem fos un estudi
            queryPacsCaller->addPatientStudy(dicomTagReader);
        }
        else if (queryRetrieveLevel == "SERIES")
        {
            // Si la query retorna un objecte sèrie
            queryPacsCaller->addPatientStudy(dicomTagReader);
            queryPacsCaller->addSeries(dicomTagReader);
        }
        else if (queryRetrieveLevel == "IMAGE")
        {
            // Si la query retorna un objecte imatge
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
    DcmDataset *dcmDatasetToQuery = ConvertDICOMMaskToDcmDataset(m_dicomMask);

    if (!m_pacsConnection->connectToPACS(PACSConnection::Query))
    {
        ERROR_LOG("S'ha produit un error al intentar connectar al PACS per fer query. AETitle: " + m_pacsDevice.getAETitle());
        return PACSRequestStatus::QueryCanNotConnectToPACS;
    }

    // Figure out which of the accepted presentation contexts should be used
    m_presId = ASC_findAcceptedPresentationContextID(m_pacsConnection->getConnection(), FindStudyAbstractSyntax);
    if (m_presId == 0)
    {
        ERROR_LOG("El PACS no ha acceptat el nivell de cerca d'estudis FINDStudyRootQueryRetrieveInformationModel");
        return PACSRequestStatus::QueryFailedOrRefused;
    }

    // Prepare the transmission of data
    bzero((char *) &findRequest, sizeof(findRequest));
    findRequest.MessageID = m_pacsConnection->getConnection()->nextMsgID;
    strcpy(findRequest.AffectedSOPClassUID, FindStudyAbstractSyntax);
    findRequest.DataSetType = DIMSE_DATASET_PRESENT;

    // Finally conduct transmission of data
    OFCondition condition = DIMSE_findUser(m_pacsConnection->getConnection(), m_presId, &findRequest, dcmDatasetToQuery, foundMatchCallback, this, DIMSE_NONBLOCKING,
                                           Settings().getValue(InputOutputSettings::PACSConnectionTimeout).toInt(), &findResponse, &statusDetail);

    m_pacsConnection->disconnect();

    if (!condition.good())
    {
        ERROR_LOG(QString("Error al fer una consulta al PACS %1, descripcio error: %2").arg(m_pacsDevice.getAETitle(), condition.text()));
    }

    PACSRequestStatus::QueryRequestStatus queryRequestStatus = processResponseStatusFromFindUser(&findResponse, statusDetail);

    // Dump status detail information if there is some
    if (statusDetail != NULL)
    {
        delete statusDetail;
    }
    delete dcmDatasetToQuery;

    return queryRequestStatus;
}

PACSRequestStatus::QueryRequestStatus QueryPacs::query(DicomMask mask)
{
    m_cancelQuery = false;
    m_cancelRequestSent = false;

    m_dicomMask = mask;

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

    // Tots els PACS està obligats pel DICOM Conformance a implementar la cancel·lació
    OFCondition cond = DIMSE_sendCancelRequest(m_pacsConnection->getConnection(), m_presId, request->MessageID);
    if (cond.bad())
    {
        ERROR_LOG("S'ha produit el seguent error al cancel.lar la query: " + QString(cond.text()));
        INFO_LOG(QString("Aborto la connexio amb el PACS %1").arg(m_pacsDevice.getAETitle()));

        // Si hi hagut un error demanant el cancel·lar, abortem l'associació, d'aquesta manera segur que cancel·lem la query
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

    // TODO: Si ens fan una cerca a nivell d'imatge inserirem la mateixa serie tantes vegades com images tenim, s'hauria de comprovar si ja conté
    // la sèrie la llista abans d'afegir-la
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

DcmDataset* QueryPacs::ConvertDICOMMaskToDcmDataset(DicomMask dicomMask)
{
    DcmDataset *maskDcmDataset = new DcmDataset();

    // Especifiquem que per defecte l'Specific character set utilitzat per fer les consultes cap al PACS serà ISO_IR 100, és a dir Latin 1, ja que els PACS
    // que utilitza l'IDI utilitzen aquesta codificació (és el que suporta dcm4chee), a més amb Latin1 és la codificació que utilitzen
    // la majoria de països europeus. Per dubtes consultar C.12.1.1.2 on s'especifiquen quins Specific characters set, també és important
    // consultar el conformance statement del PACS contra el que consultem per saber quin Specific character set suporta. Com que el character set és Latin1
    // haurem de transformar tots el tags dicom que siguin string (SH, LO, ST, PN, LT, UT) a Latin1
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SpecificCharacterSet, "ISO_IR 100");

    // Especifiquem a quin nivell es fa el QueryRetrieve
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_QueryRetrieveLevel, getQueryLevelFromDICOMMask(dicomMask));

    //Afegim els tags d'estudi
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PatientID, dicomMask.getPatientId());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PatientsName, dicomMask.getPatientName());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PatientsBirthDate, dicomMask.getPatientBirth());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PatientsSex, dicomMask.getPatientSex());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PatientsAge, dicomMask.getPatientAge());

    //Afegim els tags de sèrie
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_StudyInstanceUID, dicomMask.getStudyInstanceUID());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_StudyID, dicomMask.getStudyId());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_StudyDescription, dicomMask.getStudyDescription());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_ModalitiesInStudy, dicomMask.getStudyModality());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_StudyDate, dicomMask.getStudyDate());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_StudyTime, dicomMask.getStudyTime());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_AccessionNumber, dicomMask.getAccessionNumber());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_ReferringPhysiciansName, dicomMask.getReferringPhysiciansName());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SeriesNumber, dicomMask.getSeriesNumber());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SeriesDate, dicomMask.getSeriesDate());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_Modality, dicomMask.getSeriesModality());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SeriesTime, dicomMask.getSeriesTime());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SeriesDescription, dicomMask.getSeriesDescription());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_ProtocolName, dicomMask.getSeriesProtocolName());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SeriesInstanceUID, dicomMask.getSeriesInstanceUID());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PerformedProcedureStepStartDate, dicomMask.getPPSStartDate());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_PerformedProcedureStepStartTime, dicomMask.getPPSStartTime());

    if (!dicomMask.getRequestedProcedureID().isNull() || !dicomMask.getScheduledProcedureStepID().isNull())
    {
        DcmItem *requestedAttributeSequenceItem = new DcmItem(DCM_Item);
        requestedAttributeSequenceItem->putAndInsertString(DCM_RequestedProcedureID, dicomMask.getRequestedProcedureID().toLatin1().data());
        requestedAttributeSequenceItem->putAndInsertString(DCM_ScheduledProcedureStepID, dicomMask.getScheduledProcedureStepID().toLatin1().data());

        DcmSequenceOfItems *requestedAttributeSequence = new DcmSequenceOfItems(DCM_RequestAttributesSequence);
        requestedAttributeSequence->insert(requestedAttributeSequenceItem);
        maskDcmDataset->insert(requestedAttributeSequence, OFTrue);
    }

    //Afegim els tags d'imatges    
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_SOPInstanceUID, dicomMask.getSOPInstanceUID());
    AddTagToDcmDatsetAsString(maskDcmDataset, DCM_InstanceNumber, dicomMask.getImageNumber());

    return maskDcmDataset;
}

void QueryPacs::AddTagToDcmDatsetAsString(DcmDataset *dcmDataset, DcmTagKey dcmTagKey, QString tagValue)
{
    if (!tagValue.isNull())
    {
        DcmElement *elem = newDicomElement(dcmTagKey);
        elem->putString(tagValue.toLatin1().data());
        dcmDataset->insert(elem, OFTrue);
    }
}

QString QueryPacs::getQueryLevelFromDICOMMask(DicomMask dicomMask)
{
    bool isImageLevel = !dicomMask.getSOPInstanceUID().isNull() || !dicomMask.getImageNumber().isNull();
    bool isSeriesLevel = !dicomMask.getSeriesDescription().isNull() || !dicomMask.getSeriesDate().isNull() || !dicomMask.getSeriesModality().isNull() ||
                         !dicomMask.getSeriesNumber().isNull() || !dicomMask.getSeriesProtocolName().isNull() || !dicomMask.getSeriesTime().isNull() ||
                         !dicomMask.getSeriesInstanceUID().isNull() || !dicomMask.getRequestedProcedureID().isNull() || !dicomMask.getScheduledProcedureStepID().isNull() ||
                         !dicomMask.getPPSStartDate().isNull() || !dicomMask.getPPSStartTime().isNull();

    if (isImageLevel)
    {
        return "IMAGE";
    }
    else if (isSeriesLevel)
    {
        return "SERIES";
    }
    else
    {
        //Per defecte com a mínim són a nivell d'estudi
        return "STUDY";
    }
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
