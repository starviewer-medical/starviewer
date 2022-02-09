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

#include "localdatabasemanager.h"

#include "databaseconnection.h"
#include "dicommask.h"
#include "directoryutilities.h"
#include "encapsulateddocument.h"
#include "harddiskinformation.h"
#include "image.h"
#include "inputoutputsettings.h"
#include "localdatabasedisplayshutterdal.h"
#include "localdatabaseencapsulateddocumentdal.h"
#include "localdatabaseimagedal.h"
#include "localdatabasepatientdal.h"
#include "localdatabaseseriesdal.h"
#include "localdatabasestudydal.h"
#include "localdatabaseutildal.h"
#include "localdatabasevoilutdal.h"
#include "patient.h"
#include "thumbnailcreator.h"

#include <QDir>

namespace udg {

namespace {

// Inserts to the database all the display shutters in the given image.
void insertDisplayShutters(DatabaseConnection &databaseConnection, const Image *image)
{
    LocalDatabaseDisplayShutterDAL displayShutterDAL(databaseConnection);
    const QList<DisplayShutter> &shutterList = image->getDisplayShutters();

    foreach (const DisplayShutter &shutter, shutterList)
    {
        if (!displayShutterDAL.insert(shutter, image))
        {
            throw displayShutterDAL.getLastError();
        }
    }
}

// Deletes from the database all the display shutters that match the given mask.
void deleteDisplayShutters(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseDisplayShutterDAL displayShutterDAL(databaseConnection);

    if (!displayShutterDAL.del(mask))
    {
        throw displayShutterDAL.getLastError();
    }
}

// Deletes from the database all the display shutters from the given image.
void deleteDisplayShutters(DatabaseConnection &databaseConnection, const Image *image)
{
    DicomMask mask;
    mask.setSOPInstanceUID(image->getSOPInstanceUID());
    mask.setImageNumber(QString::number(image->getFrameNumber()));
    deleteDisplayShutters(databaseConnection, mask);
}

// Inserts to the database all the VOI LUTs that are LUTs in the given image.
void insertVoiLuts(DatabaseConnection &databaseConnection, const Image *image)
{
    LocalDatabaseVoiLutDAL voiLutDAL(databaseConnection);

    for (int i = 0; i < image->getNumberOfVoiLuts(); i++)
    {
        const VoiLut &voiLut = image->getVoiLut(i);

        if (voiLut.isLut())
        {
            if (!voiLutDAL.insert(voiLut, image))
            {
                throw voiLutDAL.getLastError();
            }
        }
    }
}

// Deletes from the database all the VOI LUTs that match the given mask.
void deleteVoiLuts(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseVoiLutDAL voiLutDAL(databaseConnection);

    if (!voiLutDAL.del(mask))
    {
        throw voiLutDAL.getLastError();
    }
}

// Deletes from the database all the VOI LUTs from the given image.
void deleteVoiLuts(DatabaseConnection &databaseConnection, const Image *image)
{
    DicomMask mask;
    mask.setSOPInstanceUID(image->getSOPInstanceUID());
    mask.setImageNumber(QString::number(image->getFrameNumber()));
    deleteVoiLuts(databaseConnection, mask);
}

// Saves to the database the given image and its display shutters and VOI LUTs, doing an insert or an update as necessary.
void saveImage(DatabaseConnection &databaseConnection, const Image *image)
{
    LocalDatabaseImageDAL imageDAL(databaseConnection);
    bool update = imageDAL.exists(image->getSOPInstanceUID(), image->getFrameNumber());
    bool ok;

    if (update)
    {
        ok = imageDAL.update(image);
    }
    else
    {
        ok = imageDAL.insert(image);
    }

    if (!ok)
    {
        throw imageDAL.getLastError();
    }

    if (update)
    {
        // Delete existing shutters and VOI LUTs because they can't be updated. They are reinserted below.
        deleteDisplayShutters(databaseConnection, image);
        deleteVoiLuts(databaseConnection, image);
    }

    insertDisplayShutters(databaseConnection, image);
    insertVoiLuts(databaseConnection, image);
}

// Saves the images in the given list to the database, inserting or updating them as necessary.
void saveImages(DatabaseConnection &databaseConnection, const QList<Image*> &imageList, const QDate &currentDate, const QTime &currentTime)
{
    foreach (Image *image, imageList)
    {
        image->setRetrievedDate(currentDate);
        image->setRetrievedTime(currentTime);
        saveImage(databaseConnection, image);
    }
}

// Saves to the database the given encapsulated document, doing an insert or an update as necessary.
void saveEncapsulatedDocument(DatabaseConnection &databaseConnection, const EncapsulatedDocument *document)
{
    LocalDatabaseEncapsulatedDocumentDAL encapsulatedDocumentDAL(databaseConnection);
    bool ok;

    if (encapsulatedDocumentDAL.exists(document->getSopInstanceUid()))
    {
        ok = encapsulatedDocumentDAL.update(document);
    }
    else
    {
        ok = encapsulatedDocumentDAL.insert(document);
    }

    if (!ok)
    {
        throw encapsulatedDocumentDAL.getLastError();
    }
}

// Saves the encapsulated documents in the given list to the database, inserting or updating them as necessary.
void saveEncapsulatedDocuments(DatabaseConnection &databaseConnection, const QList<EncapsulatedDocument*> &documentList)
{
    foreach (EncapsulatedDocument *document, documentList)
    {
        saveEncapsulatedDocument(databaseConnection, document);
    }
}

// Saves the given series to the database, doing an insert or an update as necessary.
void saveSeries(DatabaseConnection &databaseConnection, const Series *series)
{
    LocalDatabaseSeriesDAL seriesDAL(databaseConnection);
    bool ok;

    if (seriesDAL.exists(series->getInstanceUID()))
    {
        ok = seriesDAL.update(series);
    }
    else
    {
        ok = seriesDAL.insert(series);
    }

    if (!ok)
    {
        throw seriesDAL.getLastError();
    }
}

// Saves the series in the given list to the database, inserting or updating them as necessary.
void saveSeries(DatabaseConnection &databaseConnection, const QList<Series*> &seriesList, const QDate &currentDate, const QTime &currentTime)
{
    foreach (Series *series, seriesList)
    {
        saveImages(databaseConnection, series->getImages(), currentDate, currentTime);
        saveEncapsulatedDocuments(databaseConnection, series->getEncapsulatedDocuments());

        series->setRetrievedDate(currentDate);
        series->setRetrievedTime(currentTime);
        saveSeries(databaseConnection, series);
    }
}

// Saves the patient of the given study to the database, doing an insert or an update as necessary.
void savePatientOfStudy(DatabaseConnection &databaseConnection, const Study *study)
{
    LocalDatabasePatientDAL patientDAL(databaseConnection);
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    qlonglong patientID = studyDAL.getPatientIDFromStudyInstanceUID(study->getInstanceUID());
    bool ok;

    if (patientID == -1)
    {
        // The patient doesn't exist, insert it
        ok = patientDAL.insert(study->getParentPatient());
    }
    else
    {
        // The patient already exists, update it
        study->getParentPatient()->setDatabaseID(patientID);

        ok = patientDAL.update(study->getParentPatient());
    }

    if (!ok)
    {
        throw patientDAL.getLastError();
    }
}

// Saves the given study to the database, doing an insert or an update as necessary.
void saveStudy(DatabaseConnection &databaseConnection, const Study *study)
{
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    bool ok;

    if (studyDAL.exists(study->getInstanceUID()))
    {
        ok = studyDAL.update(study, QDate::currentDate());
    }
    else
    {
        ok = studyDAL.insert(study, QDate::currentDate());
    }

    if (!ok)
    {
        throw studyDAL.getLastError();
    }
}

// Saves the studies in the given list to the database, inserting or updating them as necessary.
void saveStudies(DatabaseConnection &databaseConnection, const QList<Study*> &studyList, const QDate &currentDate, const QTime &currentTime)
{
    foreach (Study *study, studyList)
    {
        INFO_LOG(QString("Inserting to the database study %1 of modality %2 from %3 of patient %4").arg(study->getInstanceUID())
                 .arg(study->getModalitiesAsSingleString()).arg(study->getDateTime().toString("dd/MM/yyyy hh:mm:ss"))
                 .arg(study->getParentPatient()->getFullName()));

        saveSeries(databaseConnection, study->getSeries(), currentDate, currentTime);

        savePatientOfStudy(databaseConnection, study);

        study->setRetrievedDate(currentDate);
        study->setRetrievedTime(currentTime);
        saveStudy(databaseConnection, study);
    }
}

// Deletes the patient with the given id from the database.
void deletePatientFromDatabase(DatabaseConnection &databaseConnection, qlonglong patientID)
{
    LocalDatabasePatientDAL patientDAL(databaseConnection);

    if (!patientDAL.del(patientID))
    {
        throw patientDAL.getLastError();
    }
}

// Deletes from the database the patient and the study that match the given mask. (Note that each study has its own patient row.)
void deletePatientOfStudyFromDatabase(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    qlonglong patientID = studyDAL.getPatientIDFromStudyInstanceUID(mask.getStudyInstanceUID());

    if (studyDAL.getLastError().isValid())
    {
        throw studyDAL.getLastError();
    }
    else if (patientID == -1)
    {
        ERROR_LOG(QString("Can't find patient to be deleted from study %1.").arg(mask.getStudyInstanceUID()));
    }
    else
    {
        deletePatientFromDatabase(databaseConnection, patientID);
    }
}

// Deletes the study that matches the given mask from the database.
void deleteStudyFromDatabase(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseStudyDAL studyDAL(databaseConnection);

    if (!studyDAL.del(mask))
    {
        throw studyDAL.getLastError();
    }
}

// Deletes the series that match the given mask from the database.
void deleteSeriesFromDatabase(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseSeriesDAL seriesDAL(databaseConnection);

    if (!seriesDAL.del(mask))
    {
        throw seriesDAL.getLastError();
    }
}

// Deletes the images that match the given mask and its shutters and VOI LUTs from the database.
void deleteImagesFromDatabase(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    // Delete shutters first
    deleteDisplayShutters(databaseConnection, mask);

    // Then delete VOI LUTs
    deleteVoiLuts(databaseConnection, mask);

    // Finally delete the image
    LocalDatabaseImageDAL imageDAL(databaseConnection);

    if (!imageDAL.del(mask))
    {
        throw imageDAL.getLastError();
    }
}

// Deletes the encapsulated documents that match the given mask from the database.
void deleteEncapsulatedDocumentsFromDatabase(DatabaseConnection &databaseConnection, const DicomMask &mask)
{
    LocalDatabaseEncapsulatedDocumentDAL encapsulatedDocumentDAL(databaseConnection);

    if (!encapsulatedDocumentDAL.del(mask))
    {
        throw encapsulatedDocumentDAL.getLastError();
    }
}

// Deletes from the database the series with the given SeriesInstanceUID from the study with the given StudyInstanceUID.
// If SeriesInstanceUID is empty, deletes all series from the study with the given StudyInstanceUID, but not the study itself.
void deleteSeriesStructureFromDatabase(DatabaseConnection &databaseConnection, const QString &studyInstanceUID, const QString &seriesIntanceUID)
{
    DicomMask mask;
    mask.setStudyInstanceUID(studyInstanceUID);
    mask.setSeriesInstanceUID(seriesIntanceUID);
    deleteSeriesFromDatabase(databaseConnection, mask);
    deleteImagesFromDatabase(databaseConnection, mask);
    deleteEncapsulatedDocumentsFromDatabase(databaseConnection, mask);
}

// Deletes from the database the patient, study, series and images from the study with the given UID.
void deleteStudyStructureFromDatabase(DatabaseConnection &databaseConnection, const QString &studyInstanceUID)
{
    DicomMask mask;
    mask.setStudyInstanceUID(studyInstanceUID);
    deletePatientOfStudyFromDatabase(databaseConnection, mask);
    deleteStudyFromDatabase(databaseConnection, mask);
    deleteSeriesStructureFromDatabase(databaseConnection, studyInstanceUID, QString());
}

// Returns how many series in the database match the given mask (only StudyUID and SeriesUID are considered). Returns -1 in case of error.
int countSeries(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseSeriesDAL seriesDAL(databaseConnection);
    int count = seriesDAL.count(mask);

    if (count == -1)
    {
        throw seriesDAL.getLastError();
    }

    return count;
}

// Deletes all the studies from the given patient from the disk.
void deleteRetrievedObjects(const Patient *patient)
{
    DirectoryUtilities directoryUtilities;

    foreach (Study *study, patient->getStudies())
    {
        directoryUtilities.deleteDirectory(LocalDatabaseManager::getCachePath() + study->getInstanceUID(), true);
    }
}

// Deletes the given series from the disk.
void deleteRetrievedObjects(const Series *series)
{
    QString studyPath = LocalDatabaseManager::getCachePath() + series->getParentStudy()->getInstanceUID();
    QString seriesDirectory = studyPath + QDir::separator() + series->getInstanceUID();

    DirectoryUtilities directoryUtilities;
    directoryUtilities.deleteDirectory(seriesDirectory, true);

    if (directoryUtilities.isDirectoryEmpty(studyPath))
    {
        directoryUtilities.deleteDirectory(studyPath, true);
    }
}

// Returns the full path of the thumbnail file for the given series and study instance UID.
QString getSeriesThumbnailPath(const QString &studyInstanceUID, const Series *series)
{
    return LocalDatabaseManager::getStudyPath(studyInstanceUID) + "/" + series->getInstanceUID() + "/thumbnail.png";
}

// Creates and saves a thumbnail for the given series in the directory of the series' images.
void createSeriesThumbnail(const Series *series)
{
    QString thumbnailFilePath = getSeriesThumbnailPath(series->getParentStudy()->getInstanceUID(), series);

    // Create thumbnail only if it doesn't already exist
    if (!QFileInfo(thumbnailFilePath).exists())
    {
        ThumbnailCreator().getThumbnail(series).save(thumbnailFilePath, "PNG");
    }
}

// Creates and saves a thumbnail for each series in the given study.
void createStudyThumbnails(const Study *study)
{
    foreach (Series *series, study->getSeries())
    {
        createSeriesThumbnail(series);
    }
}

// Loads and sets the thumbnails of the given series from the study with the given UID.
void loadSeriesThumbnails(const QString &studyInstanceUID, const QList<Series*> &seriesList)
{
    foreach (Series *series, seriesList)
    {
        QString thumbnailPath = getSeriesThumbnailPath(studyInstanceUID, series);
        QFileInfo thumbnailFile(thumbnailPath);

        if (thumbnailFile.exists())
        {
            series->setThumbnail(QPixmap(thumbnailPath));
        }
    }
}

}

// Initialize static member
QDate LocalDatabaseManager::LastAccessDateSelectedStudies;

QString LocalDatabaseManager::getDatabaseFilePath()
{
    return QDir::toNativeSeparators(Settings().getValue(InputOutputSettings::DatabaseAbsoluteFilePath).toString());
}

QString LocalDatabaseManager::getCachePath()
{
    return QDir::toNativeSeparators(Settings().getValue(InputOutputSettings::CachePath).toString());
}

QString LocalDatabaseManager::getStudyPath(const QString &studyInstanceUID)
{
    return getCachePath() + studyInstanceUID;
}

LocalDatabaseManager::LocalDatabaseManager()
{
    Settings settings;

    // Check if we should delete old studies and set the appropriate date to compare
    if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        if (!LastAccessDateSelectedStudies.isValid())
        {
            LastAccessDateSelectedStudies = QDate::currentDate().addDays(-settings.getValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy).toInt());
        }
    }
}

