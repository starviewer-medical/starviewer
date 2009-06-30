/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPACSDEVICEMANAGER_H
#define UDGPACSDEVICEMANAGER_H

#include <QList>

#include "pacsdevice.h"
#include "settings.h"

namespace udg {

/** Aquesta classe implementa les accions necessaries per afegir nous pacs o modificar/consultar els paràmetres 
  * dels PACS  que tenim disponible a l'aplicació, guardats en un fitxer de configuració.
  * @author marc
  */
class PacsDeviceManager{

public:
    ///Constructor de la classe
    PacsDeviceManager();

    ///Destructor de la classe
    ~PacsDeviceManager();

    /** Insereix els parametres d'un pacs a la base de dades, per a poder-hi cercar imatge. En un alta el camp PacsID, és assignat automàticament per l'aplicació!
     * @param Objecte PacsDevice amb les dades del pacs
     * @return true en el cas que s'hagi inserit correctament. False si el pacs ja existia.
     */
    bool insertPacs(const PacsDevice &pacs);

    /** Retorna un objecte PacsList amb tots els Pacs que hi ha la taula PacsList odernats per AEtitle. Nomes selecciona els pacs vius, és a dir els que no tenen estat d'esborrats
     * @param PacsList Conté tots els Pacs de la taula PacsList
     */
    QList<PacsDevice> queryPacsList();

    ///Retorna una llista amb tots els Pacs que l'usuari té assenyalats el paràmetre Default a true, són els pacs que l'usuari té marcats per cercar per defecte al consultar estudis
    QList<PacsDevice> queryDefaultPacs();

    /** Permet actualitzar la informació d'un pacs, el PacsID camp clau no es pot canviar!
     * @param Objecte PacsDevice amb les noves dades del PACS
     */
    void updatePacs(const PacsDevice &pacs);

    /** Es donarà de baixa el Pacs. No es dona de baixa físicament, sinó que es posa en estat donat de baixa
     * @param  Objecte pacsID del pacs a donar de baixa
     */
    bool deletePacs( const QString &pacsID );

    /** Cerca la informació d'un pacs en concret.
     * @param Conté la informació del pacs cercat
     * @param pacs a cercar
     */
    PacsDevice queryPacs( const QString &pacsID );

private:
    ///Ens indica si un Pacs ja està donat d'alta a partir del seu AETitle, IP i port
    bool existPacs(const PacsDevice &pacsAETitle );

    ///Mètode que s'ha de fer servir sempre que es vulgui accedir a la llista de pacs configurats.
    ///S'encarrega d'omplir la llista en el cas que no s'hagi fet prèviament.
    QList<PacsDevice> getConfiguredPacsList( bool onlyDefault = false );

    /// Donat un objecte PacsDevice el transformem en un conjunt de claus-valor per una manipulació de settings més còmode
    Settings::KeyValueMapType pacsDeviceToKeyValueMap( const PacsDevice &parameters );

    /// Donat un conjunt de claus-valor omple i retorna un objecte PacsDevice
    PacsDevice keyValueMapToPacsDevice( const Settings::KeyValueMapType &item );

private:
    /// Clau de settings per la llista de PACS
    static const QString PacsListConfigurationSectionName;
};

};

#endif
