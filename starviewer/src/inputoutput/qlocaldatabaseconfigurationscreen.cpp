#include "qlocaldatabaseconfigurationscreen.h"

#include <QIntValidator>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include "localdatabasemanager.h"
#include "starviewerapplication.h"
#include "inputoutputsettings.h"
#include "logging.h"
#include "databaseinstallation.h"
#include "deletedirectory.h"

namespace udg {

QLocalDatabaseConfigurationScreen::QLocalDatabaseConfigurationScreen(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    loadCacheDefaults();
    m_buttonApplyCache->setEnabled(false);

    m_configurationChanged = false;
    m_createDatabase = false;

    createConnections();
    m_buttonApplyCache->setIcon(QIcon(":images/apply.png"));
    configureInputValidator();
}

QLocalDatabaseConfigurationScreen::~QLocalDatabaseConfigurationScreen()
{
}

void QLocalDatabaseConfigurationScreen::createConnections()
{
    // Connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect(m_buttonExaminateDataBase, SIGNAL(clicked()), SLOT(examinateDataBaseRoot()));

    // Connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect(m_buttonExaminateCacheImage, SIGNAL(clicked()), SLOT(examinateCacheImagePath()));

    // Connecta el boto aplicar de la cache amb l'slot apply
    connect(m_buttonApplyCache, SIGNAL(clicked()), SLOT(applyChanges()));

    // Connecta el boto aplicar de l'informació de l'institució amb l'slot apply
    connect(m_buttonCreateDatabase, SIGNAL(clicked()), SLOT(createDatabase()));

    // Activen el boto apply quant canvia el seu valor
    connect(m_textDatabaseRoot, SIGNAL(textChanged(const QString&)), SLOT(enableApplyButtons()));
    connect(m_textCacheImagePath, SIGNAL(textChanged(const QString&)), SLOT(enableApplyButtons()));
    connect(m_textMinimumSpaceRequiredToRetrieve, SIGNAL(textChanged(const QString&)), SLOT(enableApplyButtons()));
    connect(m_textDatabaseRoot, SIGNAL(textChanged(const QString&)), SLOT (configurationChangedDatabaseRoot()));
    connect(m_textMaximumDaysNotViewed, SIGNAL(textChanged(const QString&)), SLOT(enableApplyButtons()));
    connect(m_textSpaceToFreeIfNotEnoughSpaceAvailable, SIGNAL(textChanged(const QString&)), SLOT(enableApplyButtons()));
    connect(m_checkBoxDeletedOldStudies, SIGNAL(toggled(bool)), SLOT(enableApplyButtons()));
    connect(m_checkBoxDeleteStudiesIfNotEnoughFreeSpaceAvailable, SIGNAL(toggled(bool)), SLOT(enableApplyButtons()));

    // Mateniment base de dades
    connect(m_buttonDeleteStudies, SIGNAL(clicked()), SLOT(deleteStudies()));
    connect(m_buttonCompactDatabase, SIGNAL(clicked()), SLOT(compactCache()));

    // Afegeix la / al final del Path de la cache d'imatges
    connect(m_textCacheImagePath, SIGNAL(editingFinished()), SLOT(cacheImagePathEditingFinish()));
}

void QLocalDatabaseConfigurationScreen::configureInputValidator()
{
    m_textMaximumDaysNotViewed->setValidator(new QIntValidator(0, 9999, m_textMaximumDaysNotViewed));
    m_textMinimumSpaceRequiredToRetrieve->setValidator(new QIntValidator(0, 999, m_textMinimumSpaceRequiredToRetrieve));
    m_textSpaceToFreeIfNotEnoughSpaceAvailable->setValidator(new QIntValidator(0, 9999, m_textMaximumDaysNotViewed));

}

void QLocalDatabaseConfigurationScreen::loadCacheDefaults()
{
    Settings settings;

    m_textDatabaseRoot->setText(LocalDatabaseManager::getDatabaseFilePath());
    m_textCacheImagePath->setText(LocalDatabaseManager::getCachePath());

    m_textMaximumDaysNotViewed->setText(settings.getValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy).toString());
    m_textSpaceToFreeIfNotEnoughSpaceAvailable->setText(QString().setNum(settings.getValue(InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull).toInt()));
    m_textMinimumSpaceRequiredToRetrieve->setText(QString().setNum(settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toUInt()));
    m_textSpaceToFreeIfNotEnoughSpaceAvailable->setEnabled(settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria).toBool());
    m_checkBoxDeleteStudiesIfNotEnoughFreeSpaceAvailable->
        setChecked(settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria).toBool());
    m_checkBoxDeletedOldStudies->setChecked(settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool());
    m_textMaximumDaysNotViewed->setEnabled(settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool());
}

