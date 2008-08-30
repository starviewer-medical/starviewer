/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabaseseriesdal.h"

#include <sqlite3.h>

#include "study.h"
#include "dicommask.h"
#include "logging.h"
#include "databaseconnection.h"
#include "starviewersettings.h"

namespace udg {

LocalDatabaseSeriesDAL::LocalDatabaseSeriesDAL()
{
}

void LocalDatabaseSeriesDAL::insert(Series *newSeries)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newSeries)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newSeries));
}

void LocalDatabaseSeriesDAL::update(Series *seriesToUpdate)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(seriesToUpdate)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(seriesToUpdate));
}

void LocalDatabaseSeriesDAL::del(DicomMask seriesMaskToDelete)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlDelete(seriesMaskToDelete)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(seriesMaskToDelete));
}

QList<Series*> LocalDatabaseSeriesDAL::query(DicomMask seriesMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Series*> seriesList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(seriesMask)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(seriesMask));
        return seriesList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        seriesList.append(fillSeries(reply, index, columns));
    }

    return seriesList;
}

void LocalDatabaseSeriesDAL::setDatabaseConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabaseSeriesDAL::getLastError()
{
    return m_lastSqliteError;
}

Series* LocalDatabaseSeriesDAL::fillSeries(char **reply, int row, int columns)
{
    QString studyInstanceUID;
    Series *series = new Series();
    StarviewerSettings settings;

    studyInstanceUID = reply[1 + row * columns];

    series->setInstanceUID(reply[0 + row * columns]);
    series->setSeriesNumber(reply[2 + row * columns]);
    series->setModality(reply[3 + row * columns]);
    series->setDate(reply[4 + row * columns]);
    series->setTime(reply[5 + row * columns]);
    series->setInstitutionName(reply[6 + row * columns]);
    series->setPatientPosition(reply[7 + row * columns]);
    series->setProtocolName(reply[8 + row * columns]);
    series->setDescription(reply[9 + row * columns]);
    series->setFrameOfReferenceUID(reply[10 + row * columns]);
    series->setPositionReferenceIndicator(reply[11 + row * columns]);
    series->setNumberOfPhases(QString(reply[12 + row * columns]).toInt());
    series->setNumberOfSlicesPerPhase(QString(reply[13 + row * columns]).toInt());
    series->setBodyPartExamined(reply[14 + row * columns]);
    series->setViewPosition(reply[15 + row * columns]);
    series->setManufacturer(reply[16 + row * columns]);
    series->setRetrievedDate(QDate().fromString(reply[17 + row * columns], "yyyyMMdd"));
    series->setRetrievedTime(QTime().fromString(reply[18 + row * columns], "hhmmss"));
    series->setImagesPath(settings.getCacheImagePath() + "/" + studyInstanceUID + "/" + series->getInstanceUID());

    return series;
}

QString LocalDatabaseSeriesDAL::buildSqlSelect(DicomMask seriesMaskToSelect)
{
    QString selectSentence = "Select InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, InstitutionName, "
                                    "PatientPosition, ProtocolName, Description, FrameOfReferenceUID, PositionReferenceIndicator, "
                                    "NumberOfPhases, NumberOfSlicesPerPhase, BodyPartExaminated, ViewPosition, "
                                    "Manufacturer, RetrievedDate, RetrievedTime, State "
                              "From Series ";

    return selectSentence + buildWhereSentence(seriesMaskToSelect);
}

