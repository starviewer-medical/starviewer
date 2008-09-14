/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPACSLISTDB_H
#define UDGPACSLISTDB_H

#include <QSettings>
#include <QList>

#include "pacsparameters.h"

class QString;

namespace udg {

/** Aquesta classe implementa les accions necessaries per afegir nous pacs o modificar/consultar els paràmetres dels PACS  que tenim disponible a l'aplicació, guardats en un fitxer de configuració.
@author marc
*/

class Status;

class PacsListDB{

public:

    ///Constructor de la classe
    PacsListDB( );

    ///Destructor de la classe
    ~PacsListDB( );

    /** Insereix els parametres d'un pacs a la base de dades, per a poder-hi cercar imatge. En un alta el camp PacsID, és assignat automàticament per l'aplicació!
     * @param Objecte PacsParameters amb les dades del pacs
     * @return true en el cas que s'hagi inserit correctament. False si el pacs ja existia.
     */
    bool insertPacs(const PacsParameters &pacs);

    /** Retorna un objecte PacsList amb tots els Pacs que hi ha la taula PacsList odernats per AEtitle. Nomes selecciona els pacs vius, és a dir els que no tenen estat d'esborrats
     * @param PacsList Conté tots els Pacs de la taula PacsList
     */
    QList<PacsParameters> queryPacsList();

    /** Permet actualitzar la informació d'un pacs, el PacsID camp clau no es pot canviar!
     * @param Objecte PAcsParameters ambles noves dades del PACS
     */
    void updatePacs(const PacsParameters &pacs);

    /** Es donarà de baixa el Pacs. No es dona de baixa físicament, sinó que es posa en estat donat de baixa
     * @param  Objecte pacsID del pacs a donar de baixa
     */
    void deletePacs(int pacsID);

    /** Cerca la informació d'un pacs en concret.
     * @param Conté la informació del pacs cercat
     * @param pacs a cercar
     */
    PacsParameters queryPacs(QString AETitle);

    /** Cerca la informació d'un pacs en concret.
     * @param Conté la informació del pacs cercat
     * @param pacs a cercar
     */
    PacsParameters queryPacs(int pacsID);

private:

    ///Ens indica si un Pacs ja està donat d'alta a partir del seu AETitle
    bool existPacsByAETitle(const QString &pacsAETitle );

    ///Mètode que s'ha de fer servir sempre que es vulgui accedir a la llista de pacs configurats.
    ///S'encarrega d'omplir la llista en el cas que no s'hagi fet prèviament.
    QList<PacsParameters> getConfiguredPacsList();
    void saveConfiguredPacsListToDisk();

    PacsParameters fillPacs(const QSettings &settings);

private:

    QList<PacsParameters> m_configuredPacsList;
    static const QString PacsListConfigurationSectionName;
};

};

#endif
