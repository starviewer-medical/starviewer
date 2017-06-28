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

#ifndef UDGAPPLICATIONVERSIONCHECKER_H
#define UDGAPPLICATIONVERSIONCHECKER_H

class QString;

namespace udg {

/**
 * @brief The ApplicationVersionChecker class has methods to check for updates and show release notes.
 */
class ApplicationVersionChecker {

public:
    /// If this Starviewer version has been just installed, shows release notes.
    /// Otherwise, it checks online for updates and shows release notes if there is a newer version.
    static void checkAndShowReleaseNotes();

    /// Shows the local release notes.
    static void showLocalReleaseNotes();

    /// Sets a new minimum interval between version checks, saving it to settings.
    static void setCheckVersionInterval(int days);

    /// Returns true if currentVersion is newer than oldVersion and false otherwise.
    static bool isNewerVersion(const QString &currentVersion, const QString &oldVersion);

};

} // End namespace udg

#endif // UDGAPPLICATIONVERSIONCHECKER_H
