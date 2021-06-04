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

#ifndef UDGDATABASEINSTALLATION_H
#define UDGDATABASEINSTALLATION_H

#include <QString>

namespace udg {

class DatabaseConnection;

/**
 * @brief The DatabaseInstallation class has methods to check that the cache directories and the database are properly created, and if that's not the case to
 *        create them.
 */
class DatabaseInstallation {

public:
    DatabaseInstallation();
    ~DatabaseInstallation();

    /// Checks that the directory where images are stored exists and is writable, that the database file exists and is writable,
    /// and that the database revision is the expected one. For every check that fails, the method tries to correct it.
    /// Returns true if all the checks have passed or have been corrected, and false otherwise.
    bool checkDatabase();

    /// Deletes the database and creates it again. Returns true if successful and false otherwise.
    bool reinstallDatabase();

    /// Runs the database creation script in the given connection. Returns true if successful and false otherwise.
    bool createDatabase(DatabaseConnection &databaseConnection);

    /// Returns the last error message.
    const QString& getErrorMessage() const;

private:
    /// Checks that the directory where images are stored exists and is writable. If the directory doesn't exist the method tries to create it.
    /// Returns true if the directory exists and is writable or if it has been created, and false otherwise.
    bool checkLocalImagePath();

    /// Checks that the directory that contains the database file exists and is writable. If the directory doesn't exist the method tries to create it.
    /// Returns true if the directory exists and is writable of if it has been created, and false otherwise.
    bool checkDatabasePath();

    /// Checks that the database file exists and is writable. If the file doesn't exist the method tries to create it.
    /// Returns true if the file exists and is writable or if it has been created, and false otherwise.
    bool checkDatabaseFile();

    /// Checks that the database revision is the expected one.
    /// If the database revision is not the expected one, the method tries to upgrade or (if the user agrees) reinstall the database.
    /// Returns true if the database revision is the expected one or if it has been upgraded or reinstalled, and false otherwise.
    bool checkDatabaseRevision();

    /// Creates the directory where images are stored. Returns true if successful and false otherwise.
    bool createLocalImagePath();

    /// Creates the directory that contains the database file. Returns true if successful and false otherwise.
    bool createDatabasePath();

    /// Creates the database file. Returns true if successful and false otherwise.
    bool createDatabaseFile();

    /// Deletes all local images and the database and creates the database again.
    /// Returns true if the database has been created successfully and false otherwise.
    bool deleteLocalImagesAndReinstallDatabase();

    /// Asks the user if he wants to delete local studies and reinstall de database. Returns true if the user says yes and false if the user says no.
    bool userWantsToReinstallDatabase() const;

private:
    /// Last error message.
    QString m_errorMessage;

};

} // End namespace udg

#endif
