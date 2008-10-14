/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "databaseconnection.h"

#include <sqlite3.h>
#include <QObject> // per les traduccions: tr()
#include <QSemaphore>
#include <QDir>

#include "starviewersettings.h"
#include "status.h"
#include "logging.h"


namespace udg {

DatabaseConnection::DatabaseConnection()
{
    StarviewerSettings settings;

    m_databasePath = settings.getDatabasePath();
    m_databaseLock = new QSemaphore(1);//semafor que controlarà que nomes un thread a la vegada excedeixi a la cache
    m_transactionLock = new QSemaphore(1);
}

void DatabaseConnection::setDatabasePath( QString path )
{
    m_databasePath = path;
}

void DatabaseConnection::open()
{
    sqlite3_open( qPrintable( QDir::toNativeSeparators( m_databasePath ) ) , &m_databaseConnection );
    /*En el moment que es fa el commit de les dades inserides o updates a la base de dades, sqlite bloqueja tota la base
     *de dades, per tant no es pot fer cap consulta. Indicant el busy_timeout a 10000 ms el que fem, és que si tenim una 
     *setència contra sqlite que es troba la bd o una taula bloquejada, va fent intents cada x temps per mirar si continua
     *bloqueja fins a 10000ms una vegada passat aquest temps dona errora de taula o base de dades bloquejada
     */
    sqlite3_busy_timeout(m_databaseConnection, 10000);
}

void DatabaseConnection::beginTransaction()
{
    m_transactionLock->acquire();
    sqlite3_exec(m_databaseConnection, "BEGIN TRANSACTION", 0 , 0 , 0);
}

void DatabaseConnection::endTransaction()
{
    sqlite3_exec(m_databaseConnection, "END TRANSACTION", 0 , 0 , 0);
    m_transactionLock->release();
}

void DatabaseConnection::rollbackTransaction()
{
    getLock();
    sqlite3_exec(m_databaseConnection, "ROLLBACK TRANSACTION ", 0, 0, 0);
    m_transactionLock->release();
    releaseLock();
}

sqlite3* DatabaseConnection::getConnection()
{
    if (!connected()) open();

    return m_databaseConnection;
}

bool DatabaseConnection::connected()
{
    return m_databaseConnection != NULL;
}

void DatabaseConnection::getLock()
{
    m_databaseLock->acquire();
}

void DatabaseConnection::releaseLock()
{
    m_databaseLock->release();
}

void DatabaseConnection::close()
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
        case SQLITE_OK :        state.setStatus( QObject::tr("Normal") , true , 0 );
                                break;
        case SQLITE_ERROR :     state.setStatus( QObject::tr("Database is corrupted or SQL error syntax") , false , 2001 );
                                break;
        case SQLITE_BUSY :      state.setStatus( QObject::tr("Database is locked") , false , 2006 );
                                break;
        case SQLITE_CORRUPT :   state.setStatus( QObject::tr("Database corrupted") , false , 2011 );
                                break;
        case SQLITE_CONSTRAINT: state.setStatus( QObject::tr("The new register is duplicated") , false , 2019 );
                                break;
        case 50 :               state.setStatus( QObject::tr("Not connected to database") , false , 2050 );
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus( QObject::tr("Internal Database error (SQLITE)") , false , 2000 + numState );
                                break;
    }

   return state;
}

DatabaseConnection::~DatabaseConnection()
{
    if (connected())
        close();
}

};
