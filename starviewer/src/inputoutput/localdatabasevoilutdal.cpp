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

#include <sqlite3.h>

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
VoiLut getVoiLut(sqlite3_stmt *preparedStatement)
{
    int blobSize = sqlite3_column_bytes(preparedStatement, 0);
    QByteArray byteArray(static_cast<const char*>(sqlite3_column_blob(preparedStatement, 0)), blobSize);
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

void LocalDatabaseVoiLutDAL::insert(const VoiLut &voiLut, Image *image)
{
    QString insertStatement = buildSqlInsert(image);
    sqlite3_stmt *preparedStatement;
    m_lastSqliteError = sqlite3_prepare_v2(m_dbConnection->getConnection(), insertStatement.toUtf8().constData(), -1, &preparedStatement, 0);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError("prepare(" + insertStatement + ")");
        return;
    }

    QByteArray blob = getByteArray(voiLut);
    m_lastSqliteError = sqlite3_bind_blob(preparedStatement, 1, blob.constData(), blob.size(), SQLITE_TRANSIENT);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError("bind_blob(" + insertStatement + ")");
    }

    m_lastSqliteError = sqlite3_step(preparedStatement);

    if (m_lastSqliteError != SQLITE_DONE)
    {
        logError("step(" + insertStatement + ")");
    }

    m_lastSqliteError = sqlite3_finalize(preparedStatement);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError("finalize(" + insertStatement + ")");
    }
}

void LocalDatabaseVoiLutDAL::del(const DicomMask &mask)
{
    QString deleteStatement = buildSqlDelete(mask);
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), deleteStatement.toUtf8().constData(), 0, 0, 0);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError(deleteStatement);
    }
}

QList<VoiLut> LocalDatabaseVoiLutDAL::query(const DicomMask &mask)
{
    QList<VoiLut> voiLuts;
    QString selectStatement = buildSqlSelect(mask);
    sqlite3_stmt *preparedStatement;
    m_lastSqliteError = sqlite3_prepare_v2(m_dbConnection->getConnection(), selectStatement.toUtf8().constData(), -1, &preparedStatement, 0);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError("prepare(" + selectStatement + ")");
        return voiLuts;
    }

    while ((m_lastSqliteError = sqlite3_step(preparedStatement)) == SQLITE_ROW)
    {
        voiLuts.append(getVoiLut(preparedStatement));
    }

    if (m_lastSqliteError != SQLITE_DONE)
    {
        logError("step(" + selectStatement + ")");
    }

    m_lastSqliteError = sqlite3_finalize(preparedStatement);

    if (m_lastSqliteError != SQLITE_OK)
    {
        logError("finalize(" + selectStatement + ")");
    }

    return voiLuts;
}

} // namespace udg
