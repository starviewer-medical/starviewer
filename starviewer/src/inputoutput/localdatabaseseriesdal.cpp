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
#include "localdatabasemanager.h"

namespace udg {

LocalDatabaseSeriesDAL::LocalDatabaseSeriesDAL()
{
    m_lastSqliteError = SQLITE_OK;
}

void LocalDatabaseSeriesDAL::insert(Series *newSeries)
{
    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newSeries)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newSeries));
}

void LocalDatabaseSeriesDAL::update(Series *seriesToUpdate)
{
    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(seriesToUpdate)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(seriesToUpdate));
}

void LocalDatabaseSeriesDAL::del(const DicomMask &seriesMaskToDelete)
{
    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlDelete(seriesMaskToDelete)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(seriesMaskToDelete));
}

QList<Series*> LocalDatabaseSeriesDAL::query(const DicomMask &seriesMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Series*> seriesList;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(seriesMask)),
                                    &reply, &rows, &columns, error);

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
    series->setBodyPartExamined(reply[12 + row * columns]);
    series->setViewPosition(reply[13 + row * columns]);
    series->setManufacturer(reply[14 + row * columns]);
    series->setLaterality(reply[15 + row * columns][0]);//Laterality és un char
    series->setRetrievedDate(QDate().fromString(reply[16 + row * columns], "yyyyMMdd"));
    series->setRetrievedTime(QTime().fromString(reply[17 + row * columns], "hhmmss"));
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
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getInstanceUID() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getParentStudy()->getInstanceUID() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getSeriesNumber() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getModality() ) )
                                    .arg( newSeries->getDate().toString("yyyyMMdd") )
                                    .arg( newSeries->getTime().toString("hhmmss") )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getInstitutionName() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getPatientPosition() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getProtocolName() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getDescription() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getFrameOfReferenceUID() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getPositionReferenceIndicator() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getBodyPartExamined() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getViewPosition() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getManufacturer() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( newSeries->getLaterality() ) )                                    
                                    .arg( newSeries->getRetrievedDate().toString("yyyyMMdd") )
                                    .arg( newSeries->getRetrievedTime().toString("hhmmss") )
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
                                                        "State = '%18' "
                                                 "Where InstanceUID = '%19'")
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getParentStudy()->getInstanceUID() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getSeriesNumber() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getModality() ) )
                                    .arg( seriesToUpdate->getDate().toString("yyyyMMdd") )
                                    .arg( seriesToUpdate->getTime().toString("hhmmss") )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getInstitutionName() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getPatientPosition() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getProtocolName() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getDescription() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getFrameOfReferenceUID() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getPositionReferenceIndicator() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getBodyPartExamined() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getViewPosition() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getManufacturer() ) )
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getLaterality() ) )
                                    .arg( seriesToUpdate->getRetrievedDate().toString("yyyyMMdd") )
                                    .arg( seriesToUpdate->getRetrievedTime().toString("hhmmss") )
                                    .arg("0")
                                    .arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesToUpdate->getInstanceUID() ) );

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
        whereSentence = QString("where StudyInstanceUID = '%1'").arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesMask.getStudyInstanceUID() ) );

    if (!seriesMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" InstanceUID = '%1'").arg( DatabaseConnection::formatTextToValidSQLSyntax( seriesMask.getSeriesInstanceUID() ) );
    }

    return whereSentence;
}

void LocalDatabaseSeriesDAL::logError(const QString &sqlSentence)
{
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

}