void LocalDatabaseManager::save(Series *series)
{
    if (!series)
    {
        ERROR_LOG("Can't save a null series");
        m_lastError = PatientInconsistent;
        return;
    }

    try
    {
        DatabaseConnection databaseConnection;
        databaseConnection.beginTransaction();

        Study *study = series->getParentStudy();
        savePatientOfStudy(databaseConnection, study);

        QDate currentDate = QDate::currentDate();
        QTime currentTime = QTime::currentTime();
        study->setRetrievedDate(currentDate);
        study->setRetrievedTime(currentTime);
        saveStudy(databaseConnection, study);

        QList<Series*> seriesList;
        seriesList.append(series);
        saveSeries(databaseConnection, seriesList, currentDate, currentTime);

        databaseConnection.commitTransaction();

        createSeriesThumbnail(series);

        m_lastError = Ok;
    }
    catch (const QSqlError &error)
    {
        deleteRetrievedObjects(series);
        setLastError(error);
    }
}

QList<Patient*> LocalDatabaseManager::queryPatients(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabasePatientDAL patientDAL(databaseConnection);
    QList<Patient*> patientList = patientDAL.query(mask);
    setLastError(patientDAL.getLastError());
    return patientList;
}

QList<Patient*> LocalDatabaseManager::queryPatientsAndStudies(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    QList<Patient*> patientList = studyDAL.queryPatientStudy(mask, QDate(), LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());
    return patientList;
}

