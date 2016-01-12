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

#include "localdatabasebasedal.h"

#include "databaseconnection.h"
#include "logging.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

LocalDatabaseBaseDAL::LocalDatabaseBaseDAL(DatabaseConnection &databaseConnection)
    : m_databaseConnection(databaseConnection)
{
}

const QSqlError& LocalDatabaseBaseDAL::getLastError() const
{
    return m_lastError;
}

QString LocalDatabaseBaseDAL::convertToQString(const QChar &qchar)
{
    return qchar.isNull() ? QString("") : qchar;
}

QString LocalDatabaseBaseDAL::convertToQString(const QVariant &text)
{
    QString string = text.toString();

    if (string.contains(QChar::ReplacementCharacter))
    {
        string = QString::fromLatin1(string.toUtf8().constData());
    }

    return string;
}

void LocalDatabaseBaseDAL::logError(const QSqlQuery &query)
{
    // Ignore duplicate key error
    if (query.lastError().nativeErrorCode().toInt() != DatabaseConnection::SqliteConstraint)
    {
        ERROR_LOG(QString("SQLite error %1: \"%2\", when executing \"%3\"")
                  .arg(query.lastError().nativeErrorCode()).arg(query.lastError().text()).arg(query.lastQuery()));
    }
}

QSqlQuery LocalDatabaseBaseDAL::getNewQuery()
{
    return QSqlQuery(m_databaseConnection.getConnection());
}

bool LocalDatabaseBaseDAL::executeSql(const QString &sql)
{
    QSqlQuery query(m_databaseConnection.getConnection());
    query.prepare(sql);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseBaseDAL::executeQueryAndLogError(QSqlQuery &query)
{
    bool ok = query.exec();
    m_lastError = query.lastError();

    if (!ok)
    {
        logError(query);
    }

    return ok;
}

}
