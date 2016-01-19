/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "databaseinstallation.h"

#include "databaseconnection.h"
#include "directoryutilities.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "upgradedatabaserevisioncommands.h"
#include "upgradedatabasexmlparser.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

namespace udg {

namespace {

// Returns the UpgradeDatabaseXMLParser filled with the upgrade XML.
UpgradeDatabaseXMLParser getUpgradeDatabaseXmlParser()
{
    QFile file(":cache/upgradeDatabase.xml");

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        ERROR_LOG("Can't read database upgrade file.");
        return UpgradeDatabaseXMLParser(QString());
    }

    QTextStream stream(&file);
    QString upgradeXml = stream.readAll();
    file.close();

    return UpgradeDatabaseXMLParser(upgradeXml);
}

// Returns true if the database can be upgraded to the required revision and fals otherwise.
bool databaseCanBeUpgraded(const UpgradeDatabaseXMLParser &upgradeDatabaseXmlParser)
{
    int minimumDatabaseRevisionRequired = upgradeDatabaseXmlParser.getMinimumDatabaseRevisionRequiredToUpgrade();

    if (minimumDatabaseRevisionRequired < 0)
    {
        ERROR_LOG("Could not get the minimum required version to upgrade the database.");
        return false;
    }

    if (LocalDatabaseManager().getDatabaseRevision() >= minimumDatabaseRevisionRequired)
    {
        return true;
    }
    else
    {
        INFO_LOG(QString("Database is too old to be upgraded. Current version: %1. Minimum version to upgrade: %2.")
                 .arg(LocalDatabaseManager().getDatabaseRevision()).arg(minimumDatabaseRevisionRequired));
        return false;
    }
}

// Upgrades the database to the required revision. Returns true if successful and false otherwise.
bool upgradeDatabase(const UpgradeDatabaseXMLParser &upgradeDatabaseXmlParser)
{
    UpgradeDatabaseRevisionCommands upgradeDatabaseRevisionCommands =
            upgradeDatabaseXmlParser.getUpgradeDatabaseRevisionCommands(LocalDatabaseManager().getDatabaseRevision());

    if (upgradeDatabaseRevisionCommands.getSqlUpgradeCommands().isEmpty())
    {
        ERROR_LOG("Could not read any database upgrade command.");
        return false;
    }

    DatabaseConnection databaseConnection;
    QSqlQuery query(databaseConnection.getConnection());

    foreach (const QString &sqlUpgradeCommand, upgradeDatabaseRevisionCommands.getSqlUpgradeCommands())
    {
        if (query.exec(sqlUpgradeCommand))
        {
            INFO_LOG("Database upgrade command applied successfully: " + query.lastQuery());
        }
        else
        {
            ERROR_LOG(QString("Database upgrade command failed: %1. Error: %2") .arg(query.lastQuery()).arg(query.lastError().text()));
            return false;
        }
    }

    LocalDatabaseManager localDatabaseManager;
    localDatabaseManager.setDatabaseRevision(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision());

    if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok)
    {
        INFO_LOG(QString("Database upgraded to revision %1.").arg(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision()));
        return true;
    }
    else
    {
        ERROR_LOG(QString("Error upgrading database to revision %1.").arg(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision()));
        return false;
    }
}

}

DatabaseInstallation::DatabaseInstallation()
{
}

DatabaseInstallation::~DatabaseInstallation()
{
}

bool DatabaseInstallation::checkDatabase()
{
    m_errorMessage = "";

    return checkLocalImagePath() && checkDatabaseFile() && checkDatabaseRevision();
}

bool DatabaseInstallation::reinstallDatabase()
{
    QFileInfo databaseFileInfo(LocalDatabaseManager::getDatabaseFilePath());

    if (databaseFileInfo.exists())
    {
        if (!QFile().remove(LocalDatabaseManager::getDatabaseFilePath()))
        {
            ERROR_LOG("Can't remove database file " + LocalDatabaseManager::getDatabaseFilePath());
            m_errorMessage = QObject::tr("Can't reinstall database because the current database can't be removed.");
            return false;
        }
    }

    return createDatabaseFile();
}