QList<Study*> LocalDatabaseManager::queryStudies(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    QList<Study*> studyList = studyDAL.query(mask, QDate(), LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());
    return studyList;
}

QList<Series*> LocalDatabaseManager::querySeries(DicomMask mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseSeriesDAL seriesDAL(databaseConnection);
    QList<Series*> seriesList = seriesDAL.query(mask);

    if (seriesDAL.getLastError().isValid())
    {
        setLastError(seriesDAL.getLastError());
        return seriesList;
    }

    // Fill number of images and encapsulated documents of each series
    LocalDatabaseImageDAL imageDAL(databaseConnection);
    LocalDatabaseEncapsulatedDocumentDAL encapsulatedDocumentDAL(databaseConnection);

    foreach (Series *series, seriesList)
    {
        mask.setSeriesInstanceUID(series->getInstanceUID());

        series->setNumberOfImages(imageDAL.count(mask));
        if (imageDAL.getLastError().isValid())
        {
            setLastError(imageDAL.getLastError());
            return seriesList;
        }

        series->setNumberOfEncapsulatedDocuments(encapsulatedDocumentDAL.count(mask));
        if (encapsulatedDocumentDAL.getLastError().isValid())
        {
            setLastError(encapsulatedDocumentDAL.getLastError());
            return seriesList;
        }
    }

    loadSeriesThumbnails(mask.getStudyInstanceUID(), seriesList);

    m_lastError = Ok;

    return seriesList;
}

