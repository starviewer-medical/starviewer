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

bool LocalDatabaseUtilDAL::isDatabaseCorrupted()
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    bool databaseCorrupted = true;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), "PRAGMA integrity_check",
                                    &reply, &rows, &columns, error);

    if (rows > 0)
    {
        if (QString(reply[1]) == "ok")
        {
            //Si s'ha retornat ok vol dir que la base de dades no està corrupte
            databaseCorrupted = false;
        }
        else
        {
            ERROR_LOG("BASE DE DADES CORRUPTE, S'HAN TROBAT ELS SEGÜENTS ERRORS :");
            //guardem al log els errors de la base de dades, comencem a partir de 1 perquè ignorem la capçalera
            for (int index = 1; index <= rows; index++)
            {
                ERROR_LOG(reply[index]);
            }
        }
    }
    else
    {
        //Si no ha retornat files possiblement es tracta d'un fitxer que no és una base de dades sqlite
        ERROR_LOG("BASE DE DADES CORRUPTE, SEMBLA QUE EL FITXER NO ES UN BASE DE DADES SQLITE");
    }

    return databaseCorrupted;
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
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produït l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la següent sentència sql " + sqlSentence);
    }
}

}
