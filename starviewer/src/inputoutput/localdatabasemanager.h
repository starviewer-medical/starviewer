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

#ifndef UDGLOCALDATABASEMANAGER_H
#define UDGLOCALDATABASEMANAGER_H

#include <QObject>

class QSqlError;

namespace udg {

class DicomMask;
class EncapsulatedDocument;
class Image;
class Patient;
class Series;
class Study;

/**
 * @brief The LocalDatabaseManager class centralizes all the communications with the local database.
 */
class LocalDatabaseManager : public QObject {

Q_OBJECT

public:
    /// Es defineix els tipus d'error que podem tenir, el DatabaseError indica quan és error de Sqlite
    enum LastError { Ok, DatabaseLocked, DatabaseCorrupted, SyntaxErrorSQL, DeletingFilesError, DatabaseError, PatientInconsistent };

    /// Returns the path of the database file.
    static QString getDatabaseFilePath();
    /// Returns the path of the cache directory.
    static QString getCachePath();
    /// Returns the directory where the study with the given UID should be saved.
    static QString getStudyPath(const QString &studyInstanceUID);

    LocalDatabaseManager();

    /// Saves the given series to the database, inserting or updating also the associated patient, study and images.
    void save(Series *series);

    /// Returns patients that match the given mask (only PatientID is considered). Returns only patients, without studies, series and images.
    QList<Patient*> queryPatients(const DicomMask &mask);
    /// Returns patients that contain studies that match the given mask (PatientID, PatientName, StudyDate and StudyInstanceUID are considered).
    /// Returns the patients with the studies but not series and images.
    QList<Patient*> queryPatientsAndStudies(const DicomMask &mask);
    /// Returns studies that match the given mask (only StudyInstanceUID is considered). Returns only studies, without patients, series and images.
    QList<Study*> queryStudies(const DicomMask &mask);
    /// Returns series that match the given mask (only StudyInstanceUID and SeriesInstanceUID are considered).
    /// Returns only series, without patients, studies and images.
    QList<Series*> querySeries(DicomMask mask);
    /// Returns images that match the given mask (only StudyInstanceUID, SeriesInstanceUID and SOPInstanceUID are considered).
    QList<Image*> queryImages(const DicomMask &mask);
    /// Returns encapsulated documents that match the given mask (only StudyInstanceUID, SeriesInstanceUID and SOPInstanceUID are considered).
    QList<EncapsulatedDocument*> queryEncapsulatedDocuments(const DicomMask &mask);

    /// Returns a patient structure, including studies, series and images, that matches the given mask. StudyInstanceUID, SeriesInstanceUID and SOPInstanceUID
    /// are considered. If no result is found, returns null.
    Patient* retrieve(const DicomMask &mask);

    /// Returns true if a study with the given UID exists in the database and false otherwise.
    bool studyExists(const QString &studyInstanceUID);

    /// Deletes the study with the given UID from the database and the disk.
    void deleteStudy(const QString &studyInstanceUID);
    /// Deletes the series with the given SeriesInstanceUID from the study with the given StudyInstanceUID. If the study becomes empty, it's also deleted.
    void deleteSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID);

    /// Deletes studies that have not been open in a number of days specified in settings,
    /// as long as the setting to delete old studies is set to true, otherwise it does nothing.
    void deleteOldStudies();

    /// Compacts the database.
    void compact();

    /// Returns the current database revision, or -1 in case of error.
    int getDatabaseRevision();
    /// Updates the database revision to the given one.
    void setDatabaseRevision(int databaseRevision);

    /// Checks if there is enough space on the hard disk to download studies, according to settings.
    /// If there's not enough space it will (if enabled in settings) delete old studies to free up space.
    /// Returns true if at the end there's enough space and false otherwise.
    bool thereIsAvailableSpaceOnHardDisk();

    /// Saves a setting to know that a study with the given UID is being retrieved. This is saved in order to delete a half-downloaded study in case the
    /// application crashes in the middle of a download. Note: only one study at a time can be marked as being retrieved.
    /// TODO should this really be here?
    void setStudyBeingRetrieved(const QString &studyInstanceUID);
    /// Clears the setting set in the above method to indicated that no study is being retrieved.
    /// TODO should this really be here?
    void setNoStudyBeingRetrieved();
    /// Return true if a study is being retrieved.
    /// TODO should this really be here?
    bool isAStudyBeingRetrieved() const;
    /// If there is a study marked as being retrieved, this method will delete its images and leave the database in a consistent state. This method is intended
    /// to delete a half-downloaded study in case the application crashes in the middle of a download. It should be called at the start of the application.
    /// TODO should this really be here?
    void deleteStudyBeingRetrieved();

    /// Returns the last error encountered.
    LastError getLastError() const;

public slots:
    /// Saves the given patient to the database, inserting or updating also the associated studies, series and images.
    void save(Patient *patient);

signals:
    /// This signal is emitted before a study is deleted from the local database and the disk to free up space.
    void studyWillBeDeleted(const QString &studyInstanceUID);

private:
    /// Deletes old studies until the given number of megabytes have been deleted.
    void freeUpSpaceDeletingStudies(quint64 megbytesToFreeUp);

    /// Returns all the studies sorted by last access date.
    QList<Study*> getAllStudiesOrderedByLastAccessDate();

    /// Deletes the study with the given UID from the disk.
    void deleteStudyFromHardDisk(const QString &studyInstanceUID);
    /// Deletes the series with the given UID from the study with the given UID from the disk.
    void deleteSeriesFromHardDisk(const QString &studyInstanceUID, const QString &seriesInstanceUID);

    /// Sets the last error according to the given SQL error.
    void setLastError(const QSqlError &error);

private:
    /// Studies older than this date will be considered old. It's static and initialized the first time this class is instantiated in order to use the same
    /// reference date for the whole lifetime of the application process.
    static QDate LastAccessDateSelectedStudies;

    /// Last error encountered.
    LastError m_lastError;

};

}

#endif