QList<Image*> LocalDatabaseManager::queryImages(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseImageDAL imageDAL(databaseConnection);
    QList<Image*> imageList = imageDAL.query(mask);
    setLastError(imageDAL.getLastError());
    return imageList;
}

QList<EncapsulatedDocument*> LocalDatabaseManager::queryEncapsulatedDocuments(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseEncapsulatedDocumentDAL encapsulatedDocumentDAL(databaseConnection);
    QList<EncapsulatedDocument*> documentList = encapsulatedDocumentDAL.query(mask);
    setLastError(encapsulatedDocumentDAL.getLastError());
    return documentList;
}

// TODO Possible memory leaks in this method: patients, and images
Patient* LocalDatabaseManager::retrieve(const DicomMask &mask)
{
    DatabaseConnection databaseConnection;

    // Get patient and studies
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    QList<Patient*> patientList = studyDAL.queryPatientStudy(mask, QDate(), LastAccessDateSelectedStudies);

    if (patientList.isEmpty())
    {
        setLastError(studyDAL.getLastError());
        return 0;
    }

    Patient *patient = patientList.first();

    // Get series
    LocalDatabaseSeriesDAL seriesDAL(databaseConnection);
    QList<Series*> seriesList = seriesDAL.query(mask);

    if (seriesDAL.getLastError().isValid())
    {
        setLastError(seriesDAL.getLastError());
        return 0;
    }

    // Get images and encapsulated documents
    LocalDatabaseImageDAL imageDAL(databaseConnection);
    LocalDatabaseEncapsulatedDocumentDAL encapsulatedDocumentDAL(databaseConnection);
    DicomMask imagesMask;
    imagesMask.setStudyInstanceUID(mask.getStudyInstanceUID());

    foreach (Series *series, seriesList)
    {
        imagesMask.setSeriesInstanceUID(series->getInstanceUID());

        QList<Image*> imageList = imageDAL.query(imagesMask);

        if (imageDAL.getLastError().isValid())
        {
            return 0;
        }

        foreach (Image *image, imageList)
        {
            series->addImage(image);
        }

        QList<EncapsulatedDocument*> documentList = encapsulatedDocumentDAL.query(imagesMask);

        if (encapsulatedDocumentDAL.getLastError().isValid())
        {
            return 0;
        }

        foreach (EncapsulatedDocument *document, documentList)
        {
            series->addEncapsulatedDocument(document);
        }

        patient->getStudy(mask.getStudyInstanceUID())->addSeries(series);
    }

    // Update last access date in study
    Study *study = patient->getStudy(mask.getStudyInstanceUID());
    studyDAL.update(study, QDate::currentDate());
    setLastError(studyDAL.getLastError());

    return patient;
}