QString LocalDatabaseSeriesDAL::buildSqlInsert(Series *newSeries)
{
    QString insertSentence = QString ("Insert into Series   (InstanceUID, StudyInstanceUID, Number, Modality, Date, Time, "
                                                            "InstitutionName, PatientPosition, ProtocolName, Description, "
                                                            "FrameOfReferenceUID, PositionReferenceIndicator, NumberOfPhases, "
                                                            "NumberOfSlicesPerPhase, BodyPartExaminated, ViewPosition, "
                                                            "Manufacturer, RetrievedDate, RetrievedTime, State) "
                                                    "values ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', "
                                                            "'%12', %13, %14, '%15', '%16', '%17', '%18', '%19', %20)")
                                    .arg(newSeries->getInstanceUID())
                                    .arg(newSeries->getParentStudy()->getInstanceUID())
                                    .arg(newSeries->getSeriesNumber())
                                    .arg(newSeries->getModality())
                                    .arg(newSeries->getDate().toString("yyyyMMdd"))
                                    .arg(newSeries->getTime().toString("hhmmss"))
                                    .arg(newSeries->getInstitutionName())
                                    .arg(newSeries->getPatientPosition())
                                    .arg(newSeries->getProtocolName())
                                    .arg(newSeries->getDescription())
                                    .arg(newSeries->getFrameOfReferenceUID())
                                    .arg(newSeries->getPositionReferenceIndicator())
                                    .arg(newSeries->getNumberOfPhases())
                                    .arg(newSeries->getNumberOfSlicesPerPhase())
                                    .arg(newSeries->getBodyPartExamined())
                                    .arg(newSeries->getViewPosition())
                                    .arg(newSeries->getManufacturer())
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
                                                        "NumberOfPhases = %12, "
                                                        "NumberOfSlicesPerPhase = %13, "
                                                        "BodyPartExaminated = '%14', "
                                                        "ViewPosition = '%15', "
                                                        "Manufacturer = '%16', "
                                                        "RetrievedDate = '%17', "
                                                        "RetrievedTime = '%18', "
                                                        "State = '%19' "
                                                 "Where InstanceUID = '%20'")
                                    .arg(seriesToUpdate->getParentStudy()->getInstanceUID())
                                    .arg(seriesToUpdate->getSeriesNumber())
                                    .arg(seriesToUpdate->getModality())
                                    .arg(seriesToUpdate->getDate().toString("yyyyMMdd"))
                                    .arg(seriesToUpdate->getTime().toString("hhmmss"))
                                    .arg(seriesToUpdate->getInstitutionName())
                                    .arg(seriesToUpdate->getPatientPosition())
                                    .arg(seriesToUpdate->getProtocolName())
                                    .arg(seriesToUpdate->getDescription())
                                    .arg(seriesToUpdate->getFrameOfReferenceUID())
                                    .arg(seriesToUpdate->getPositionReferenceIndicator())
                                    .arg(seriesToUpdate->getNumberOfPhases())
                                    .arg(seriesToUpdate->getNumberOfSlicesPerPhase())
                                    .arg(seriesToUpdate->getBodyPartExamined())
                                    .arg(seriesToUpdate->getViewPosition())
                                    .arg(seriesToUpdate->getManufacturer())
                                    .arg(seriesToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(seriesToUpdate->getRetrievedTime().toString("hhmmss"))
                                    .arg("0")
                                    .arg(seriesToUpdate->getInstanceUID());

    return updateSentence;
}

QString LocalDatabaseSeriesDAL::buildSqlDelete(DicomMask seriesMaskToDelete)
{
    return "Delete From Series " + buildWhereSentence(seriesMaskToDelete);
}

QString LocalDatabaseSeriesDAL::buildWhereSentence(DicomMask seriesMask)
{
    QString whereSentence = "";

    if (!seriesMask.getStudyUID().isEmpty()) 
        whereSentence = QString("where StudyInstanceUID = '%1'").arg(seriesMask.getStudyUID());

    if (!seriesMask.getSeriesUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" InstanceUID = '%1'").arg(seriesMask.getSeriesUID());
    }

    return whereSentence;
}

void LocalDatabaseSeriesDAL::logError(QString sqlSentence)
{
    QString errorNumber;

    errorNumber = errorNumber.setNum(getLastError(), 10);
    ERROR_LOG("S'ha produït l'error: " + errorNumber + " al executar la següent sentència sql " + sqlSentence);
}

}
