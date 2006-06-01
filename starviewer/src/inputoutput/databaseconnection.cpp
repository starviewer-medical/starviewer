/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <string>

#include "databaseconnection.h"
#include "starviewersettings.h"

namespace udg {

class string;

DatabaseConnection::DatabaseConnection()
{
   StarviewerSettings settings;
   
   m_databasePath = settings.getDatabasePath().toAscii().constData();
   m_databaseLock = ( sem_t* ) malloc( sizeof( sem_t ) );
   sem_init( m_databaseLock , 0 , 1 );//semafor que controlarà que nomes un thread a la vegada excedeixi a la cache
   connectDB();
   
}

void DatabaseConnection::setDatabasePath( std::string path )
{
    m_databasePath = path;
}

void DatabaseConnection::connectDB()
{
  m_db = sqlite_open( m_databasePath.c_str() , 0 , NULL );
}

sqlite* DatabaseConnection::getConnection()
{
    if ( !connected() ) connectDB();
    
    return m_db;
}

bool DatabaseConnection::connected()
{
    return m_db != NULL;
}

void DatabaseConnection::getLock()
{
    sem_wait( m_databaseLock );
}

void DatabaseConnection::releaseLock()
{
    sem_post( m_databaseLock );
}

void DatabaseConnection::closeDB()
{
    if ( connected() )
    {
        sqlite_close( m_db );
    }
}

DatabaseConnection::~DatabaseConnection()
{
    closeDB();
}

};
