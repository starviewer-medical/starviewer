#ifndef PACSPARAMETERS
#define PACSPARAMETERS

#define HAVE_CONFIG_H 1
#include <cond.h>
#include "study.h"
#include <list>
#include <string.h>

/** Aquesta classe ens permet configurar els paràmetres necessaris que l'usuari ens ha d'entrar per connectar-nos en el pacs. A més d'altres dades d'interés
  * Aquests paràmetres són :
  *            -IP o Adr. del Host
  *            - Port de connexió
  *            - AE title de la màquina local
  *            - AE title del PACS al que es vol connectar l'usuari
  *            - Institució al que pertany al PACS
  *            - Descripció del Pacs
  *            - Localitzacio del Pacs
  *            - Default, estableix si aquest PACS és el predeterminat per realitzar les cerques 
*/

namespace udg{
class PacsParameters{

private :

    std::string m_aeCalled;
    std::string m_aeTitle;
    std::string m_Port;
    std::string m_Adr;
    std::string m_LocalPort; //especifica el port pel qual rebrem imatges
    std::string m_Desc;
    std::string m_Inst;
    std::string m_Location;
    std::string m_Default;
    int          m_PacsID; //camp clau de la taula PacsList, serveix per identificar els PACS, és invisible per a l'usuari, aquest camp l'assigna l'aplicacio
    int m_TimeOut;

public :
    
    PacsParameters(std::string, std::string, std::string ,std::string );
    PacsParameters();
    
    void setPacsAdr(std::string);
    void setPacsPort(std::string);
    void setAELocal(std::string);
    void setAEPacs(std::string);
    void setLocalPort(std::string);
    void setDescription(std::string);
    void setInstitution(std::string);
    void setLocation(std::string);
    void setDefault(std::string);
    void setTimeOut(int);
    void setPacsID(int);
    
    std::string getPacsAdr();
    std::string getPacsPort();
    std::string getAELocal();
    std::string getAEPacs();
    std::string getLocalPort();
    std::string getDescription();
    std::string getInstitution();
    std::string getLocation(); 
    std::string getDefault();
    int getPacsID();
    int getTimeOut();
};
};
#endif
