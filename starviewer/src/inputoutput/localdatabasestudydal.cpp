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

QList<Study*> LocalDatabaseStudyDAL::query(DicomMask studyMask, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Study*> studyList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(studyMask, lastAccessDateMinor, lastAccessDateEqualOrMajor)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(studyMask, lastAccessDateMinor, lastAccessDateEqualOrMajor));
        return studyList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        studyList.append(fillStudy(reply, index, columns));
    }

    return studyList;
}

QList<Patient*> LocalDatabaseStudyDAL::queryPatientStudy(DicomMask patientStudyMaskToQuery, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Patient*> patientList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelectStudyPatient(patientStudyMaskToQuery, lastAccessDateMinor, lastAccessDateEqualOrMajor)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelectStudyPatient(patientStudyMaskToQuery, lastAccessDateMinor, lastAccessDateEqualOrMajor));
        return patientList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        Patient *patient = fillPatient(reply, index, columns);
        patient->addStudy(fillStudy(reply, index, columns));

        patientList.append(patient);
    }

    return patientList;
}

int LocalDatabaseStudyDAL::countHowManyStudiesHaveAPatient(QString patientID)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Study*> studyList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlCountHowManyStudiesHaveAPatient(patientID)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlCountHowManyStudiesHaveAPatient(patientID));
        return -1;
    }

    return QString(reply[1]).toInt();
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
    study->setRetrievedDate(QDate().fromString(reply[13 + row * columns], "yyyyMMdd"));
    study->setRetrievedTime(QTime().fromString(reply[14 + row * columns], "hhmmss"));

    return study;
}

Patient* LocalDatabaseStudyDAL::fillPatient(char **reply, int row, int columns)
{
    Patient *patient = new Patient();

    patient->setID(reply[16 + row * columns]);
    patient->setFullName(reply[17 + row * columns]);
    patient->setBirthDate(reply[18 + row * columns]);
    patient->setSex(reply[19 + row * columns]);

    return patient;
}

QString LocalDatabaseStudyDAL::buildSqlSelect(DicomMask studyMaskToSelect, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    QString selectSentence, whereSentence, orderSentence;

    selectSentence = "Select InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, "
                            "AccessionNumber, Description, ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, "
                            "State "
                       "From Study ";

    if (!studyMaskToSelect.getStudyUID().isEmpty())
        whereSentence = QString(" Where InstanceUID = '%1' ").arg(studyMaskToSelect.getStudyUID());

    if (lastAccessDateMinor.isValid())
    {
        if (!whereSentence.isEmpty())
            whereSentence += " and ";
        else whereSentence = " where ";

        whereSentence += QString(" LastAccessDate < '%1' ").arg(lastAccessDateMinor.toString("yyyyMMdd"));
    }

    if (lastAccessDateEqualOrMajor.isValid())
    {
        if (!whereSentence.isEmpty())
            whereSentence += " and ";
        else whereSentence = " where ";

        whereSentence += QString(" '%1' <= LastAccessDate ").arg(lastAccessDateEqualOrMajor.toString("yyyyMMdd"));
    }


    orderSentence = " Order by LastAccessDate";

    return selectSentence + whereSentence + orderSentence;
}

