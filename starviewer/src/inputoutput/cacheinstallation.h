/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEINSTALLATION_H
#define UDGCACHEINSTALLATION_H

namespace udg {

/** Aquesta classe comprova que els directoris i la base de dades de la cache estiguin correctament creats
  * si no es aquest el cas, els crea, per a que l'aplicacio pugui funcionar correctament
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheInstallation{
public:
    CacheInstallation();
    
    bool checkInstallation();
    bool reinstallDatabase();
    
    bool existsDatabasePath();
    bool existsCacheImagePath();
    bool existsDatabaseFile();
    
    bool reinstallDatabaseFile();
    
    ~CacheInstallation();
    
private :
    
    bool createCacheImageDir();
    bool createDatabaseDir();
    void createDatabaseFile();

};

}

#endif
