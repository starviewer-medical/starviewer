/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "senddicomfilestopacs.h"

#include <diutil.h>
#include <dcfilefo.h>
#include <assoc.h>
#include <dcmtk/dcmdata/dctagkey.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <QDir>

#include "logging.h"
#include "pacsconnection.h"
#include "image.h"
#include "pacsserver.h"
#include "pacsrequeststatus.h"
#include "status.h"

namespace udg {

SendDICOMFilesToPACS::SendDICOMFilesToPACS(PacsDevice pacsDevice)
{
    m_pacs = pacsDevice;
    m_abortIsRequested = false;
}

PacsDevice SendDICOMFilesToPACS::getPacs()
{
    return m_pacs;
}

PACSRequestStatus::SendRequestStatus SendDICOMFilesToPACS::send(QList<Image*> imageListToSend)
{
    PacsServer pacsServer( m_pacs );
    Status state;
    //TODO: S'hauria de comprovar que es tracti d'un PACS amb el servei d'store configurat
    state = pacsServer.connect( PacsServer::storeImages );
    
    if ( !state.good() )
    {
        ERROR_LOG( " S'ha produit un error al intentar connectar al PACS per fer un send. AETitle: " + m_pacs.getAETitle() + ", IP: " + m_pacs.getAddress() +
            ", port: " + QString().setNum(m_pacs.getStoreServicePort()) + ", Descripcio error : " + state.text() );
        return PACSRequestStatus::CanNotConnectPACSToSend;
    }

    initialitzeImagesCounters(imageListToSend.count());

    foreach(Image *imageToStore, imageListToSend)
    {
        if (m_abortIsRequested)
        {
            break;
        }
        
        INFO_LOG(QString("S'enviara al PACS %1 el fitxer %2").arg(m_pacs.getAETitle(), imageToStore->getPath()));
        if (storeSCU(pacsServer.getConnection(), qPrintable(imageToStore->getPath())))
        {
            emit DICOMFileSent(imageToStore, getNumberOfImagesSentSuccesfully() + this->getNumberOfImagesSentWarning());
        }
    }

    pacsServer.disconnect();

    return getStatusStoreSCU();
}

void SendDICOMFilesToPACS::requestCancel()
{
    m_abortIsRequested = true;
    INFO_LOG("Ens han demanat cancel·lar l'enviament d'imatges al PACS");
}

void SendDICOMFilesToPACS::initialitzeImagesCounters(int numberOfImagesToSend)
{
    //Inicialitzem els comptadors 

    m_numberOfSendImagesSuccessful = 0;
    m_numberOfSendImagesWithWarning = 0;
    m_numberOfImagesToSend = numberOfImagesToSend;
}

/*
 * This function will read all the information from the given file,
 * figure out a corresponding presentation context which will be used
 * to transmit the information over the network to the SCP, and it
 * will finally initiate the transmission of all data to the SCP.
 *
 * Parameters:
 *   association - [in] The associationiation (network connection to another DICOM application).
 *   filepathToStore - [in] Name of the file which shall be processed.
 */
bool SendDICOMFilesToPACS::storeSCU(PacsConnection pacsConnection, QString filepathToStore)
{
    T_ASC_Association * association = pacsConnection.getPacsConnection();
    DIC_US msgId = association->nextMsgID++;
    T_ASC_PresentationContextID presentationContextID;
    T_DIMSE_C_StoreRQ request;
    T_DIMSE_C_StoreRSP response;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    DcmDataset *statusDetail = NULL;
    DcmFileFormat dcmff;

    OFCondition condition = dcmff.loadFile(qPrintable(QDir::toNativeSeparators(filepathToStore)));

    /* figure out if an error occured while the file was read*/
    if (condition.bad()) 
    {
        ERROR_LOG("No s'ha pogut obrir el fitxer " + filepathToStore);
        return false;
    }
    /* figure out which SOP class and SOP instance is encapsulated in the file */
    if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(), sopClass, sopInstance, OFFalse))
    {
        ERROR_LOG("No s'ha pogut obtenir el SOPClass i SOPInstance del fitxer " + filepathToStore);
        return false;
    }

    /* figure out which of the accepted presentation contexts should be used */
    DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());

    //Busquem dels presentationContextID que hem establert al connectar quin és el que hem d'utilitzar per transferir aquesta imatge
    if (filexfer.getXfer() != EXS_Unknown)
    {
        presentationContextID = ASC_findAcceptedPresentationContextID(association, sopClass, filexfer.getXferID());
    }
    else presentationContextID = ASC_findAcceptedPresentationContextID(association, sopClass);

    if (presentationContextID == 0)
    {
        //No hem trobat cap presentation context vàlid dels que hem configuarat a la connexió pacsserver.cpp
        const char *modalityName = dcmSOPClassUIDToModality(sopClass);

        if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);

        if (!modalityName) modalityName = "unknown SOP class";

        ERROR_LOG("No s'ha trobat un presentation context vàlid en la connexió per la modalitat : " + QString(modalityName) 
                   + " amb la SOPClass " + QString(sopClass)  + " pel fitxer " + filepathToStore);

        return false;
    }
    else
    {
        /* prepare the transmission of data */
        bzero((char*)&request, sizeof(request));
        request.MessageID = msgId;
        strcpy(request.AffectedSOPClassUID, sopClass);
        strcpy(request.AffectedSOPInstanceUID, sopInstance);
        request.DataSetType = DIMSE_DATASET_PRESENT;
        request.Priority = DIMSE_PRIORITY_LOW;

        condition = DIMSE_storeUser(association, presentationContextID, &request, NULL /*imageFileName*/, dcmff.getDataset(), NULL /*progressCallback*/, 
            NULL /*callbackData */, DIMSE_NONBLOCKING, m_pacs.getConnectionTimeout(), &response, &statusDetail, NULL /*check for cancel parameters*/, 
            DU_fileSize(qPrintable(filepathToStore)));

        if (condition.bad())
        {
            ERROR_LOG("S'ha produït un error al fer el store de la imatge " + filepathToStore + ", descripció de l'error" + QString(condition.text()));
        }

        processResponseFromStoreSCP(&response, statusDetail, filepathToStore);

        if (statusDetail != NULL)
        {
            delete statusDetail;
        }

        return condition.good() && response.DimseStatus == STATUS_Success;
    }
}

