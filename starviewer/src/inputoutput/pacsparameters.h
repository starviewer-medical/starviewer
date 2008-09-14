#ifndef PACSPARAMETERS
#define PACSPARAMETERS

#include <QString>

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

public:

    /** Constructor ràpid de la classe. Aquest constructor inclou tots els paràmentres mínim requerits per connectar-nos amb el PACS.
     *  IMPORTANT!!!! si l'objectiu de la connexió és descarregar imatges s'ha d'invocar la funció setLocalPort
     * @param   adr Adreça de pacs al qual ens volem connectar
     * @param   port Port en el qual el pacs esta escoltant les peticions
     * @param   myAEtitle AETitle del PACS al qual ens volem connectar
     * @param   remoteServerAEtitle AETitle de la nostra màquina. Cada màquina ha de tenir un AETitle únic. El PACS comprova al connectar-nos si la nostra IP correspont al nostre AETitle que li enviem
    */
    PacsParameters(const QString &address, const QString &port, const QString &myAEtitle, const QString &remoteServerAEtitle);

    /// Constructor buit de la classe
    PacsParameters();

    /// Assigna/Retorna l'aet title de la nostra màquina local
    void setAELocal(const QString &AETitle);
    QString getAELocal() const;

    /// Assigna/Retorna el port pel qual nosaltres volem rebre les imatges del PACS
    void setLocalPort(const QString &port);
    QString getLocalPort() const;

    /// Assigna/Retorna l'adreça al pacs al qual ens volem connectar
    void setPacsAddress(const QString &address);
    QString getPacsAddress() const;

    /// Assigna/Retorna el port del pacs al qual ens volem connectar
    void setPacsPort(const QString &port);
    QString getPacsPort() const;

    /// Assigna/Retorna l'AETitle del pacs al qual ens volem connectar
    void setAEPacs(const QString &remoteServerAETitle);
    QString getAEPacs() const;

    /// Assigna/Retorna la descripció del PACS
    void setDescription(const QString &description);
    QString getDescription() const;

    /// Assigna/Retorna la institucio a la qual pertany el pacs
    void setInstitution(const QString &institution);
    QString getInstitution() const;

    /// Assigna/Retorna la localització del PACS
    void setLocation(const QString &location);
    QString getLocation() const;

    /// Assigna/Retorna al Pacs si és el Predeterminat. Si val 'S' voldrà di que és el pacs predeterminat per realitzar les cerques
    void setDefault(const QString &isDefault);
    QString getDefault() const;

    /// Assigna el timeout, en segons, de la connexio
    void setTimeOut(int timeoutInSeconds);
    /// Retorna el temps de time out en ms
    int getTimeOut() const;

    /// Assigna/Retorna l'id del PACS, aquest és un camp clau per diferenciar els PACS que l'assigna l'aplicacio.
    void setPacsID(int ID);
    int getPacsID() const;

    bool operator ==(const PacsParameters &parameters);

private:

    QString m_localPort; //especifica el port pel qual rebrem imatges
    QString m_myAETitle;

    QString m_pacsAETitle;
    QString m_pacsPort;
    QString m_pacsAddress;
    QString m_pacsDescription;
    QString m_pacsInstitution;
    QString m_pacsLocation;
    QString m_pacsIsDefault;
    int m_pacsID;
    int m_connectionTimeOutInMilliseconds;
};

};
#endif