bool DatabaseInstallation::createDatabase(DatabaseConnection &databaseConnection)
{
    QFile sqlTablesScriptFile(":cache/database.sql");

    if (!sqlTablesScriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ERROR_LOG("Can't read database creation script.");
        m_errorMessage = QObject::tr("Can't read database creation script. Can't create the database.");
        return false;
    }

    QTextStream stream(&sqlTablesScriptFile);
    QString sqlTablesScript = stream.readAll();
    sqlTablesScriptFile.close();

    // Trimmed to remove newline at end of file
    QStringList sqlCommands = sqlTablesScript.trimmed().split(';', QString::SkipEmptyParts);

    QSqlQuery query(databaseConnection.getConnection());

    foreach (const QString &command, sqlCommands)
    {
        if (!query.exec(command))
        {
            ERROR_LOG(QString("Database creation SQL command failed: %1. Error: %2").arg(query.lastQuery()).arg(query.lastError().text()));
            m_errorMessage = QObject::tr("Database creation script failed.");
            return false;
        }
    }

    INFO_LOG("Database created successfully.");

    return true;
}

const QString& DatabaseInstallation::getErrorMessage() const
{
    return m_errorMessage;
}

bool DatabaseInstallation::checkLocalImagePath()
{
    QFileInfo imagePathInfo(LocalDatabaseManager::getCachePath());

    if (!imagePathInfo.exists())
    {
        if (!createLocalImagePath())
        {
            return false;
        }
    }

    if (!imagePathInfo.isWritable())
    {
        ERROR_LOG("Local image cache directory doesn't have write permission: " + LocalDatabaseManager::getCachePath());
        m_errorMessage = QObject::tr("You don't have write permission on cache image directory. Retrieval or importing of new studies will fail.");
        return false;
    }

    INFO_LOG("Local image cache check OK.");
    INFO_LOG("Local image path: " + LocalDatabaseManager::getCachePath());

    return true;
}

bool DatabaseInstallation::checkDatabasePath()
{
    QFileInfo databasePathInfo(QFileInfo(LocalDatabaseManager::getDatabaseFilePath()).path());

    if (!databasePathInfo.exists())
    {
        if (!createDatabasePath())
        {
            return false;
        }
    }

    if (!databasePathInfo.isWritable())
    {
        ERROR_LOG("Database directory doesn't have write permission: " + databasePathInfo.path());
        m_errorMessage = QObject::tr("You don't have write permission on the database directory. Can't create the database.");
        return false;
    }

    return true;
}

bool DatabaseInstallation::checkDatabaseFile()
{
    QFileInfo databaseFileInfo(LocalDatabaseManager::getDatabaseFilePath());

    if (!databaseFileInfo.exists())
    {
        if (!createDatabaseFile())
        {
            return false;
        }
    }

    if (!databaseFileInfo.isWritable())
    {
        ERROR_LOG("Database file doesn't have write permission: " + LocalDatabaseManager::getDatabaseFilePath());
        m_errorMessage = QObject::tr("You don't have write permission on %1 database. Retrieval or importing of new studies will fail.")
                .arg(ApplicationNameString);
        return false;
    }

    INFO_LOG("Database file check OK.");
    INFO_LOG("Database file: " + LocalDatabaseManager::getDatabaseFilePath());

    return true;
}

