#include "pacsserver.h"

#include <dimse.h>
#include <ofstring.h>
#include <ofcond.h>
#include <assoc.h>
#include "dcmtk/dcmdata/dcdebug.h"

#include "pacsdevice.h"
#include "status.h"
#include "pacsconnection.h"
#include "pacsnetwork.h"
#include "errordcmtk.h"
#include "logging.h"

namespace udg{

/*Tot els talls de codi dins el QT_NO_DEBUG van ser afegits per anar al connectathon de berlin, allà es demanava que les operacions
 *de comunicació amb el PACS es fessin en mode verbose */

PacsServer::PacsServer(PacsDevice p)
{
    // Variable global de dcmtk per evitar el dnslookup, que dona problemes de lentitu a windows.
    // TODO: Al fer refactoring aquesta inicialització hauria de quedar en un lloc central de configuracions per dcmtk.
    dcmDisableGethostbyaddr.set(OFTrue);

    m_pacs = p;
    m_net = NULL;
    m_params = NULL;
    m_assoc = NULL;
    m_pacsNetwork = new PacsNetwork();
}

PacsServer::PacsServer()
{
    m_net = NULL;
    m_params = NULL;
    m_assoc = NULL;
    m_pacsNetwork = new PacsNetwork();
}

Status PacsServer::echo()
{
    OFCondition status_echo;
    Status state;

    DIC_US id = m_assoc->nextMsgID++; // generate next message ID
    DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
    DcmDataset *sd = NULL; // status detail will be stored here
    // send C-ECHO-RQ and handle response
    status_echo=DIMSE_echoUser( m_assoc , id , DIMSE_BLOCKING , 0 , &status, &sd );

    delete sd; // we don't care about status detail

    return state.setStatus( status_echo );
}

OFCondition PacsServer::configureEcho()
{
    int pid;
    // list of transfer syntaxes, only a single entry here
    const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };

    // add presentation pid to association request

    pid=1;//pid always has to be odd

    return ASC_addPresentationContext( m_params , pid , UID_VerificationSOPClass , transferSyntaxes , DIM_OF( transferSyntaxes ) );
}

OFCondition PacsServer::configureFind( levelConnection level )
{
    int pid = 1;//sempre ha de ser imparell, el seu valor és 1 perquè només passem un presentation context

    //we specify the type transfer
    const char* transferSyntaxes[] = { NULL , NULL , UID_LittleEndianImplicitTransferSyntax };

    /* gLocalByteOrder is defined in dcxfer.h */
    if ( gLocalByteOrder == EBO_LittleEndian )
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

    static const char *     opt_abstractSyntax;

    //specified the level
    if ( level == studyLevel)
    {
       opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
    }
    else if ( level == patientLevel )
    {
        opt_abstractSyntax = UID_FINDPatientStudyOnlyQueryRetrieveInformationModel;
    }
    else if ( level == seriesLevel )
    {// UID_FINDStudyRootQueryRetrieveInformationModel includes the information of series level
        opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
    }
    else if ( level == imageLevel )
    {// UID_FINDStudyRootQueryRetrieveInformationModel includes the information of image level
        opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
    }

    return ASC_addPresentationContext( m_params , pid , opt_abstractSyntax , transferSyntaxes , DIM_OF(transferSyntaxes) );
}

