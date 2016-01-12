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

#ifndef UDGLOCALDATABASEBASEDAL_H
#define UDGLOCALDATABASEBASEDAL_H

#include <QSqlError>

class QSqlQuery;
class QVariant;

namespace udg {

class DatabaseConnection;

/**
 * @brief The LocalDatabaseBaseDAL class is the base class for all Data Access Layer classes.
 */
class LocalDatabaseBaseDAL {

public:
    LocalDatabaseBaseDAL(DatabaseConnection &databaseConnection);

    /// Returns the last error.
    const QSqlError& getLastError() const;

    /// Converts the given QChar to the corresponding QString. If the QChar is null, returns a null QString instead of a QString containing the null character.
    static QString convertToQString(const QChar &qchar);

protected:
    /// Converts the given text to a QString, interpreting the input as either UTF-8 or Latin-1 depending on its content.
    static QString convertToQString(const QVariant &text);

    /// Logs the last error produced in the given query. It ignores SqliteConstraint errors (duplicate key).
    static void logError(const QSqlQuery &query);

    /// Returns a new query that uses the current database connection.
    QSqlQuery getNewQuery();

    /// Executes the given SQL command, keeps the last error and logs it, if any. Returns true if there's no error and false otherwise.
    bool executeSql(const QString &sql);

    /// Executes the given query, keeps the last error and logs it, if any. Returns true if there's no error and false otherwise.
    bool executeQueryAndLogError(QSqlQuery &query);

protected:
    /// Database connection that will be used.
    DatabaseConnection &m_databaseConnection;

    /// Last error produced in the last executed command.
    QSqlError m_lastError;

};

}

#endif
