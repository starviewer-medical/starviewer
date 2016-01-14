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

#ifndef UDGLOCALDATABASESTUDY_H
#define UDGLOCALDATABASESTUDY_H

#include "localdatabasebasedal.h"

#include <QDate>

namespace udg {

class DicomMask;
class Patient;
class Study;

/**
 * @brief The LocalDatabaseStudyDAL class is the Data Access Layer class for studies.
 */
class LocalDatabaseStudyDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabaseStudyDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given study and sets the given date as the last access date. Returns true if successful and false otherwise.
    bool insert(const Study *study, const QDate &lastAccessDate);

    /// Updates in the database the given study and sets the given date as the last access date. Returns true if successful and false otherwise.
    bool update(const Study *study, const QDate &lastAccessDate);

    /// Deletes from the database the studies that match the given mask (only StudyUID is considered). Returns true if successful and false otherwise.
    bool del(const DicomMask &mask);

    /// Retrieves from the database the studies that match the given mask (only StudyUID is considered) and whose last access date is in the range
    /// (\a accessedBefore, \a accessedAfter], and returns them in a list.
    QList<Study*> query(const DicomMask &mask, const QDate &accessedBefore = QDate(), const QDate &accessedAfter = QDate());

    /// Retrieves from the database the studies that match the given mask (only StudyUID is considered) and whose last access date is in the range
    /// (\a accessedBefore, \a accessedAfter], and returns them in a list sorted by last access date in ascending order.
    QList<Study*> queryOrderByLastAccessDate(const DicomMask &mask, const QDate &accessedBefore = QDate(), const QDate &accessedAfter = QDate());

    /// Retrieves from the database the patients that contain studies that match the given mask (patient id, patient name, study date, study instance UID and
    /// modalities are considered) and whose last access date is in the range (\a accessedBefore, \a accessedAfter], and returns the patients in a list.
    /// For each matching study a Patient object with one Study object will be returned, so there may be multiple Patient objects representing the same patient.
    QList<Patient*> queryPatientStudy(const DicomMask &mask, const QDate &accessedBefore = QDate(), const QDate &accessedAfter = QDate());

    /// Returns true if there's a study with the given UID in the database, and false otherwise.
    bool exists(const QString &studyInstanceUID);

    /// Returns the PatientID field (referencing the database ID of a patient) from the study with the given StudyInstanceUID.
    /// If there is no such study, returns -1.
    qlonglong getPatientIDFromStudyInstanceUID(const QString &studyInstanceUID);

private:
    /// Creates and returns a study with the information of the current row of the given query.
    static Study* getStudy(const QSqlQuery &query);

    /// Creates and returns a patient with the information of the current row of the given query.
    static Patient* getPatient(const QSqlQuery &query);

    /// Retrieves from the database the studies that match the given mask (only StudyUID is considered) and whose last access date is in the range
    /// (\a accessedBefore, \a accessedAfter], and returns them in a list sorted according to \a orderBy.
    QList<Study*> query(const DicomMask &mask, const QDate &accessedBefore, const QDate &accessedAfter, const QString &orderBy);

};

}

#endif
