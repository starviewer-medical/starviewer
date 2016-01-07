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

    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), compactSentence.toUtf8().constData(), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(compactSentence);
    }
}

int LocalDatabaseUtilDAL::getDatabaseRevision()
{
    int columns;
    int rows;
    char **reply = NULL;
    char **error = NULL;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), buildSqlGetDatabaseRevision().toUtf8().constData(),
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
        sqlite3_free_table(reply);

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
        // Si no trobem la fila amb la revisió de la base de dades retornem -1
        return -1;
    }
}

void LocalDatabaseUtilDAL::updateDatabaseRevision(int databaseRevision)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), buildSqlUpdateDatabaseRevision(databaseRevision).toUtf8().constData(), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(buildSqlUpdateDatabaseRevision(databaseRevision));
    }
}

QString LocalDatabaseUtilDAL::buildSqlGetDatabaseRevision()
{
    return "select * from DatabaseRevision";
}

QString LocalDatabaseUtilDAL::buildSqlUpdateDatabaseRevision(int databaseRevision)
{
    return "Update DatabaseRevision Set Revision = " + QString::number(databaseRevision);
}

}