QString LocalDatabaseStudyDAL::buildSqlSelectStudyPatient(DicomMask studyMaskToSelect, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    QString selectSentence, whereSentence, orderBySentence;

    selectSentence = "Select InstanceUID, PatientID, Study.ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, "
                            "AccessionNumber, Description, ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, " "Study.State, Patient.Id, Patient.Name, Patient.Birthdate, Patient.Sex "
                       "From Study, Patient ";

    whereSentence = "Where Study.PatientId = Patient.Id ";

    if (!studyMaskToSelect.getStudyUID().isEmpty())
        whereSentence += QString(" and InstanceUID = '%1' ").arg(studyMaskToSelect.getStudyUID());

    if (!studyMaskToSelect.getPatientId().isEmpty() && studyMaskToSelect.getPatientId() != "*")
    {
        whereSentence += QString(" and Patient.Id like '%%1%' ").arg(studyMaskToSelect.getPatientId().replace("*",""));
    }
    if (!studyMaskToSelect.getPatientName().isEmpty() && studyMaskToSelect.getPatientName() != "*")
        whereSentence += QString(" and Patient.Name like '%%1%' ").arg(studyMaskToSelect.getPatientName().replace("*",""));

    //Si filtrem per data
    if (studyMaskToSelect.getStudyDate().length() == 8)
    {
        whereSentence += QString(" and Date = '%1'" ).arg(studyMaskToSelect.getStudyDate());
    }
    else if (studyMaskToSelect.getStudyDate().length() == 9)
    {
        if (studyMaskToSelect.getStudyDate().at(0) == '-')
        {
            whereSentence += QString(" and Date <= '%1'").arg(studyMaskToSelect.getStudyDate().mid(1, 8));
        }
        else if (studyMaskToSelect.getStudyDate().at(8) == '-')
        {
            whereSentence += QString(" and Date >= '%1'").arg(studyMaskToSelect.getStudyDate().mid(0, 8));
        }
    }
    else if (studyMaskToSelect.getStudyDate().length() == 17)
    {
        whereSentence += QString(" and Date between '%1' and '%2'").arg(studyMaskToSelect.getStudyDate().mid(0, 8)).arg( studyMaskToSelect.getStudyDate().mid(9, 8));
    }

    if (lastAccessDateMinor.isValid())
        whereSentence += QString(" and LastAccessDate < '%1' ").arg(lastAccessDateMinor.toString("yyyyMMdd"));
    if (lastAccessDateEqualOrMajor.isValid())
        whereSentence += QString(" and '%1' <= LastAccessDate ").arg(lastAccessDateEqualOrMajor.toString("yyyyMMdd"));

    orderBySentence = " Order by Patient.Name";

    return selectSentence + whereSentence + orderBySentence;
}

QString LocalDatabaseStudyDAL::buildSqlCountHowManyStudiesHaveAPatient(QString patientID)
{
    QString selectSentence = QString ("Select count(*) "
                                      " From Patient, Study "
                                      "Where Patient.Id = Study.PatientId  and"
                                      "      Patient.Id = '%1'")
                                    .arg(patientID);

    return selectSentence;
}

QString LocalDatabaseStudyDAL::buildSqlInsert(Study *newStudy, QDate lastAcessDate)
{
    QString insertSentence = QString ("Insert into Study   (InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, "
                                                           "Modalities, Date, Time, AccessionNumber, Description, "
                                                           "ReferringPhysicianName, LastAccessDate, RetrievedDate, "
                                                           "RetrievedTime , State) "
                                                   "values ('%1', '%2', '%3', %4, %5, %6, '%7', '%8', '%9', '%10', '%11', "
                                                            "'%12', '%13', '%14', '%15', %16)")
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
                                    .arg(newStudy->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(newStudy->getRetrievedTime().toString("hhmmss"))
                                    .arg("0");

    return insertSentence;
}

QString LocalDatabaseStudyDAL::buildSqlUpdate(Study *studyToUpdate, QDate lastAccessDate)
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
                                                       "RetrievedDate = '%13', "
                                                       "RetrievedTime = '%14', "
                                                       "State = %15 "
                                                "Where InstanceUid = '%16'")
                                    .arg(studyToUpdate->getParentPatient()->getID())
                                    .arg(studyToUpdate->getID())
                                    .arg(studyToUpdate->getPatientAge())
                                    .arg(studyToUpdate->getWeight())
                                    .arg(studyToUpdate->getHeight())
                                    .arg(studyToUpdate->getModalitiesAsSingleString())
                                    .arg(studyToUpdate->getDate().toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getTime().toString("hhmmss"))
                                    .arg(studyToUpdate->getAccessionNumber())
                                    .arg(studyToUpdate->getDescription())
                                    .arg(studyToUpdate->getReferringPhysiciansName())
                                    .arg(lastAccessDate.toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getRetrievedTime().toString("hhmmss"))
                                    .arg("0")
                                    .arg(studyToUpdate->getInstanceUID());

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
