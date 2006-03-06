#ifndef PACS
#define PACS

#define HAVE_CONFIG_H 1

#include "pacsconnection.h"
#include <assoc.h>
#include <diutil.h>
#include <ofcond.h>
#include "status.h"
#include "pacsparameters.h"
#include "pacsnetwork.h"

/** Aquest classe és la que ens ajuda interectuar amb el pacs, conté les principals funcions i accions per connectar-nos en el pacs amb l'objectiu, de fer un  
  * echo, buscar informació o descarregar imatges. Alhora de connectar-nos al constructor passem els paràmetres mínim per connectar-nos. És molt important llegir
  * molt bé la documentació d'aquesta classe per saber quins paràmetres utiltizar, si no voleu tenir problemes alhora de buscar informació,descarrega imatges
 */
namespace udg{
class PacsServer{

public:
 
    enum modalityConnection {query,retrieveImages,echoPacs };
    
    //any it's only can be used with echoPacs modality
    enum levelConnection {patientLevel,studyLevel,seriesLevel,any,imageLevel};

   
    PacsServer(PacsParameters);
    PacsServer();
    
    Status Connect(modalityConnection,levelConnection);
    Status Connect(modalityConnection,levelConnection,T_ASC_Network *);
    Status Echo();//This function makes an echo to the PACS
 
    void setPacs(PacsParameters);
    
    PacsConnection getConnection();
    T_ASC_Network * getNetwork();
    
    void Disconnect();

private:
    
   T_ASC_Network *m_net; // network struct, contains DICOM upper layer FSM etc.
   T_ASC_Parameters *m_params; // parameters of association request
   T_ASC_Association *m_assoc; // request DICOM association;
   PacsNetwork *m_pacsNetwork; //configures the T_ASC_Network
    
   PacsParameters m_pacs;
   
   OFCondition ConfigureEcho(); 
   OFCondition ConfigureFind(levelConnection);
   OFCondition ConfigureMove(levelConnection);
   OFCondition addPresentationContextMove(T_ASC_Parameters*, T_ASC_PresentationContextID, const char* );
    
   string ConstructAdrServer(string,string); //construct PACS address
    
};
};
#endif
