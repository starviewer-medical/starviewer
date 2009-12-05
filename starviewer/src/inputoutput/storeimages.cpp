/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "storeimages.h"

#include <diutil.h>
#include <dcfilefo.h>
#include <ofconapp.h>
#include <assoc.h>
#include <dcmtk/dcmdata/dctagkey.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <QDir>

#include "status.h"
#include "processimagesingleton.h"
#include "logging.h"
#include "pacsconnection.h"
#include "errordcmtk.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "pacsserver.h"

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */
namespace udg {

StoreImages::StoreImages()
{
}

void StoreImages::setConnection(PacsServer pacsServer)
{
    m_association = pacsServer.getConnection().getPacsConnection();
    m_timeOut = pacsServer.getPacs().getConnectionTimeout();
}

Status StoreImages::store(QList<Image*> imageListToStore)
{
    OFCondition cond = EC_Normal;
    Status state;
    ProcessImageSingleton* piSingleton;
    QString statusMessage;

    initialitzeImagesCounters();

    //A la connexió del Pacs ja s'han establert els PresentationContext (SopClass-TransferSyntax) amb els que s'enviara les imatges
    //If not connection has been setted, return error because we need a PACS connection
    if (m_association == NULL)
    {
        return state.setStatus(DcmtkNoConnectionError);
    }

    //proces que farà el tractament de la imatge enviada des de la nostra aplicació, en el cas de l'starviewer informar a QOperationStateScreen que s'ha guardar una imatge més
    piSingleton=ProcessImageSingleton::getProcessImageSingleton();

    foreach(Image *imageToStore, imageListToStore)
    {
        if (storeSCU(m_association, qPrintable(imageToStore->getPath())))
        {
            //Si l'ha imatge s'ha enviat correctament la processem
            piSingleton->process(imageToStore->getParentSeries()->getParentStudy()->getInstanceUID(), imageToStore);
        }
    }

    return getStatusStoreSCU(imageListToStore.count());
}

void StoreImages::initialitzeImagesCounters()
{
    //Inicialitzem els comptadors control d'errors 

    m_numberOfStoredImagesSuccessful = 0;
    m_numberOfStoredImagesWithWarning = 0;
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
bool StoreImages::storeSCU(T_ASC_Association * association, QString filepathToStore)
{
    DIC_US msgId = association->nextMsgID++;
    T_ASC_PresentationContextID presentationContextID;
    T_DIMSE_C_StoreRQ request;
    T_DIMSE_C_StoreRSP response;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    DcmDataset *statusDetail = NULL;
    DcmFileFormat dcmff;

    OFCondition cond = dcmff.loadFile(qPrintable(QDir::toNativeSeparators(filepathToStore)));

    /* figure out if an error occured while the file was read*/
    if (cond.bad()) 
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

        cond = DIMSE_storeUser(association, presentationContextID, &request, NULL /*imageFileName*/, dcmff.getDataset(), NULL /*progressCallback*/, 
                                NULL /*callbackData */, DIMSE_NONBLOCKING, m_timeOut, &response, &statusDetail, NULL /*check for cancel parameters*/, 
                                DU_fileSize(qPrintable(filepathToStore)));

        if (cond.bad())
        {
            ERROR_LOG("S'ha produït un error al fer el store de la imatge " + filepathToStore + ", descripció de l'error" + QString(cond.text()));
        }

        processResponseFromStoreSCP(&response, statusDetail, filepathToStore);

        if (statusDetail != NULL)
        {
            delete statusDetail;
        }

        if (cond.good() && response.DimseStatus == STATUS_Success)
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}

void StoreImages::processResponseFromStoreSCP(T_DIMSE_C_StoreRSP *response, DcmDataset *statusDetail, QString filePathDicomObjectStoredFailed)
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
        m_numberOfStoredImagesSuccessful++;
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
            m_numberOfStoredImagesWithWarning++;
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

Status StoreImages::getStatusStoreSCU(int numberOfImagesToStore)
{
    Status state;
    /*El tractament d'erros d'StoreSCU és diferent del moveSCU, en moveSCU rebem un status final indicant com ha anat l'operació, mentre que 
      en storeSCU per cada imatge que s'envia és rep un status, com podem tenir al enviar un estudi, status failure, warning, ..., cap a l'usuari
      només enviarem un error i mostrarem el més crític, per exemple si tenim 5 errors Warning i un de Failure, enviarem error indica que l'enviament 
      d'algunes imatges ha fallat. */

    if (m_numberOfStoredImagesSuccessful == 0)
    {
        //No hem guardat cap imatge
        state.setStatus(DcmtkStorescuFailureStatus);
        ERROR_LOG("Ha fallat l'enviament de totes les imatges al PACS");
    }
    else if (m_numberOfStoredImagesSuccessful + m_numberOfStoredImagesWithWarning < numberOfImagesToStore)
    {
        //No s'han pogut guardar les imatges
        state.setStatus(DcmkStorescuSomeImagesFailedStatus);
        ERROR_LOG(QString("L'enviament al PACS de %1 de %2 imatges ha fallat").arg(QString().setNum(numberOfImagesToStore - m_numberOfStoredImagesSuccessful + m_numberOfStoredImagesWithWarning), QString().setNum(numberOfImagesToStore)));
    }
    else if (m_numberOfStoredImagesWithWarning > 0)
    {
        //Alguna imatge s'ha guardat amb l'Status de warning
        state.setStatus(DcmkStorescuWarningStatus);
        WARN_LOG(QString("En l'enviament de %1 de %2 imatges s'ha rebut un warning").arg(QString().setNum(m_numberOfStoredImagesWithWarning), QString().setNum(numberOfImagesToStore)));
    }
    else 
    {
        state.setStatus("",true,0);
        INFO_LOG("Totes les imatges s'han enviat al PACS correctament");
    }   

    return state;
}

}
