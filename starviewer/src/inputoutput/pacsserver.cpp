#include "pacsserver.h"

#include <dimse.h>
#include <ofcond.h>
#include <assoc.h>
#include <QHostInfo>

#include "status.h"
#include "pacsconnection.h"
#include "pacsnetwork.h"
#include "errordcmtk.h"
#include "logging.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */

PacsServer::PacsServer(PacsDevice pacsDevice)
{
    // Variable global de dcmtk per evitar el dnslookup, que dona problemes de lentitu a windows.
    // TODO: Al fer refactoring aquesta inicialització hauria de quedar en un lloc central de configuracions per dcmtk.
    dcmDisableGethostbyaddr.set(OFTrue);

    m_pacs = pacsDevice;
    m_associationNetwork = NULL;
    m_associationParameters = NULL;
    m_dicomAssociation = NULL;
    m_pacsNetwork = new PacsNetwork();
}

PacsServer::PacsServer()
{
    m_associationNetwork = NULL;
    m_associationParameters = NULL;
    m_dicomAssociation = NULL;
    m_pacsNetwork = new PacsNetwork();
}

Status PacsServer::echo()
{
    OFCondition status_echo;
    Status state;

    DIC_US id = m_dicomAssociation->nextMsgID++; // generate next message ID
    DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
    DcmDataset *dcmDataset = NULL;

    status_echo = DIMSE_echoUser(m_dicomAssociation, id, DIMSE_BLOCKING, 0, &status, &dcmDataset);

    delete dcmDataset; // we don't care about status detail

    return state.setStatus(status_echo);
}

OFCondition PacsServer::configureEcho()
{
    const char* transferSyntaxes[] = {UID_LittleEndianImplicitTransferSyntax};
    int presentationContextID = 1; //presentationContextID always has to be odd

    return ASC_addPresentationContext(m_associationParameters, presentationContextID, UID_VerificationSOPClass, transferSyntaxes, DIM_OF( transferSyntaxes) );
}

OFCondition PacsServer::configureFind()
{
    const char* transferSyntaxes[] = {NULL, NULL, UID_LittleEndianImplicitTransferSyntax};
    int presentationContextID = 1;//sempre ha de ser imparell, el seu valor és 1 perquè només passem un presentation context

    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian)
    {
        /* we are on a little endian machine */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    }
    else
    {
        /* we are on a big endian machine */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }

    return ASC_addPresentationContext(m_associationParameters, presentationContextID, UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes, 
        DIM_OF(transferSyntaxes));
}

OFCondition PacsServer::configureMove()
{
    int presentationContextID = 1;//sempre ha de ser imparell, el seu valor és 1 perquè només passem un presentation context
    const char* transferSyntaxes[] = { NULL, NULL, UID_LittleEndianImplicitTransferSyntax };
    OFCondition status;

    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder  ==  EBO_LittleEndian) {
        /* we are on a little endian machine */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    }
    else
    {
        /* we are on a big endian machine */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }

   return addPresentationContextMove(m_associationParameters, presentationContextID, UID_MOVEStudyRootQueryRetrieveInformationModel);
}

OFCondition PacsServer::addPresentationContextMove(T_ASC_Parameters *associationParameters, int presentationContextID, const char* abstractSyntax)
{
    /* We prefer to use Explicitly encoded transfer syntaxes. If we are running on a Little Endian machine we prefer LittleEndianExplicitTransferSyntax 
    to BigEndianTransferSyntax. Some SCP implementations will just select the first transfer syntax they support (this is not part of the standard) so
    organise the proposed transfer syntaxes to take advantage of such behaviour.
    The presentation presentationContextIDs proposed here are only used for C-FIND and C-MOVE, so there is no need to support compressed transmission. */

    T_ASC_PresentationContextID associationPresentationContextID = presentationContextID;
    const char* transferSyntaxes[] = { NULL, NULL, NULL };

    /*We prefer explicit transfer syntaxes.
     If we are running on a Little Endian machine we prefer LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax. */
    if (gLocalByteOrder  ==  EBO_LittleEndian)  /* defined in dcxfer.h */
    {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    }
    else
    {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }
    transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;

    return ASC_addPresentationContext(associationParameters, associationPresentationContextID, abstractSyntax, transferSyntaxes, 3 /*number of TransferSyntaxes*/);
}

