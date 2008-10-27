/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDATABASEINSTALLATION_H
#define UDGDATABASEINSTALLATION_H

namespace udg {

/** Aquesta classe comprova que els directoris i la base de dades de la cache estiguin correctament creats si no es aquest el cas, els crea, per a que l'aplicacio pugui funcionar correctament
 *	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
 */
class DatabaseInstallation
{
public:

    /// Constructor de la classe
    DatabaseInstallation();

    /** Comprova que el directori on es guarden les imatges descarregades existeixi si no l'intenta crear
     * @return indicat si el directori existeix o no
     */
    bool checkInstallationDatabaseImagePath();

    /** Comprova que el directori on es guarda la base dades i la base de dades existeixin sinó l'intenta crear
     * @return indica si la base de dades existeix
     */
    bool checkInstallationDatabase();

    /** Comprova si existeix el directori de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabasePath();

    /** Comprova si existeix el directori on es guarden les imatges descarregades
     * @return indica si el directori existeix
     */
    bool existsDatabaseImagePath();

    /** Comprova si existeix el fitxer de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabaseFile();

    ///reinstal·lar la base de dades i esborra les imatges descarregades o importades a la base de dades local
    bool reinstallDatabaseFile();

    ///Aplica els canvis a fets a la última revisió de la base de dades a la base de dades locals
    bool updateDatabaseRevision();

    ///destructor de la classe
    ~DatabaseInstallation();

private :

    /** Crea el directori per guardar les imatges de la cache
     * @return indica si s'ha pogut crear el directori
     */
    bool createDatabaseImageDir();

    /** Crea el directori per guardar la base de dades
     * @return indica si s'ha pogut crear el directori
     */
    bool createDatabaseDir();

    /** Crea la base de dades
     * @return indica si s'ha pogut crear la base de dades
     */
    bool createDatabaseFile();
};
}//end namespace udg

#endif
