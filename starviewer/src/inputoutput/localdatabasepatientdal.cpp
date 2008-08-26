/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <sqlite3.h>
#include <QString>

#include "localdatabasepatientdal.h"
#include "databaseconnection.h"
#include "logging.h"
#include "dicommask.h"

namespace udg {

LocalDatabasePatientDAL::LocalDatabasePatientDAL()
{
}

void LocalDatabasePatientDAL::insert(Patient *newPatient)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newPatient)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newPatient));
}

void LocalDatabasePatientDAL::update(Patient *patientToUpdate)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(patientToUpdate)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(patientToUpdate));
}

void LocalDatabasePatientDAL::del(DicomMask patientMaskToDelete)
{
    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlDelete(patientMaskToDelete)), 0, 0, 0);

    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(patientMaskToDelete));
}

QList<Patient*> LocalDatabasePatientDAL::query(DicomMask patientMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Patient*> patientList;

    m_dbConnection->getLock();

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(patientMask)),
                                    &reply, &rows, &columns, error);
    m_dbConnection->releaseLock();

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(patientMask));
        return patientList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        patientList.append(fillPatient(reply, index, columns));
    }

    return patientList;
}

void LocalDatabasePatientDAL::setConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabasePatientDAL::getLastError()
{
    return m_lastSqliteError;
}

Patient* LocalDatabasePatientDAL::fillPatient(char **reply, int row, int columns)
{
    Patient *patient = new Patient();

    patient->setID(reply[0 + row * columns]);
    patient->setFullName(reply[1 + row * columns]);
    patient->setBirthDate(reply[2 + row * columns]);
    patient->setSex(reply[3 + row * columns]);

    return patient;
}

QString LocalDatabasePatientDAL::buildSqlSelect(DicomMask patientMaskToSelect)
{
    QString selectSentence, whereSentence;

    selectSentence = "Select ID, Name, Birthdate, Sex "
                       "From Patient ";

    if (!patientMaskToSelect.getPatientId().isEmpty())
        whereSentence = QString(" Where ID = '%1' ").arg(patientMaskToSelect.getPatientId());

    return selectSentence + whereSentence;
}

QString LocalDatabasePatientDAL::buildSqlInsert(Patient *newPatient)
{
    QString insertSentence = QString ("Insert into Patient  (ID, Name, Birthdate, Sex) "
                                                   "values ('%1', '%2', '%3', '%4')")
                                    .arg(newPatient->getID())
                                    .arg(newPatient->getFullName())
                                    .arg(newPatient->getBirthDate().toString("yyyyMMdd"))
                                    .arg(newPatient->getSex());

    return insertSentence;
}

QString LocalDatabasePatientDAL::buildSqlUpdate(Patient *patientToUpdate)
{
    QString updateSentence = QString ("Update Patient Set   Name = '%1', "
                                                           "Birthdate = '%2', "
                                                           "Sex = '%3' "
                                                   "Where   ID = '%4'")
                                    .arg(patientToUpdate->getFullName())
                                    .arg(patientToUpdate->getBirthDate().toString("yyyyMMdd"))
                                    .arg(patientToUpdate->getSex())
                                    .arg(patientToUpdate->getID());
    return updateSentence;
}

QString LocalDatabasePatientDAL::buildSqlDelete(DicomMask patientMaskToDelete)
{
    QString deleteSentence, whereSentence = "";

    deleteSentence = "Delete From Patient ";
    if (!patientMaskToDelete.getPatientId().isEmpty())
        whereSentence = QString(" Where ID = '%1'").arg(patientMaskToDelete.getPatientId());

    return deleteSentence + whereSentence;
}

void LocalDatabasePatientDAL::logError(QString sqlSentence)
{
    QString errorNumber;

    errorNumber = errorNumber.setNum(getLastError(), 10);
    ERROR_LOG("S'ha produït l'error: " + errorNumber + " al executar la següent sentència sql " + sqlSentence);
}

}