bool QLocalDatabaseConfigurationScreen::validateChanges()
{
    QDir dir;
    bool valid = true;
    QString messageBoxText = tr("Some configuration options are not valid:\n");

    if (m_textDatabaseRoot->isModified())
    {
        // Si el fitxer indicat no existeix i no s'ha demanat que es crei una nova base de dades, el path és invàlid
        if (!dir.exists(m_textDatabaseRoot->text()) && m_createDatabase == false)
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Invalid database path."));
            return false;
        }
    }

    if (m_textCacheImagePath->isModified())
    {
        if (!dir.exists(m_textCacheImagePath->text()))
        {
            switch (QMessageBox::question(this,
                    tr("Create directory?"),
                    tr("The cache image directory does not exist. Do you want to create it?"),
                    tr("&Yes"), tr("&No"), 0, 1))
            {
                case 0:
                    if (!dir.mkpath(m_textCacheImagePath->text()))
                    {
                        QMessageBox::critical(this, ApplicationNameString, tr("Unable to create the directory. Please check user permissions."));
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                    break;
                case 1:
                    return false;
                    break;
            }
        }
    }

    if (m_textMinimumSpaceRequiredToRetrieve->isModified())
    {
        if (m_textMinimumSpaceRequiredToRetrieve->text().toUInt() < 1)
        {
            messageBoxText += tr("\n- At least 1 GB of free space in hard disk is necessary to retrieve or import new studies.");
            valid = false;
        }
    }

    if (m_textSpaceToFreeIfNotEnoughSpaceAvailable->isModified())
    {
        if (m_textSpaceToFreeIfNotEnoughSpaceAvailable->text().toUInt() < 1)
        {
            messageBoxText += tr("\n- At least 1 GB of studies must be deleted when there is not enough space to retrieve or import new studies.");
            valid = false;
        }
    }

    if (m_textMaximumDaysNotViewed->isModified())
    {
        if (m_textMaximumDaysNotViewed->text().toUInt() < 1)
        {
            messageBoxText += tr("\n- Unable to delete studies not viewed in the last 0 days, must be at least 1 day.");
            valid = false;
        }
    }

    if (!valid)
    {
        QMessageBox::information(this, ApplicationNameString, messageBoxText);
    }

    return valid;
}

bool QLocalDatabaseConfigurationScreen::applyChanges()
{
    if (validateChanges())
    {
        applyChangesCache();

        // Només s'ha de reiniciar en el cas que que s'hagi canviat el path de la base de dades, per una ja existent. En el cas que la base de dades
        // no existeixi, a l'usuari al fer click al botó crear base de dades, ja se li haurà informat que s'havia de reiniciar l'aplicació
        if (m_textDatabaseRoot->isModified() && m_createDatabase == false)
        {
            QMessageBox::warning(this, ApplicationNameString, tr("The application has to be restarted to apply the changes."));
        }

        m_configurationChanged = false;

        return true;
    }
    else
    {
        return false;
    }
}

void QLocalDatabaseConfigurationScreen::enableApplyButtons()
{
    m_buttonApplyCache->setEnabled(true);
    m_configurationChanged = true;
}

void QLocalDatabaseConfigurationScreen::configurationChangedDatabaseRoot()
{
    // Indiquem no s'ha demanat que es creï la base de dades indicada a m_textDatabaseRoot
    m_createDatabase = false;
    enableApplyButtons();
}

void QLocalDatabaseConfigurationScreen::examinateDataBaseRoot()
{
    // A la pàgina de QT indica que en el cas que nomes deixem seleccionar un fitxer, agafar el primer element de la llista i punt, no hi ha cap mètode que
    // te retornin directament el fitxer selccionat
    QFileDialog *dialog = new QFileDialog(0, QFileDialog::tr("Open"), "./", ApplicationNameString + " Database (*.sdb)");
    dialog->setFileMode(QFileDialog::ExistingFile);

    if (dialog->exec() == QDialog::Accepted)
    {
        if (!dialog->selectedFiles().empty())
        {
            m_textDatabaseRoot->setText(dialog->selectedFiles().takeFirst());
            // Indiquem que m_textDatabaseRoot ha modificat el seu valor
            m_textDatabaseRoot->setModified(true);
        }
    }

    delete dialog;
}

void QLocalDatabaseConfigurationScreen::examinateCacheImagePath()
{
    QString path = QFileDialog::getExistingDirectory(0, tr("Choose the Cache images path..."), m_textCacheImagePath->text());
    if (!path.isEmpty())
    {
        m_textCacheImagePath->setText(path);
        cacheImagePathEditingFinish();
    }
}

