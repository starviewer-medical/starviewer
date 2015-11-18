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

#include "databaseconnection.h"

#include <QSqlDatabase>
#include <QSqlError>

// Per les traduccions: tr()
#include <QObject>
#include <QSemaphore>
#include <QDir>
#include <QString>
#include "localdatabasemanager.h"
#include "logging.h"

namespace udg {

DatabaseConnection::DatabaseConnection()
{
    m_databasePath = LocalDatabaseManager::getDatabaseFilePath();
    m_transactionLock = new QSemaphore(1);
}

void DatabaseConnection::setDatabasePath(const QString &path)
{
    m_databasePath = path;
}

void DatabaseConnection::open()
{
    if (isConnected())
    {
        return;
    }

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(m_databasePath);
    database.setConnectOptions("QSQLITE_BUSY_TIMEOUT=15000");

    if (!database.open())
    {
        ERROR_LOG(database.lastError().text());
    }
}

void DatabaseConnection::beginTransaction()
{
    if (!isConnected())
    {
        open();
    }

    m_transactionLock->acquire();
    QSqlDatabase::database().transaction();
}

void DatabaseConnection::commitTransaction()
{
    QSqlDatabase::database().commit();
    m_transactionLock->release();
}

void DatabaseConnection::rollbackTransaction()
{
    QSqlDatabase::database().rollback();
    m_transactionLock->release();
    INFO_LOG("S'ha cancel.lat transaccio de la BD");
}

QString DatabaseConnection::formatTextToValidSQLSyntax(QString string)
{
    return string.isNull() ? "" : string.replace("'", "''");
}

QString DatabaseConnection::formatTextToValidSQLSyntax(QChar qchar)
{
    // Retornem un QString perquè si retornem QChar('') si qchar és null al converti-lo a QString(QChar('')) el QString s'inicialitza incorrectament agafant
    // com a valor un caràcter estrany en comptes de QString("")
    return qchar.isNull() ? "" : QString(qchar);
}

//sqlite3* DatabaseConnection::getConnection()
//{
//    if (!isConnected())
//    {
//        open();
//    }

//    return m_databaseConnection;
//}

bool DatabaseConnection::isConnected()
{
    return QSqlDatabase::database().isOpen();
}

void DatabaseConnection::close()
{
    if (isConnected())
    {
        QString name;
        {
            QSqlDatabase database = QSqlDatabase::database();
            name = database.connectionName();
            database.close();
        }
        QSqlDatabase::removeDatabase(name);
    }
}

QString DatabaseConnection::getLastErrorMessage()
{
    return QSqlDatabase::database().lastError().text();
}

QSqlError DatabaseConnection::getLastError()
{
    return QSqlDatabase::database().lastError();
}

DatabaseConnection::~DatabaseConnection()
{
    close();
}

}