/*TODO Estudiar si el millor transferSyntax per defecte és UID_LittleEndianExplicitTransferSyntax o com els cas del move és el JPegLossLess
 */
OFCondition PacsServer::configureStore()
{
    /*Each SOP Class will be proposed in two presentation contexts (unless the opt_combineProposedTransferSyntaxes global variable is true).
     The command line specified a preferred transfer syntax to use. This prefered transfer syntax will be proposed in one presentation context 
     and a set of alternative (fallback) transfer syntaxes will be proposed in a different presentation context.
     
     Generally, we prefer to use Explicitly encoded transfer syntaxes and if running on a Little Endian machine we prefer LittleEndianExplicitTransferSyntax 
     to BigEndianTransferSyntax. Some SCP implementations will just select the first transfer syntax they support (this is not part of the standard) so
     organise the proposed transfer syntaxes to take advantage of such behaviour. */

    QList<QString> sopClasses;
    QList<const char*> fallbackSyntaxes;
    const char *preferredTransferSyntax;

    ///Indiquem que con Transfer syntax preferida volem utilitzar JpegLossless
    preferredTransferSyntax = UID_JPEGProcess14SV1TransferSyntax;

    fallbackSyntaxes.append(UID_LittleEndianExplicitTransferSyntax);
    fallbackSyntaxes.append(UID_BigEndianExplicitTransferSyntax);
    fallbackSyntaxes.append(UID_LittleEndianImplicitTransferSyntax);

    /*Afegim totes les classes SOP de transfarència d'imatges. com que desconeixem de quina modalitat són
     les imatges alhora de preparar la connexió les hi incloem totes les modalitats. Si alhora de connectar sabèssim de quina modalitat és 
     l'estudi només caldria afegir-hi la de la motalitat de l'estudi
     
     Les sopClass o també conegudes com AbstractSyntax és equivalent amb el Move quina acció volem fer per exemple 
     UID_MOVEStudyRootQueryRetrieveInformationModel, en el case del move, en el cas de StoreScu, el sopClass que tenim van en funció del tipus d'imatge 
     per exemple tenim ComputedRadiographyImageStorage, CTImageStore, etc.. aquestes sopClass indiquen  quin tipus d'imatge anirem a guardar, per això sinó 
     sabem de quin tipus de SOPClass són les imatges que anem a guardar al PACS, li indiquem una llista per defecte que cobreix la gran majoria i més 
     comuns de SOPClass que existeixen */

    /*Amb l'Abstract syntax o SOPClass definim quina operació volem fer, i amb els transfer syntax indiquem amb quin protocol es farà
     * la transfarència de les dades, LittleEndian, JPegLossLess, etc.. */

    /* TODO Si que la podem arribar a saber la transfer syntax, només hem de mirar la SOPClassUID de cada imatge a enviar, mirar
     * codi storescu.cc a partir de la línia 639
     */
    for (int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; i++)
    {
        // comprovem que no hi hagi que cap SOPClas duplicada
        if (!sopClasses.contains(QString(dcmShortSCUStorageSOPClassUIDs[i])))
        {
            sopClasses.append(QString(dcmShortSCUStorageSOPClassUIDs[i]));
        }
    }

    OFCondition condition = EC_Normal;
    int presentationContextID = 3; // presentation context id ha de començar el 3 pq el 1 és el echo

    /*Creem un presentation context per cada SOPClass que tinguem, indicant per cada SOPClass quina transfer syntax utilitzarem*/
    /*En el cas del Store amb el presentation Context indiquem que per cada tipus d'imatge que volem guardar SOPClass amb quins 
     *transfer syntax ens podem comunicar, llavors el PACS ens indicarà si ell pot guardar aquest tipus de SOPClass, i amb quin
     *transfer syntax li hem d'enviar la imatge*/

    foreach(QString sopClass, sopClasses)
    {
        // No poden haver més de 255 presentation context
        if (presentationContextID > 255) return ASC_BADPRESENTATIONCONTEXTID;

        // sop class with preferred transfer syntax
        condition = addPresentationContext(presentationContextID, sopClass, preferredTransferSyntax);
        presentationContextID += 2;   /* only odd presentation context id's */

        if (fallbackSyntaxes.size() > 0)
        {
            if (presentationContextID > 255) return ASC_BADPRESENTATIONCONTEXTID;

            // sop class with fallback transfer syntax
            condition = addPresentationContext(presentationContextID, sopClass, fallbackSyntaxes);
            presentationContextID += 2;       /* only odd presentation context id's */
        }
    }

    return condition;
}