bool DatabaseInstallation::checkDatabaseRevision()
{
    LocalDatabaseManager localDatabaseManager;
    int databaseRevision = localDatabaseManager.getDatabaseRevision();

    if (databaseRevision < StarviewerDatabaseRevisionRequired)
    {
        INFO_LOG(QString("Current database revision %1 is older than the required revision %2 for this version of %3.")
                 .arg(databaseRevision).arg(StarviewerDatabaseRevisionRequired).arg(ApplicationNameString));

        UpgradeDatabaseXMLParser upgradeDatabaseXmlParser = getUpgradeDatabaseXmlParser();
        bool canUpgrade = databaseCanBeUpgraded(upgradeDatabaseXmlParser);

        if (canUpgrade)
        {
            INFO_LOG("Database will be upgraded.");

            if (!upgradeDatabase(upgradeDatabaseXmlParser))
            {
                canUpgrade = false;
            }
        }

        if (!canUpgrade)
        {
            INFO_LOG("Database will be reinstalled.");

            if (!deleteLocalImagesAndReinstallDatabase())
            {
                return false;
            }
        }
    }
    else if (databaseRevision > StarviewerDatabaseRevisionRequired)
    {
        INFO_LOG(QString("Current database revision %1 is newer than the required revision %2 for this version of %3. "
                         "Asking user to choose between quitting or deleting studies and reinstalling the database.")
                 .arg(databaseRevision).arg(StarviewerDatabaseRevisionRequired).arg(ApplicationNameString));

        if (userWantsToReinstallDatabase())
        {
            INFO_LOG("Database will be reinstalled.");

            if (!deleteLocalImagesAndReinstallDatabase())
            {
                return false;
            }
        }
        else
        {
            INFO_LOG(QString("User doesn't want to reinstall the database. %1 will close.").arg(ApplicationNameString));
            // TODO find a cleaner way to exit
            throw 0;
        }
    }

    INFO_LOG("Database revision check OK.");
    INFO_LOG(QString("Database revision: %1").arg(localDatabaseManager.getDatabaseRevision()));

    return true;
}

bool DatabaseInstallation::createLocalImagePath()
{
    QDir localImageDir;

    if (localImageDir.mkpath(LocalDatabaseManager::getCachePath()))
    {
        INFO_LOG("Created local image cache directory: " + LocalDatabaseManager::getCachePath());
        return true;
    }
    else
    {
        ERROR_LOG("Could not create local image cache directory: " + LocalDatabaseManager::getCachePath());
        m_errorMessage = QObject::tr("Unable to create the cache image directory. Please check user permissions.");
        return false;
    }
}

bool DatabaseInstallation::createDatabasePath()
{
    QString databasePath = QFileInfo(LocalDatabaseManager::getDatabaseFilePath()).path();
    QDir databaseDir;

    if (databaseDir.mkpath(databasePath))
    {
        INFO_LOG("Created database directory: " + databasePath);
        return true;
    }
    else
    {
        ERROR_LOG("Could not create database directory: " + databasePath);
        m_errorMessage = QObject::tr("Unable to create the database directory. Please check user permissions.");
        return false;
    }
}

bool DatabaseInstallation::createDatabaseFile()
{
    if (!checkDatabasePath())
    {
        return false;
    }

    DatabaseConnection databaseConnection;
    return createDatabase(databaseConnection);
}

bool DatabaseInstallation::deleteLocalImagesAndReinstallDatabase()
{
    QProgressDialog progressDialog(QObject::tr("Reinstalling database"), QString(), 0, 0);
    progressDialog.setCancelButton(0);
    progressDialog.setModal(true);
    progressDialog.show();

    // Return value is ignored
    DirectoryUtilities directoryUtilities;
    directoryUtilities.deleteDirectory(LocalDatabaseManager::getCachePath(), false);

    progressDialog.close();

    return reinstallDatabase();
}

bool DatabaseInstallation::userWantsToReinstallDatabase() const
{
    QString questionMessage = QObject::tr("Current database is of a newer version. "
                                          "In order to run %1, local studies must be deleted and the database reinstalled. Do you want to continue?")
            .arg(ApplicationNameString);

    return QMessageBox::question(0, ApplicationNameString, questionMessage, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;
}

}
