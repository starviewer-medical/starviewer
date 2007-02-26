/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>

#include "databaseconnection.h"
#include "starviewersettings.h"
#include "status.h"
#include "logging.h"

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
    sqlite3_open( m_databasePath.c_str() , &m_databaseConnection );
}

sqlite3* DatabaseConnection::getConnection()
{
    if ( !connected() ) connectDB();

    return m_databaseConnection;
}

bool DatabaseConnection::connected()
{
    return m_databaseConnection != NULL;
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
        sqlite3_close( m_databaseConnection );
    }
}

Status DatabaseConnection::databaseStatus( int numState )
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;
	QString logMessage, codeError;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar
        case SQLITE_OK :        state.setStatus( "Normal" , true , 0 );
                                break;
        case SQLITE_ERROR :     state.setStatus( "Database is corrupted or SQL error syntax " , false , 2001 );
                                break;
        case SQLITE_BUSY :      state.setStatus( "Database is locked" , false , 2006 );
                                break;
        case SQLITE_CORRUPT :   state.setStatus( "Database corrupted" , false , 2011 );
                                break;
        case SQLITE_CONSTRAINT: state.setStatus( "The new register is duplicated" , false , 2019 );
                                break;
        case 50 :               state.setStatus( "Not connected to database" , false , 2050 );
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus( "SQLITE internal error" , false , 2000 + numState );
                                break;
    }

   return state;
}

DatabaseConnection::~DatabaseConnection()
{
    closeDB();
}

};