void SendDICOMFilesToPACS::processResponseFromStoreSCP(T_DIMSE_C_StoreRSP *response, DcmDataset *statusDetail, QString filePathDicomObjectStoredFailed)
{
    QList< DcmTagKey > relatedFieldsList;// Llista de camps relacionats amb l'error que poden contenir informació adicional
    QString messageErrorLog = "No s'ha pogut enviar el fitxer " + filePathDicomObjectStoredFailed + ", descripció error rebuda";

    // A la secció B.2.3, taula B.2-1 podem trobar un descripció dels errors.
    // Per a detalls sobre els "related fields" consultar PS 3.7, Annex C - Status Type Enconding
    
    /*Tenir en compte també que el significat dels Status és diferent que els de MoveScu.
            - Failure la imatgen o s'ha pogut pujar
            - Warning la imatge s'ha pujat, però no condorcada la SOPClass, s'ha fet coerció d'algunes dades...*/

    if (response->DimseStatus == STATUS_Success)
    {
        //La imatge s'ha enviat correctament
        m_numberOfSendImagesSuccessful++;
        return;
    }

    switch(response->DimseStatus)
    {
        case STATUS_STORE_Refused_OutOfResources: // 0xA7XX
            // Refused: Out of Resources             
            // Related Fields DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cstoreStatusString(response->DimseStatus)));
            break;
        case STATUS_STORE_Refused_SOPClassNotSupported: //0x0122
        case STATUS_STORE_Error_DataSetDoesNotMatchSOPClass: //0xA9XX
        case STATUS_STORE_Error_CannotUnderstand: //0xCXXX
            //Error: Sop Class Not Supported or Data Set Doest Not Match SOP Class or Can not Understant
            // Related fields DCM_OffendingElement (0000,0901) DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_OffendingElement << DCM_ErrorComment;

            ERROR_LOG(messageErrorLog + QString(DU_cstoreStatusString(response->DimseStatus)));
            break;
        //coersió entre tipus, s'ha convertit un tipus a un altre tipus i es pot haver perdut dades, per exemple passar de decimal a enter
        case STATUS_STORE_Warning_CoersionOfDataElements: // 0xB000 
        case STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass: //0xB007
        case STATUS_STORE_Warning_ElementsDiscarded: //0xB006
            // Warning: Coersion Of Data Elements, Data set Dos Not Match SOP Class or Elements Discarded
            // Related fields DCM_OffendingElement (0000,0901) DCM_ErrorComment (0000,0902)
            relatedFieldsList << DCM_OffendingElement << DCM_ErrorComment;
            
            ERROR_LOG(messageErrorLog + QString(DU_cstoreStatusString(response->DimseStatus)));
            m_numberOfSendImagesWithWarning++;
            break;
        default:
            // S'ha produït un error no contemplat. En principi no s'hauria d'arribar mai a aquesta branca
            ERROR_LOG(messageErrorLog + QString(DU_cstoreStatusString(response->DimseStatus)));
            break;
    }

    if (statusDetail)
    {
        // Mostrem els detalls de l'status rebut, si se'ns han proporcionat
        if(!relatedFieldsList.isEmpty())
        {
            const char *text;
            INFO_LOG("Status details");
            foreach(DcmTagKey tagKey, relatedFieldsList)
            {
                // Fem un log per cada camp relacionat amb l'error amb el format 
                // NomDelTag (xxxx,xxxx): ContingutDelTag
                statusDetail->findAndGetString(tagKey, text, false);
                INFO_LOG(QString(DcmTag(tagKey).getTagName()) + " " + QString(tagKey.toString().c_str()) + ": " + QString(text));
            } 
        }
    }
}

