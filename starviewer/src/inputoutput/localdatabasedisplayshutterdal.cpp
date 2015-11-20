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

#include "displayshutter.h"
#include "dicommask.h"
#include "databaseconnection.h"
#include "image.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

LocalDatabaseDisplayShutterDAL::LocalDatabaseDisplayShutterDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabaseDisplayShutterDAL::insert(const DisplayShutter &shutter, Image *shuttersImage)
{
    QSqlQuery query;

    if (!query.exec(buildSQLInsert(shutter, shuttersImage)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

void LocalDatabaseDisplayShutterDAL::update(const QList<DisplayShutter> &shuttersList, Image *shuttersImage)
{
    // Fem un delete de tots els shutters existents i fem un insert de nou,
    // ja que la classe DisplayShuter no té l'ID de la taula DisplayShutter de la BdD
    DicomMask mask;
    mask.setImageNumber(QString::number(shuttersImage->getFrameNumber()));
    mask.setSOPInstanceUID(shuttersImage->getSOPInstanceUID());
    del(mask);
    foreach (const DisplayShutter &shutter, shuttersList)
    {
        insert(shutter, shuttersImage);
    }
}

bool LocalDatabaseDisplayShutterDAL::del(const DicomMask &mask)
{
    QSqlQuery query;

    if (!query.exec(buildSQLDelete(mask)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

QList<DisplayShutter> LocalDatabaseDisplayShutterDAL::query(const DicomMask &mask)
{
    QList<DisplayShutter> shutterList;
    QSqlQuery query;

    if (!query.exec(buildSQLSelect(mask)))
    {
        logError(query.lastQuery());
        return shutterList;
    }

    while (query.next())
    {
        shutterList << fillDisplayShutter(query);
    }

    return shutterList;
}

DisplayShutter LocalDatabaseDisplayShutterDAL::fillDisplayShutter(const QSqlQuery &query)
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

    QString shutterPoints = query.value("PointsList").toString();
    shutter.setPoints(shutterPoints);

    shutter.setShutterValue(query.value("ShutterValue").toUInt());

    return shutter;
}

QString LocalDatabaseDisplayShutterDAL::buildSQLSelect(const DicomMask &mask)
{
    QString selectSentence = "SELECT Shape, ShutterValue, PointsList FROM DisplayShutter ";

    return selectSentence + buildWhereSentence(mask);
}

QString LocalDatabaseDisplayShutterDAL::buildSQLInsert(const DisplayShutter &shutter, Image *shuttersImage)
{
    QString insertSentence = "INSERT INTO DisplayShutter ";
    insertSentence += "(Shape, ShutterValue, PointsList, ImageInstanceUID, ImageFrameNumber) ";
    insertSentence += QString("VALUES ('%1', %2, '%3', '%4', %5)")
        .arg(formatTextToValidSQLSyntax(shutter.getShapeAsDICOMString()))
        .arg(shutter.getShutterValue())
        .arg(formatTextToValidSQLSyntax(shutter.getPointsAsString()))
        .arg(formatTextToValidSQLSyntax(shuttersImage->getSOPInstanceUID()))
        .arg(shuttersImage->getFrameNumber());

    return insertSentence;
}

QString LocalDatabaseDisplayShutterDAL::buildSQLDelete(const DicomMask &mask)
{
    return "DELETE FROM DisplayShutter " + buildWhereSentence(mask);
}

QString LocalDatabaseDisplayShutterDAL::buildWhereSentence(const DicomMask &mask)
{
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        QString whereSentence = QString("WHERE ImageInstanceUID = '%1' ").arg(formatTextToValidSQLSyntax(mask.getSOPInstanceUID()));
        if (!mask.getImageNumber().isEmpty())
        {
            whereSentence += QString("AND ImageFrameNumber = %1").arg(mask.getImageNumber());
        }

        return whereSentence;
    }
    
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE SeriesInstanceUID = '%1')")
            .arg(formatTextToValidSQLSyntax(mask.getSeriesInstanceUID()));
    }
    
    if (!mask.getStudyInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE StudyInstanceUID = '%1')")
            .arg(formatTextToValidSQLSyntax(mask.getStudyInstanceUID()));
    }
    
    return QString();
}

}
