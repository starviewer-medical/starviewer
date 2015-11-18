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

#include "databaseconnection.h"
#include "logging.h"
#include "dicommask.h"

#include <QSqlQuery>
#include <QString>
#include <QVariant>

namespace udg {

LocalDatabasePatientDAL::LocalDatabasePatientDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabasePatientDAL::insert(Patient *newPatient)
{
    QSqlQuery query;

    if (!query.exec(buildSqlInsert(newPatient)))
    {
        logError(query.lastQuery());
        return false;
    }
    else
    {
        newPatient->setDatabaseID(query.lastInsertId().toLongLong());
        return true;
    }
}

bool LocalDatabasePatientDAL::update(Patient *patientToUpdate)
{
    QSqlQuery query;

    if (!query.exec(buildSqlUpdate(patientToUpdate)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

bool LocalDatabasePatientDAL::del(qlonglong patientID)
{
    QSqlQuery query;

    if (!query.exec(buildSqlDelete(patientID)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

QList<Patient*> LocalDatabasePatientDAL::query(const DicomMask &patientMask)
{
    QList<Patient*> patientList;
    QSqlQuery query;

    if (!query.exec(buildSqlSelect(patientMask)))
    {
        logError(query.lastQuery());
        return patientList;
    }

    while (query.next())
    {
        patientList.append(fillPatient(query));
    }

    return patientList;
}

Patient* LocalDatabasePatientDAL::fillPatient(const QSqlQuery &query)
{
    Patient *patient = new Patient();

    patient->setDatabaseID(query.value("ID").toLongLong());
    patient->setID(query.value("DICOMPatientId").toString());
    patient->setFullName(convertToQString(query.value("Name")));
    patient->setBirthDate(query.value("BirthDate").toString());
    patient->setSex(query.value("Sex").toString());

    return patient;
}

QString LocalDatabasePatientDAL::buildSqlSelect(const DicomMask &patientMaskToSelect)
{
    QString selectSentence = "Select ID, DICOMPatientID, Name, Birthdate, Sex "
                             "From Patient ";

    QString whereSentence;
    if (!patientMaskToSelect.getPatientID().isEmpty())
    {
        whereSentence = QString(" Where DICOMPatientID = '%1' ").arg(DatabaseConnection::formatTextToValidSQLSyntax(patientMaskToSelect.getPatientID()));
    }

    return selectSentence + whereSentence;
}

QString LocalDatabasePatientDAL::buildSqlInsert(Patient *newPatient)
{
    QString insertSentence = QString ("Insert into Patient  (DICOMPatientID, Name, Birthdate, Sex) "
                                                   "values ('%1', '%2', '%3', '%4')")
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newPatient->getID()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newPatient->getFullName()))
                                    .arg(newPatient->getBirthDate().toString("yyyyMMdd"))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newPatient->getSex()));

    return insertSentence;
}

QString LocalDatabasePatientDAL::buildSqlUpdate(Patient *patientToUpdate)
{
    QString updateSentence = QString ("Update Patient Set  DICOMPatientID = '%1', "
                                                           "Name = '%2', "
                                                           "Birthdate = '%3', "
                                                           "Sex = '%4' "
                                                    " Where ID = %5")
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(patientToUpdate->getID()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(patientToUpdate->getFullName()))
                                    .arg(patientToUpdate->getBirthDate().toString("yyyyMMdd"))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(patientToUpdate->getSex()))
                                    .arg(patientToUpdate->getDatabaseID());
    return updateSentence;
}

QString LocalDatabasePatientDAL::buildSqlDelete(qlonglong patientID)
{
    QString deleteSentence = "Delete From Patient ";
    QString whereSentence = QString(" Where ID = %1").arg(patientID);

    return deleteSentence + whereSentence;
}
}