bool LocalDatabaseManager::studyExists(const QString &studyInstanceUID)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    bool exists = studyDAL.exists(studyInstanceUID);
    setLastError(studyDAL.getLastError());
    return exists;
}

void LocalDatabaseManager::deleteStudy(const QString &studyInstanceUID)
{
    if (studyInstanceUID.isEmpty())
    {
        return;
    }

    INFO_LOG(QString("Deleting study %1 from local database.").arg(studyInstanceUID));

    try
    {
        DatabaseConnection databaseConnection;
        databaseConnection.beginTransaction();
        deleteStudyStructureFromDatabase(databaseConnection, studyInstanceUID);
        databaseConnection.commitTransaction();

        deleteStudyFromHardDisk(studyInstanceUID);

        m_lastError = Ok;
    }
    catch (const QSqlError &error)
    {
        setLastError(error);
    }
}

void LocalDatabaseManager::deleteSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    if (studyInstanceUID.isEmpty() || seriesInstanceUID.isEmpty())
    {
        return;
    }

    INFO_LOG(QString("Deleting series %1 from study %2 from local database.").arg(seriesInstanceUID).arg(studyInstanceUID));

    DicomMask mask;
    mask.setStudyInstanceUID(studyInstanceUID);

    try
    {
        if (countSeries(mask) == 1)
        {
            INFO_LOG("The study contains only this series, so the study will be also deleted.");
            deleteStudy(studyInstanceUID);
        }
        else
        {
            DatabaseConnection databaseConnection;
            databaseConnection.beginTransaction();
            deleteSeriesStructureFromDatabase(databaseConnection, studyInstanceUID, seriesInstanceUID);
            databaseConnection.commitTransaction();

            deleteSeriesFromHardDisk(studyInstanceUID, seriesInstanceUID);

            m_lastError = Ok;
        }
    }
    catch (const QSqlError &error)
    {
        setLastError(error);
    }
}

