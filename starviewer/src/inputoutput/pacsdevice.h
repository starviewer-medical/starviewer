#ifndef PACSDEVICE
#define PACSDEVICE

#include <QString>

namespace udg{

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
class PacsDevice
{
public:

    /// Constructor buit de la classe
    PacsDevice();

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
    void setDefault(bool isDefault);
    bool isDefault() const;

    /// Assigna/Retorna l'id del PACS, aquest és un camp clau per diferenciar els PACS que l'assigna l'aplicacio.
    void setPacsID(QString ID);
    QString getPacsID() const;

    bool operator ==(const PacsDevice &parameters);

    // TODO aquests mètodes poden desaparèixer quan estigui enllestit tot el tema de setting
    static QString getLocalAETitle();
    static int getMaximumConnections();
    static int getConnectionTimeout();
    static int getQueryRetrievePort();

private:

    QString m_pacsAETitle;
    QString m_pacsPort;
    QString m_pacsAddress;
    QString m_pacsDescription;
    QString m_pacsInstitution;
    QString m_pacsLocation;
    bool m_isDefaultPACS;
    QString m_pacsID;
};

};
#endif
