/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cacheinstallation.h"
#include "starviewersettings.h"
#include <QDir>
#include <QFile>
#include "databaseconnection.h"
#include "logging.h"


namespace udg {

CacheInstallation::CacheInstallation()
{
}

bool CacheInstallation::checkInstallationCacheImagePath()
{
    QString missatgeLog;
    StarviewerSettings settings;

    if ( !existsCacheImagePath() )
    {
        if ( !createCacheImageDir() )
        {
            missatgeLog = "Error el path de la cache d'imatges no s'ha pogut crear ";
            missatgeLog.append( settings.getCacheImagePath() );

            ERROR_LOG ( missatgeLog.toAscii().constData() );
            return false;
        }
    }

    INFO_LOG( "Estat de la cache d'imatges correcte " );

    missatgeLog = "Cache d'imatges utilitzada : ";
    missatgeLog.append( settings.getCacheImagePath());

    INFO_LOG( missatgeLog.toAscii().constData()  );

    return true;
}

bool CacheInstallation::checkInstallationCacheDatabase()
{
    StarviewerSettings settings;
    QString missatgeLog;

    if ( !existsDatabasePath() )
    {
        if ( !createDatabaseDir() )
        {
            missatgeLog = "Error el path de la base de dades no s'ha pogut crear ";
            missatgeLog.append( settings.getDatabasePath() );
            ERROR_LOG( missatgeLog.toAscii().constData() );

            return false;
        }
    }

    if ( !existsDatabaseFile() )
    {
        if ( !createDatabaseFile() )
        {
            missatgeLog = "Error no s'ha pogut crear la base de dades a ";
            missatgeLog.append( settings.getDatabasePath() );
            ERROR_LOG( missatgeLog.toAscii().constData() );

            return false;
        }
    }

    INFO_LOG( "Estat de la base de dades correcte " );

    missatgeLog =  "Base de dades utilitzada : ";
    missatgeLog.append( settings.getDatabasePath() );
    INFO_LOG( missatgeLog.toAscii().constData() );
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
	QString missatgeLog;

    if ( cacheImageDir.mkpath( settings.getCacheImagePath() ) )
    {
        missatgeLog = "S'ha creat el directori de la cache d'imatges ";
        missatgeLog.append( settings.getCacheImagePath() );
		INFO_LOG( missatgeLog.toAscii().constData() );
        return true;
    }
    else
    {
        missatgeLog = "No s'ha pogut crear el directori de la cache d'imatges ";
        missatgeLog.append( settings.getCacheImagePath() );
        ERROR_LOG( missatgeLog.toAscii().constData() );
        return false;
    }
}

bool CacheInstallation::createDatabaseDir()
{
    StarviewerSettings settings;
    QDir databaseDir;
    QString databaseFile,databasePath , missatgeLog;

    //al path de la base de dades, hi ha inclos el nom del fitxer de la base de dades, per crear el directori hem de treure el fitxer de la cadena
    databaseFile = settings.getDatabasePath();
    databasePath.insert( 0 , databaseFile.left (databaseFile.lastIndexOf( "/" , -1 , Qt::CaseInsensitive ) ) );


    if ( databaseDir.mkpath( databasePath ) )
    {
        missatgeLog = "S'ha creat el directori de la cache d'imatges ";
        missatgeLog.append( databasePath );
	INFO_LOG( missatgeLog.toAscii().constData() );
        return true;
    }
    else
    {
        missatgeLog = "No s'ha pogut crear el directori de la cache d'imatges ";
        missatgeLog.append( databasePath.toAscii().constData() );
        ERROR_LOG( missatgeLog.toAscii().constData() );
        return false;
    }

}

bool CacheInstallation::createDatabaseFile()
{
    QFile sqlTablesScriptFile( ":cache/database.sql" );
    QByteArray sqlTablesScript;
    DatabaseConnection *DBConnect = DatabaseConnection::getDatabaseConnection();//obrim la bdd
    int estat;

    sqlTablesScriptFile.open( QIODevice::ReadOnly ); //obrim el fitxer

    sqlTablesScript = sqlTablesScriptFile.read( sqlTablesScriptFile.size() ); //el llegim

    if ( !DBConnect->connected() ) return false;

    estat = sqlite3_exec( DBConnect->getConnection() , sqlTablesScript.constData() , 0 , 0 , 0 ); //creem les taules i els registres

    sqlTablesScriptFile.close(); //tanquem el fitxer

    if ( estat == 0 )
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
