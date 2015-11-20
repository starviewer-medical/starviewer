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

#include "study.h"
#include "dicommask.h"
#include "logging.h"
#include "databaseconnection.h"
#include "localdatabasemanager.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

LocalDatabaseSeriesDAL::LocalDatabaseSeriesDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabaseSeriesDAL::insert(Series *newSeries)
{
    QSqlQuery query;

    if (!query.exec(buildSqlInsert(newSeries)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

bool LocalDatabaseSeriesDAL::update(Series *seriesToUpdate)
{
    QSqlQuery query;

    if (!query.exec(buildSqlUpdate(seriesToUpdate)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

bool LocalDatabaseSeriesDAL::del(const DicomMask &seriesMaskToDelete)
{
    QSqlQuery query;

    if (!query.exec(buildSqlDelete(seriesMaskToDelete)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

QList<Series*> LocalDatabaseSeriesDAL::query(const DicomMask &seriesMask)
{
    QList<Series*> seriesList;
    QSqlQuery query;

    if (!query.exec(buildSqlSelect(seriesMask)))
    {
        logError(query.lastQuery());
        return seriesList;
    }

    while (query.next())
    {
        seriesList.append(fillSeries(query));
    }

    return seriesList;
}

Series* LocalDatabaseSeriesDAL::fillSeries(const QSqlQuery &query)
{
    QString studyInstanceUID;
    Series *series = new Series();

    studyInstanceUID = query.value("StudyInstanceUID").toString();

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
    // Laterality és un char
    series->setLaterality(query.value("Laterality").toString()[0]);
    series->setRetrievedDate(QDate().fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    series->setRetrievedTime(QTime().fromString(query.value("RetrievedTime").toString(), "hhmmss"));
    series->setImagesPath(LocalDatabaseManager::getCachePath() + "/" + studyInstanceUID + "/" + series->getInstanceUID());

    return series;
}

QString LocalDatabaseSeriesDAL::buildSqlSelect(const DicomMask &seriesMaskToSelect)
{
    QString selectSentence = "Select InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, InstitutionName, "
                                    "PatientPosition, ProtocolName, Description, FrameOfReferenceUID, PositionReferenceIndicator, "
                                    "BodyPartExaminated, ViewPosition,  Manufacturer, Laterality, RetrievedDate, "
                                    "RetrievedTime, State "
                              "From Series ";

    return selectSentence + buildWhereSentence(seriesMaskToSelect);
}

QString LocalDatabaseSeriesDAL::buildSqlInsert(Series *newSeries)
{
    QString insertSentence = QString ("Insert into Series   (InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, "
                                                            "InstitutionName, PatientPosition, ProtocolName, Description, "
                                                            "FrameOfReferenceUID, PositionReferenceIndicator, BodyPartExaminated, ViewPosition, "
                                                            "Manufacturer, Laterality, RetrievedDate, RetrievedTime, State) "
                                                    "values ('%1', '%2', '%3', '%4', '%5', '%6', "
                                                            "'%7', '%8', '%9', '%10', "
                                                            "'%11', '%12', '%13', '%14', "
                                                            "'%15', '%16', '%17', '%18', %19)")
                                    .arg(formatTextToValidSQLSyntax(newSeries->getInstanceUID()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getParentStudy()->getInstanceUID()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getSeriesNumber()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getModality()))
                                    .arg(newSeries->getDate().toString("yyyyMMdd"))
                                    .arg(newSeries->getTime().toString("hhmmss"))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getInstitutionName()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getPatientPosition()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getProtocolName()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getDescription()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getFrameOfReferenceUID()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getPositionReferenceIndicator()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getBodyPartExamined()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getViewPosition()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getManufacturer()))
                                    .arg(formatTextToValidSQLSyntax(newSeries->getLaterality()))
                                    .arg(newSeries->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(newSeries->getRetrievedTime().toString("hhmmss"))
                                    .arg("0");

    return insertSentence;
}

QString LocalDatabaseSeriesDAL::buildSqlUpdate(Series *seriesToUpdate)
{
    QString updateSentence = QString ("Update Series Set StudyInstanceUID = '%1', "
                                                        "Number = '%2', "
                                                        "Modality = '%3', "
                                                        "Date = '%4', "
                                                        "Time = '%5', "
                                                        "InstitutionName = '%6', "
                                                        "PatientPosition = '%7', "
                                                        "ProtocolName = '%8', "
                                                        "Description = '%9', "
                                                        "FrameOfReferenceUID = '%10', "
                                                        "PositionReferenceIndicator = '%11', "
                                                        "BodyPartExaminated = '%12', "
                                                        "ViewPosition = '%13', "
                                                        "Manufacturer = '%14', "
                                                        "Laterality = '%15', "
                                                        "RetrievedDate = '%16', "
                                                        "RetrievedTime = '%17', "
                                                        "State = %18 "
                                                 "Where InstanceUID = '%19'")
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getParentStudy()->getInstanceUID()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getSeriesNumber()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getModality()))
                                    .arg(seriesToUpdate->getDate().toString("yyyyMMdd"))
                                    .arg(seriesToUpdate->getTime().toString("hhmmss"))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getInstitutionName()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getPatientPosition()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getProtocolName()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getDescription()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getFrameOfReferenceUID()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getPositionReferenceIndicator()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getBodyPartExamined()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getViewPosition()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getManufacturer()))
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getLaterality()))
                                    .arg(seriesToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(seriesToUpdate->getRetrievedTime().toString("hhmmss"))
                                    .arg("0")
                                    .arg(formatTextToValidSQLSyntax(seriesToUpdate->getInstanceUID()));

    return updateSentence;
}

QString LocalDatabaseSeriesDAL::buildSqlDelete(const DicomMask &seriesMaskToDelete)
{
    return "Delete From Series " + buildWhereSentence(seriesMaskToDelete);
}

QString LocalDatabaseSeriesDAL::buildWhereSentence(const DicomMask &seriesMask)
{
    QString whereSentence = "";

    if (!seriesMask.getStudyInstanceUID().isEmpty())
    {
        whereSentence = QString("where StudyInstanceUID = '%1'").arg(formatTextToValidSQLSyntax(seriesMask.getStudyInstanceUID()));
    }

    if (!seriesMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty())
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" InstanceUID = '%1'").arg(formatTextToValidSQLSyntax(seriesMask.getSeriesInstanceUID()));
    }

    return whereSentence;
}

}
