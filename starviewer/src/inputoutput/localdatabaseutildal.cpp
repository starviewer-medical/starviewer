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

#include <QString>
#include <QRegExp>
#include <QSqlQuery>
#include <QVariant>

#include "localdatabaseutildal.h"
#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseUtilDAL::LocalDatabaseUtilDAL(DatabaseConnection &dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

void LocalDatabaseUtilDAL::compact()
{
    QString compactSentence = "vacuum";
    QSqlQuery query;

    if (!query.exec(compactSentence))
    {
        logError(query);
    }
}

int LocalDatabaseUtilDAL::getDatabaseRevision()
{
    QSqlQuery query;

    if (!query.exec(buildSqlGetDatabaseRevision()))
    {
        logError(query);
        return -1;
    }

    if (query.next())
    {
        QRegExp rexRevisionDatabase("\\d+");//La Revisió es guarda en el format $Revision \d+ $, nosaltres només volem el número per això busquem el \d+
        int pos = rexRevisionDatabase.indexIn(query.value("Revision").toString());

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
    QSqlQuery query;

    if (!query.exec(buildSqlUpdateDatabaseRevision(databaseRevision)))
    {
        logError(query);
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