void QLocalDatabaseConfigurationScreen::applyChangesCache()
{
    Settings settings;

    // Aquest els guardem sempre
    settings.setValue(InputOutputSettings::CachePath, m_textCacheImagePath->text());
    settings.setValue(InputOutputSettings::DatabaseAbsoluteFilePath, m_textDatabaseRoot->text());

    if (m_textMinimumSpaceRequiredToRetrieve->isModified())
    {
        INFO_LOG("Es modificarà l'espai mínim necessari per descarregar" + m_textMinimumSpaceRequiredToRetrieve->text());
        settings.setValue(InputOutputSettings::MinimumFreeGigaBytesForCache, m_textMinimumSpaceRequiredToRetrieve->text().toUInt());
    }

    if (m_textCacheImagePath->isModified())
    {
        INFO_LOG("Es modificarà el directori de la cache d'imatges " + m_textCacheImagePath->text());
        settings.setValue(InputOutputSettings::CachePath, m_textCacheImagePath->text());
    }

    if (m_textMaximumDaysNotViewed->isModified())
    {
        INFO_LOG("Es modificarà el nombre maxim de dies d'un estudi a la cache" + m_textMaximumDaysNotViewed->text());
        settings.setValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy, m_textMaximumDaysNotViewed->text());
    }

    if (m_textSpaceToFreeIfNotEnoughSpaceAvailable->isModified())
    {
        INFO_LOG("Es modificarà el Gbytes a alliberar quan no hi ha suficent espai per descarregar nous estudis" +
                 m_textSpaceToFreeIfNotEnoughSpaceAvailable->text());
        settings.setValue(InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull, m_textSpaceToFreeIfNotEnoughSpaceAvailable->text().toUInt());
    }

    settings.setValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria, m_checkBoxDeletedOldStudies->isChecked());
    settings.setValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria,
                      m_checkBoxDeleteStudiesIfNotEnoughFreeSpaceAvailable->isChecked());

    m_buttonApplyCache->setEnabled(false);
}

void QLocalDatabaseConfigurationScreen::deleteStudies()
{
    QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                       tr("Are you sure you want to delete all Studies of the cache?"),
                                                                       QMessageBox::Yes | QMessageBox::No,
                                                                       QMessageBox::No);
    if (response == QMessageBox::Yes)
    {
        INFO_LOG("Neteja de la cache");

        clearCache();
        emit configurationChanged("Pacs/CacheCleared");
    }
}

void QLocalDatabaseConfigurationScreen::clearCache()
{
    // Esborrem les imatges que tenim a la base de dades local , i reinstal·lem la bd ja que no té sentit eliminar tots els registres i compactar-la, ja que
    // tardaríem més que si la tornem a reinstal·lar.
    DeleteDirectory deleteDirectory;
    QProgressDialog qprogressDialog(tr("Deleting studies"), ApplicationNameString, 0, 0, this);;

    qprogressDialog.setCancelButton(0);
    qprogressDialog.setValue(1);
    qprogressDialog.setModal(true);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool successDeletingFiles = deleteDirectory.deleteDirectory(LocalDatabaseManager::getCachePath(), false);
    bool successReinstallingDatabase = DatabaseInstallation().reinstallDatabase();

    QApplication::restoreOverrideCursor();

    if (!successDeletingFiles)
    {
        QMessageBox::critical(this, ApplicationNameString, tr("Some files cannot be deleted. \nThese files have to be deleted manually."));
    }
    if (!successReinstallingDatabase)
    {
        QMessageBox::critical(this, ApplicationNameString, tr("An error has occurred deleting studies from database, be sure you have write "
                                                              "permissions on database directory. ") +
                                                           tr("\n\nClose all %1 windows and try again.").arg(ApplicationNameString) +
                                                           tr("If the problem persists contact with an administrator."));
    }
}

void QLocalDatabaseConfigurationScreen::compactCache()
{
    INFO_LOG("Compactació de la base de dades");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    LocalDatabaseManager localDatabaseManager;
    localDatabaseManager.compact();

    QApplication::restoreOverrideCursor();

    if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        QMessageBox::critical(this, ApplicationNameString, tr("The database cannot be compacted, an unknown error has occurred.\n\n") +
            tr("Close all %1 windows and try again.").arg(ApplicationNameString) + tr("If the problem persists contact with an administrator."));
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("Database has been compacted successfully."));
    }
}

void QLocalDatabaseConfigurationScreen::cacheImagePathEditingFinish()
{
    if (!m_textCacheImagePath->text().endsWith(QDir::toNativeSeparators("/"), Qt::CaseInsensitive))
    {
        m_textCacheImagePath->setText(QDir::toNativeSeparators(m_textCacheImagePath->text() + "/"));
    }
}

void QLocalDatabaseConfigurationScreen::createDatabase()
{
    QFile databaseFile;
    QString stringDatabasePath;

    if (m_textDatabaseRoot->text().right(4) != ".sdb")
    {
        QMessageBox::warning(this, ApplicationNameString, tr("The extension of the database has to be '.sdb'"));
    }
    else
    {
        if (databaseFile.exists(m_textDatabaseRoot->text()))
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Unable to create the database because a database with the same name exists in the directory."));
        }
        else
        {
            Settings settings;
            settings.setValue(InputOutputSettings::DatabaseAbsoluteFilePath, m_textDatabaseRoot->text());
            QMessageBox::warning(this, ApplicationNameString, tr("The application has to be restarted to apply the changes."));
            m_createDatabase = true;
        }
    }
}

};