void LocalDatabaseManager::deleteOldStudies()
{
    m_lastError = Ok;

    // If the setting is false don't do anything
    if (!Settings().getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        return;
    }

    DatabaseConnection databaseConnection;
    LocalDatabaseStudyDAL studyDAL(databaseConnection);

    INFO_LOG(QString("Deleting studies that haven't been open since %1").arg(LastAccessDateSelectedStudies.addDays(-1).toString("dd/MM/yyyy")));

    QList<Study*> studiesToDelete = studyDAL.query(DicomMask(), LastAccessDateSelectedStudies);

    if (studyDAL.getLastError().isValid())
    {
        setLastError(studyDAL.getLastError());
        return;
    }

    if (studiesToDelete.isEmpty())
    {
        INFO_LOG("No studies to delete.");
    }

    foreach (Study *study, studiesToDelete)
    {
        deleteStudy(study->getInstanceUID());
        delete study;
    }
}

void LocalDatabaseManager::compact()
{
    DatabaseConnection databaseConnection;
    LocalDatabaseUtilDAL utilDAL(databaseConnection);
    utilDAL.compact();
    setLastError(utilDAL.getLastError());
}

int LocalDatabaseManager::getDatabaseRevision()
{
    DatabaseConnection databaseConnection;
    LocalDatabaseUtilDAL utilDAL(databaseConnection);
    int databaseRevision = utilDAL.getDatabaseRevision();
    setLastError(utilDAL.getLastError());
    return databaseRevision;
}

