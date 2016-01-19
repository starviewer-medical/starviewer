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

#include "localdatabasepatientdal.h"

#include "dicommask.h"
#include "patient.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Binds the necessary values of the given query with the information of the given patient.
void bindValues(QSqlQuery &query, const Patient *patient)
{
    query.bindValue(":dicomPatientId", patient->getID());
    query.bindValue(":name", patient->getFullName());
    query.bindValue(":birthDate", patient->getBirthDate().toString("yyyyMMdd"));
    query.bindValue(":sex", patient->getSex());
}

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask.
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand)
{
    if (mask.getPatientID().isEmpty())
    {
        query.prepare(sqlCommand);
    }
    else
    {
        query.prepare(sqlCommand + " WHERE DICOMPatientId = :dicomPatientId");
        query.bindValue(":dicomPatientId", mask.getPatientID());
    }
}


}

LocalDatabasePatientDAL::LocalDatabasePatientDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabasePatientDAL::insert(Patient *patient)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO Patient (DICOMPatientId, Name, BirthDate, Sex) VALUES (:dicomPatientId, :name, :birthDate, :sex)");
    bindValues(query, patient);

    if (executeQueryAndLogError(query))
    {
        patient->setDatabaseID(query.lastInsertId().toLongLong());
        return true;
    }
    else
    {
        return false;
    }
}

bool LocalDatabasePatientDAL::update(const Patient *patient)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE Patient SET DICOMPatientId = :dicomPatientId, Name = :name, BirthDate = :birthDate, Sex = :sex WHERE ID = :id");
    bindValues(query, patient);
    query.bindValue(":id", patient->getDatabaseID());
    return executeQueryAndLogError(query);
}

bool LocalDatabasePatientDAL::del(qlonglong patientID)
{
    QSqlQuery query = getNewQuery();
    query.prepare("DELETE FROM Patient WHERE ID = :id");
    query.bindValue(":id", patientID);
    return executeQueryAndLogError(query);
}

QList<Patient*> LocalDatabasePatientDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT ID, DICOMPatientId, Name, Birthdate, Sex FROM Patient");
    QList<Patient*> patientList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            Patient *patient = new Patient();
            patient->setDatabaseID(query.value("ID").toLongLong());
            patient->setID(query.value("DICOMPatientId").toString());
            patient->setFullName(convertToQString(query.value("Name")));
            patient->setBirthDate(query.value("BirthDate").toString());
            patient->setSex(query.value("Sex").toString());
            patientList.append(patient);
        }
    }

    return patientList;
}

}
