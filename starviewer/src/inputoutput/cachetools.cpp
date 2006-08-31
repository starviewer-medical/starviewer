/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
#include <sqlite3.h>

#include "cachetools.h"
#include "databaseconnection.h"
#include "status.h"

namespace udg {

CacheTools::CacheTools()
{
}

Status CacheTools::compactCachePacs()
{
    int stateDatabase;
    Status state;
    std::string sql;
    DatabaseConnection *databaseConnection = DatabaseConnection::getDatabaseConnection();
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "vacuum" );//amb l'acció vacuum es compacta la base de dades
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , sql.c_str() , 0 , 0 , 0 );
    databaseConnection->releaseLock();
                                
    state = databaseConnection->databaseStatus ( stateDatabase );

    return state;
}


CacheTools::~CacheTools()
{
}


}
