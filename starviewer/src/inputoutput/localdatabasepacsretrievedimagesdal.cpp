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

#include "databaseconnection.h"
#include "pacsdevice.h"
#include "logging.h"

#include <QSqlQuery>
#include <QString>
#include <QVariant>

namespace udg {

LocalDatabasePACSRetrievedImagesDAL::LocalDatabasePACSRetrievedImagesDAL(DatabaseConnection &dbConnect):LocalDatabaseBaseDAL(dbConnect)
{
}

qlonglong LocalDatabasePACSRetrievedImagesDAL::insert(const PacsDevice &pacsDevice)
{
    QSqlQuery query;

    if (!query.exec(buildSqlInsert(pacsDevice)))
    {
        logError(query);
        return -1;
    }
    else
    {
        return query.lastInsertId().toLongLong();
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
    QSqlQuery query;

    if (!query.exec(sqlQuerySentence))
    {
        logError(query);
        return PacsDevice();
    }

    PacsDevice pacsDevice;

    if (query.next())
    {
        pacsDevice = fillPACSDevice(query);
    }

    return pacsDevice;
}

PacsDevice LocalDatabasePACSRetrievedImagesDAL::fillPACSDevice(const QSqlQuery &query)
{
    PacsDevice pacsDevice;

    pacsDevice.setID(query.value("ID").toString());
    pacsDevice.setAETitle(query.value("AETitle").toString());
    pacsDevice.setAddress(query.value("Address").toString());
    pacsDevice.setQueryRetrieveServicePort(query.value("QueryPort").toInt());

    return pacsDevice;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlInsert(const PacsDevice &pacsDevice)
{
    QString insertSentence = QString ("Insert into PACSRetrievedImages  (AETitle, Address, QueryPort) "
                                                   "values ('%1', '%2', %3)")
                                    .arg(formatTextToValidSQLSyntax(pacsDevice.getAETitle()))
                                    .arg(formatTextToValidSQLSyntax(pacsDevice.getAddress()))
                                    .arg(pacsDevice.getQueryRetrieveServicePort());
    return insertSentence;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect()
{
    QString selectSentence = "Select ID, AETitle, Address, QueryPort "
                             "From PACSRetrievedImages ";

    return selectSentence;
}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect(const qlonglong &IDPACSInDatabase)
{
    QString whereSentence = QString(" Where ID = %1 ").arg(IDPACSInDatabase);

    return buildSqlSelect() + whereSentence;

}

QString LocalDatabasePACSRetrievedImagesDAL::buildSqlSelect(const QString AETitle, const QString address, int queryPort)
{
    QString whereSentence = QString(" Where AETitle = '%1' and "
                                   "Address = '%2' and "
                                   "QueryPort = %3")
            .arg(formatTextToValidSQLSyntax(AETitle))
            .arg(formatTextToValidSQLSyntax(address))
            .arg(queryPort);

    return buildSqlSelect() + whereSentence;
}
}
