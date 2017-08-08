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

#ifndef UDGDATABASECONNECTION_H
#define UDGDATABASECONNECTION_H

#include <QMutex>
#include <QString>

class QSqlDatabase;
class QSqlError;

namespace udg {

/**
 * @brief The DatabaseConnection class provides the connection to the database.
 *
 * This class automatically opens the database connection when getConnection() is called and closes it in its destructor.
 */
class DatabaseConnection {

public:
    /// SQLite error codes.
    enum SqliteError { SqliteOk = 0, SqliteError = 1, SqliteBusy = 5, SqliteLocked = 6, SqliteCorrupt = 11, SqliteEmpty = 16, SqliteSchema = 17,
                       SqliteConstraint = 19, SqliteMismatch = 20, SqliteNotADb = 26 };

    DatabaseConnection();
    ~DatabaseConnection();

    /// Sets the path to the database. If not set, it's read from settings.
    void setDatabasePath(const QString &databasePath);

    /// Returns the connection to the database. It opens the connection if it's not already open.
    QSqlDatabase getConnection();

    /// Returns the last error in the database.
    QSqlError getLastError();
    /// Returns the text of the last error in the database.
    QString getLastErrorMessage();

    /// Begins a trasaction in the database.
    void beginTransaction();
    /// Commits the current transaction in the database.
    void commitTransaction();
    /// Rolls back the current transaction in the database. If the connexion is closed, the current transaction is rolled back automatically.
    void rollbackTransaction();

private:
    /// Opens the connection to the database specified in the database path.
    void open();

    /// Closes the connection to the database.
    void close();

    /// Returns true if the connection is open and false otherwise.
    bool isConnected();

private:
    /// Path to the database file.
    QString m_databasePath;
    /// Auto-generated connection name, different for each instance.
    QString m_connectionName;

    /// SQLite doesn't support simultaneous transactions with the same connection, thus a mutex is needed for transactions.
    QMutex m_mutex;

};

} // End namespace

#endif
