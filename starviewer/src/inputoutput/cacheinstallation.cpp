/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cacheinstallation.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <sqlite3.h>

#include "starviewersettings.h"
#include "databaseconnection.h"
#include "logging.h"

namespace udg {

CacheInstallation::CacheInstallation()
{
}

bool CacheInstallation::checkInstallationCacheImagePath()
{
    StarviewerSettings settings;

    if ( !existsCacheImagePath() )
    {
        if ( !createCacheImageDir() )
        {
            ERROR_LOG( "Error el path de la cache d'imatges no s'ha pogut crear " + settings.getCacheImagePath() );
            return false;
        }
    }

    INFO_LOG( "Estat de la cache d'imatges correcte " );
    INFO_LOG( "Cache d'imatges utilitzada : " + settings.getCacheImagePath() );

    return true;
}

bool CacheInstallation::checkInstallationCacheDatabase()
{
    StarviewerSettings settings;

    if ( !existsDatabasePath() )
    {
        if ( !createDatabaseDir() )
        {
            ERROR_LOG( "Error el path de la base de dades no s'ha pogut crear " + settings.getDatabasePath() );
            return false;
        }
    }

    if ( !existsDatabaseFile() )
    {
        if ( !createDatabaseFile() )
        {
            ERROR_LOG( "Error no s'ha pogut crear la base de dades a " + settings.getDatabasePath() );
            return false;
        }
    }

    INFO_LOG( "Estat de la base de dades correcte " );
    INFO_LOG( "Base de dades utilitzada : " + settings.getDatabasePath() );
    return true;
}

bool CacheInstallation::existsCacheImagePath()
{
    StarviewerSettings settings;
    QDir cacheImageDir;

    return cacheImageDir.exists( settings.getCacheImagePath() );
}

bool CacheInstallation::existsDatabasePath()
{
    StarviewerSettings settings;
    QDir databaseDir;

    return databaseDir.exists( settings.getDatabasePath() );
}

bool CacheInstallation::existsDatabaseFile()
{
    StarviewerSettings settings;
    QFile databaseFile;

    return databaseFile.exists( settings.getDatabasePath() );
}

bool CacheInstallation::createCacheImageDir()
{
    StarviewerSettings settings;
    QDir cacheImageDir;

    if ( cacheImageDir.mkpath( settings.getCacheImagePath() ) )
    {
		INFO_LOG( "S'ha creat el directori de la cache d'imatges " + settings.getCacheImagePath() );
        return true;
    }
    else
    {
        ERROR_LOG( "No s'ha pogut crear el directori de la cache d'imatges " + settings.getCacheImagePath() );
        return false;
    }
}

bool CacheInstallation::createDatabaseDir()
{
    StarviewerSettings settings;
    QDir databaseDir;

    QFileInfo databaseFilePath( settings.getDatabasePath() );
    QString databasePath = databaseFilePath.path();

    if ( databaseDir.mkpath( databasePath ) )
    {
        INFO_LOG( "S'ha creat el directori de la base de dades " + databasePath );
        return true;
    }
    else
    {
        ERROR_LOG( "No s'ha pogut crear el directori de la base de dades " + databasePath );
        return false;
    }
}

bool CacheInstallation::createDatabaseFile()
{
    QFile sqlTablesScriptFile( ":cache/database.sql" );
    QByteArray sqlTablesScript;
    DatabaseConnection *DBConnect = DatabaseConnection::getDatabaseConnection();//obrim la bdd
    int status;

    sqlTablesScriptFile.open( QIODevice::ReadOnly ); //obrim el fitxer

    sqlTablesScript = sqlTablesScriptFile.read( sqlTablesScriptFile.size() ); //el llegim

    if ( !DBConnect->connected() ) return false;

    status = sqlite3_exec( DBConnect->getConnection() , sqlTablesScript.constData() , 0 , 0 , 0 ); //creem les taules i els registres

    sqlTablesScriptFile.close(); //tanquem el fitxer

    if ( status == 0 )
    {
		INFO_LOG( "S'ha creat correctament la base de dades" );
        return true;
    }
    else
 	{
		ERROR_LOG( "No s'ha pogut crear la base de dades" );
		return false;
	}
}

bool CacheInstallation:: reinstallDatabaseFile()
{
    QDir databaseFile;
    StarviewerSettings settings;

    //si existeix l'esborrem
    if ( existsDatabaseFile() )
    {
        QFile databaseFile;
        databaseFile.remove( settings.getDatabasePath() );
    }

    //si no existeix el directori de la base de dades el creem
    if ( !existsDatabasePath() )
    {
        createDatabaseDir();
    }
    createDatabaseFile();

    return existsDatabaseFile();
}

CacheInstallation::~CacheInstallation()
{

}

}
