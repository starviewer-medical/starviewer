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

#include "localdatabasevoilutdal.h"

#include "databaseconnection.h"
#include "dicommask.h"
#include "image.h"

#include <QDataStream>
#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

/// Returns a SQL INSERT string with the given image.
QString buildSqlInsert(Image *image)
{
    return QString("INSERT INTO VoiLut (Lut, ImageInstanceUID, ImageFrameNumber) VALUES (?, '%1', %2)")
            .arg(DatabaseConnection::formatTextToValidSQLSyntax(image->getSOPInstanceUID()))
            .arg(image->getFrameNumber());
}

/// Returns a SQL WHERE clause corresponding to the given mask.
QString buildSqlWhere(const DicomMask &mask)
{
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        QString where = QString("WHERE ImageInstanceUID = '%1' ").arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getSOPInstanceUID()));

        if (!mask.getImageNumber().isEmpty())
        {
            where += QString("AND ImageFrameNumber = %1").arg(mask.getImageNumber());
        }

        return where;
    }

    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE SeriesInstanceUID = '%1')")
                .arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getSeriesInstanceUID()));
    }

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE StudyInstanceUID = '%1')")
                .arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getStudyInstanceUID()));
    }

    return QString();
}

/// Returns a SQL DELETE string corresponding to the given mask.
QString buildSqlDelete(const DicomMask &mask)
{
    return "DELETE FROM VoiLut " + buildSqlWhere(mask);
}

/// Returns a SQL SELECT string corresponding to the given mask.
QString buildSqlSelect(const DicomMask &mask)
{
    return "SELECT Lut, ImageInstanceUID, ImageFrameNumber FROM VoiLut " + buildSqlWhere(mask);
}

/// Returns the given VOI LUT encoded in a QByteArray using a QDataStream.
QByteArray getByteArray(const VoiLut &voiLut)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_3);
    stream << voiLut.getLut();
    return byteArray;
}

/// Returns a VOI LUT constructed from the blob in column 0 of the given prepared statement read using a QDataStream.
VoiLut getVoiLut(const QSqlQuery &query)
{
    QByteArray byteArray = query.value("Lut").toByteArray();
    QDataStream stream(byteArray);
    stream.setVersion(QDataStream::Qt_5_3);
    TransferFunction voiLut;
    stream >> voiLut;
    return voiLut;
}

}

LocalDatabaseVoiLutDAL::LocalDatabaseVoiLutDAL(DatabaseConnection *dbConnection)
    : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabaseVoiLutDAL::insert(const VoiLut &voiLut, Image *image)
{
    QString insertStatement = buildSqlInsert(image);
    QSqlQuery query;

    if (!query.prepare(insertStatement))
    {
        logError("prepare(" + query.lastQuery() + ")");
        return false;
    }

    QByteArray blob = getByteArray(voiLut);
    query.bindValue(0, blob);

    if (!query.exec())
    {
        logError("exec(" + query.lastQuery() + ")");
        return false;
    }

    return true;
}

bool LocalDatabaseVoiLutDAL::del(const DicomMask &mask)
{
    QString deleteStatement = buildSqlDelete(mask);
    QSqlQuery query;

    if (!query.exec(deleteStatement))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

QList<VoiLut> LocalDatabaseVoiLutDAL::query(const DicomMask &mask)
{
    QList<VoiLut> voiLuts;
    QString selectStatement = buildSqlSelect(mask);
    QSqlQuery query;

    if (!query.exec(selectStatement))
    {
        logError(query.lastQuery());
        return voiLuts;
    }

    while (query.next())
    {
        voiLuts.append(getVoiLut(query));
    }

    return voiLuts;
}

} // namespace udg
