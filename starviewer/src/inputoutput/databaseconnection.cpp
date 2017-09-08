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

#include "localdatabasemanager.h"
#include "logging.h"

#include <QSqlDatabase>
#include <QSqlError>

namespace udg {

DatabaseConnection::DatabaseConnection()
{
    static const QString ConnectionNamePrefix = "starviewer";
    static QAtomicInt connectionNumber = 0;

    m_databasePath = LocalDatabaseManager::getDatabaseFilePath();
    m_connectionName = ConnectionNamePrefix + connectionNumber++;
}

DatabaseConnection::~DatabaseConnection()
{
    close();
}

void DatabaseConnection::setDatabasePath(const QString &path)
{
    m_databasePath = path;
}

QSqlDatabase DatabaseConnection::getConnection()
{
    if (!isConnected())
    {
        open();
    }

    return QSqlDatabase::database(m_connectionName);
}

QSqlError DatabaseConnection::getLastError()
{
    return getConnection().lastError();
}

QString DatabaseConnection::getLastErrorMessage()
{
    return getLastError().text();
}

void DatabaseConnection::beginTransaction()
{
    m_mutex.lock();
    getConnection().transaction();
}

void DatabaseConnection::commitTransaction()
{
    getConnection().commit();
    m_mutex.unlock();
}

void DatabaseConnection::rollbackTransaction()
{
    getConnection().rollback();
    m_mutex.unlock();
    INFO_LOG("Transaction in the database rolled back.");
}

void DatabaseConnection::open()
{
    if (isConnected())
    {
        return;
    }

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    database.setDatabaseName(m_databasePath);
    database.setConnectOptions("QSQLITE_BUSY_TIMEOUT=15000");

    if (!database.open())
    {
        ERROR_LOG(database.lastError().text());
    }
}

void DatabaseConnection::close()
{
    if (isConnected())
    {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool DatabaseConnection::isConnected()
{
    return QSqlDatabase::database(m_connectionName).isOpen();
}

}
