/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <sqlite3.h>
#include <QString>
#include <QDate>

#include "patient.h"
#include "localdatabasestudydal.h"
#include "databaseconnection.h"
#include "logging.h"
#include "dicommask.h"

namespace udg {

LocalDatabaseStudyDAL::LocalDatabaseStudyDAL()
{
}

void LocalDatabaseStudyDAL::insert(Study *newStudy, QDate lastAccessDate)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newStudy, lastAccessDate)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newStudy, lastAccessDate));
}

void LocalDatabaseStudyDAL::update(Study *studyToUpdate, QDate lastAccessDate)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(studyToUpdate, lastAccessDate)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(studyToUpdate, lastAccessDate));
}

void LocalDatabaseStudyDAL::del(DicomMask studyMaskToDelete)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlDelete(studyMaskToDelete)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(studyMaskToDelete));
}

QList<Study*> LocalDatabaseStudyDAL::query(DicomMask studyMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Study*> studyList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(studyMask)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(studyMask));
        return studyList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        studyList.append(fillStudy(reply, index, columns));
    }

    return studyList;
}

void LocalDatabaseStudyDAL::setDatabaseConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabaseStudyDAL::getLastError()
{
    return m_lastSqliteError;
}

Study* LocalDatabaseStudyDAL::fillStudy(char **reply, int row, int columns)
{
    QString studyInstanceUID;
    Study *study = new Study();

    study->setInstanceUID(reply[0 + row * columns]);
    study->setID(reply[2 + row * columns]);
    study->setPatientAge(QString(reply[3 + row * columns]).toInt());
    study->setWeight(QString(reply[4 + row * columns]).toDouble());
    study->setHeight(QString(reply[5 + row * columns]).toDouble());
    study->setDate(reply[7 + row * columns]);
    study->setTime(reply[8 + row * columns]);
    study->setAccessionNumber(reply[9 + row * columns]);
    study->setDescription(reply[10 + row * columns]);
    study->setReferringPhysiciansName(reply[11 + row * columns]);

    return study;
}

QString LocalDatabaseStudyDAL::buildSqlSelect(DicomMask studyMaskToSelect)
{
    QString selectSentence, whereSentence;

    selectSentence = "Select InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, "
                            "AccessionNumber, Description, ReferringPhysicianName, LastAccessDate, State "
                       "From Study ";

    if (!studyMaskToSelect.getStudyUID().isEmpty())
        whereSentence = QString(" Where InstanceUID = '%1' ").arg(studyMaskToSelect.getStudyUID());

    if (!studyMaskToSelect.getLastAccessDate().isNull())
    {
        if (!whereSentence.isEmpty())
            whereSentence += " and ";
        else whereSentence = " where ";

        whereSentence += QString(" LastAccessDate < '%1' ").arg(studyMaskToSelect.getLastAccessDate().toString("yyyyMMdd"));
    }

    return selectSentence + whereSentence;
}

QString LocalDatabaseStudyDAL::buildSqlInsert(Study *newStudy, QDate lastAcessDate)
{
    QString insertSentence = QString ("Insert into Study   (InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, "
                                                           "Modalities, Date, Time, AccessionNumber, Description, "
                                                           "ReferringPhysicianName, LastAccessDate, State) "
                                                   "values ('%1', '%2', '%3', %4, %5, %6, '%7', '%8', '%9', '%10', '%11', "
                                                            "'%12', '%13', %14 )")
                                    .arg(newStudy->getInstanceUID())
                                    .arg(newStudy->getParentPatient()->getID())
                                    .arg(newStudy->getID())
                                    .arg(newStudy->getPatientAge())
                                    .arg(newStudy->getWeight())
                                    .arg(newStudy->getHeight())
                                    .arg(newStudy->getModalitiesAsSingleString())
                                    .arg(newStudy->getDate().toString("yyyyMMdd"))
                                    .arg(newStudy->getTime().toString("hhmmss"))
                                    .arg(newStudy->getAccessionNumber())
                                    .arg(newStudy->getDescription())
                                    .arg(newStudy->getReferringPhysiciansName())
                                    .arg(lastAcessDate.toString("yyyyMMdd"))
                                    .arg("0");

    return insertSentence;
}

QString LocalDatabaseStudyDAL::buildSqlUpdate(Study *newStudy, QDate lastAccessDate)
{
    QString updateSentence = QString ("Update Study set PatientID = '%1', " 
                                                       "ID = '%2', " 
                                                       "PatientAge = %3,"
                                                       "PatientWeigth = %4, "
                                                       "PatientHeigth = %5, "
                                                       "Modalities = '%6', "
                                                       "Date = '%7', "
                                                       "Time = '%8', "
                                                       "AccessionNumber = '%9', "
                                                       "Description = '%10', "
                                                       "ReferringPhysicianName = '%11', "
                                                       "LastAccessDate = '%12', "
                                                       "State = %13 "
                                                "Where InstanceUid = %14")
                                    .arg(newStudy->getParentPatient()->getID())
                                    .arg(newStudy->getID())
                                    .arg(newStudy->getPatientAge())
                                    .arg(newStudy->getWeight())
                                    .arg(newStudy->getHeight())
                                    .arg(newStudy->getModalitiesAsSingleString())
                                    .arg(newStudy->getDate().toString("yyyyMMdd"))
                                    .arg(newStudy->getTime().toString("hhmmss"))
                                    .arg(newStudy->getAccessionNumber())
                                    .arg(newStudy->getDescription())
                                    .arg(newStudy->getReferringPhysiciansName())
                                    .arg(lastAccessDate.toString("yyyyMMdd"))
                                    .arg("0")
                                    .arg(newStudy->getInstanceUID());

    return updateSentence;
}

QString LocalDatabaseStudyDAL::buildSqlDelete(DicomMask studyMaskToDelete)
{
    QString deleteSentence, whereSentence = "";

    deleteSentence = "Delete From Study ";
    if (!studyMaskToDelete.getStudyUID().isEmpty())
        whereSentence = QString(" Where InstanceUID = '%1'").arg(studyMaskToDelete.getStudyUID());

    return deleteSentence + whereSentence;
}

void LocalDatabaseStudyDAL::logError(QString sqlSentence)
{
    QString errorNumber;

    errorNumber = errorNumber.setNum(getLastError(), 10);
    ERROR_LOG("S'ha produït l'error: " + errorNumber + " al executar la següent sentència sql " + sqlSentence);
}

}
