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

#include "qlocaldatabaseconfigurationscreen.h"

#include <QIntValidator>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "localdatabasemanager.h"
#include "starviewerapplication.h"
#include "inputoutputsettings.h"
#include "logging.h"
#include "databaseinstallation.h"
#include "directoryutilities.h"
#include "singleton.h"
#include "queryscreen.h"
#include "usermessage.h"

namespace udg {

typedef SingletonPointer<QueryScreen> QueryScreenSingleton;

QLocalDatabaseConfigurationScreen::QLocalDatabaseConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    
    m_databasePathWarningLabel->setVisible(false);
    m_databasePathWarningIcon->setVisible(false);
    m_cachePathWarningIcon->setVisible(false);
    m_cachePathWarningLabel->setVisible(false);
    m_minimumSpaceWarningIcon->setVisible(false);
    m_minimumSpaceWarningLabel->setVisible(false);
    m_deleteNotViewedStudiesWarningIcon->setVisible(false);
    m_deleteNotViewedStudiesWarningLabel->setVisible(false);
    m_freeSpaceWarningIcon->setVisible(false);
    m_freeSpaceWarningLabel->setVisible(false);
    m_mustRestartToApplyWarningIcon->setVisible(false);
    m_mustRestartToApplyWarningLabel->setVisible(false);

    loadCacheDefaults();

    createConnections();
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

    // Updates
    // Database and cache location
    connect(m_textDatabaseRoot, SIGNAL(textChanged(QString)), SLOT(checkAndUpdateDatabasePathSetting(QString)));
    // Automatic cache management
    connect(m_textMinimumSpaceRequiredToRetrieve, SIGNAL(textChanged(QString)), SLOT(updateMinimumSpaceRequiredSetting(QString)));
    connect(m_checkBoxDeletedOldStudies, SIGNAL(toggled(bool)), SLOT(updateDeleteOldStudiesSetting(bool)));
    connect(m_textMaximumDaysNotViewed, SIGNAL(textChanged(QString)), SLOT(updateMaximumDaysNotViewedSetting(QString)));
    connect(m_checkBoxDeleteStudiesIfNotEnoughFreeSpaceAvailable, SIGNAL(toggled(bool)), SLOT(updateDeleteStudiesWhenNoFreeSpaceSetting(bool)));
    connect(m_textSpaceToFreeIfNotEnoughSpaceAvailable, SIGNAL(textChanged(QString)), SLOT(updateDiskSpaceToFreeSetting(QString)));
    // Local database maintenance
    connect(m_buttonDeleteStudies, SIGNAL(clicked()), SLOT(deleteStudies()));
    connect(m_buttonCompactDatabase, SIGNAL(clicked()), SLOT(compactCache()));
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

void QLocalDatabaseConfigurationScreen::checkAndUpdateDatabasePathSetting(const QString &text)
{
    bool databasePathHasChanged = false;
    if (validateDatabaseApplicationPath(text))
    {
        // Update only if path has changed from current setting
        if (LocalDatabaseManager::getDatabaseFilePath() != text)
        {
            Settings settings;
            settings.setValue(InputOutputSettings::DatabaseAbsoluteFilePath, text);
            INFO_LOG("Updated database file path setting: " + text);

            databasePathHasChanged = true;
        }
    }

    m_mustRestartToApplyWarningIcon->setVisible(databasePathHasChanged);
    m_mustRestartToApplyWarningLabel->setVisible(databasePathHasChanged);
}

void QLocalDatabaseConfigurationScreen::checkAndUpdateCachePathSetting(const QString &text)
{
    // TODO If we change cache path, what is suposed to be done with the previous cache items?
    if (validateDICOMFilesCachePath(text))
    {
        // Update only if path has changed from current setting
        if (LocalDatabaseManager::getCachePath() != text)
        {
            Settings settings;
            settings.setValue(InputOutputSettings::CachePath, text);
            INFO_LOG("Updated cache path setting: " + text);
        }
    }
}

void QLocalDatabaseConfigurationScreen::updateMinimumSpaceRequiredSetting(const QString &text)
{
    if (m_textMinimumSpaceRequiredToRetrieve->isModified())
    {
        if (text.isEmpty())
        {
            m_minimumSpaceWarningIcon->setVisible(true);
            m_minimumSpaceWarningLabel->setVisible(true);
            m_minimumSpaceWarningLabel->setText(tr("Please, enter a value."));
        }
        else if (text.toUInt() < 1)
        {
            m_minimumSpaceWarningIcon->setVisible(true);
            m_minimumSpaceWarningLabel->setVisible(true);
            m_minimumSpaceWarningLabel->setText(tr("At least 1 GB of free space in hard disk is required."));
        }
        else
        {
            m_minimumSpaceWarningIcon->setVisible(false);
            m_minimumSpaceWarningLabel->setVisible(false);

            Settings settings;
            settings.setValue(InputOutputSettings::MinimumFreeGigaBytesForCache, text.toUInt());
            INFO_LOG("Updated minimum free space to download studies settings: " + text);
        }
    }
}

void QLocalDatabaseConfigurationScreen::updateDeleteOldStudiesSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria, enable);

    m_deleteNotViewedStudiesWarningIcon->setEnabled(enable);
    m_deleteNotViewedStudiesWarningLabel->setEnabled(enable);

}

