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

#include "localdatabasepacsretrievedimagesdal.h"

#include "pacsdevice.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

LocalDatabasePACSRetrievedImagesDAL::LocalDatabasePACSRetrievedImagesDAL(DatabaseConnection &databaseConnection)
    : LocalDatabaseBaseDAL(databaseConnection)
{
}

qlonglong LocalDatabasePACSRetrievedImagesDAL::insert(const PacsDevice &pacsDevice)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO PACSRetrievedImages (AETitle, Address, QueryPort) VALUES (:aeTitle, :address, :queryPort)");
    query.bindValue(":aeTitle", pacsDevice.getAETitle());
    query.bindValue(":address", pacsDevice.getAddress());
    query.bindValue(":queryPort", pacsDevice.getQueryRetrieveServicePort());

    if (executeQueryAndLogError(query))
    {
        return query.lastInsertId().toLongLong();
    }
    else
    {
        return -1;
    }
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(qlonglong pacsId)
{
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT ID, AETitle, Address, QueryPort FROM PACSRetrievedImages WHERE ID = :id");
    query.bindValue(":id", pacsId);
    return this->query(query);
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(const QString &aeTitle, const QString &address, int queryPort)
{
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT ID, AETitle, Address, QueryPort FROM PACSRetrievedImages WHERE AETitle = :aeTitle AND Address = :address AND QueryPort = :queryPort");
    query.bindValue(":aeTitle", aeTitle);
    query.bindValue(":address", address);
    query.bindValue(":queryPort", queryPort);
    return this->query(query);
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::query(QSqlQuery &query)
{
    PacsDevice pacsDevice;

    if (executeQueryAndLogError(query) && query.next())
    {
        pacsDevice.setID(query.value("ID").toString());
        pacsDevice.setAETitle(query.value("AETitle").toString());
        pacsDevice.setAddress(query.value("Address").toString());
        pacsDevice.setQueryRetrieveServicePort(query.value("QueryPort").toInt());
    }

    return pacsDevice;
}

}
