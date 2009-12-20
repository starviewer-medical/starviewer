#ifndef PACSDEVICE
#define PACSDEVICE

#include <QString>

namespace udg{

/** 
 * Classe que encapsula els paràmetres necessaris per definir un servidor PACS al qual connectar-nos
 * Aquests paràmetres són:
 *
 * Dades de connexió (imprescindibles)
 *      - AETitle del PACS
 *      - IP del servidor PACS
 *      - Port de connexió del PACS
 *
 * Dades Descriptives (opcionals)
 *      - Institució a la que pertany el PACS
 *      - Descripció del PACS
 *      - Ubicació del PACS
 *
 * Altres dades 
 *      - AETitle de la màquina local
 *      - Nombre màxim de connexions simultànies
 *      - Timeout de la connexió
 *      - Port Local de Query/Retrieve
 * TODO aquestes dades s'haurien de reubicar en un lloc més adient ja que són
 * les mateixes per a qualsevol connexió amb un PACS i per tant no haurien de formar part d'aquesta classe
 *
 */
class PacsDevice
{
public:
    PacsDevice();

    /// Assigna/Retorna l'adreça al PACS al qual ens volem connectar
    void setAddress(const QString &address);
    QString getAddress() const;

    /// Assigna/Retorna el port del PACS al qual ens volem connectar
    void setPort(const QString &port);
    QString getPort() const;

    /// Assigna/Retorna l'AETitle del PACS al qual ens volem connectar
    void setAETitle(const QString &AETitle);
    QString getAETitle() const;

    /// Assigna/Retorna la descripció del PACS
    void setDescription(const QString &description);
    QString getDescription() const;

    /// Assigna/Retorna la institucio a la qual pertany el PACS
    void setInstitution(const QString &institution);
    QString getInstitution() const;

    /// Assigna/Retorna la ubicació del PACS
    void setLocation(const QString &location);
    QString getLocation() const;

    /// Assigna/Retorna si aquest PACS és un de predeterminat per fer les consultes.
    void setDefault(bool isDefault);
    bool isDefault() const;

    /// Assigna/Retorna l'ID del PACS. Camp clau assignat per l'aplicació. 
    void setID(const QString &id);
    QString getID() const;

    /// Ens diu si aquest objecte conté dades o no
    bool isEmpty() const;
    
    bool operator ==(const PacsDevice &device);

    // TODO Aquests mètodes podrien desaparèixer. Caldria desvincular aquesta informació d'aquesta classe
    static QString getLocalAETitle();
    static int getMaximumConnections();
    static int getConnectionTimeout();
    static int getIncomingDICOMConnectionsPort();

private:
    /// Ens retorna el KeyName que identifica el PACS
    QString getKeyName() const;
    
    /// Ens retorna la llista de noms claus de PACS seleccionats per defecte
    QStringList getDefaultPACSKeyNamesList() const;

private:
    QString m_AETitle;
    QString m_port;
    QString m_address;
    QString m_description;
    QString m_institution;
    QString m_location;
    QString m_id;
};

};
#endif
