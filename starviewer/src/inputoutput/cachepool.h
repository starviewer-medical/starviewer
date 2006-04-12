/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEPOOL_H
#define UDGCACHEPOOL_H
#include <QString>
#include "databaseconnection.h"

namespace udg {

class Status;
class string;
/** Classe que s'encarrega de gestionar l'spool de la caché
@author Grup de Gràfics de Girona  ( GGG )
*/
class CachePool{
public:
    static const unsigned int MinimumMBytesOfDiskSpaceRequired = 1000;
    static const unsigned int MBytesToEraseWhenDiskOrCacheFull = 2000;

    CachePool();

    void removeStudy(std::string studyUID);    

    Status resetPoolSpace();
    Status updatePoolSpace(int);
    
    Status getPoolUsedSpace(int &usedSpace);
    Status getPoolTotalSize(int &totalSize);
    Status updatePoolTotalSize(int);    
    Status getPoolFreeSpace( int &freeSpace );

    ~CachePool();
private:
    
    DatabaseConnection *m_DBConnect;
    Status constructState(int numState);
    void removeSeries(std::string absPath);
    bool removeImage(std::string absPath);    
    bool removeDir(std::string absDirPath);
};
};

#endif