void QLocalDatabaseConfigurationScreen::updateMaximumDaysNotViewedSetting(const QString &text)
{
    if (m_textMaximumDaysNotViewed->isModified())
    {
        if (text.isEmpty())
        {
            m_deleteNotViewedStudiesWarningIcon->setVisible(true);
            m_deleteNotViewedStudiesWarningLabel->setVisible(true);
            m_deleteNotViewedStudiesWarningLabel->setText(tr("Please, enter a value."));
        }
        else if (text.toUInt() < 1)
        {
            m_deleteNotViewedStudiesWarningIcon->setVisible(true);
            m_deleteNotViewedStudiesWarningLabel->setVisible(true);
            m_deleteNotViewedStudiesWarningLabel->setText(tr("Must be at least 1 day."));
        }
        else
        {
            m_deleteNotViewedStudiesWarningIcon->setVisible(false);
            m_deleteNotViewedStudiesWarningLabel->setVisible(false);

            Settings settings;
            settings.setValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy, text);
            INFO_LOG("Updated maximum days study remains in cache if not viewed setting: " + text);
        }
    }
}

void QLocalDatabaseConfigurationScreen::updateDeleteStudiesWhenNoFreeSpaceSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria, enable);

    m_freeSpaceWarningIcon->setEnabled(enable);
    m_freeSpaceWarningLabel->setEnabled(enable);
}

void QLocalDatabaseConfigurationScreen::updateDiskSpaceToFreeSetting(const QString &text)
{
    if (m_textSpaceToFreeIfNotEnoughSpaceAvailable->isModified())
    {
        if (text.isEmpty())
        {
            m_freeSpaceWarningIcon->setVisible(true);
            m_freeSpaceWarningLabel->setVisible(true);
            m_freeSpaceWarningLabel->setText(tr("Please, enter a value."));
        }
        else if (text.toUInt() < 1)
        {
            m_freeSpaceWarningIcon->setVisible(true);
            m_freeSpaceWarningLabel->setVisible(true);
            m_freeSpaceWarningLabel->setText(tr("At least 1 GB of free space is required."));
        }
        else
        {
            m_freeSpaceWarningIcon->setVisible(false);
            m_freeSpaceWarningLabel->setVisible(false);
            
            Settings settings;
            settings.setValue(InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull, text.toUInt());
            INFO_LOG("Updated Giga Bytes to free if cache is full setting: " + text);
        }
    }
}

bool QLocalDatabaseConfigurationScreen::validateDatabaseApplicationPath(const QString &text)
{
    QFileInfo databaseFilePathInfo(text);
    if (!databaseFilePathInfo.dir().exists())
    {
        m_databasePathWarningLabel->setText(tr("Invalid database path."));
        m_databasePathWarningLabel->setVisible(true);
        m_databasePathWarningIcon->setVisible(true);

        return false;
    }
    else
    {
        if (text.right(4) != ".sdb")
        {
            m_databasePathWarningLabel->setText(tr("The database must have '.sdb' extension"));
            m_databasePathWarningLabel->setVisible(true);
            m_databasePathWarningIcon->setVisible(true);

            return false;
        }
        else
        {
            m_databasePathWarningLabel->setVisible(false);
            m_databasePathWarningIcon->setVisible(false);
            return true;
        }
    }
}

bool QLocalDatabaseConfigurationScreen::validateDICOMFilesCachePath(const QString &text)
{
    QFileInfo cachePathFileInfo(text);
    if (!cachePathFileInfo.dir().exists())
    {
        m_cachePathWarningLabel->setText(tr("Invalid database path."));
        m_cachePathWarningLabel->setVisible(true);
        m_cachePathWarningIcon->setVisible(true);

        return false;
    }
    else
    {
        m_cachePathWarningLabel->setVisible(false);
        m_cachePathWarningIcon->setVisible(false);
        return true;
    }
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
    QString path = QFileDialog::getExistingDirectory(0, tr("Choose the cache images directory..."), m_textCacheImagePath->text());
    if (!path.isEmpty())
    {
        m_textCacheImagePath->setText(path);
        cacheImagePathEditingFinish();
        checkAndUpdateCachePathSetting(m_textCacheImagePath->text());
    }
}

void QLocalDatabaseConfigurationScreen::deleteStudies()
{
    QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                       tr("Are you sure you want to delete all studies from the cache?"),
                                                                       QMessageBox::Yes | QMessageBox::No,
                                                                       QMessageBox::No);
    if (response == QMessageBox::Yes)
    {
        INFO_LOG("Neteja de la cache");

        clearCache();
        QueryScreenSingleton::instance()->updateConfiguration("Pacs/CacheCleared");
    }
}

void QLocalDatabaseConfigurationScreen::clearCache()
{
    // Esborrem les imatges que tenim a la base de dades local , i reinstal·lem la bd ja que no té sentit eliminar tots els registres i compactar-la, ja que
    // tardaríem més que si la tornem a reinstal·lar.
    DirectoryUtilities deleteDirectory;
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
        QString message = tr("Some files cannot be deleted.");
        message += "\n";
        message += tr("These files have to be deleted manually.");
        
        QMessageBox::critical(this, ApplicationNameString, message);
    }
    if (!successReinstallingDatabase)
    {
        QString message = tr("An error has occurred while deleting studies from the database, make sure you have write permission on the database directory.");
        message += "\n\n";
        message += UserMessage::getCloseWindowsAndTryAgainAdvice();
        message += "\n\n";
        message += UserMessage::getProblemPersistsAdvice();

        QMessageBox::critical(this, ApplicationNameString, message);
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
        QMessageBox::critical(this, ApplicationNameString, tr("The database cannot be compacted, an unknown error has occurred.") + "\n\n" +
            UserMessage::getCloseWindowsAndTryAgainAdvice() + " " + UserMessage::getProblemPersistsAdvice());
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

};
