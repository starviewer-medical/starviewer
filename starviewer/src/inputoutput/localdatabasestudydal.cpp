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

#include "localdatabasestudydal.h"

#include "dicommask.h"
#include "patient.h"
#include "study.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Binds the necessary values of the given query with the information of the given study.
void bindValues(QSqlQuery &query, const Study *study, const QDate &lastAccessDate)
{
    query.bindValue(":instanceUID", study->getInstanceUID());
    query.bindValue(":patientId", study->getParentPatient()->getDatabaseID());
    query.bindValue(":id", study->getID());
    query.bindValue(":patientAge", study->getPatientAge());
    query.bindValue(":patientWeight", study->getWeight());
    query.bindValue(":patientHeight", study->getHeight());
    query.bindValue(":modalities", study->getModalitiesAsSingleString());
    query.bindValue(":date", study->getDate().toString("yyyyMMdd"));
    query.bindValue(":time", study->getTime().toString("hhmmss"));
    query.bindValue(":accessionNumber", study->getAccessionNumber());
    query.bindValue(":description", study->getDescription());
    query.bindValue(":referringPhysicianName", study->getReferringPhysiciansName());
    query.bindValue(":lastAccessDate", lastAccessDate.toString("yyyyMMdd"));
    query.bindValue(":retrievedDate", study->getRetrievedDate().toString("yyyyMMdd"));
    query.bindValue(":retrievedTime", study->getRetrievedTime().toString("hhmmss"));
    query.bindValue(":state", 0);
}

// Prepares the given query to query studies according to the given mask and access dates and ordered according to orderBy.
void prepareSelectFromStudy(QSqlQuery &query, const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter,
                            const QString &orderBy = QString())
{
    QString select("SELECT InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, AccessionNumber, Description, "
                          "ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, State "
                   "FROM Study");
    QString where;
    if (!mask.getStudyInstanceUID().isEmpty())
    {
        where += "InstanceUID = :instanceUID";
    }
    if (accessedBefore.isValid())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "LastAccessDate < :accessedBefore";
    }
    if (accessedAfter.isValid())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "LastAccessDate >= :accessedAfter";
    }
    if (!where.isEmpty())
    {
        where = " WHERE " + where;
    }

    query.prepare(select + where + orderBy);

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":instanceUID", mask.getStudyInstanceUID());
    }
    if (accessedBefore.isValid())
    {
        query.bindValue(":accessedBefore", accessedBefore.toString("yyyyMMdd"));
    }
    if (accessedAfter.isValid())
    {
        query.bindValue(":accessedAfter", accessedAfter.toString("yyyyMMdd"));
    }
}

// Prepares the given query to query studies and patients according to the given mask and access dates.
void prepareSelectFromStudyPatient(QSqlQuery &query, const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter)
{
    QString select("SELECT InstanceUID, PatientID, Study.ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, AccessionNumber, Description, "
                          "ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, Study.State, "
                          "Patient.ID AS Patient_ID, Patient.DICOMPatientId AS Patient_DICOMPatientId, Patient.Name AS Patient_Name, "
                          "Patient.BirthDate AS Patient_BirthDate, Patient.Sex AS Patient_Sex "
                   "FROM Study, Patient");
    QString where(" WHERE PatientID = Patient_ID");
    if (!mask.getStudyInstanceUID().isEmpty())
    {
        where += " AND InstanceUID = :instanceUID";
    }
    if (!mask.getPatientID().isEmpty() && mask.getPatientID() != "*")
    {
        where += " AND Patient_DICOMPatientId LIKE :patient_dicomPatientId";
    }
    if (!mask.getPatientName().isEmpty() && mask.getPatientName() != "*")
    {
        where += " AND Patient_Name LIKE :patient_patientName";
    }
    if (mask.getStudyDateMinimum().isValid())
    {
        where += " AND Date >= :minimumDate";
    }
    if (mask.getStudyDateMaximum().isValid())
    {
        where += " AND Date <= :maximumDate";
    }
    if (accessedBefore.isValid())
    {
        where += " AND LastAccessDate < :accessedBefore";
    }
    if (accessedAfter.isValid())
    {
        where += " AND LastAccessDate >= :accessedAfter";
    }
    if (!mask.getSeriesModality().isEmpty())
    {
        where += " AND Modalities LIKE :modalities";
    }
    QString orderBy(" ORDER BY Patient_Name");

    query.prepare(select + where + orderBy);

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":instanceUID", mask.getStudyInstanceUID());
    }
    if (!mask.getPatientID().isEmpty() && mask.getPatientID() != "*")
    {
        query.bindValue(":patient_dicomPatientId", QString("%1").arg(mask.getPatientID().replace("*", "%")));
    }
    if (!mask.getPatientName().isEmpty() && mask.getPatientName() != "*")
    {
        query.bindValue(":patient_patientName", QString("%%1%").arg(mask.getPatientName().replace("*", "")));
    }
    if (mask.getStudyDateMinimum().isValid())
    {
        query.bindValue(":minimumDate", mask.getStudyDateMinimum().toString("yyyyMMdd"));
    }
    if (mask.getStudyDateMaximum().isValid())
    {
        query.bindValue(":maximumDate", mask.getStudyDateMaximum().toString("yyyyMMdd"));
    }
    if (accessedBefore.isValid())
    {
        query.bindValue(":accessedBefore", accessedBefore.toString("yyyyMMdd"));
    }
    if (accessedAfter.isValid())
    {
        query.bindValue(":accessedAfter", accessedAfter.toString("yyyyMMdd"));
    }
    if (!mask.getSeriesModality().isEmpty())
    {
        query.bindValue(":modalities", QString("%%1%").arg(mask.getSeriesModality()));
    }
}

}

