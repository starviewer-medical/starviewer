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

#include "dicommask.h"
#include "image.h"

#include <QDataStream>
#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Returns the given VOI LUT encoded in a QByteArray using a QDataStream.
QByteArray getByteArray(const VoiLut &voiLut)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_3);
    stream << voiLut.getLut();
    return byteArray;
}

// Returns a VOI LUT decoded from the given QByteArray using a QDataStream.
VoiLut getVoiLut(const QByteArray &byteArray)
{
    QDataStream stream(byteArray);
    stream.setVersion(QDataStream::Qt_5_3);
    TransferFunction voiLut;
    stream >> voiLut;
    return voiLut;
}

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask.
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand)
{
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        QString where(" WHERE ImageInstanceUID = :imageInstanceUID");
        if (!mask.getImageNumber().isEmpty())
        {
            where += " AND ImageFrameNumber = :imageFrameNumber";
        }

        query.prepare(sqlCommand + where);

        query.bindValue(":imageInstanceUID", mask.getSOPInstanceUID());
        if (!mask.getImageNumber().isEmpty())
        {
            query.bindValue(":imageFrameNumber", mask.getImageNumber());
        }
    }
    else if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE SeriesInstanceUID = :seriesInstanceUID)");
        query.bindValue(":seriesInstanceUID", mask.getSeriesInstanceUID());
    }
    else if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE StudyInstanceUID = :studyInstanceUID)");
        query.bindValue(":studyInstanceUID", mask.getStudyInstanceUID());
    }
    else
    {
        query.prepare(sqlCommand);
    }
}

}

LocalDatabaseVoiLutDAL::LocalDatabaseVoiLutDAL(DatabaseConnection &databaseConnection)
    : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseVoiLutDAL::insert(const VoiLut &voiLut, const Image *image)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO VoiLut (Lut, ImageInstanceUID, ImageFrameNumber) VALUES (:lut, :imageInstanceUID, :imageFrameNumber)");
    QByteArray blob = getByteArray(voiLut);
    query.bindValue(":lut", blob);
    query.bindValue(":imageInstanceUID", image->getSOPInstanceUID());
    query.bindValue(":imageFrameNumber", image->getFrameNumber());
    return executeQueryAndLogError(query);
}

bool LocalDatabaseVoiLutDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "DELETE FROM VoiLut");
    return executeQueryAndLogError(query);
}

QList<VoiLut> LocalDatabaseVoiLutDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT Lut, ImageInstanceUID, ImageFrameNumber FROM VoiLut");
    QList<VoiLut> voiLutList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            voiLutList.append(getVoiLut(query.value("Lut").toByteArray()));
        }
    }

    return voiLutList;
}

} // namespace udg
