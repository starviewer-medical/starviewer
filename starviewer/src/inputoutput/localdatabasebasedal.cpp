#include "localdatabasebasedal.h"

#include <QString>
#include <sqlite3.h>

#include "databaseconnection.h"
#include "logging.h"

namespace udg
{

LocalDatabaseBaseDAL::LocalDatabaseBaseDAL(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
    m_lastSqliteError = SQLITE_OK;
}

int LocalDatabaseBaseDAL::getLastError()
{
    return m_lastSqliteError;
}

void LocalDatabaseBaseDAL::logError(const QString &sqlSentence)
{
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

}