OFCondition PacsServer::addPresentationContext(int presentationContextId, const QString &abstractSyntax, QList<const char*> transferSyntaxList)
{
    // create an array of supported/possible transfer syntaxes
    const char** transferSyntaxes = new const char*[transferSyntaxList.size()];
    int transferSyntaxCount = 0;

    foreach(const char *transferSyntax, transferSyntaxList)
    {
        transferSyntaxes[transferSyntaxCount++] = transferSyntax;
    }

    OFCondition condition = ASC_addPresentationContext(m_associationParameters, presentationContextId, qPrintable(abstractSyntax), transferSyntaxes, 
        transferSyntaxCount, ASC_SC_ROLE_DEFAULT);

    delete[] transferSyntaxes;
    return condition;
}

OFCondition PacsServer::addPresentationContext(int presentationContextId, const QString &abstractSyntax, const char *transferSyntax)
{
    return ASC_addPresentationContext(m_associationParameters, presentationContextId, qPrintable(abstractSyntax), &transferSyntax, 1, ASC_SC_ROLE_DEFAULT);
}

Status PacsServer::connect(modalityConnection modality)
{
    OFCondition condition;
    Status state;

    //create the parameters of the connection
    condition = ASC_createAssociationParameters(&m_associationParameters, ASC_DEFAULTMAXPDU);
    if (!condition.good()) return state.setStatus(condition);

    // set calling and called AE titles
    ASC_setAPTitles(m_associationParameters, qPrintable(PacsDevice::getLocalAETitle()), qPrintable(m_pacs.getAETitle()), NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* strucutre. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    //defineix el nivell de seguretat de la connexió en aquest cas diem que no utilitzem cap nivell de seguretat
    condition = ASC_setTransportLayerType(m_associationParameters, OFFalse);
    if (!condition.good()) return state.setStatus(condition);

    // the DICOM server accepts connections at server.nowhere.com port
    condition = ASC_setPresentationAddresses(m_associationParameters, qPrintable( QHostInfo::localHostName()), 
        qPrintable( constructPacsServerAddress(modality, m_pacs) ) );
    if (!condition.good()) return state.setStatus(condition);

    //Especifiquem el timeout de connexió, si amb aquest temps no rebem resposta donem error per time out
    int timeout = PacsDevice::getConnectionTimeout();
    dcmConnectionTimeout.set(timeout);

    switch (modality)
    {
        case echoPacs : //configure echo
                        condition = configureEcho();
                        if (!condition.good()) return state.setStatus(condition);

                        state = m_pacsNetwork->createNetworkQuery(timeout);
                        if (!state.good()) return state;

                        m_associationNetwork = m_pacsNetwork->getNetworkQuery();

                        break;
        case query :    //configure the find paramaters depending on modality connection
                        condition = configureFind();
                        if (!condition.good()) return state.setStatus(condition);

                        state = m_pacsNetwork->createNetworkQuery(timeout);
                        if (!state.good()) return state;

                        m_associationNetwork = m_pacsNetwork->getNetworkQuery();

                        break;
        case retrieveImages : //configure the move paramaters depending on modality connection
                        condition=configureMove();
                        if (!condition.good()) return state.setStatus(condition);

                        ///Preparem la connexió amb el PACS i obrim el local port per acceptar connexions DICOM
                        state = m_pacsNetwork->createNetworkRetrieve(PacsDevice::getIncomingDICOMConnectionsPort(), PacsDevice::getConnectionTimeout());
                        if (!state.good()) return state;

                        m_associationNetwork = m_pacsNetwork->getNetworkRetrieve();

                        break;
        case storeImages :
                        condition = configureStore();
                        if (!condition.good()) return state.setStatus(condition);

                        state = m_pacsNetwork->createNetworkQuery(timeout);
                        if (!state.good()) return state;

                        m_associationNetwork = m_pacsNetwork->getNetworkQuery();

                        break;
    }

    //try to connect
    condition = ASC_requestAssociation(m_associationNetwork, m_associationParameters, &m_dicomAssociation);

    if (condition.good())
    {
        if (ASC_countAcceptedPresentationContexts(m_associationParameters)  ==  0)
        {
            ERROR_LOG("El PACS no ens ha acceptat cap dels Presentation Context presentats.");
            return state.setStatus(DcmtkCanNotConnectError);
        }
    }
    else
    {
        /*Si no hem pogut connectar al PACS i és una descàrrega haurem obert el port per rebre connexions entrants DICOM,
         *com no que podrem descarregar les imatges perquè no hem pogut connectar amb el PACS per sol·licitar-ne la descarrega,
         *tanquem el port local que espera per connexions entrants.*/
        if (modality == retrieveImages)
        {
            m_pacsNetwork->disconnect();
        }

        return state.setStatus(condition);
    }

   return state.setStatus(DcmtkNoError);
}

void PacsServer::disconnect()
{
    ASC_releaseAssociation(m_dicomAssociation); // release association
    ASC_destroyAssociation(&m_dicomAssociation); // delete assoc structure

	m_pacsNetwork->disconnect();//desconectem les adreces de xarxa
}

QString PacsServer::constructPacsServerAddress(modalityConnection modality, PacsDevice pacsDevice)
{
    //The format is "server:port"
    QString pacsServerAddress = pacsDevice.getAddress() + ":";

    switch (modality)
    {
        case PacsServer::query:
        case PacsServer::retrieveImages:
            pacsServerAddress += QString().setNum(pacsDevice.getQueryRetrieveServicePort());
            break;
        case PacsServer::storeImages:
            pacsServerAddress += QString().setNum(pacsDevice.getStoreServicePort());
            break;
        case PacsServer::echoPacs:
            if (pacsDevice.isQueryRetrieveServiceEnabled())
            {
                pacsServerAddress +=  QString().setNum(pacsDevice.getQueryRetrieveServicePort());
            }
            else if (pacsDevice.isStoreServiceEnabled())
            {
                pacsServerAddress += QString().setNum(pacsDevice.getStoreServicePort());
            }
            else
            {
                ERROR_LOG("No s'ha pogut configurar per quin port fer l'echo perquè el PACS " + pacsDevice.getAETitle() + " no té cap servei activat");
            }
            break;
        default:
            ERROR_LOG("No s'ha pogut configurar per quin port fer l'echo al PACS " + pacsDevice.getAETitle() + " perquè la modalitat de connexió és invàlida");
    }

    INFO_LOG("Pacs Adress build:" + pacsServerAddress);

    return pacsServerAddress;
}

void PacsServer:: setPacs(PacsDevice pacsDevice)
{
    m_pacs = pacsDevice;
}

PacsDevice PacsServer::getPacs()
{
    return m_pacs;
}

PacsConnection PacsServer::getConnection()
{
    PacsConnection connection;
    connection.setPacsConnection(m_dicomAssociation);
    return connection;
}

T_ASC_Network * PacsServer::getNetwork()
{
    return m_associationNetwork;
}

}
