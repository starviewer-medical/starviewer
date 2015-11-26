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

#include "localdatabasedisplayshutterdal.h"

#include "dicommask.h"
#include "displayshutter.h"
#include "image.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask.
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand)
{
    if (!mask.getSOPInstanceUID().isEmpty() && !mask.getImageNumber().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID = :imageInstanceUID AND ImageFrameNumber = :imageFrameNumber");
    }
    else if (!mask.getSOPInstanceUID().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID = :imageInstanceUID");
    }
    else if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE SeriesInstanceUID = :seriesInstanceUID)");
    }
    else if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.prepare(sqlCommand + " WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE StudyInstanceUID = :studyInstanceUID)");
    }
    else
    {
        query.prepare(sqlCommand);
    }

    if (!mask.getSOPInstanceUID().isEmpty())
    {
        query.bindValue(":imageInstanceUID", mask.getSOPInstanceUID());
    }
    if (!mask.getImageNumber().isEmpty())
    {
        query.bindValue(":imageFrameNumber", mask.getImageNumber().toInt());
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.bindValue(":seriesInstanceUID", mask.getSeriesInstanceUID());
    }
    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":studyInstanceUID", mask.getStudyInstanceUID());
    }
}

// Creates and returns a display shutter with the information of the current row of the given query.
DisplayShutter getDisplayShutter(const QSqlQuery &query)
{
    DisplayShutter shutter;
    QString shape = query.value("Shape").toString();

    if (shape == "RECTANGULAR")
    {
        shutter.setShape(DisplayShutter::RectangularShape);
    }
    else if (shape == "CIRCULAR")
    {
        shutter.setShape(DisplayShutter::CircularShape);
    }
    else if (shape == "POLYGONAL")
    {
        shutter.setShape(DisplayShutter::PolygonalShape);
    }
    else
    {
        shutter.setShape(DisplayShutter::UndefinedShape);
    }

    shutter.setPoints(query.value("PointsList").toString());
    shutter.setShutterValue(query.value("ShutterValue").toUInt());

    return shutter;
}

}

LocalDatabaseDisplayShutterDAL::LocalDatabaseDisplayShutterDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseDisplayShutterDAL::insert(const DisplayShutter &shutter, const Image *shuttersImage)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO DisplayShutter (Shape, ShutterValue, PointsList, ImageInstanceUID, ImageFrameNumber) "
                  "VALUES (:shape, :shutterValue, :pointsList, :imageInstanceUID, :imageFrameNumber)");
    query.bindValue(":shape", shutter.getShapeAsDICOMString());
    query.bindValue(":shutterValue", shutter.getShutterValue());
    query.bindValue(":pointsList", shutter.getPointsAsString());
    query.bindValue(":imageInstanceUID", shuttersImage->getSOPInstanceUID());
    query.bindValue(":imageFrameNumber", shuttersImage->getFrameNumber());

    return executeQueryAndLogError(query);
}

bool LocalDatabaseDisplayShutterDAL::update(const QList<DisplayShutter> &shuttersList, const Image *shuttersImage)
{
    // We have to delete existing shutters and insert the new ones because the
    // DisplayShutter class doesn't have the ID of the DisplayShutter record in the database
    DicomMask mask;
    mask.setImageNumber(QString::number(shuttersImage->getFrameNumber()));
    mask.setSOPInstanceUID(shuttersImage->getSOPInstanceUID());

    if (!del(mask))
    {
        return false;
    }

    foreach (const DisplayShutter &shutter, shuttersList)
    {
        if (!insert(shutter, shuttersImage))
        {
            return false;
        }
    }

    return true;
}

bool LocalDatabaseDisplayShutterDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "DELETE FROM DisplayShutter");
    return executeQueryAndLogError(query);
}

QList<DisplayShutter> LocalDatabaseDisplayShutterDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT Shape, ShutterValue, PointsList FROM DisplayShutter");
    QList<DisplayShutter> shutterList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            shutterList << getDisplayShutter(query);
        }
    }

    return shutterList;
}

}
