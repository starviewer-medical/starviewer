#include <string.h>

#include "const.h"
#include "pacsserver.h"
#include "status.h"
#include "pacsconnection.h"

namespace udg{

PacsServer::PacsServer( PacsParameters p )
{

    m_pacs = p;  
    m_net = NULL;
    m_params = NULL;
    m_assoc = NULL;
    m_pacsNetwork = PacsNetwork::getPacsNetwork();
}

PacsServer::PacsServer()
{
    m_net = NULL;
    m_params = NULL;
    m_assoc = NULL;
    m_pacsNetwork = PacsNetwork::getPacsNetwork();
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
    int pid;
    
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
      
    pid = 3; //pid always have to be an odd number
   
    return ASC_addPresentationContext( m_params , pid , opt_abstractSyntax , transferSyntaxes , DIM_OF(transferSyntaxes) );
} 

OFCondition PacsServer::configureMove( levelConnection level )
{
    int pid;
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
   static const char *     opt_abstractSyntaxFind;
   static const char *     opt_abstractSyntaxMove;  
   
   //Alhora de moure les imatges, el PACS primer ha de verificar que existexi unes imatges que compleixin la màscara que se li ha passat
   //per això primer ha de fer un "find", degut aquest fet aquí hem d'especificar dos funcions a fer la de buscar "UID_FIND" i descarregar "UID_MOVE"
    if ( level == studyLevel )
    {
        opt_abstractSyntaxFind = UID_FINDStudyRootQueryRetrieveInformationModel;        
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;  
    }  
    else if ( level == patientLevel )
    {
        opt_abstractSyntaxFind = UID_FINDPatientStudyOnlyQueryRetrieveInformationModel; 
        opt_abstractSyntaxMove = UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel; 
    }
    else if ( level == seriesLevel )
    {
        opt_abstractSyntaxFind = UID_FINDStudyRootQueryRetrieveInformationModel;
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;
    }
    else if ( level == imageLevel )
    {
        opt_abstractSyntaxFind = UID_FINDStudyRootQueryRetrieveInformationModel;
        opt_abstractSyntaxMove = UID_MOVEStudyRootQueryRetrieveInformationModel;
    }
        
   pid = 3; //sempre ha de ser imparell
       
   status = addPresentationContextMove( m_params, pid , opt_abstractSyntaxFind );

   if ( status.bad() ) return status;
  
   pid = pid + 2; //pid always have to be an odd number

   status = addPresentationContextMove( m_params, pid , opt_abstractSyntaxMove );

   if ( status.bad() ) return status;
   
   return status;
}

OFCondition PacsServer::addPresentationContextMove(T_ASC_Parameters *m_params ,
                        T_ASC_PresentationContextID pid ,
                        const char* abstractSyntax )
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

    return ASC_addPresentationContext( m_params , pid , abstractSyntax , transferSyntaxes , numTransferSyntaxes );
}

Status PacsServer::connect( modalityConnection modality , levelConnection level )
{
    OFCondition status;
    char adrLocal[255];
    Status state;
    std::string AdrServer;

    //create the parameters of the connection
    status = ASC_createAssociationParameters( &m_params , ASC_DEFAULTMAXPDU );
    if ( !status.good() ) return state.setStatus( status );
    
    // set calling and called AE titles
    
    //el c_str, converteix l'string que ens retornen les funcions get a un char
    ASC_setAPTitles( m_params , m_pacs.getAELocal().c_str() , m_pacs.getAEPacs().c_str() , NULL );
    
    AdrServer= constructAdrServer( m_pacs.getPacsAdr() , m_pacs.getPacsPort() );
    
    //get localhost name
    
    gethostname( adrLocal , 255 );
    
    // the DICOM server accepts connections at server.nowhere.com port 
    status = ASC_setPresentationAddresses( m_params , adrLocal , AdrServer.c_str() );
    if ( !status.good() ) return state.setStatus( status );

    //default, always configure the echo
    status = configureEcho();
    if ( !status.good() ) return state.setStatus( status );


    switch ( modality )
    {
        case echoPacs : //configure the pacs parameters
                        state = m_pacsNetwork->createNetworkQuery( m_pacs.getTimeOut() );
                        if ( !state.good() ) return state;

                        m_net = m_pacsNetwork->getNetworkQuery();
                        break;
        case query :    //configure the find paramaters depending on modality connection
                        status = configureFind( level );
                        if ( !status.good() ) return state.setStatus( status );
        
                        state = m_pacsNetwork->createNetworkQuery( m_pacs.getTimeOut() );
                        if ( !state.good() ) return state;
        
                        m_net = m_pacsNetwork->getNetworkQuery();
                        break;
        case retrieveImages : //configure the move paramaters depending on modality connection
                        status=configureMove( level );
                        if ( !status.good() ) return state.setStatus( status );   
                
                        state = m_pacsNetwork->createNetworkRetrieve( atoi( m_pacs.getLocalPort().c_str() ) , m_pacs.getTimeOut() );
                        if ( !state.good() ) return state;
                        
                        m_net = m_pacsNetwork->getNetworkRetrieve();
                        break;
    }    
    
    //try to connect
    status = ASC_requestAssociation( m_net , m_params , &m_assoc );

    if ( status.good() )
    {
        if ( ASC_countAcceptedPresentationContexts(m_params)  ==  0 )
        {
            return state.setStatus( error_NoConnect );
        }
    }
    else return state.setStatus( status );
    
   return state.setStatus( CORRECT );
}

void PacsServer::disconnect()
{
    ASC_releaseAssociation( m_assoc ); // release association
    ASC_destroyAssociation( &m_assoc ); // delete assoc structure
    
}

std::string PacsServer:: constructAdrServer( std::string host , std::string port )
{
//The format is "server:port"
    std::string adrServer;
    
    adrServer.insert( 0 , host );
    adrServer.insert( adrServer.length() , ":" );
    adrServer.insert( adrServer.length() , port );   
    
    return adrServer;
}

void PacsServer:: setPacs( PacsParameters p )
{
    m_pacs = p;
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