LocalDatabaseStudyDAL::LocalDatabaseStudyDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseStudyDAL::insert(const Study *study, const QDate &lastAccessDate)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO Study (InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, AccessionNumber, "
                                     "Description, ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime , State) "
                  "VALUES (:instanceUID, :patientId, :id, :patientAge, :patientWeight, :patientHeight, :modalities, :date, :time, :accessionNumber, "
                          ":description, :referringPhysicianName, :lastAccessDate, :retrievedDate, :retrievedTime, :state)");
    bindValues(query, study, lastAccessDate);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseStudyDAL::update(const Study *study, const QDate &lastAccessDate)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE Study SET ID = :id, PatientAge = :patientAge, PatientWeigth = :patientWeight, PatientHeigth = :patientHeight, "
                                   "Modalities = :modalities, Date = :date, Time = :time, AccessionNumber = :accessionNumber, Description = :description, "
                                   "ReferringPhysicianName = :referringPhysicianName, LastAccessDate = :lastAccessDate, RetrievedDate = :retrievedDate, "
                                   "RetrievedTime = :retrievedTime, State = :state "
                  "WHERE InstanceUid = :instanceUID");
    bindValues(query, study, lastAccessDate);
    return executeQueryAndLogError(query);
}

// TODO We could pass just the StudyInstanceUID instead of a mask
bool LocalDatabaseStudyDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();

    if (mask.getStudyInstanceUID().isEmpty())
    {
        query.prepare("DELETE FROM Study");
    }
    else
    {
        query.prepare("DELETE FROM Study WHERE InstanceUID = :instanceUID");
        query.bindValue(":instanceUID", mask.getStudyInstanceUID());
    }

    return executeQueryAndLogError(query);
}

// TODO We could pass just the StudyInstanceUID instead of a mask or include access dates in the mask
QList<Study*> LocalDatabaseStudyDAL::query(const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter)
{
    return query(mask, accessedBefore, accessedAfter, "");
}

// TODO We could pass just the StudyInstanceUID instead of a mask or include access dates in the mask
QList<Study*> LocalDatabaseStudyDAL::queryOrderByLastAccessDate(const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter)
{
    return query(mask, accessedBefore, accessedAfter, " ORDER BY LastAccessDate");
}

QList<Patient*> LocalDatabaseStudyDAL::queryPatientStudy(const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter)
{
    QSqlQuery query = getNewQuery();
    prepareSelectFromStudyPatient(query, mask, accessedBefore, accessedAfter);
    QList<Patient*> patientList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            Patient *patient = getPatient(query);
            patient->addStudy(getStudy(query));
            patientList.append(patient);
        }
    }

    return patientList;
}

bool LocalDatabaseStudyDAL::exists(const QString &studyInstanceUID)
{
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT InstanceUID FROM Study WHERE InstanceUID = :instanceUID");
    query.bindValue(":instanceUID", studyInstanceUID);
    return executeQueryAndLogError(query) && query.next();
}

qlonglong LocalDatabaseStudyDAL::getPatientIDFromStudyInstanceUID(const QString &studyInstanceUID)
{
    QSqlQuery query = getNewQuery();
    query.prepare("SELECT PatientID FROM Study WHERE InstanceUID = :instanceUID");
    query.bindValue(":instanceUID", studyInstanceUID);

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value("PatientID").toString().toLongLong();
    }
    else
    {
        return -1;
    }
}

Study* LocalDatabaseStudyDAL::getStudy(const QSqlQuery &query)
{
    Study *study = new Study();
    study->setInstanceUID(query.value("InstanceUID").toString());
    study->setID(query.value("ID").toString());
    study->setPatientAge(query.value("PatientAge").toString());
    study->setWeight(query.value("PatientWeigth").toString().toDouble());
    study->setHeight(query.value("PatientHeigth").toString().toDouble());
    study->setDate(query.value("Date").toString());
    study->setTime(query.value("Time").toString());
    study->setAccessionNumber(query.value("AccessionNumber").toString());
    study->setDescription(convertToQString(query.value("Description")));
    study->setReferringPhysiciansName(convertToQString(query.value("ReferringPhysicianName")));
    study->setRetrievedDate(QDate::fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    study->setRetrievedTime(QTime::fromString(query.value("RetrievedTime").toString(), "hhmmss"));

    // Add modalities, which are separated by "/"
    // TODO This knowledge should be somewhere else
    QStringList modalities = query.value("Modalities").toString().split("/");
    foreach (const QString &modality, modalities)
    {
        study->addModality(modality);
    }

    return study;
}

Patient* LocalDatabaseStudyDAL::getPatient(const QSqlQuery &query)
{
    Patient *patient = new Patient();
    patient->setDatabaseID(query.value("Patient_ID").toLongLong());
    patient->setID(query.value("Patient_DICOMPatientId").toString());
    patient->setFullName(convertToQString(query.value("Patient_Name")));
    patient->setBirthDate(query.value("Patient_BirthDate").toString());
    patient->setSex(query.value("Patient_Sex").toString());

    return patient;
}

QList<Study*> LocalDatabaseStudyDAL::query(const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter, const QString &orderBy)
{
    QSqlQuery query = getNewQuery();
    prepareSelectFromStudy(query, mask, accessedBefore, accessedAfter, orderBy);
    QList<Study*> studyList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            studyList.append(getStudy(query));
        }
    }

    return studyList;
}

}
