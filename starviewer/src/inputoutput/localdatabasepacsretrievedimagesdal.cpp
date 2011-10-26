#include "localdatabasepacsretrievedimagesdal.h"

#include <sqlite3.h>
#include <QString>
#include "databaseconnection.h"
#include "pacsdevice.h"
#include "logging.h"

namespace udg {

LocalDatabasePACSRetrievedImagesDAL::LocalDatabasePACSRetrievedImagesDAL(DatabaseConnection *dbConnect):LocalDatabaseBaseDAL(dbConnect)
{
}

qlonglong LocalDatabasePACSRetrievedImagesDAL::insert(const PacsDevice &pacsDevice)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlInsert(pacsDevice)), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(buildSqlInsert(pacsDevice));
        return -1;
    }
    else
    {
        // El mètode retorna un tipus sqlite3_int64 aquest en funció de l'entorn de compilació equival a un determinat tipus
        // http://www.sqlite.org/c3ref/int64.html __int64 per windows i long long int per la resta, qlonglong de qt
        // http://doc.qt.nokia.com/4.1/qtglobal.html#qlonglong-typedef equival als mateixos tipus pel mateix entorn de compilació per això retornem el
        // ID com un qlonglong.
        return sqlite3_last_insert_rowid(m_dbConnection->getConnection());
    }
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(const qlonglong &IDPacsInDatabase)
{
    return query(buildSqlSelect(IDPacsInDatabase));
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(const QString AETitle, const QString address, int queryPort)
{
    return query(buildSqlSelect(AETitle, address, queryPort));
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(const QString &sqlQuerySentence)
{
    int columns, rows;
    char **reply = NULL, **error = NULL;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), qPrintable(sqlQuerySentence), &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (sqlQuerySentence);
        return PacsDevice();
    }

    PacsDevice pacsDevice;

    if (rows >= 1)
    {
        // la primera columna és la capçalera
        pacsDevice = fillPACSDevice(reply, 1, columns);
    }

    sqlite3_free_table(reply);

    return pacsDevice;
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::fillPACSDevice(char **reply, int row, int columns)
{
    PacsDevice pacsDevice;

    pacsDevice.setID(reply[0 + row * columns]);
    pacsDevice.setAETitle(reply[1 + row * columns]);
    pacsDevice.setAddress(reply[2 + row * columns]);
    pacsDevice.setQueryRetrieveServicePort(QString(reply[3 + row * columns]).toInt());

    return pacsDevice;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlInsert(const PacsDevice &pacsDevice)
{
    QString insertSentence = QString ("Insert into PACSRetrievedImages  (AETitle, Address, QueryPort) "
                                                   "values ('%1', '%2', %3)")
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(pacsDevice.getAETitle()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(pacsDevice.getAddress()))
                                    .arg(pacsDevice.getQueryRetrieveServicePort());
    return insertSentence;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect()
{
    QString selectSentence;
    selectSentence = "Select ID, AETitle, Address, QueryPort "
                       "From PACSRetrievedImages ";

    return selectSentence;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect(const qlonglong &IDPACSInDatabase)
{
    QString whereSentence;
    whereSentence = QString(" Where ID = %1 ").arg(IDPACSInDatabase);

    return buildSqlSelect() + whereSentence;

}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect(const QString AETitle, const QString address, int queryPort)
{
    QString whereSentence;

    whereSentence = QString(" Where AETitle = '%1' and "
                                   "Address = '%2' and "
                                   "QueryPort = %3")
            .arg(DatabaseConnection::formatTextToValidSQLSyntax(AETitle))
            .arg(DatabaseConnection::formatTextToValidSQLSyntax(address))
            .arg(queryPort);

    return buildSqlSelect() + whereSentence;
}
}
