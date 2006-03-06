#include "pacsparameters.h"
#include "const.h"

namespace udg{

/** Constructor ràpid de la classe. Aquest constructor inclou tots els paràmentres mínim requerits per connectar-nos amb el PACS.
 *  IMPORTANT!!!! si l'objectiu de la connexió és descarregar imatges s'ha d'invocar la funció setLocalPort
 *               @param   adr [in] Adreça de pacs al qual ens volem connectar
 *               @param   port [in] Port en el qual el pacs esta escoltant les peticions          
 *               @param   aet [in]  AETitle del PACS al qual ens volem connectar
 *               @param   aec [in]  AETitle de la nostra màquina. Cada màquina ha de tenir un AETitle únic. El PACS comprova al connectar-nos si la nostra IP correspont al nostre AETitle que li enviem
 */
PacsParameters::PacsParameters(std::string adr, std::string port,std::string aet,std::string aec)
{
    m_Adr = adr;
    m_Port = port;
    m_aeTitle = aet;
    m_aeCalled = aec;
    m_TimeOut = 15000; //establim que per defecte el timeout és de 15000 ms
    m_PacsID = 0;
}

/** Constructor buit de la classe
  */
PacsParameters::PacsParameters()
{ 
    m_PacsID = 0;
}

/** assigna l'adreça al pacs al qual ens volem connectar
  *               @param  Adreça del PACS 
  */
void PacsParameters::setPacsAdr(std::string adr)
{     
     m_Adr = adr;
}  

/** assigna el port del pacs al qual ens volem connectar
  *               @param port de connexió del pacs
  */
void PacsParameters::setPacsPort(std::string port)
{     
     m_Port = port;
}  

/** assigna l'aet title de la nostre màquina local       
  *               @param AE Title de la màquina local
  */
void PacsParameters::setAELocal(std::string aet)
{     
     m_aeTitle = aet;
}  

/** assigna l'ae title del pacs al qual ens volem connectar
  *               @param AE title del pacs al que es desitja connectar 
  */
void PacsParameters::setAEPacs(std::string aec)
{     
     m_aeCalled = aec;
}  

/** assigna el port pel qual nosaltres volem rebre les imatges del PACS
  *               @param Port local
  */
void PacsParameters::setLocalPort(std::string port)
{
    m_LocalPort = port;
}

/** assigna la institucio a la qual pertany el pacs                 
  *               @param Institucio a la qual pertany al pcacs
  */
void PacsParameters::setInstitution(std::string inst)
{
    m_Inst = inst;
}

/** assigna la localització del PACS 
  *               @param Localització del PACS
  */
void PacsParameters::setLocation(std::string local)
{
    m_Location = local;
}

/** assigna la descripció del PACS 
  *               @param descripció del PACS
  */
void PacsParameters::setDescription(std::string desc)
{
    m_Desc = desc;
}

/** assigna al Pacs si és el Predeterminat
  *               @param indica si el PACS és el predeterminat per defecte
  */
void PacsParameters::setDefault(std::string def)
{
    m_Default = def;
}

/** assigna el timeout de la connexio
  *               @param  time out en segons
  */
void PacsParameters::setTimeOut(int time)
{
    m_TimeOut = time*1000; //convertim a ms
}

/** assigna l'id del PACS, aquest és un camp clau de la base de dades per diferenciar els PACS
  *               @param id el pacs
  */
void PacsParameters::setPacsID(int id)
{
    m_PacsID = id;
}

/*****************************************************************************************/

/** retorna l'adreça del pacs 
  *               @return Adreça del pacs 
  */
std::string PacsParameters::getPacsAdr()
{
    return m_Adr;
}

/** retorna el port del pacs 
  *               @return Port del pacs 
  */
std::string PacsParameters::getPacsPort()
{
    return m_Port;
}

/** retorna l'AE Local
  *               @return AE title local 
  */
std::string PacsParameters::getAELocal()
{
    return m_aeTitle;
}

/** retorna l'AE del pacs 
  *               @return Adreça del pacs 
  */
std::string PacsParameters::getAEPacs()
{
    return m_aeCalled;
}

/** retorna el port Local pel qual desitgem rebre les imatges
  *               @return Port local 
  */
std::string PacsParameters::getLocalPort()
{
    return m_LocalPort;
}

/** retorna la institució a la que pertany el PACS
  *               @return institucio
  */
std::string PacsParameters::getInstitution()
{
    return m_Inst;
}

/** retorna la localització del PACS
  *               @return localització
  */
std::string PacsParameters::getLocation()
{
    return m_Location;
}

/** retorna la descripció del PACS
  *               @return descripció
  */
std::string PacsParameters::getDescription()
{
    return m_Desc;
}

/** retorna si és el pacs predeterminat
  *               @return si val 'S' voldrà di que és el pacs predeterminat per realitzar les cerques
  */
std::string PacsParameters::getDefault()
{
    return m_Default;
}

/** retorna el temps de time out en ms 
  *               @return Time out
  */
int PacsParameters::getTimeOut()
{
    return m_TimeOut;
}

/** retorna l'id del PACS, aquest es un camp clau de la taula PacsList, que l'assigna l'aplicacio
  *               @return Id del pacs
  */
int PacsParameters::getPacsID()
{
    return m_PacsID;
}

}