PACSRequestStatus::SendRequestStatus SendDICOMFilesToPACS::getStatusStoreSCU()
{
    /*El tractament d'erros d'StoreSCU és diferent del moveSCU, en moveSCU rebem un status final indicant com ha anat l'operació, mentre que 
      en storeSCU per cada imatge que s'envia és rep un status, com podem tenir al enviar un estudi, status failure, warning, ..., cap a l'usuari
      només enviarem un error i mostrarem el més crític, per exemple si tenim 5 errors Warning i un de Failure, enviarem error indica que l'enviament 
      d'algunes imatges ha fallat. */

    if (m_abortIsRequested)
    {
        INFO_LOG("S'ha abortat l'enviament d'imatges al PACS");
        return PACSRequestStatus::CancelledSend;
    }
    else if (getNumberOfImagesSentSuccesfully() == 0)
    {
        //No hem guardat cap imatge (Failure Status)
        ERROR_LOG("Ha fallat l'enviament de totes les imatges al PACS");
        return PACSRequestStatus::FailureSend;
    }
    else if (getNumberOfImagesSentFailed() > 0)
    {
        //No s'han pogut guardar les imatges (Warning Status);
        ERROR_LOG(QString("L'enviament al PACS de %1 de %2 imatges ha fallat").arg(QString().setNum(getNumberOfImagesSentFailed()), QString().setNum(m_numberOfImagesToSend)));
        return PACSRequestStatus::SomeImagesFailedSend;
    }
    else if (getNumberOfImagesSentWarning() > 0)
    {
        //Alguna imatge s'ha guardat amb l'Status de warning
        WARN_LOG(QString("En l'enviament de %1 de %2 imatges s'ha rebut un warning").arg(QString().setNum(getNumberOfImagesSentWarning()), QString().setNum(m_numberOfImagesToSend)));
        return PACSRequestStatus::SomeImagesFailedSend;
    }
    
    INFO_LOG("Totes les imatges s'han enviat al PACS correctament");

    return PACSRequestStatus::OkSend;
}

int SendDICOMFilesToPACS::getNumberOfImagesSentSuccesfully()
{
    return m_numberOfSendImagesSuccessful;
}

int SendDICOMFilesToPACS::getNumberOfImagesSentFailed()
{
    return m_numberOfImagesToSend - m_numberOfSendImagesSuccessful - m_numberOfSendImagesWithWarning;
}

int SendDICOMFilesToPACS::getNumberOfImagesSentWarning()
{
    return m_numberOfSendImagesWithWarning;
}

}
