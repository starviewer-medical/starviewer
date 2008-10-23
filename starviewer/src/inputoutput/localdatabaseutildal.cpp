/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <sqlite3.h>
#include <QString>
#include <QRegExp>

#include "localdatabaseutildal.h"
#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseUtilDAL::LocalDatabaseUtilDAL() 
: m_dbConnection(0)
{
}

void LocalDatabaseUtilDAL::compact()
{
    Q_ASSERT( m_dbConnection );

    QString compactSentence = "vacuum";

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(compactSentence), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
        logError(compactSentence);
}

int LocalDatabaseUtilDAL::getDatabaseRevision()
{
    int columns , rows;
    char **reply = NULL , **error = NULL;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), qPrintable(buildSqlGetDatabaseRevision()),
                                    &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlGetDatabaseRevision());
        return -1;
    }

    if (rows > 0)
    {
        QRegExp rexRevisionDatabase("\\d+");//La Revisió es guarda en el format $Revision \d+ $, nosaltres només volem el número per això busquem el \d+
        int pos = rexRevisionDatabase.indexIn(reply[1]);

        if (pos > -1)
        {
            return rexRevisionDatabase.cap(0).toInt();
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

void LocalDatabaseUtilDAL::setDatabaseConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabaseUtilDAL::getLastError()
{
    return m_lastSqliteError;
}

void LocalDatabaseUtilDAL::logError(QString sqlSentence)
{
    QString errorNumber;

    errorNumber = errorNumber.setNum(getLastError(), 10);
    ERROR_LOG("S'ha produït l'error: " + errorNumber + " al executar la següent sentència sql " + sqlSentence);
}

}
