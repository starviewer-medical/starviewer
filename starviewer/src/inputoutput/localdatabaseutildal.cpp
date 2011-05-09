#include <sqlite3.h>
#include <QString>
#include <QRegExp>

#include "localdatabaseutildal.h"
#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseUtilDAL::LocalDatabaseUtilDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

void LocalDatabaseUtilDAL::compact()
{
    Q_ASSERT(m_dbConnection);

    QString compactSentence = "vacuum";

    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(compactSentence), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(compactSentence);
    }
}

int LocalDatabaseUtilDAL::getDatabaseRevision()
{
    int columns, rows;
    char **reply = NULL, **error = NULL;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), qPrintable(buildSqlGetDatabaseRevision()),
                                          &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlGetDatabaseRevision());
        return -1;
    }

    if (rows > 0)
    {
        QString revisionDatabaseAsQString(reply[1]);
        sqlite3_free_table(reply);

        bool databaseReplyIsAnIntegerValue = false;
        int revisionDatabaseAsInt = revisionDatabaseAsQString.toInt(&databaseReplyIsAnIntegerValue);
        
        if (databaseReplyIsAnIntegerValue)
        {
            //El número de revisió ha de tenir format numèric, sinó s'ha produït un error.
            return revisionDatabaseAsInt;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        //si no trobem la fila amb la revisió de la base de dades retornem -1
        return -1;
    }
}

QString LocalDatabaseUtilDAL::buildSqlGetDatabaseRevision()
{
    return "select * from DatabaseRevision";
}

}
