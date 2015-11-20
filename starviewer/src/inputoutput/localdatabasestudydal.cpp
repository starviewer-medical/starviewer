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

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QVariant>

#include "patient.h"
#include "localdatabasestudydal.h"
#include "databaseconnection.h"
#include "logging.h"
#include "dicommask.h"

namespace udg {

LocalDatabaseStudyDAL::LocalDatabaseStudyDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabaseStudyDAL::insert(Study *newStudy, const QDate &lastAccessDate)
{
    QSqlQuery query;

    if (!query.exec(buildSqlInsert(newStudy, lastAccessDate)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

bool LocalDatabaseStudyDAL::update(Study *studyToUpdate, const QDate &lastAccessDate)
{
    QSqlQuery query;

    if (!query.exec(buildSqlUpdate(studyToUpdate, lastAccessDate)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

bool LocalDatabaseStudyDAL::del(const DicomMask &studyMaskToDelete)
{
    QSqlQuery query;

    if (!query.exec(buildSqlDelete(studyMaskToDelete)))
    {
        logError(query.lastQuery());
        return false;
    }

    return true;
}

QList<Study*> LocalDatabaseStudyDAL::queryOrderByLastAccessDate(const DicomMask &studyMask, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    QList<Study*> studyList;
    QString sqlSentence = buildSqlSelect(studyMask, lastAccessDateMinor, lastAccessDateEqualOrMajor) + " Order by LastAccessDate";
    QSqlQuery query;

    if (!query.exec(sqlSentence))
    {
        logError(query.lastQuery());
        return studyList;
    }

    while (query.next())
    {
        studyList.append(fillStudy(query));
    }

    return studyList;
}

QList<Study*> LocalDatabaseStudyDAL::query(const DicomMask &studyMask, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    QList<Study*> studyList;
    QSqlQuery query;

    if (!query.exec(buildSqlSelect(studyMask, lastAccessDateMinor, lastAccessDateEqualOrMajor)))
    {
        logError(query.lastQuery());
        return studyList;
    }

    while (query.next())
    {
        studyList.append(fillStudy(query));
    }

    return studyList;
}

QList<Patient*> LocalDatabaseStudyDAL::queryPatientStudy(const DicomMask &patientStudyMaskToQuery, QDate lastAccessDateMinor, QDate lastAccessDateEqualOrMajor)
{
    QList<Patient*> patientList;
    QSqlQuery query;

    if (!query.exec(buildSqlSelectStudyPatient(patientStudyMaskToQuery, lastAccessDateMinor, lastAccessDateEqualOrMajor)))
    {
        logError(query.lastQuery());
        return patientList;
    }

    while (query.next())
    {
        Patient *patient = fillPatient(query);
        patient->addStudy(fillStudy(query));

        patientList.append(patient);
    }

    return patientList;
}

qlonglong LocalDatabaseStudyDAL::getPatientIDFromStudyInstanceUID(const QString &studyInstanceUID)
{
    qlonglong patientID = -1;
    QSqlQuery query;

    if (!query.exec(buildSqlGetPatientIDFromStudyInstanceUID(studyInstanceUID)))
    {
        logError(query.lastQuery());
    }
    else
    {
        if (query.next())
        {
            // Si cerquem per UID només podem tenir un resultat, ja que UID és camp clau al a taula Study
            patientID = query.value("PatientID").toString().toLongLong();
        }
    }

    return patientID;
}

Study* LocalDatabaseStudyDAL::fillStudy(const QSqlQuery &query)
{
    Study *study = new Study();
    QStringList modalities;

    study->setInstanceUID(query.value("InstanceUID").toString());
    study->setID(query.value(2).toString());
    study->setPatientAge(query.value("PatientAge").toString());
    study->setWeight(query.value("PatientWeigth").toString().toDouble());
    study->setHeight(query.value("PatientHeigth").toString().toDouble());
    study->setDate(query.value("Date").toString());
    study->setTime(query.value("Time").toString());
    study->setAccessionNumber(query.value("AccessionNumber").toString());
    study->setDescription(convertToQString(query.value("Description")));
    study->setReferringPhysiciansName(convertToQString(query.value("ReferringPhysicianName")));
    study->setRetrievedDate(QDate().fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    study->setRetrievedTime(QTime().fromString(query.value("RetrievedTime").toString(), "hhmmss"));

    // Afegim la modalitat que estan separades per "/"
    modalities = query.value("Modalities").toString().split("/");
    foreach (const QString &modality, modalities)
    {
        study->addModality(modality);
    }

    return study;
}

Patient* LocalDatabaseStudyDAL::fillPatient(const QSqlQuery &query)
{
    Patient *patient = new Patient();

    patient->setDatabaseID(query.value(16).toLongLong());
    patient->setID(query.value(17).toString());
    patient->setFullName(convertToQString(query.value(18)));
    patient->setBirthDate(query.value(19).toString());
    patient->setSex(query.value(20).toString());

    return patient;
}

// TODO: Si només acceptem com a paràmatre de filtrar de la DICOMMask l'studyInstanceUID el que s'hauria de fer és directament passar un QString amb
// StudyInstanceUID
QString LocalDatabaseStudyDAL::buildSqlSelect(const DicomMask &studyMaskToSelect, const QDate &lastAccessDateMinor, const QDate &lastAccessDateEqualOrMajor)
{
    QString selectSentence = "Select InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, "
                            "AccessionNumber, Description, ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, "
                            "State "
                            "From Study ";
    QString whereSentence;
    if (!studyMaskToSelect.getStudyInstanceUID().isEmpty())
    {
        whereSentence = QString(" Where InstanceUID = '%1' ").arg(formatTextToValidSQLSyntax(studyMaskToSelect.getStudyInstanceUID()));
    }

    if (lastAccessDateMinor.isValid())
    {
        if (!whereSentence.isEmpty())
        {
            whereSentence += " and ";
        }
        else
        {
            whereSentence = " where ";
        }

        whereSentence += QString(" LastAccessDate < '%1' ").arg(lastAccessDateMinor.toString("yyyyMMdd"));
    }

    if (lastAccessDateEqualOrMajor.isValid())
    {
        if (!whereSentence.isEmpty())
        {
            whereSentence += " and ";
        }
        else
        {
            whereSentence = " where ";
        }

        whereSentence += QString(" '%1' <= LastAccessDate ").arg(lastAccessDateEqualOrMajor.toString("yyyyMMdd"));
    }

    return selectSentence + whereSentence;
}

QString LocalDatabaseStudyDAL::buildSqlSelectStudyPatient(const DicomMask &studyMaskToSelect, const QDate &lastAccessDateMinor,
                                                          const QDate &lastAccessDateEqualOrMajor)
{
    QString selectSentence = "Select InstanceUID, PatientID, Study.ID, PatientAge, PatientWeigth, PatientHeigth, Modalities, Date, Time, "
                            "AccessionNumber, Description, ReferringPhysicianName, LastAccessDate, RetrievedDate, RetrievedTime, "
                            "Study.State, Patient.ID, Patient.DICOMPatientId, Patient.Name, "
                            "Patient.Birthdate, Patient.Sex "
                       "From Study, Patient ";

    QString whereSentence = "Where Study.PatientID = Patient.ID ";

    if (!studyMaskToSelect.getStudyInstanceUID().isEmpty())
    {
        whereSentence += QString(" and InstanceUID = '%1' ").arg(formatTextToValidSQLSyntax(studyMaskToSelect.getStudyInstanceUID()));
    }

    if (!studyMaskToSelect.getPatientID().isEmpty() && studyMaskToSelect.getPatientID() != "*")
    {
        whereSentence += QString(" and Patient.DICOMPatientID like '%%1%' ")
                            .arg(formatTextToValidSQLSyntax(studyMaskToSelect.getPatientID().replace("*", "")));
    }
    if (!studyMaskToSelect.getPatientName().isEmpty() && studyMaskToSelect.getPatientName() != "*")
    {
        whereSentence += QString(" and Patient.Name like '%%1%' ")
                            .arg(formatTextToValidSQLSyntax(studyMaskToSelect.getPatientName().replace("*", "")));
    }

    // Si filtrem per data
    if (studyMaskToSelect.getStudyDateMinimum().isValid())
    {
        whereSentence += QString(" and Date >= '%1'").arg(studyMaskToSelect.getStudyDateMinimum().toString("yyyyMMdd"));
    }

    if (studyMaskToSelect.getStudyDateMaximum().isValid())
    {
        whereSentence += QString(" and Date <= '%1'").arg(studyMaskToSelect.getStudyDateMaximum().toString("yyyyMMdd"));
    }

    if (lastAccessDateMinor.isValid())
    {
        whereSentence += QString(" and LastAccessDate < '%1' ").arg(lastAccessDateMinor.toString("yyyyMMdd"));
    }
    if (lastAccessDateEqualOrMajor.isValid())
    {
        whereSentence += QString(" and '%1' <= LastAccessDate ").arg(lastAccessDateEqualOrMajor.toString("yyyyMMdd"));
    }

    if (studyMaskToSelect.getSeriesModality().length() > 0)
    {
        whereSentence += QString(" and Modalities like '%%1%' ").arg(studyMaskToSelect.getSeriesModality());
    }

    QString orderBySentence = " Order by Patient.Name";

    return selectSentence + whereSentence + orderBySentence;
}

QString LocalDatabaseStudyDAL::buildSqlGetPatientIDFromStudyInstanceUID(const QString &studyInstanceUID)
{
    QString selectSentence = QString ("Select PatientID "
                                      " From Study "
                                      "Where InstanceUID = '%1'")
                                    .arg(formatTextToValidSQLSyntax(studyInstanceUID));

    return selectSentence;
}

QString LocalDatabaseStudyDAL::buildSqlInsert(Study *newStudy, const QDate &lastAcessDate)
{
    QString insertSentence = QString ("Insert into Study   (InstanceUID, PatientID, ID, PatientAge, PatientWeigth, PatientHeigth, "
                                                           "Modalities, Date, Time, AccessionNumber, Description, "
                                                           "ReferringPhysicianName, LastAccessDate, RetrievedDate, "
                                                           "RetrievedTime , State) "
                                                   "values ('%1', %2, '%3', '%4', %5, %6, '%7', '%8', '%9', '%10', '%11', "
                                      "'%12', '%13', '%14', '%15', %16)")
                                    .arg(formatTextToValidSQLSyntax(newStudy->getInstanceUID()))
                                    .arg(newStudy->getParentPatient()->getDatabaseID())
                                    .arg(formatTextToValidSQLSyntax(newStudy->getID()))
                                    .arg(formatTextToValidSQLSyntax(newStudy->getPatientAge()))
                                    .arg(newStudy->getWeight())
                                    .arg(newStudy->getHeight())
                                    .arg(formatTextToValidSQLSyntax(newStudy->getModalitiesAsSingleString()))
                                    .arg(newStudy->getDate().toString("yyyyMMdd"))
                                    .arg(newStudy->getTime().toString("hhmmss"))
                                    .arg(formatTextToValidSQLSyntax(newStudy->getAccessionNumber()))
                                    .arg(formatTextToValidSQLSyntax(newStudy->getDescription()))
                                    .arg(formatTextToValidSQLSyntax(newStudy->getReferringPhysiciansName()))
                                    .arg(lastAcessDate.toString("yyyyMMdd"))
                                    .arg(newStudy->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(newStudy->getRetrievedTime().toString("hhmmss"))
                                    .arg(0);

    return insertSentence;
}

QString LocalDatabaseStudyDAL::buildSqlUpdate(Study *studyToUpdate, const QDate &lastAccessDate)
{
    QString updateSentence = QString ("Update Study set ID = '%1', "
                                                       "PatientAge = '%2',"
                                                       "PatientWeigth = %3, "
                                                       "PatientHeigth = %4, "
                                                       "Modalities = '%5', "
                                                       "Date = '%6', "
                                                       "Time = '%7', "
                                                       "AccessionNumber = '%8', "
                                                       "Description = '%9', "
                                                       "ReferringPhysicianName = '%10', "
                                                       "LastAccessDate = '%11', "
                                                       "RetrievedDate = '%12', "
                                                       "RetrievedTime = '%13', "
                                                       "State = %14 "
                                                "Where InstanceUid = '%15'")
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getID()))
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getPatientAge()))
                                    .arg(studyToUpdate->getWeight())
                                    .arg(studyToUpdate->getHeight())
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getModalitiesAsSingleString()))
                                    .arg(studyToUpdate->getDate().toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getTime().toString("hhmmss"))
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getAccessionNumber()))
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getDescription()))
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getReferringPhysiciansName()))
                                    .arg(lastAccessDate.toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(studyToUpdate->getRetrievedTime().toString("hhmmss"))
                                    .arg("0")
                                    .arg(formatTextToValidSQLSyntax(studyToUpdate->getInstanceUID()));

    return updateSentence;
}

// TODO: Si només acceptem com a paràmtre per eliminar de la DICOMMask l'studyInstanceUID el que s'hauria de fer és directament passar un QString amb
// StudyInstanceUID
QString LocalDatabaseStudyDAL::buildSqlDelete(const DicomMask &studyMaskToDelete)
{
    QString deleteSentence = "Delete From Study ";
    QString whereSentence;
    if (!studyMaskToDelete.getStudyInstanceUID().isEmpty())
    {
        whereSentence = QString(" Where InstanceUID = '%1'").arg(formatTextToValidSQLSyntax(studyMaskToDelete.getStudyInstanceUID()));
    }

    return deleteSentence + whereSentence;
}
}
