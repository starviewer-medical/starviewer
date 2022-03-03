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

#include "logging.h"
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

    if (pacsDevice.getType() == PacsDevice::Type::Dimse)
    {
        query.prepare("INSERT INTO PACSRetrievedImages (Type, AETitle, Address, QueryPort) VALUES (:type, :aeTitle, :address, :queryPort)");
        query.bindValue(":type", "DIMSE");
        query.bindValue(":aeTitle", pacsDevice.getAETitle());
        query.bindValue(":address", pacsDevice.getAddress());
        query.bindValue(":queryPort", pacsDevice.getQueryRetrieveServicePort());
    }
    else    // WADO
    {
        query.prepare("INSERT INTO PACSRetrievedImages (Type, BaseUri) VALUES (:type, :baseUri)");
        query.bindValue(":type", "WADO");
        query.bindValue(":baseUri", pacsDevice.getBaseUri());
    }

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
    PacsDevice pacsDevice;
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT ID, Type, AETitle, Address, QueryPort, BaseUri FROM PACSRetrievedImages WHERE ID = :id");
    query.bindValue(":id", pacsId);

    if (executeQueryAndLogError(query) && query.next())
    {
        QString type = query.value("Type").toString();

        if (type == "DIMSE")
        {
            pacsDevice.setID(query.value("ID").toString());
            pacsDevice.setType(PacsDevice::Type::Dimse);
            pacsDevice.setAETitle(query.value("AETitle").toString());
            pacsDevice.setAddress(query.value("Address").toString());
            pacsDevice.setQueryRetrieveServicePort(query.value("QueryPort").toInt());
        }
        else if (type == "WADO")
        {
            pacsDevice.setID(query.value("ID").toString());
            pacsDevice.setType(PacsDevice::Type::Wado);
            pacsDevice.setBaseUri(query.value("BaseUri").toUrl());
        }
        else
        {
            WARN_LOG(QString("Found PACS in database with unexpected type: %1").arg(type));
        }
    }

    return pacsDevice;
}

QVariant LocalDatabasePACSRetrievedImagesDAL::queryId(const PacsDevice &pacsDevice)
{
    QSqlQuery query = getNewQuery();

    if (pacsDevice.getType() == PacsDevice::Type::Dimse)
    {
        query.prepare("SELECT ID FROM PACSRetrievedImages WHERE AETitle = :aeTitle AND Address = :address AND QueryPort = :queryPort");
        query.bindValue(":aeTitle", pacsDevice.getAETitle());
        query.bindValue(":address", pacsDevice.getAddress());
        query.bindValue(":queryPort", pacsDevice.getQueryRetrieveServicePort());
    }
    else    // WADO
    {
        query.prepare("SELECT ID FROM PACSRetrievedImages WHERE BaseUri = :baseUri");
        query.bindValue(":baseUri", pacsDevice.getBaseUri());
    }

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value("ID");
    }
    else
    {
        return QVariant(QVariant::LongLong);
    }
}

}
