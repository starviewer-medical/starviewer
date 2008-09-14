#ifndef PACSPARAMETERS
#define PACSPARAMETERS

#include <cond.h>
#include <list>

#include <QString>//No es pot treure perquè PacsList l'inclou en un llista stl a PacsParameters

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
    PacsParameters( QString ,  QString ,  QString  , QString  );

    /// Constructor buit de la classe
    PacsParameters();

    /** assigna l'adreça al pacs al qual ens volem connectar
     * @param  Adreça del PACS
     */
    void setPacsAdr( QString );

    /** assigna el port del pacs al qual ens volem connectar
     * @param port de connexió del pacs
     */
    void setPacsPort( QString );

    /** assigna l'aet title de la nostre màquina local
     * @param AE Title de la màquina local
     */
    void setAELocal( QString );

    /** assigna l'ae title del pacs al qual ens volem connectar
     * @param AE title del pacs al que es desitja connectar
     */
    void setAEPacs( QString );

    /** assigna el port pel qual nosaltres volem rebre les imatges del PACS
     * @param Port local
     */
    void setLocalPort( QString );

    /** assigna la descripció del PACS
     * @param descripció del PACS
     */
    void setDescription( QString );

    /** assigna la institucio a la qual pertany el pacs
     * @param Institucio a la qual pertany al pcacs
     */
    void setInstitution( QString );

    /** assigna la localització del PACS
     * @param Localització del PACS
     */
    void setLocation( QString );

    /** assigna al Pacs si és el Predeterminat
     * @param indica si el PACS és el predeterminat per defecte
     */
    void setDefault( QString );

    /** assigna el timeout de la connexio
     * @param  time out en segons
     */
    void setTimeOut( int );

    /** assigna l'id del PACS, aquest és un camp clau de la base de dades per diferenciar els PACS
     * @param id el pacs
     */
    void setPacsID( int );

    ///Indica si el pacs està esborrat, és a dir no és visible pels usuaris
    void setIsDeleted( bool );

    /** retorna l'adreça del pacs
     * @return Adreça del pacs
     */
     QString  getPacsAdr() const;

    /** retorna el port del pacs
     * @return Port del pacs
     */
     QString  getPacsPort() const;

    /** retorna l'AE Local
     * @return AE title local
     */
     QString  getAELocal() const;

    /** retorna l'AE del pacs
     * @return Adreça del pacs
     */
     QString  getAEPacs() const;

    /** retorna el port Local pel qual desitgem rebre les imatges
     * @return Port local
     */
     QString  getLocalPort() const;

    /** retorna la descripció del PACS
     * @return descripció
     */
     QString  getDescription() const;

    /** retorna la institució a la que pertany el PACS
     * @return institucio
     */
     QString  getInstitution() const;

    /** retorna la localització del PACS
     * @return localització
     */
     QString  getLocation() const;

    /** retorna si és el pacs predeterminat
     * @return si val 'S' voldrà di que és el pacs predeterminat per realitzar les cerques
     */
     QString  getDefault() const;

    /** retorna l'id del PACS, aquest es un camp clau de la taula PacsList, que l'assigna l'aplicacio
     * @return Id del pacs
     */
    int getPacsID() const;

    /** retorna el temps de time out en ms
     * @return Time out
     */
    int getTimeOut() const;

    ///Indica si el pacs està esborrat, és a dir no és visible per a l'usuari per poguer-s'hi connectar
    bool isDeleted() const;

    bool operator ==(const PacsParameters &parameters);

private :

    QString  m_aeCalled;
    QString  m_aeTitle;
    QString  m_Port;
    QString  m_Adr;
    QString  m_LocalPort; //especifica el port pel qual rebrem imatges
    QString  m_Desc;
    QString  m_Inst;
    QString  m_Location;
    QString  m_Default;

    bool  m_isDeleted;

    int          m_PacsID; //camp clau de la taula PacsList, serveix per identificar els PACS, és invisible per a l'usuari, aquest camp l'assigna l'aplicacio
    int m_TimeOut;

};
};
#endif
