/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cacheinstallation.h"
#include "starviewersettings.h"
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include "databaseconnection.h"

namespace udg {

CacheInstallation::CacheInstallation()
{
}

bool CacheInstallation::checkInstallation()
{
    if ( !existsCacheImagePath() )
    {
        if ( !createCacheImageDir() ) return false;
    }

    if ( !existsDatabasePath() )
    {
        if ( !createDatabaseDir() ) return false;
    }
    
    if ( !existsDatabaseFile() )
    {
        if ( !createDatabaseFile() ) return false;
        
    }
    
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
    return cacheImageDir.mkpath( settings.getCacheImagePath() );
}

bool CacheInstallation::createDatabaseDir()
{
    StarviewerSettings settings;
    QDir databaseDir;
    QString databaseFile,databasePath;
    
    //al path de la base de dades, hi ha inclos el nom del fitxer de la base de dades, per crear el directori hem de treure el fitxer de la cadena    
    databaseFile = settings.getDatabasePath();
    databasePath.insert( 0 , databaseFile.left (databaseFile.lastIndexOf( "/" , -1 , Qt::CaseInsensitive ) ) );
    
    return databaseDir.mkpath( databasePath );
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
    
    estat = sqlite_exec_printf( DBConnect->getConnection() , sqlTablesScript.constData() , 0 , 0 , 0 ); //creem les taules i els registres
    
    sqlTablesScriptFile.close(); //tanquem el fitxer
    
    if ( estat == 0 )
    {
        return true;
    }
    else return false;
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
