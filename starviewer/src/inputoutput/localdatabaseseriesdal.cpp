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

#include "localdatabaseseriesdal.h"

#include "dicommask.h"
#include "localdatabasemanager.h"
#include "series.h"
#include "study.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Binds the necessary values of the given query with the information of the given series.
void bindValues(QSqlQuery &query, const Series *series)
{
    query.bindValue(":instanceUID", series->getInstanceUID());
    query.bindValue(":studyInstanceUID", series->getParentStudy()->getInstanceUID());
    query.bindValue(":number", series->getSeriesNumber());
    query.bindValue(":modality", series->getModality());
    query.bindValue(":date", series->getDate().toString("yyyyMMdd"));
    query.bindValue(":time", series->getTime().toString("hhmmss"));
    query.bindValue(":institutionName", series->getInstitutionName());
    query.bindValue(":patientPosition", series->getPatientPosition());
    query.bindValue(":protocolName", series->getProtocolName());
    query.bindValue(":description", series->getDescription());
    query.bindValue(":frameOfReferenceUID", series->getFrameOfReferenceUID());
    query.bindValue(":positionReferenceIndicator", series->getPositionReferenceIndicator());
    query.bindValue(":bodyPartExamined", series->getBodyPartExamined());
    query.bindValue(":viewPosition", series->getViewPosition());
    query.bindValue(":manufacturer", series->getManufacturer());
    query.bindValue(":laterality", LocalDatabaseBaseDAL::convertToQString(series->getLaterality()));
    query.bindValue(":retrievedDate", series->getRetrievedDate().toString("yyyyMMdd"));
    query.bindValue(":retrievedTime", series->getRetrievedTime().toString("hhmmss"));
    query.bindValue(":state", 0);
}

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask.
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand)
{
    QString where;

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        where += "StudyInstanceUID = :studyInstanceUID";
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "InstanceUID = :seriesInstanceUID";
    }
    if (!where.isEmpty())
    {
        where = " WHERE " + where;
    }

    query.prepare(sqlCommand + where);

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":studyInstanceUID", mask.getStudyInstanceUID());
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.bindValue(":seriesInstanceUID", mask.getSeriesInstanceUID());
    }
}

}

LocalDatabaseSeriesDAL::LocalDatabaseSeriesDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseSeriesDAL::insert(const Series *series)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO Series (InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, InstitutionName, PatientPosition, ProtocolName, "
                                      "Description, FrameOfReferenceUID, PositionReferenceIndicator, BodyPartExaminated, ViewPosition, Manufacturer, "
                                      "Laterality, RetrievedDate, RetrievedTime, State) "
                  "VALUES (:instanceUID, :studyInstanceUID, :number, :modality, :date, :time, :institutionName, :patientPosition, :protocolName, "
                          ":description, :frameOfReferenceUID, :positionReferenceIndicator, :bodyPartExamined, :viewPosition, :manufacturer, "
                          ":laterality, :retrievedDate, :retrievedTime, :state)");
    bindValues(query, series);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseSeriesDAL::update(const Series *series)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE Series SET StudyInstanceUID = :studyInstanceUID, Number = :number, Modality = :modality, Date = :date, Time = :time, "
                                    "InstitutionName = :institutionName, PatientPosition = :patientPosition, ProtocolName = :protocolName, "
                                    "Description = :description, FrameOfReferenceUID = :frameOfReferenceUID, "
                                    "PositionReferenceIndicator = :positionReferenceIndicator, BodyPartExaminated = :bodyPartExamined, "
                                    "ViewPosition = :viewPosition, Manufacturer = :manufacturer, Laterality = :laterality, RetrievedDate = :retrievedDate, "
                                    "RetrievedTime = :retrievedTime, State = :state "
                  "WHERE InstanceUID = :instanceUID");
    bindValues(query, series);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseSeriesDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "DELETE FROM Series");
    return executeQueryAndLogError(query);
}

QList<Series*> LocalDatabaseSeriesDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, InstitutionName, PatientPosition, ProtocolName, "
                                             "Description, FrameOfReferenceUID, PositionReferenceIndicator, BodyPartExaminated, ViewPosition,  Manufacturer, "
                                             "Laterality, RetrievedDate, RetrievedTime, State "
                                      "FROM Series");
    QList<Series*> seriesList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            seriesList.append(getSeries(query));
        }
    }

    return seriesList;
}

int LocalDatabaseSeriesDAL::count(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT count(*) FROM Series");

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

Series* LocalDatabaseSeriesDAL::getSeries(const QSqlQuery &query)
{
    Series *series = new Series();
    series->setInstanceUID(query.value("InstanceUID").toString());
    series->setSeriesNumber(query.value("Number").toString());
    series->setModality(query.value("Modality").toString());
    series->setDate(query.value("Date").toString());
    series->setTime(query.value("Time").toString());
    series->setInstitutionName(convertToQString(query.value("InstitutionName")));
    series->setPatientPosition(query.value("PatientPosition").toString());
    series->setProtocolName(convertToQString(query.value("ProtocolName")));
    series->setDescription(convertToQString(query.value("Description")));
    series->setFrameOfReferenceUID(query.value("FrameOfReferenceUID").toString());
    series->setPositionReferenceIndicator(convertToQString(query.value("PositionReferenceIndicator")));
    series->setBodyPartExamined(query.value("BodyPartExaminated").toString());
    series->setViewPosition(query.value("ViewPosition").toString());
    series->setManufacturer(convertToQString(query.value("Manufacturer")));
    // Laterality is a char
    series->setLaterality(query.value("Laterality").toString()[0]);
    series->setRetrievedDate(QDate::fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    series->setRetrievedTime(QTime::fromString(query.value("RetrievedTime").toString(), "hhmmss"));

    QString studyInstanceUID = query.value("StudyInstanceUID").toString();
    series->setImagesPath(LocalDatabaseManager::getCachePath() + "/" + studyInstanceUID + "/" + series->getInstanceUID());

    return series;
}

}
