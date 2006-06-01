#ifndef PACSPARAMETERS
#define PACSPARAMETERS

#define HAVE_CONFIG_H 1
#include <cond.h>
#include <list>

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

class string;
class Study;
class PacsParameters
{

public :
    
    /** Constructor ràpid de la classe. Aquest constructor inclou tots els paràmentres mínim requerits per connectar-nos amb el PACS.
     *  IMPORTANT!!!! si l'objectiu de la connexió és descarregar imatges s'ha d'invocar la funció setLocalPort
     * @param   adr Adreça de pacs al qual ens volem connectar
     * @param   port Port en el qual el pacs esta escoltant les peticions          
     * @param   aet AETitle del PACS al qual ens volem connectar
     * @param   aec AETitle de la nostra màquina. Cada màquina ha de tenir un AETitle únic. El PACS comprova al connectar-nos si la nostra IP correspont al nostre AETitle que li enviem
    */
    PacsParameters( std::string ,  std::string ,  std::string  , std::string  );
    
    /// Constructor buit de la classe
    PacsParameters();
    
    /** assigna l'adreça al pacs al qual ens volem connectar
     * @param  Adreça del PACS 
     */
    void setPacsAdr( std::string );

    /** assigna el port del pacs al qual ens volem connectar
     * @param port de connexió del pacs
     */
    void setPacsPort( std::string );

    /** assigna l'aet title de la nostre màquina local       
     * @param AE Title de la màquina local
     */
    void setAELocal( std::string );
    
    /** assigna l'ae title del pacs al qual ens volem connectar
     * @param AE title del pacs al que es desitja connectar 
     */
    void setAEPacs( std::string );
            
    /** assigna el port pel qual nosaltres volem rebre les imatges del PACS
     * @param Port local
     */
    void setLocalPort( std::string );
    
    /** assigna la descripció del PACS 
     * @param descripció del PACS
     */
    void setDescription( std::string );
    
    /** assigna la institucio a la qual pertany el pacs                 
     * @param Institucio a la qual pertany al pcacs
     */
    void setInstitution( std::string );
    
    /** assigna la localització del PACS 
     * @param Localització del PACS
     */
    void setLocation( std::string );
        
    /** assigna al Pacs si és el Predeterminat
     * @param indica si el PACS és el predeterminat per defecte
     */
    void setDefault( std::string );
    
    /** assigna el timeout de la connexio
     * @param  time out en segons
     */
    void setTimeOut( int );
    
    /** assigna l'id del PACS, aquest és un camp clau de la base de dades per diferenciar els PACS
     * @param id el pacs
     */
    void setPacsID( int );
    
    /** retorna l'adreça del pacs 
     * @return Adreça del pacs 
     */
     std::string  getPacsAdr();
    
    /** retorna el port del pacs 
     * @return Port del pacs 
     */
     std::string  getPacsPort();
    
    /** retorna l'AE Local
     * @return AE title local 
     */
     std::string  getAELocal();
    
    /** retorna l'AE del pacs 
     * @return Adreça del pacs 
     */
     std::string  getAEPacs();
    
    /** retorna el port Local pel qual desitgem rebre les imatges
     * @return Port local 
     */
     std::string  getLocalPort();
    
    /** retorna la descripció del PACS
     * @return descripció
     */
     std::string  getDescription();
    
    /** retorna la institució a la que pertany el PACS
     * @return institucio
     */
     std::string  getInstitution();
    
    /** retorna la localització del PACS
     * @return localització
     */
     std::string  getLocation(); 
        
    /** retorna si és el pacs predeterminat
     * @return si val 'S' voldrà di que és el pacs predeterminat per realitzar les cerques
     */
     std::string  getDefault();
    
    /** retorna l'id del PACS, aquest es un camp clau de la taula PacsList, que l'assigna l'aplicacio
     * @return Id del pacs
     */
    int getPacsID();
    
    /** retorna el temps de time out en ms 
     * @return Time out
     */
    int getTimeOut();

private :

    std::string  m_aeCalled;
    std::string  m_aeTitle;
    std::string  m_Port;
    std::string  m_Adr;
    std::string  m_LocalPort; //especifica el port pel qual rebrem imatges
    std::string  m_Desc;
    std::string  m_Inst;
    std::string  m_Location;
    std::string  m_Default;
    int          m_PacsID; //camp clau de la taula PacsList, serveix per identificar els PACS, és invisible per a l'usuari, aquest camp l'assigna l'aplicacio
    int m_TimeOut;

};
};
#endif
