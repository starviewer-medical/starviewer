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

#include "applicationversionchecker.h"

#include "applicationupdatechecker.h"
#include "coresettings.h"
#include "logging.h"
#include "qreleasenotes.h"
#include "starviewerapplication.h"

#include <QFile>
#include <QDate>
#include <QMessageBox>

namespace udg {

namespace {

// Returns true if we are in development mode, i.e. this is a devel version, and false otherwise.
bool isDevelopmentMode()
{
    return StarviewerVersionString.contains("devel");
}

// Returns true if the current version is newer than the last for which release notes were shown, and false otherwise.
bool isNewVersionInstalled()
{
    QString lastReleaseNotesVersionShown = Settings().getValue(CoreSettings::LastReleaseNotesVersionShown).toString();
    return ApplicationVersionChecker::isNewerVersion(StarviewerVersionString, lastReleaseNotesVersionShown);
}

// Returns the local URL for the release notes.
QUrl getLocalReleaseNotesUrl()
{
    // Installed path (<installdir>/releasenotes/releasenotesX.Y.Z.html)
    QString path = installationPath() + "/releasenotes/releasenotes" + StarviewerVersionString.section('-', 0, 0) + ".html";

    if (!QFile::exists(path))
    {
        // Development path (<sourcedir>/releasenotes/changelog.html)
        path = sourcePath() + "/releasenotes/changelog.html";
    }

    return QUrl::fromLocalFile(path);
}

// Returns true if the file pointed to by the given URL exists, and false otherwise.
bool checkLocalReleaseNotesUrl(const QUrl &url)
{
    return QFile::exists(url.toLocalFile());
}

// Returns the value of the setting to disable online version checks.
bool isOnlineCheckDisabled()
{
    return Settings().getValue(CoreSettings::DontCheckNewVersionsOnline).toBool();
}

// Returns true if the check version interval allows to check for new versions today, and false otherwise.
bool checkTimeInterval()
{
    Settings settings;
    QString lastVersionCheckedDate = settings.getValue(CoreSettings::LastVersionCheckedDate).toString();
    int checkVersionInterval = settings.getValue(CoreSettings::CheckVersionInterval).toInt();

    QDate today = QDate::currentDate();
    QDate lastTime = QDate::fromString(lastVersionCheckedDate, QString("yyyyMMdd"));

    int daysElapsed = checkVersionInterval; // by default assume that enough days have elapsed

    if (lastTime.isValid())
    {
        daysElapsed = lastTime.daysTo(today);
    }

    return daysElapsed >= checkVersionInterval;
}

// Checks online for newer versions and shows release notes if there is an update.
void checkOnline()
{
    ApplicationUpdateChecker *updateChecker = new ApplicationUpdateChecker();

    QObject::connect(updateChecker, &ApplicationUpdateChecker::checkFinished, [=] {
        Settings settings;
        settings.setValue(CoreSettings::LastVersionCheckedDate, QDate::currentDate().toString(QString("yyyyMMdd")));
        QString lastVersionChecked = settings.getValue(CoreSettings::LastVersionChecked).toString();

        if (updateChecker->isNewVersionAvailable())
        {

            if (lastVersionChecked != updateChecker->getVersion())
            {
                settings.setValue(CoreSettings::LastVersionChecked, updateChecker->getVersion());
                bool neverShowNewVersionReleaseNotes = settings.getValue(CoreSettings::NeverShowNewVersionReleaseNotes).toBool();

                if (neverShowNewVersionReleaseNotes)
                {
                    INFO_LOG("Not showing new version notes because the user doesn't want it.");
                }
                else
                {
                    INFO_LOG(QString("Showing new version notes from %1").arg(updateChecker->getReleaseNotesUrl()));

                    QReleaseNotes *releaseNotes = new QReleaseNotes();
                    releaseNotes->setDontShowVisible(true);
                    releaseNotes->setWindowTitle(QObject::tr("New Version Available"));
                    releaseNotes->showIfUrlLoadsSuccessfully(updateChecker->getReleaseNotesUrl());
                }
            }
            else
            {
                INFO_LOG("Not showing new version notes because they have been previously shown.");
            }

        }
        else
        {
            if (lastVersionChecked.isEmpty())
            {
                settings.setValue(CoreSettings::LastVersionChecked, StarviewerVersionString);
            }

            if (!updateChecker->isOnlineCheckOk())
            {
                ERROR_LOG("Error in version check:");
                ERROR_LOG(updateChecker->getErrorDescription());
            }
        }

        delete updateChecker;
    });

    updateChecker->checkForUpdates();
}

}

void ApplicationVersionChecker::checkAndShowReleaseNotes()
{
    if (isDevelopmentMode())
    {
        return; // don't do anything in development mode
    }

    if (isNewVersionInstalled())
    {
        // Show local release notes if possible

        // In any case we save this version as the last version shown
        Settings().setValue(CoreSettings::LastReleaseNotesVersionShown, StarviewerVersionString);

        QUrl url = getLocalReleaseNotesUrl();
        if (checkLocalReleaseNotesUrl(url))
        {
            showLocalReleaseNotes();
            return;
        }
    }

    // If we haven't shown local release notes we may try an online check

    if (isOnlineCheckDisabled())
    {
        INFO_LOG("Online update check disabled in settings.");
        return;
    }

    if (checkTimeInterval())
    {
        checkOnline();
    }
}

void ApplicationVersionChecker::showLocalReleaseNotes()
{
    QUrl url = getLocalReleaseNotesUrl();

    if (checkLocalReleaseNotesUrl(url))
    {
        QReleaseNotes *releaseNotes = new QReleaseNotes();
        releaseNotes->setDontShowVisible(false);
        releaseNotes->setWindowTitle(QObject::tr("Release Notes"));
        releaseNotes->showIfUrlLoadsSuccessfully(url);
    }
    else
    {
        WARN_LOG(QString("Release notes for the current version not found at %1").arg(url.toString()));

        QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Release notes for the current version not found."),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
}

void ApplicationVersionChecker::setCheckVersionInterval(int days)
{
    if (days > 0)
    {
        Settings().setValue(CoreSettings::CheckVersionInterval, days);
    }
}

bool ApplicationVersionChecker::isNewerVersion(const QString &currentVersion, const QString &oldVersion)
{
    if (currentVersion.isEmpty()) return false;
    if (oldVersion.isEmpty()) return true;

    // Initial: x.y.z-foo
    // Split by '-'. If there is no '-' it will return the whole string, so we are guaranteed to get a non-empty vector
    auto currentVersionParts = currentVersion.splitRef('-');
    auto oldVersionParts = oldVersion.splitRef('-');

    // Get x.y.z[.w]
    auto currentVersionNumber = currentVersionParts[0];
    auto oldVersionNumber = oldVersionParts[0];

    // Get [x, y, z, ...]
    auto currentVersionNumberParts = currentVersionNumber.split('.');
    auto oldVersionNumberParts = oldVersionNumber.split('.');

    for (int i = 0; i < std::min(currentVersionNumberParts.size(), oldVersionNumberParts.size()); i++)
    {
        int current = currentVersionNumberParts[i].toInt();
        int old = oldVersionNumberParts[i].toInt();

        if (current > old) return true;
        if (current < old) return false;
    }

    // If we arrive here, it means that both versions have the same numbers at the beginning. Let's check if one of them has additional numbers
    if (currentVersionNumberParts.size() > oldVersionNumberParts.size()) return true;
    if (currentVersionNumberParts.size() < oldVersionNumberParts.size()) return false;

    // The version number is exactly equal. Let's compare if there is some suffix
    if (currentVersionParts.size() == 1 && oldVersionParts.size() == 1) return false;   // neither has suffix, so they are equal
    if (currentVersionParts.size() == 1 && oldVersionParts.size() > 1) return true;     // currentVersion = x.y.z, oldVersion = x.y.z-(devel|alpha|beta|RC)...
    if (currentVersionParts.size() > 1 && oldVersionParts.size() == 1) return false;    // reverse of above

    // Both have suffix
    QRegularExpression suffixRegularExpression("(devel|alpha|beta|RC)(\\d*)", QRegularExpression::CaseInsensitiveOption);
    auto currentMatch = suffixRegularExpression.match(currentVersionParts[1]);
    auto oldMatch = suffixRegularExpression.match(oldVersionParts[1]);

    if (!currentMatch.hasMatch() || !oldMatch.hasMatch()) return false; // unexpected suffix: we can't say that currentVersion is newer than oldVersion

    QVector<QString> suffixes{"devel", "alpha", "beta", "RC"};
    int currentSuffixIndex = suffixes.indexOf(currentMatch.captured(1));
    int oldSuffixIndex = suffixes.indexOf(oldMatch.captured(1));

    if (currentSuffixIndex > oldSuffixIndex) return true;
    if (currentSuffixIndex < oldSuffixIndex) return false;

    // Same suffix type. Let's compare the number
    int currentSuffixNumber = currentMatch.captured(2).toInt();
    int oldSuffixNumber = oldMatch.captured(2).toInt();

    return currentSuffixNumber > oldSuffixNumber;
}

} // End namespace udg
