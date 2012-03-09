#include "databaseinstallation.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <sqlite3.h>

#include "databaseconnection.h"
#include "logging.h"
#include "deletedirectory.h"
#include "localdatabasemanager.h"
#include "starviewerapplication.h"
#include "upgradedatabasexmlparser.h"
#include "upgradedatabaserevisioncommands.h"

namespace udg {

DatabaseInstallation::DatabaseInstallation()
 : m_qprogressDialog(0)
{
}

DatabaseInstallation::~DatabaseInstallation()
{
}

bool DatabaseInstallation::checkStarviewerDatabase()
{
    LocalDatabaseManager localDatabaseManager;
    m_errorMessage = "";
    bool isCorrect;

    // Comprovem que existeix el path on s'importen les imatges, sinó existeix l'intentarà crear
    isCorrect = checkLocalImagePath();

    if (!existsDatabaseFile())
    {
        if (!createDatabaseFile())
        {
            ERROR_LOG("Error no s'ha pogut crear la base de dades a " + LocalDatabaseManager::getDatabaseFilePath());
            m_errorMessage.append(tr("\nCan't create database, be sure you have write permissions on database directory."));
            isCorrect = false;
        }
    }
    else
    {
        // Comprovar que tenim permisos d'escriptura a la BDD
        if (!isDatabaseFileWritable())
        {
            // TODO què fem? cal retornar fals? Avisar a l'usuari?
            ERROR_LOG("L'arxiu de base de dades [" + LocalDatabaseManager::getDatabaseFilePath() + "] no es pot obrir amb permisos d'escriptura, " +
                      "no podrem guardar estudis nous ni modificar els ja existents.");
            m_errorMessage.append(tr("\nYou don't have write permissions on %1 database, you couldn't retrieve or import new studies.")
                                    .arg(ApplicationNameString));
        }

        isCorrect = checkDatabaseRevision();
        if (!isCorrect)
        {
            m_errorMessage.append(tr("\nCan't upgrade database file, be sure you have write permissions on database directory."));
        }
    }

    if (!isCorrect)
    {
        return false;
    }

    INFO_LOG("Estat de la base de dades correcte ");
    INFO_LOG("Base de dades utilitzada : " + LocalDatabaseManager::getDatabaseFilePath() + " revisio " +
             QString().setNum(localDatabaseManager.getDatabaseRevision()));
    return true;
}

bool DatabaseInstallation::checkLocalImagePath()
{
    if (!existsLocalImagePath())
    {
        if (!createLocalImageDir())
        {
            ERROR_LOG("Error el path de la cache d'imatges no s'ha pogut crear " + LocalDatabaseManager::getCachePath());
            m_errorMessage.append(tr("\nCan't create the cache image directory. Please check users permissions."));
            return false;
        }
    }
    else
    {
        // Comprovar que tenim permisos d'escriptura al directori local d'imatges
        QFileInfo imagePathInfo(LocalDatabaseManager::getCachePath());
        if (!imagePathInfo.isWritable())
        {
            ERROR_LOG("El directori de la cache d'imatges no te permisos d'escriptura: " + LocalDatabaseManager::getCachePath());
            m_errorMessage.append(tr("\nYou don't have write permissions on cache image directory. You couldn't retrieve or import new studies."));
            return false;
        }
    }

    INFO_LOG("Estat de la cache d'imatges correcte ");
    INFO_LOG("Cache d'imatges utilitzada : " + LocalDatabaseManager::getCachePath());

    return true;
}

bool DatabaseInstallation::checkDatabasePath()
{
    if (!existsDatabasePath())
    {
        if (!createDatabaseDirectory())
        {
            ERROR_LOG("Error el path de la base de dades no s'ha pogut crear " + LocalDatabaseManager::getDatabaseFilePath());
            return false;
        }
    }

    return true;
}

bool DatabaseInstallation::checkDatabaseRevision()
{
    LocalDatabaseManager localDatabaseManager;

    if (localDatabaseManager.getDatabaseRevision() == StarviewerDatabaseRevisionRequired)
    {
        return true;
    }
    else if (StarviewerDatabaseRevisionRequired  <localDatabaseManager.getDatabaseRevision())
    {
        INFO_LOG(QString("La base de dades es de la versio %1 una versio mes nova que la necessaria la %2, aquesta versio es incomptabile"
                         " amb la versio d'%3 que s'esta executant, es demanara a l'usuari si vol abandonar %3, o reinstal.lar la base de dades.")
                 .arg(QString::number(localDatabaseManager.getDatabaseRevision()), QString::number(StarviewerDatabaseRevisionRequired), ApplicationNameString));

        if (askToUserIfDowngradeDatabase())
        {
            INFO_LOG("L'usuari ha indicat que vol reinstal.lar la base de dades");
            recreateDatabase();
        }
        else
        {
            INFO_LOG(QString("L'usuari ha indicat que no vol reinstal.lar la base de dades per tant es tancara %1").arg(ApplicationNameString));
            exit(0);
        }
    }
    else if (localDatabaseManager.getDatabaseRevision() < StarviewerDatabaseRevisionRequired)
    {
        INFO_LOG("La revisio actual de la base de dades es " + QString().setNum(localDatabaseManager.getDatabaseRevision()) + " per aquesta versio d'" +
                 ApplicationNameString + " es necessaria la " + QString().setNum(StarviewerDatabaseRevisionRequired) +
                 ", es procedira a actualitzar la base de dades");

        return tryToUpgradeDatabaseIfNotRecreateDatabase();
    }

    return true;
}

bool DatabaseInstallation::isDatabaseFileWritable()
{
    QFileInfo databaseFilePath(LocalDatabaseManager::getDatabaseFilePath());

    return databaseFilePath.isWritable();
}

bool DatabaseInstallation::reinstallDatabase()
{
    // Si existeix l'esborrem la base de dades
    if (existsDatabaseFile())
    {
        if (!QFile().remove(LocalDatabaseManager::getDatabaseFilePath()))
        {
            ERROR_LOG("Reinstal.lant la base de dades no s'ha pogut esborrar el fitxer de la base de dades " + LocalDatabaseManager::getDatabaseFilePath());
            return false;
        }
    }

    if (!createDatabaseFile())
    {
        return false;
    }
    else
    {
        return existsDatabaseFile();
    }
}

bool DatabaseInstallation::removeCacheAndReinstallDatabase()
{
    DeleteDirectory *deleteDirectory = new DeleteDirectory();

    if (m_qprogressDialog == NULL)
    {
        // Si nó existeix creem barra de progrés per donar feedback
        m_qprogressDialog = new QProgressDialog(tr ("Reinstalling database"), "", 0, 0);
        m_qprogressDialog->setCancelButton(0);
        m_qprogressDialog->setValue(1);
        m_qprogressDialog->setModal(true);
    }

    // Esborrem les imatges que tenim a la base de dades local, al reinstal·lar la bd ja no té sentit mantenir-les, i per cada directori esborrat movem
    // la barra de progrés
    connect(deleteDirectory, SIGNAL(directoryDeleted()), this, SLOT(setValueProgressBar()));
    deleteDirectory->deleteDirectory(LocalDatabaseManager::getCachePath(), false);
    delete deleteDirectory;

    m_qprogressDialog->close();

    return reinstallDatabase();
}

bool DatabaseInstallation::recreateDatabase()
{
    bool status;

    // Creem barra de progrés per donar feedback
    m_qprogressDialog = new QProgressDialog(tr ("Updating database"), "", 0, 0);
    m_qprogressDialog->setCancelButton(0);
    m_qprogressDialog->setValue(1);

    // Per aquesta versió degut a que s'ha tornat a reimplementar i a reestructurar tota la base de dades fent importants
    // canvis, no s'ha fet cap codi per transformar la bd antiga amb la nova, per això es reinstal·la la BD
    status = removeCacheAndReinstallDatabase();

    if (!status)
    {
        ERROR_LOG("HA FALLAT L'ACTUALITZACIO DE LA BASE DE DADES");
    }

    return status;
}

bool DatabaseInstallation::tryToUpgradeDatabaseIfNotRecreateDatabase()
{
    LocalDatabaseManager localDatabaseManager;

    if (canBeUpgradedDatabase())
    {
        if (!upgradeDatabase())
        {
            ERROR_LOG("S'ha produit un error al actualtizar la base de dades, es procedira a reinstal.lar la base de dades");
            return recreateDatabase();
        }

        return true;
    }
    else
    {
        UpgradeDatabaseXMLParser upgradeDatabaseXMLParser(getUpgradeDatabaseRevisionXmlData());
        INFO_LOG("La base de dades actual no es actualitzable. Versio minima necessaria : " +
                 QString::number(upgradeDatabaseXMLParser.getMinimumDatabaseRevisionRequiredToUpgrade())
                  + ", versio de la base de dades actual: " + QString::number(localDatabaseManager.getDatabaseRevision()));
        INFO_LOG("Es procedira a reinstal.lar la base de dades");

        return recreateDatabase();
    }
}

bool DatabaseInstallation::upgradeDatabase()
{
    UpgradeDatabaseXMLParser upgradeDatabaseXMLParser(getUpgradeDatabaseRevisionXmlData());
    UpgradeDatabaseRevisionCommands upgradeDatabaseRevisionCommands = upgradeDatabaseXMLParser.getUpgradeDatabaseRevisionCommands(LocalDatabaseManager().getDatabaseRevision());

    if (upgradeDatabaseRevisionCommands.getSqlUpgradeCommands().count() == 0)
    {
        ERROR_LOG("No s'ha pogut llegir cap comanda d'actualitzacio de la base de dades");
        return false;
    }

    foreach(QString sqlUpgradeCommand, upgradeDatabaseRevisionCommands.getSqlUpgradeCommands())
    {
        if (!applySqlUpgradeCommandToDatabase(sqlUpgradeCommand))
        {
            return false;
        }
    }

    LocalDatabaseManager localDatabaseManager;
    localDatabaseManager.setDatabaseRevision(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision());

    if (localDatabaseManager.getLastError() == LocalDatabaseManager::Ok)
    {
        INFO_LOG(QString("Base de dades actualitzada a la revisio %1").arg(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision()));
    }
    else
    {
        ERROR_LOG(QString("Error al actualitzar la nase de dades a la revisio %1").arg(upgradeDatabaseRevisionCommands.getUpgradeToDatabaseRevision()));
        return false;
    }

    return true;
}

bool DatabaseInstallation::applySqlUpgradeCommandToDatabase(QString sqlUpgradeCommand)
{
    DatabaseConnection databaseConnection;

    sqlite3_exec(databaseConnection.getConnection(), qPrintable(sqlUpgradeCommand), 0, 0, 0);

    if (databaseConnection.getLastErrorCode() != SQLITE_OK)
    {
        ERROR_LOG(QString("No s'ha pogut aplicar la comanda d'actualitzacio %1, Descripcio error: %2") .arg(sqlUpgradeCommand, databaseConnection.getLastErrorMessage()));
        return false;
    }

    INFO_LOG("S'ha aplicat la comanda d'actualitzacio a la base de dades : " + sqlUpgradeCommand);

    return true;
}

bool DatabaseInstallation::existsLocalImagePath()
{
    QDir cacheImageDir;

    return cacheImageDir.exists(LocalDatabaseManager::getCachePath());
}

bool DatabaseInstallation::existsDatabasePath()
{
    QDir databaseDir;

    return databaseDir.exists(LocalDatabaseManager::getDatabaseFilePath());
}

bool DatabaseInstallation::existsDatabaseFile()
{
    QFile databaseFile;

    return databaseFile.exists(LocalDatabaseManager::getDatabaseFilePath());
}

bool DatabaseInstallation::createLocalImageDir()
{
    QDir cacheImageDir;

    if (cacheImageDir.mkpath(LocalDatabaseManager::getCachePath()))
    {
        INFO_LOG("S'ha creat el directori de la cache d'imatges " + LocalDatabaseManager::getCachePath());
        return true;
    }
    else
    {
        ERROR_LOG("No s'ha pogut crear el directori de la cache d'imatges " + LocalDatabaseManager::getCachePath());
        return false;
    }
}

bool DatabaseInstallation::createDatabaseDirectory()
{
    QDir databaseDir;

    QFileInfo databaseFilePath(LocalDatabaseManager::getDatabaseFilePath());
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
    // Obrim la bdd
    DatabaseConnection DBConnect;
    int status;

    // Comprovem que existeixi el path on s'ha de crear la base de dades, sinó el crea
    if (!checkDatabasePath())
    {
        return false;
    }

    // Obrim el fitxer
    sqlTablesScriptFile.open(QIODevice::ReadOnly);

    // El llegim
    sqlTablesScript = sqlTablesScriptFile.read(sqlTablesScriptFile.size());

    // Creem les taules i els registres
    status = sqlite3_exec(DBConnect.getConnection(), sqlTablesScript.constData(), 0, 0, 0);

    // Tanquem el fitxer
    sqlTablesScriptFile.close();

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

bool DatabaseInstallation::canBeUpgradedDatabase()
{
    int minimumDatabaseRevisionRequired = UpgradeDatabaseXMLParser(getUpgradeDatabaseRevisionXmlData()).getMinimumDatabaseRevisionRequiredToUpgrade();

    if (minimumDatabaseRevisionRequired < 0)
    {
        ERROR_LOG("No s'ha pogut obtenir quina és la versió mínima requerida de base de dades");
        return false;
    }

    return minimumDatabaseRevisionRequired <= LocalDatabaseManager().getDatabaseRevision();
}

QString DatabaseInstallation::getUpgradeDatabaseRevisionXmlData()
{
    QFile file(":cache/upgradeDatabase.xml");

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        ERROR_LOG("No s'ha trobat el fitxer per actualitzar la base de dades");
        return NULL;
    }

    return file.readAll();
}

void DatabaseInstallation::setValueProgressBar()
{
    m_qprogressDialog->setValue(m_qprogressDialog->value() + 1);
}

QString DatabaseInstallation::getErrorMessage()
{
    return m_errorMessage;
}

bool DatabaseInstallation::askToUserIfDowngradeDatabase()
{
    QString questionMessage = tr("Current database is of newer version. In order to run %1, local studies must be deleted and database will be reinstalled."
                                  " Do you want to continue?").arg(ApplicationNameString);

    return QMessageBox::question(NULL, ApplicationNameString, questionMessage, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes ;
}

}
