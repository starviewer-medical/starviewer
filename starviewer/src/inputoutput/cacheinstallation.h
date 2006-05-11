/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEINSTALLATION_H
#define UDGCACHEINSTALLATION_H

namespace udg {

/** Aquesta classe comprova que els directoris i la base de dades de la cache estiguin correctament creats si no es aquest el cas, els crea, per a que l'aplicacio pugui funcionar correctament
 *	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
 */
class CacheInstallation
{
public:
    
    /// Constructor de la classe
    CacheInstallation();
    
    /** Comprova que la cache estigui correctament instal·lada. Comprova :
     *             Comprova que existeixi el directori de la base de dades, sino existeix el crea
     *             Comprova que existeixi la base de dades,sino existeix la crea
     *             Comprova que existeixi el directori on es guarden les imatges a la cache
     */
    bool checkInstallation();
    
    /** Comprova si existeix el directori de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabasePath();
    
    /** Comprova si existeix el directori on es guarden les imatges descarregades
     * @return indica si el directori existeix
     */
    bool existsCacheImagePath();
    
    /** Comprova si existeix el fitxer de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabaseFile();
    
    /// reinstal·lar la base de dades
    bool reinstallDatabaseFile();
    
    ///destructor de la classe
    ~CacheInstallation();
    
private :
    
    /** Crea el directori per guardar les imatges de la cache
     * @return indica si s'ha pogut crear el directori
     */
    bool createCacheImageDir();
    
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