void LocalDatabaseManager::setDatabaseRevision(int databaseRevision)
{
    DatabaseConnection databaseConnection;
    LocalDatabaseUtilDAL utilDAL(databaseConnection);
    utilDAL.updateDatabaseRevision(databaseRevision);
    setLastError(utilDAL.getLastError());
}

bool LocalDatabaseManager::thereIsAvailableSpaceOnHardDisk()
{
    m_lastError = Ok;

    HardDiskInformation hardDiskInformation;
    quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(getCachePath());
    Settings settings;
    quint64 minimumSpaceRequired = quint64(settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toULongLong() * 1024);

    if (freeSpaceInHardDisk >= minimumSpaceRequired)
    {
        return true;
    }

    INFO_LOG(QString("Not enough free space in disk to download studies. Free space: %1 MiB. Required: %2 MiB.").arg(freeSpaceInHardDisk)
                                                                                                                .arg(minimumSpaceRequired));

    // Check if we should try to free up space. If not, return false
    if (!settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria).toBool())
    {
        return false;
    }

    INFO_LOG("Old studies will be deleted to free up space.");

    // Delete studies until we have the needed space (minimumSpaceRequired - freeSpaceInHardDisk)
    // plus a constant quantity to ensure that we don't have to free up space too often
    quint64 additionalMegabytesToErase = settings.getValue(InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull).toULongLong() * 1024;
    quint64 megabytesToFreeUp = minimumSpaceRequired - freeSpaceInHardDisk + additionalMegabytesToErase;
    freeUpSpaceDeletingStudies(megabytesToFreeUp);

    if (getLastError() != Ok)
    {
        ERROR_LOG("Error while freeing up space.");
        return false;
    }

    // Finally check free space again
    freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(getCachePath());

    if (freeSpaceInHardDisk >= minimumSpaceRequired)
    {
        return true;
    }
    else
    {
        INFO_LOG(QString("Not enough free space in disk to download studies. Free space: %1 MiB. Required: %2 MiB.").arg(freeSpaceInHardDisk)
                                                                                                                    .arg(minimumSpaceRequired));
        return false;
    }
}

LocalDatabaseManager::LastError LocalDatabaseManager::getLastError() const
{
    return m_lastError;
}

