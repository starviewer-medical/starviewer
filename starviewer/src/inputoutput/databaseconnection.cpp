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
    m_transactionLock = new QSemaphore(1);
}

void DatabaseConnection::setDatabasePath(const QString &path)
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
    sqlite3_exec(m_databaseConnection, "ROLLBACK TRANSACTION ", 0, 0, 0);
    m_transactionLock->release();
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

void DatabaseConnection::close()
{
    if ( connected() )
    {
        sqlite3_close( m_databaseConnection );
    }
}

DatabaseConnection::~DatabaseConnection()
{
    if (connected())
        close();
}

};
