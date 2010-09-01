#ifndef PACSCONNECTION
#define PACSCONNECTION


#include "pacsdevice.h"

struct T_ASC_Network;
struct T_ASC_Parameters;
struct T_ASC_Association;
class OFCondition;
class QString;

/** Aquest classe s'encarrega de configurar la connexió i connectar amb el PACS en funció del servei que li volguem sol·licitar.*/
namespace udg{

class PACSConnection
{

public:

    enum ModalityConnection {Query, RetrieveImages, Echo, StoreImages};

    /** Constuctor de la classe. Se li ha de passar un objecte PacsDevice, amb els paràmetres del pacs correctament especificats
     * @param Parametres del Pacs a connectar
     */
    PACSConnection(PacsDevice pacsDevice);

    /** Aquesta funció ens intenta connectar al PACS
     * @param Especifica en quina modalitat ens volem connectar, fer echo, busca informació o descarregar imatges
     * @return retorna l'estat de la connexió
     */
    bool connect(ModalityConnection modalityConnection);

    /** retorna els paràmetres del PACS
     * @return paràmetres del Pacs
     */
    PacsDevice getPacs();

    /** Retorna una connexió per a poder buscar informació, o descarregar imatges
     * @return retorna una connexió de PACS
     */
    T_ASC_Association* getConnection();

    /** Retorna una configuració de xarxa. Només és necessària quan l'objectiu de la connexió sigui el de descarregar imatges
     * @return retorna la configuració de la xarxa
     */
    T_ASC_Network* getNetwork();

    /// This action close the session with PACS's machine and release all the resources
    void disconnect();

private:

    /** Aquesta funció és privada. És utilitzada per especificar en el PACS, que una de les possibles operacions que volem fer amb ell és un echo. Per defecte en qualsevol modalitat de connexió podrem fer un echo
     * @return retorna l'estat de la configuració
     */
    OFCondition configureEcho();

    /** Aquesta funció privada, configura els paràmetres de la connexió per especificar, que el motiu de la nostre connexió és buscar informació.
     * @return retorna l'estat de la configuració
     */
    OFCondition configureFind();

    /** Aquesta funció privada permet configurar la connexió per a descarregar imatges al ordinador local. IMPORTANT!!! Abans de connectar s'ha d'invocar la funció setLocalhostPort
     * @return retorna l'estat de la configuracióDUL_PRESENTATIONCONTEXTID
     */
    OFCondition configureMove();

    /** Aquesta funció privada permet configurar la connexió per a guardar estudis en el pacs.
     * @return retorna l'estat de la configuració
     */
    OFCondition configureStore();

    /** Construeix l'adreça del servidor en format ip:port, per connectar-se al PACS
     * @param adreça del servidor
     * @param port del servidor
     */
    QString constructPacsServerAddress(ModalityConnection modality, PacsDevice pacsDevice); //construct PACS address

    /** Afageix un objecte SOP a la connexió amb el PACS
     * @param presentationContextId número de pid
     * @param abstractSyntax classe SOP a afegir
     * @param transferSyntaxList
     * @return estat del mètode
     */
    OFCondition addPresentationContext(int presentationContextId, const QString &abstractSyntax, QList<const char*> transferSyntaxList);

    ///Aquest mètode inicialitza l'objecte AssociationNetwork en funció de la modalitat amb els paràmetres del PACS, aquest mètode no obre la connexió
    ///simplement inicialitza l'objecte amb les dades necessàries per poder obrir connexió, qui obra la connexió és al invocar el mètode
    ///de dcmtk ASC_requestAssociation dins del mètode connect connect();
    T_ASC_Network* initializeAssociationNetwork(ModalityConnection modality);

    ///Omple l'array passada per paràmetres amb la transfer syntax a utilitzar per les connexions per fer FIND o Move
    void getTransferSyntaxForFindOrMoveConnection(const char *transferSyntaxes[3]);

private:

    PacsDevice m_pacs;
    //network struct, contains DICOM upper layer FSM etc. A nivell DICOM no és res és un objecte propi de DCMTK, conté paràmetres de la connexió i en el cas 
    //descàrrega d'imatges se li indica per quin port escoltem les peticions DICOM. 
    T_ASC_Network *m_associationNetwork; 
    //Defineix els paràmetres de l'associació que s'utilitzarà per la comunicació entre Starviewer i el PACS, conté adreça del PACS, tipus de connexió,....
    T_ASC_Parameters *m_associationParameters; //parameters of association request
    //L'associació és el canal de comunicació que s'utilitza per l'intercanvi d'informació entre dispositius DICOM (és la connexió amb el PACS)
    T_ASC_Association *m_dicomAssociation; 
};
};
#endif