void LocalDatabaseManager::save(Patient *patient)
{
    if (!patient)
    {
        ERROR_LOG("Can't save a null patient");
        m_lastError = PatientInconsistent;
        return;
    }

    if (patient->getNumberOfStudies() == 0)
    {
        ERROR_LOG("Won't save a patient without studies");
        m_lastError = PatientInconsistent;
        return;
    }

    try {
        DatabaseConnection databaseConnection;
        databaseConnection.beginTransaction();

        saveStudies(databaseConnection, patient->getStudies(), QDate::currentDate(), QTime::currentTime());

        databaseConnection.commitTransaction();

        foreach (Study *study, patient->getStudies())
        {
            createStudyThumbnails(study);
        }

        m_lastError = Ok;
    }
    catch (const QSqlError &error)
    {
        deleteRetrievedObjects(patient);
        setLastError(error);
    }
}

void LocalDatabaseManager::freeUpSpaceDeletingStudies(quint64 megabytesToFreeUp)
{
    QList<Study*> studyList = getAllStudiesOrderedByLastAccessDate();

    if (getLastError() != Ok)
    {
        return;
    }

    quint64 megabytesErased = 0;

    while (!studyList.isEmpty() && megabytesErased < megabytesToFreeUp)
    {
        Study *study = studyList.takeFirst();
        emit studyWillBeDeleted(study->getInstanceUID());
        megabytesErased += HardDiskInformation::getDirectorySizeInBytes(getCachePath() + study->getInstanceUID()) / 1024 / 1024;
        deleteStudy(study->getInstanceUID());
        delete study;

        if (getLastError() != Ok)
        {
            break;
        }
    }

    // Delete remaining studies (in case of error)
    foreach (Study *study, studyList)
    {
        delete study;
    }
}

QList<Study*> LocalDatabaseManager::getAllStudiesOrderedByLastAccessDate()
{
    DatabaseConnection databaseConnection;
    LocalDatabaseStudyDAL studyDAL(databaseConnection);
    QList<Study*> studyList = studyDAL.queryOrderByLastAccessDate(DicomMask(), QDate(), LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());
    return studyList;
}

void LocalDatabaseManager::deleteStudyFromHardDisk(const QString &studyInstanceUID)
{
    if (DirectoryUtilities().deleteDirectory(getStudyPath(studyInstanceUID), true))
    {
        m_lastError = Ok;
    }
    else
    {
        m_lastError = DeletingFilesError;
    }
}

void LocalDatabaseManager::deleteSeriesFromHardDisk(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    if (DirectoryUtilities().deleteDirectory(getStudyPath(studyInstanceUID) + QDir::separator() + seriesInstanceUID, true))
    {
        m_lastError = Ok;
    }
    else
    {
        m_lastError = DeletingFilesError;
    }
}

void LocalDatabaseManager::setLastError(const QSqlError &error)
{
    int sqliteLastError = error.nativeErrorCode().toInt();

    if (error.isValid() && error.nativeErrorCode().isEmpty())
    {
        m_lastError = DatabaseError;
    }
    else if (sqliteLastError == DatabaseConnection::SqliteOk)
    {
        m_lastError = Ok;
    }
    else if (sqliteLastError == DatabaseConnection::SqliteError)
    {
        m_lastError = SyntaxErrorSQL;
    }
    else if (sqliteLastError == DatabaseConnection::SqliteLocked || sqliteLastError == DatabaseConnection::SqliteBusy)
    {
        m_lastError = DatabaseLocked;
    }
    else if (sqliteLastError == DatabaseConnection::SqliteCorrupt || sqliteLastError == DatabaseConnection::SqliteEmpty ||
             sqliteLastError == DatabaseConnection::SqliteSchema || sqliteLastError == DatabaseConnection::SqliteMismatch ||
             sqliteLastError == DatabaseConnection::SqliteNotADb)
    {
        m_lastError = DatabaseCorrupted;
    }
    else
    {
        m_lastError = DatabaseError;
    }

    if (sqliteLastError != DatabaseConnection::SqliteOk)
    {
        ERROR_LOG(QString("Database error code: %1").arg(sqliteLastError));
    }
}

}
