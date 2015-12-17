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

#include "localdatabaseutildal.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

LocalDatabaseUtilDAL::LocalDatabaseUtilDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseUtilDAL::compact()
{
    QSqlQuery query = getNewQuery();
    query.prepare("VACUUM");
    return executeQueryAndLogError(query);
}

int LocalDatabaseUtilDAL::getDatabaseRevision()
{
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT Revision FROM DatabaseRevision");

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

bool LocalDatabaseUtilDAL::updateDatabaseRevision(int databaseRevision)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE DatabaseRevision SET Revision = :revision");
    query.bindValue(0, databaseRevision);
    return executeQueryAndLogError(query);
}

}
