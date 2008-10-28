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

bool DatabaseInstallation::checkStarviewerDatabase()
{
    StarviewerSettings settings;
    LocalDatabaseManager localDatabaseManager;

    //Comprovem que existeix el path on s'importen les imatges, sinó existeix l'intentarà crear
    if (!checkLocalImagePath()) return false;

    //Comprovem que existeix el path on es guarda la base de dades, sinó existeix l'intentarà crear
    if (!checkDatabasePath()) return false;

    if (!existsDatabaseFile())
    {
        if (!createDatabaseFile())
        {
            ERROR_LOG("Error no s'ha pogut crear la base de dades a " + settings.getDatabasePath());
            return false;
        }
    }
    else
    {
        checkDatabaseRevision();
    }

    INFO_LOG("Estat de la base de dades correcte ");
    INFO_LOG("Base de dades utilitzada : " + settings.getDatabasePath() + " revisió " +  QString().setNum(localDatabaseManager.getDatabaseRevision()));
    return true;
}

bool DatabaseInstallation::checkLocalImagePath()
{
    StarviewerSettings settings;

    if (!existsLocalImagePath())
    {
        if (!createLocalImageDir())
        {
            ERROR_LOG("Error el path de la cache d'imatges no s'ha pogut crear " + settings.getCacheImagePath());
            return false;
        }
    }

    INFO_LOG("Estat de la cache d'imatges correcte ");
    INFO_LOG("Cache d'imatges utilitzada : " + settings.getCacheImagePath());

    return true;
}

bool DatabaseInstallation::checkDatabasePath()
{
    StarviewerSettings settings;

    if (!existsDatabasePath())
    {
        if (!createDatabaseDirectory())
        {
            ERROR_LOG("Error el path de la base de dades no s'ha pogut crear " + settings.getDatabasePath());
            return false;
        }
    }

    return true;
}

bool DatabaseInstallation::checkDatabaseRevision()
{
    LocalDatabaseManager localDatabaseManager;

    if (localDatabaseManager.getDatabaseRevision() != StarviewerDatabaseRevisionRequired)
    {
        INFO_LOG("La revisió actual de la base de dades és " + QString().setNum(localDatabaseManager.getDatabaseRevision()) + " per aquesta versió d'Starviewer és necessària la " + QString().setNum(StarviewerDatabaseRevisionRequired) + ", es procedirà a actualitzar la base de dades");

        return updateDatabaseRevision();
    }
    else return true;
}

bool DatabaseInstallation::reinstallDatabaseFile()
{
    QDir databaseFile;
    StarviewerSettings settings;
    DeleteDirectory *deleteDirectory = new DeleteDirectory();

    //si existeix l'esborrem
    if (existsDatabaseFile())
    {
        QFile databaseFile;
        databaseFile.remove(settings.getDatabasePath());
    }

    //si no existeix el directori de la base de dades el creem
    if (!existsDatabasePath())
    {
        createDatabaseDirectory();
    }
    createDatabaseFile();

    //Esborrem les imatges que tenim a la base de dades local, al reinstal·lar la bd ja no té sentit mantenir-les, i per cada directori esborrat movem la barra de progrés
    connect(deleteDirectory, SIGNAL(directoryDeleted()), this, SLOT(setValueProgressBar()));
    deleteDirectory->deleteDirectory(settings.getCacheImagePath(), false);
    delete deleteDirectory;

    return existsDatabaseFile();
}

bool DatabaseInstallation::updateDatabaseRevision()
{
    bool status;

    //Creem barra de progrés per donar feedback
    m_qprogressDialog = new QProgressDialog(tr ("Updating database"), "", 0, 0);
    m_qprogressDialog->setValue(1);

    /*Per aquesta versió degut a que s'ha tornat a reimplementar i a reestructurar tota la base de dades fent importants 
     *canvis, no s'ha fet cap codi per transformar la bd antiga amb la nova, per això es reinstal·la la BD*/
    status = reinstallDatabaseFile();

    if (!status)
    {
        ERROR_LOG("HA FALLAT L'ACTUALITZACIÓ DE LA BASE DE DADES");
    }

    m_qprogressDialog->close();

    return status;
}

bool DatabaseInstallation::existsLocalImagePath()
{
    StarviewerSettings settings;
    QDir cacheImageDir;

    return cacheImageDir.exists(settings.getCacheImagePath());
}

bool DatabaseInstallation::existsDatabasePath()
{
    StarviewerSettings settings;
    QDir databaseDir;

    return databaseDir.exists(settings.getDatabasePath());
}

bool DatabaseInstallation::existsDatabaseFile()
{
    StarviewerSettings settings;
    QFile databaseFile;

    return databaseFile.exists(settings.getDatabasePath());
}

bool DatabaseInstallation::createLocalImageDir()
{
    StarviewerSettings settings;
    QDir cacheImageDir;

    if (cacheImageDir.mkpath(settings.getCacheImagePath()))
    {
        INFO_LOG("S'ha creat el directori de la cache d'imatges " + settings.getCacheImagePath());
        return true;
    }
    else
    {
        ERROR_LOG("No s'ha pogut crear el directori de la cache d'imatges " + settings.getCacheImagePath());
        return false;
    }
}

bool DatabaseInstallation::createDatabaseDirectory()
{
    StarviewerSettings settings;
    QDir databaseDir;

    QFileInfo databaseFilePath(settings.getDatabasePath());
    QString databasePath = databaseFilePath.path();

    if (databaseDir.mkpath(databasePath))
    {
        INFO_LOG("S'ha creat el directori de la base de dades " + databasePath);
        return true;
    }
    else
    {
        ERROR_LOG("No s'ha pogut crear el directori de la base de dades " + databasePath);
        return false;
    }
}

bool DatabaseInstallation::createDatabaseFile()
{
    QFile sqlTablesScriptFile(":cache/database.sql");
    QByteArray sqlTablesScript;
    DatabaseConnection DBConnect;//obrim la bdd
    int status;

    sqlTablesScriptFile.open(QIODevice::ReadOnly); //obrim el fitxer

    sqlTablesScript = sqlTablesScriptFile.read(sqlTablesScriptFile.size()); //el llegim

    DBConnect.open();
    if (!DBConnect.connected()) return false;

    status = sqlite3_exec(DBConnect.getConnection(), sqlTablesScript.constData(), 0, 0, 0); //creem les taules i els registres
    DBConnect.close();

    sqlTablesScriptFile.close(); //tanquem el fitxer

    if (status == 0)
    {
        INFO_LOG("S'ha creat correctament la base de dades");
        return true;
    }
    else
    {
        ERROR_LOG("No s'ha pogut crear la base de dades");
        return false;
    }
}

void DatabaseInstallation::setValueProgressBar()
{
    m_qprogressDialog->setValue(m_qprogressDialog->value() + 1);
}

DatabaseInstallation::~DatabaseInstallation()
{
}

}
