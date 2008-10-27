/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "databaseinstallation.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <sqlite3.h>

#include "starviewersettings.h"
#include "databaseconnection.h"
#include "logging.h"
#include "deletedirectory.h"
#include "localdatabasemanager.h"
#include "starviewerapplication.h"

namespace udg {

DatabaseInstallation::DatabaseInstallation()
{
}

bool DatabaseInstallation::checkInstallationDatabaseImagePath()
{
    StarviewerSettings settings;

    if ( !existsDatabaseImagePath() )
    {
        if ( !createDatabaseImageDir() )
        {
            ERROR_LOG( "Error el path de la cache d'imatges no s'ha pogut crear " + settings.getCacheImagePath() );
            return false;
        }
    }

    INFO_LOG( "Estat de la cache d'imatges correcte " );
    INFO_LOG( "Cache d'imatges utilitzada : " + settings.getCacheImagePath() );

    return true;
}

bool DatabaseInstallation::checkInstallationDatabase()
{
    StarviewerSettings settings;
    LocalDatabaseManager localDatabaseManager;

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
    else
    {
        if (localDatabaseManager.getDatabaseRevision() != StarviewerDatabaseRevisionRequired)
        {
            INFO_LOG("La revisió actual de la base de dades és " + QString().setNum(localDatabaseManager.getDatabaseRevision()) + " per aquesta versió d'Starviewer és necessària la " + QString().setNum(StarviewerDatabaseRevisionRequired) + ", es procedirà a actualitzar la base de dades");
            updateDatabaseRevision();
        }
    }

    INFO_LOG( "Estat de la base de dades correcte " );
    INFO_LOG( "Base de dades utilitzada : " + settings.getDatabasePath() + " revisió " +  QString().setNum(localDatabaseManager.getDatabaseRevision()));
    return true;
}

bool DatabaseInstallation::existsDatabaseImagePath()
{
    StarviewerSettings settings;
    QDir cacheImageDir;

    return cacheImageDir.exists( settings.getCacheImagePath() );
}

bool DatabaseInstallation::existsDatabasePath()
{
    StarviewerSettings settings;
    QDir databaseDir;

    return databaseDir.exists( settings.getDatabasePath() );
}

bool DatabaseInstallation::existsDatabaseFile()
{
    StarviewerSettings settings;
    QFile databaseFile;

    return databaseFile.exists( settings.getDatabasePath() );
}

bool DatabaseInstallation::createDatabaseImageDir()
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

bool DatabaseInstallation::createDatabaseDir()
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

bool DatabaseInstallation::createDatabaseFile()
{
    QFile sqlTablesScriptFile( ":cache/database.sql" );
    QByteArray sqlTablesScript;
    DatabaseConnection DBConnect;//obrim la bdd
    int status;

    sqlTablesScriptFile.open( QIODevice::ReadOnly ); //obrim el fitxer

    sqlTablesScript = sqlTablesScriptFile.read( sqlTablesScriptFile.size() ); //el llegim

    DBConnect.open();
    if (!DBConnect.connected()) return false;

    status = sqlite3_exec(DBConnect.getConnection(), sqlTablesScript.constData(), 0, 0, 0); //creem les taules i els registres
    DBConnect.close();

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

bool DatabaseInstallation::reinstallDatabaseFile()
{
    QDir databaseFile;
    StarviewerSettings settings;
    DeleteDirectory deleteDirectory;

    //si existeix l'esborrem
    if (existsDatabaseFile())
    {
        QFile databaseFile;
        databaseFile.remove(settings.getDatabasePath());
    }

    //si no existeix el directori de la base de dades el creem
    if (!existsDatabasePath())
    {
        createDatabaseDir();
    }
    createDatabaseFile();

    //Esborrem les imatges que tenim a la base de dades local, al reinstal·lar la bd ja no té sentit mantenir-les
    deleteDirectory.deleteDirectory(settings.getCacheImagePath(), false);

    return existsDatabaseFile();
}

bool DatabaseInstallation::updateDatabaseRevision()
{
    /*Per aquesta versió degut a que s'ha tornat a reimplementar i a reestructurar tota la base de dades fent importants 
        *canvis, no s'ha fet cap codi per transformar la bd antiga amb la nova, per això es reinstal·la la BD*/
     if  (!reinstallDatabaseFile())
    {
        ERROR_LOG("HA FALLAT L'ACTUALITZACIÓ DE LA BASE DE DADES");
        return false;
    }
    else return true;
}

DatabaseInstallation::~DatabaseInstallation()
{

}

}