OFCondition PacsServer::configureMove( levelConnection level )
{
    int pid = 1;//sempre ha de ser imparell, el seu valor és 1 perquè només passem un presentation context
    OFCondition status;

    //we specify the transfer
    const char* transferSyntaxes[] = { NULL , NULL , UID_LittleEndianImplicitTransferSyntax };

    /* gLocalByteOrder is defined in dcxfer.h */
    if ( gLocalByteOrder  ==  EBO_LittleEndian ) {
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

   //specify the modality of the find,this is necessary in move, bescause first of all the pacs has to search the study
   static const char *     opt_abstractSyntaxMove;

   //Alhora de moure les imatges, el PACS primer ha de verificar que existexi unes imatges que compleixin la màscara que se li ha passat
   //per això primer ha de fer un "find", degut aquest fet aquí hem d'especificar dos funcions a fer la de buscar "UID_FIND" i descarregar "UID_MOVE"
    if ( level == studyLevel )
    {
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;
    }
    else if ( level == patientLevel )
    {
        opt_abstractSyntaxMove = UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel;
    }
    else if ( level == seriesLevel )
    {
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;
    }
    else if ( level == imageLevel )
    {
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;
    }

   status = addPresentationContextMove( m_params, pid , opt_abstractSyntaxMove );

   if ( status.bad() ) return status;

   return status;
}

OFCondition PacsServer::addPresentationContextMove(T_ASC_Parameters *m_params , int pid , const char* abstractSyntax )
{
    /*
    ** We prefer to use Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    **
    ** The presentation pids proposed here are only used for
    ** C-FIND and C-MOVE, so there is no need to support compressed
    ** transmission.
    */
    T_ASC_PresentationContextID presentationContextID = pid;

    const char* transferSyntaxes[] = { NULL , NULL , NULL };
    int numTransferSyntaxes = 0;

    /* We prefer explicit transfer syntaxes.
     * If we are running on a Little Endian machine we prefer
     * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
     */
    if ( gLocalByteOrder  ==  EBO_LittleEndian )  /* defined in dcxfer.h */
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
    numTransferSyntaxes = 3;

    return ASC_addPresentationContext( m_params , presentationContextID , abstractSyntax , transferSyntaxes , numTransferSyntaxes );
}

OFCondition PacsServer::configureStore()
{
    OFCondition cond;
    cond = addStoragePresentationContexts();

    return cond;
}

/*TODO Estudiar si el millor transferSyntax per defecte és UID_LittleEndianExplicitTransferSyntax o com els cas del move és el JPegLossLess
 */
OFCondition PacsServer::addStoragePresentationContexts()
{
    /*
     * Each SOP Class will be proposed in two presentation contexts (unless
     * the opt_combineProposedTransferSyntaxes global variable is true).
     * The command line specified a preferred transfer syntax to use.
     * This prefered transfer syntax will be proposed in one
     * presentation context and a set of alternative (fallback) transfer
     * syntaxes will be proposed in a different presentation context.
     *
     * Generally, we prefer to use Explicitly encoded transfer syntaxes
     * and if running on a Little Endian machine we prefer
     * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
     * Some SCP implementations will just select the first transfer
     * syntax they support (this is not part of the standard) so
     * organise the proposed transfer syntaxes to take advantage
     * of such behaviour.
     */

    OFList<OFString> sopClasses;
    OFList<OFString> fallbackSyntaxes;
    OFString preferredTransferSyntax;

    ///Indiquem que con Transfer syntax preferida volem utilitzar JpegLossless
    preferredTransferSyntax = UID_JPEGProcess14SV1TransferSyntax;

    fallbackSyntaxes.push_back( UID_LittleEndianExplicitTransferSyntax );
    fallbackSyntaxes.push_back( UID_BigEndianExplicitTransferSyntax );
    fallbackSyntaxes.push_back( UID_LittleEndianImplicitTransferSyntax );

    OFListIterator( OFString ) s_cur;
    OFListIterator( OFString ) s_end;

    /*Afegim totes les classes SOP de transfarència d'imatges. com que desconeixem de quina modalitat són
     * les imatges alhora de preparar la connexió les hi incloem totes les modalitats. Si alhora de connectar sabèssim de quina modalitat és l'estudi només caldria afegir-hi la de la motalitat de l'estudi
    /*Les sopClass o també conegudes com AbstractSyntax és equivalent amb el Move quina acció volem fer per exemple 
     * UID_MOVEStudyRootQueryRetrieveInformationModel , en el case del move, en el cas de StoreScu, el sopClass que tenim van en 
     * funció del tipus d'imatge per exemple tenim ComputedRadiographyImageStorage, CTImageStore, etc.. aquestes sopClass indiquen 
     * quin tipus d'imatge anirem a guardar, per això sinó sabem de quin tipus de SOPClass són les imatges que anem a guardar al 
     * PACS, li indiquem una llista per defecte que cobreix la gran majoria i més comuns de SOPClass que existeixen */

    /*Amb l'Abstract syntax o SOPClass definim quina operació volem fer, i amb els transfer syntax indiquem amb quin protocol es farà
     * la transfarència de les dades, LittleEndian, JPegLossLess, etc.. */

    /* TODO Si que la podem arribar a saber la transfer syntax, només hem de mirar la SOPClassUID de cada imatge a enviar, mirar
     * codi storescu.cc a partir de la línia 639
     */
    //Les SOPClasses també són conegudes com les Ab
    for ( int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; i++ )
    {
        sopClasses.push_back( dcmShortSCUStorageSOPClassUIDs[i] );
    }

    // comprovem que no hi hagi que cap SOPClas duplicada
    OFList< OFString > sops;
    s_cur = sopClasses.begin();
    s_end = sopClasses.end();

    while ( s_cur != s_end )
    {
        if ( !isaListMember( sops , *s_cur ) )
        {
            sops.push_back( *s_cur );
        }
        ++s_cur;
    }

    // add a presentations context for each sop class / transfer syntax pair
    OFCondition cond = EC_Normal;
    int pid = 3; // presentation context id ha de començar el 3 pq el 1 és el echo
    s_cur = sops.begin();
    s_end = sops.end();

    /*Creem un presentation context per cada SOPClass que tinguem, indicant per cada SOPClass quina transfer syntax utilitzarem*/
    /*En el cas del Store amb el presentation Context indiquem que per cada tipus d'imatge que volem guardar SOPClass amb quins 
     *transfer syntax ens podem comunicar, llavors el PACS ens indicarà si ell pot guardar aquest tipus de SOPClass, i amb quin
     *transfer syntax li hem d'enviar la imatge*/
    while ( s_cur != s_end && cond.good() )
    {
        // No poden haver més de 255 presentation context
        if ( pid > 255 ) return ASC_BADPRESENTATIONCONTEXTID;

        // sop class with preferred transfer syntax
        cond = addPresentationContext(pid, *s_cur, preferredTransferSyntax);
        pid += 2;   /* only odd presentation context id's */

        if ( fallbackSyntaxes.size() > 0 )
        {
            if ( pid > 255 ) return ASC_BADPRESENTATIONCONTEXTID;

            // sop class with fallback transfer syntax
            cond = addPresentationContext( pid , *s_cur , fallbackSyntaxes );
            pid += 2;       /* only odd presentation context id's */
        }
        ++s_cur;
    }

    return cond;
}

OFCondition PacsServer::addPresentationContext( int presentationContextId , const OFString& abstractSyntax , const  OFList<OFString>& transferSyntaxList)
{
    // create an array of supported/possible transfer syntaxes
    const char** transferSyntaxes = new const char*[transferSyntaxList.size()];
    int transferSyntaxCount = 0;
    OFListConstIterator( OFString ) s_cur = transferSyntaxList.begin();
    OFListConstIterator( OFString ) s_end = transferSyntaxList.end();

    while ( s_cur != s_end )
    {
        transferSyntaxes[transferSyntaxCount++] = ( *s_cur ).c_str();
        ++s_cur;
    }

    OFCondition cond = ASC_addPresentationContext(m_params , presentationContextId ,        abstractSyntax.c_str() , transferSyntaxes , transferSyntaxCount , ASC_SC_ROLE_DEFAULT);

    delete[] transferSyntaxes;
    return cond;
}

OFCondition PacsServer::addPresentationContext( int presentationContextId , const OFString& abstractSyntax , const OFString& transferSyntax )
{
    const char* c_p = transferSyntax.c_str();
    OFCondition cond = ASC_addPresentationContext( m_params , presentationContextId ,       abstractSyntax.c_str() , &c_p , 1 , ASC_SC_ROLE_DEFAULT );
    return cond;
}

OFBool PacsServer::isaListMember( OFList<OFString>& list , OFString& string )
{
    OFListIterator( OFString ) cur = list.begin();
    OFListIterator( OFString ) end = list.end();

    OFBool found = OFFalse;

    while ( cur != end && !found )
    {
        found = (string == *cur);
        ++cur;
    }

    return found;
}

Status PacsServer::connect( modalityConnection modality , levelConnection level )
{
    OFCondition cond;
    char adrLocal[255];
    Status state;
    QString AdrServer;

    //create the parameters of the connection
    cond = ASC_createAssociationParameters( &m_params , ASC_DEFAULTMAXPDU );
    if ( !cond.good() ) return state.setStatus( cond );

    // set calling and called AE titles
    //el c_str, converteix l'string que ens retornen les funcions get a un char
    ASC_setAPTitles( m_params , qPrintable( PacsDevice::getLocalAETitle() ) , qPrintable(m_pacs.getAETitle()) , NULL );

    /* Set the transport layer type (type of network connection) in the params */
    /* strucutre. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    //defineix el nivell de seguretat de la connexió en aquest cas diem que no utilitzem cap nivell de seguretat
    cond = ASC_setTransportLayerType(m_params, OFFalse);
    if (!cond.good()) return state.setStatus( cond );

    AdrServer = constructAdrServer(modality, m_pacs );

    //get localhost name TODO substituir per QHostInfo::localHostName()
    gethostname( adrLocal , 255 );

    // the DICOM server accepts connections at server.nowhere.com port
    cond = ASC_setPresentationAddresses( m_params , adrLocal , qPrintable(AdrServer) );
    if ( !cond.good() ) return state.setStatus( cond );

    //Especifiquem el timeout de connexió, si amb aquest temps no rebem resposta donem error per time out
    int timeout = PacsDevice::getConnectionTimeout();
    dcmConnectionTimeout.set( timeout );

    switch ( modality )
    {
        case echoPacs : //configure echo
                        cond = configureEcho();
                        if ( !cond.good() ) return state.setStatus( cond );

                        state = m_pacsNetwork->createNetworkQuery( timeout );
                        if ( !state.good() ) return state;

                        m_net = m_pacsNetwork->getNetworkQuery();

                        break;
        case query :    //configure the find paramaters depending on modality connection
                        cond = configureFind( level );
                        if ( !cond.good() ) return state.setStatus( cond );

                        state = m_pacsNetwork->createNetworkQuery( timeout );
                        if ( !state.good() ) return state;

                        m_net = m_pacsNetwork->getNetworkQuery();

                        break;
        case retrieveImages : //configure the move paramaters depending on modality connection
                        cond=configureMove( level );
                        if ( !cond.good() ) return state.setStatus( cond );

                        state = m_pacsNetwork->createNetworkRetrieve( PacsDevice::getIncomingDICOMConnectionsPort() , PacsDevice::getConnectionTimeout() );
                        if ( !state.good() ) return state;

                        m_net = m_pacsNetwork->getNetworkRetrieve();

                        break;
        case storeImages :
                        cond = configureStore();
                        if ( !cond.good() ) return state.setStatus( cond );

                        state = m_pacsNetwork->createNetworkQuery( timeout );
                        if ( !state.good() ) return state;

                        m_net = m_pacsNetwork->getNetworkQuery();

                        break;
    }

    //try to connect
    cond = ASC_requestAssociation( m_net , m_params , &m_assoc );

    if ( cond.good() )
    {
        if ( ASC_countAcceptedPresentationContexts(m_params)  ==  0 )
        {
            ERROR_LOG("El PACS no ens ha acceptat cap dels Presentation Context presentats.");
            return state.setStatus( DcmtkCanNotConnectError );
        }
    }
    else return state.setStatus( cond );

   return state.setStatus( DcmtkNoError );
}

void PacsServer::disconnect()
{
    ASC_releaseAssociation( m_assoc ); // release association
    ASC_destroyAssociation( &m_assoc ); // delete assoc structure

	m_pacsNetwork->disconnect();//desconectem les adreces de xarxa
}

QString PacsServer:: constructAdrServer( modalityConnection modality, PacsDevice pacsDevice )
{
//The format is "server:port"
    QString adrServer;

    adrServer.insert( 0 , pacsDevice.getAddress() );
    adrServer.insert( adrServer.length() , ":" );

    switch (modality)
    {
        case PacsServer::query:
        case PacsServer::retrieveImages:
            adrServer.insert( adrServer.length() , QString().setNum(pacsDevice.getQueryRetrieveServicePort()) );
            break;
        case PacsServer::storeImages:
            adrServer.insert( adrServer.length() , QString().setNum(pacsDevice.getStoreServicePort()) );
            break;
        case PacsServer::echoPacs:
            if (pacsDevice.isQueryRetrieveServiceEnabled())
            {
                adrServer.insert( adrServer.length() , QString().setNum(pacsDevice.getQueryRetrieveServicePort()) );
            }
            else if (pacsDevice.isStoreServiceEnabled())
            {
                adrServer.insert( adrServer.length() , QString().setNum(pacsDevice.getStoreServicePort()) );
            }
            else
            {
                ERROR_LOG("No s'ha pogut configurar per quin port fer l'echo perquè el PACS " + pacsDevice.getAETitle() + " no té cap servei activat");
            }
            break;
        default:
            ERROR_LOG("No s'ha pogut configurar per quin port fer l'echo al PACS " + pacsDevice.getAETitle() + " perquè la modalitat de connexió és invàlida");
    }

    INFO_LOG("Pacs Adress build:" + adrServer);

    return adrServer;
}

void PacsServer:: setPacs( PacsDevice p )
{
    m_pacs = p;
}

PacsDevice PacsServer::getPacs()
{
    return m_pacs;
}

PacsConnection PacsServer:: getConnection()
{
    PacsConnection connection;
    connection.setPacsConnection( m_assoc );
    return connection;
}

T_ASC_Network * PacsServer:: getNetwork()
{
    return m_net;
}

}
