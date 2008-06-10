#ifndef PACSSERVER
#define PACSSERVER


#include <oflist.h>

#include "pacsparameters.h"

struct T_ASC_Network;
struct T_ASC_Parameters;
struct T_ASC_Association;
class OFString;
class OFCondition;
class QString;

/** Aquest classe és la que ens ajuda interectuar amb el pacs.
 * La classe conté les principals funcions i accions per connectar-nos en el pacs amb l'objectiu, de fer un echo, buscar informació o descarregar imatges. Alhora de connectar-nos al constructor passem els paràmetres mínim per connectar-nos. És molt important llegir molt bé la documentació d'aquesta classe per saber quins paràmetres utiltizar, si no voleu tenir problemes alhora de buscar informació,descarrega imatges
 */
namespace udg{

class PacsConnection;
class PacsNetwork;
class Status;

class PacsServer
{

public:

    enum modalityConnection { query , retrieveImages , echoPacs, storeImages };

    //any it's only can be used with echoPacs modality
    enum levelConnection { patientLevel , studyLevel , seriesLevel , any , imageLevel };

    /** Constuctor de la classe. Se li ha de passar un objecte PacsParameters, amb els paràmetres del pacs correctament especificats
     * @param Parametres del Pacs a connectar
     */
    PacsServer( PacsParameters );

    /// Constructor buit de la classe
    PacsServer();

    /** Aquesta funció ens intenta connectar al PACS
     * @param Especifica en quina modalitat ens volem connectar, fer echo, busca informació o descarregar imatges
     * @param l Especifica a quin nivell durem a terme l'objectiu especificat al paràmetre anterior. A nivell de  Pacient,estudi o sèrie. Per comprendre millor els nivells  consultar la documentació del dicom Càpitol 4. C.6
     * @return retorna l'estat de la connexió
     */
    Status connect( modalityConnection , levelConnection );

    /** Ens permet fer un echo al PACS. Per defecte per qualsevol modalitat de connexió bé sigui busca informació o descarregar imatges per defecte permet fer un echo, per comprovar si els PACS està viu
     * @return retorna l'estatus del echo
     */
    Status echo();//This function makes an echo to the PACS

    /** Estableix un pacs per a la connexió
     * @param Pacs amb els paràmetres per a la connexió
     */
    void setPacs( PacsParameters );

    /** retorna els paràmetres del PACS
     * @return paràmetres del Pacs
     */
    PacsParameters getPacs();

    /** Retorna una connexió per a poder buscar informació, o descarregar imatges
     * @return retorna una connexió de PACS
     */
    PacsConnection getConnection();

    /** Retorna una configuració de xarxa. Només és necessària quan l'objectiu de la connexió sigui el de descarregar imatges
     * @return retorna la configuració de la xarxa
     */
    T_ASC_Network * getNetwork();

    /// This action close the session with PACS's machine and release all the resources
    void disconnect();

private:

   T_ASC_Network *m_net; // network struct, contains DICOM upper layer FSM etc.
   T_ASC_Parameters *m_params; // parameters of association request
   T_ASC_Association *m_assoc; // request DICOM association;
   PacsNetwork *m_pacsNetwork; //configures the T_ASC_Network

   PacsParameters m_pacs;

    /** Aquesta funció és privada. És utilitzada per especificar en el PACS, que una de les possibles operacions que volem fer amb ell és un echo. Per defecte en qualsevol modalitat de connexió podrem fer un echo
     * @return retorna l'estat de la configuració
     */
   OFCondition configureEcho();

    /** Aquesta funció privada, configura els paràmetres de la connexió per especificar, que el motiu de la nostre connexió és buscar informació.
     * @param Especifiquem a quin nivell volem buscar la informació, de pacient, d'estudi o de sèrie
     * @return retorna l'estat de la configuració
     */
   OFCondition configureFind( levelConnection );

    /** Aquesta funció privada permet configurar la connexió per a descarregar imatges al ordinador local. IMPORTANT!!! Abans de connectar s'ha d'invocar la funció setLocalhostPort
     * @param Especifiquem a quin nivell volem descarregar les imatges, de pacient, d'estudi o de sèrie
     * @return retorna l'estat de la configuracióDUL_PRESENTATIONCONTEXTID
     */
   OFCondition configureMove( levelConnection );

    /** Aquesta funci�privada permet configurar la connexi�per a guardar estudis en el pacs.
     * @return retorna l'estat de la configuració
     */
    OFCondition configureStore();

    /** Aquesta funció privada, configura els paràmetres pel tipus de connexió per descarregat imatges segons la màquina que tinguem
     * @param paràmetres de la connexió
     * @param Identificador de funcio
     * @param Objectiu de la funcio
     * @return retorna l'estat de la funció
     */
   OFCondition addPresentationContextMove( T_ASC_Parameters * , int, const char* );

    /** Construeix l'adreça del servidor en format ip:port, per connectar-se al PACS
     * @param adreça del servidor
     * @param port del servidor
     */
   QString constructAdrServer( QString , QString ); //construct PACS address

    /** Afegeix tots els PresentationContexts necessaris per a guardar imatges en el PACS
    * @return retorna l'estat del m�ode
    */
    OFCondition addStoragePresentationContexts();


    /** Afageix un objecte SOP a la connexió amb el PACS
     * @param presentationContextId número de pid
     * @param abstractSyntax classe SOP a afegir
     * @param transferSyntaxList
     * @return estat del mètode
     */
    OFCondition addPresentationContext( int presentationContextId, const OFString& abstractSyntax, const  OFList<OFString>& transferSyntaxList);


    /** Afageix un objecte SOP a la connexió amb el PACS
     * @param presentationContextId número de pid
     * @param abstractSyntax classe SOP a afegir
     * @param transferSyntax
     * @return estat del mètode
     */
    OFCondition addPresentationContext(int presentationContextId, const OFString& abstractSyntax, const OFString& transferSyntax);


    /** comprova si un OFString pertany dins una llista
     * @param list llista
     * @param string string a comprovar si existeix a lla llista
     * @return indica is l'string està dins la llista
     */
    OFBool isaListMember(OFList<OFString>& list, OFString& string);
};
};
#endif
