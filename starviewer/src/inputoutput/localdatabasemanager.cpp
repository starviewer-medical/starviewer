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

#include <QDir>
#include <QSqlError>

#include "patient.h"
#include "study.h"
#include "localdatabasedisplayshutterdal.h"
#include "localdatabaseimagedal.h"
#include "localdatabaseseriesdal.h"
#include "localdatabasestudydal.h"
#include "localdatabasepatientdal.h"
#include "localdatabaseutildal.h"
#include "localdatabasevoilutdal.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "logging.h"
#include "directoryutilities.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"
#include "harddiskinformation.h"
#include "thumbnailcreator.h"

#define SQLITE_OK           0   /* Successful result */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */

namespace udg {

namespace {

/// Inserts in the database all the VOI LUTs that are LUTs in the given image.
bool insertVoiLuts(DatabaseConnection *dbConnect, Image *image)
{
    LocalDatabaseVoiLutDAL voiLutDal(dbConnect);

    for (int i = 0; i < image->getNumberOfVoiLuts(); i++)
    {
        const VoiLut &voiLut = image->getVoiLut(i);

        if (voiLut.isLut())
        {
            if (!voiLutDal.insert(voiLut, image))
            {
                return false;
            }
        }
    }

    return true;
}

/// Deletes from the database all the VOI LUTs that match the given mask.
bool deleteVoiLuts(DatabaseConnection *dbConnect, const DicomMask &mask)
{
    LocalDatabaseVoiLutDAL voiLutDal(dbConnect);
    return voiLutDal.del(mask);
}

/// Deletes from the database all the VOI LUTs from the given image.
bool deleteVoiLuts(DatabaseConnection *dbConnect, Image *image)
{
    DicomMask mask;
    mask.setSOPInstanceUID(image->getSOPInstanceUID());
    mask.setImageNumber(QString::number(image->getFrameNumber()));
    return deleteVoiLuts(dbConnect, mask);
}

}

// Nom de la llista de Settings que guardarà els estudis que tenim en aquell moment descarregant
QDate LocalDatabaseManager::LastAccessDateSelectedStudies;

LocalDatabaseManager::LocalDatabaseManager()
{
    Settings settings;

    /// Comprovem si està activada la opció de configuració que indica si s'ha d'esborrar automàticament els estudis vells,
    /// sinó està activada, construim una data nul·la perquè no s'ha de tenir en compte alhora de fer las cerques
    if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        if (!LocalDatabaseManager::LastAccessDateSelectedStudies.isValid())
        {
            LocalDatabaseManager::LastAccessDateSelectedStudies =
                QDate::currentDate().addDays(-settings.getValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy).toInt());
        }
    }
    else
    {
        LocalDatabaseManager::LastAccessDateSelectedStudies = QDate();
    }
}

void LocalDatabaseManager::save(Patient *newPatient)
{
    if (newPatient == NULL)
    {
        ERROR_LOG("No es pot inserir a la base de dades l'estudi perque el patient es null");
        m_lastError = PatientInconsistent;
    }
    else
    {
        DatabaseConnection dbConnect;
        dbConnect.open();
        bool status = true;

        dbConnect.beginTransaction();
        /// Guardem primer els estudis
        if (newPatient->getStudies().count() > 0)
        {
            status = saveStudies(&dbConnect, newPatient->getStudies(), QDate::currentDate(), QTime::currentTime());

            if (!status)
            {
                deleteRetrievedObjects(newPatient);
                setLastError(dbConnect.getLastError());
                return;
            }
        }
        else
        {
            ERROR_LOG("El pacient que s'intenta inserir a la base de dades, no te cap estudi.");
        }

        if (!status)
        {
            deleteRetrievedObjects(newPatient);
        }
        else
        {
            dbConnect.commitTransaction();
        }

        foreach (Study *study, newPatient->getStudies())
        {
            createStudyThumbnails(study);
        }

        setLastError(dbConnect.getLastError());
    }
}

void LocalDatabaseManager::save(Series *seriesToSave)
{
    if (seriesToSave == NULL)
    {
        ERROR_LOG("No es pot inserir a la base de dades la serie perque te valor null");
        m_lastError = PatientInconsistent;
    }
    else
    {
        QDate currentDate = QDate::currentDate();
        QTime currentTime = QTime::currentTime();
        DatabaseConnection dbConnect;
        dbConnect.open();

        dbConnect.beginTransaction();
        bool status = savePatientOfStudy(&dbConnect, seriesToSave->getParentStudy());

        if (!status)
        {
            deleteRetrievedObjects(seriesToSave);
            setLastError(dbConnect.getLastError());
            return;
        }

        Study *studyParent = seriesToSave->getParentStudy();
        studyParent->setRetrievedDate(currentDate);
        studyParent->setRetrievedTime(currentTime);

        status = saveStudy(&dbConnect, studyParent);

        if (!status)
        {
            deleteRetrievedObjects(seriesToSave);
            setLastError(dbConnect.getLastError());
            return;
        }

        QList<Series*> seriesList;
        seriesList.append(seriesToSave);

        status = saveSeries(&dbConnect, seriesList, currentDate, currentTime);

        if (!status)
        {
            deleteRetrievedObjects(seriesToSave);
            setLastError(dbConnect.getLastError());
            return;
        }
        else
        {
            dbConnect.commitTransaction();
        }

        createSeriesThumbnail(seriesToSave);

        setLastError(dbConnect.getLastError());
    }
}

QList<Patient*> LocalDatabaseManager::queryPatient(const DicomMask &patientMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabasePatientDAL patientDAL(&dbConnect);
    QList<Patient*> queryResult;

    queryResult = patientDAL.query(patientMaskToQuery);
    setLastError(patientDAL.getLastError());

    return queryResult;
}

QList<Patient*> LocalDatabaseManager::queryPatientStudy(const DicomMask &patientStudyMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseStudyDAL studyDAL(&dbConnect);
    QList<Patient*> queryResult;

    queryResult = studyDAL.queryPatientStudy(patientStudyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    return queryResult;
}

QList<Study*> LocalDatabaseManager::queryStudy(const DicomMask &studyMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseStudyDAL studyDAL(&dbConnect);
    QList<Study*> queryResult;

    queryResult = studyDAL.query(studyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    return queryResult;
}

QList<Study*> LocalDatabaseManager::queryStudyOrderByLastAccessDate(const DicomMask &studyMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseStudyDAL studyDAL(&dbConnect);
    QList<Study*> queryResult;

    queryResult = studyDAL.queryOrderByLastAccessDate(studyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    return queryResult;
}

QList<Series*> LocalDatabaseManager::querySeries(const DicomMask &seriesMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseSeriesDAL seriesDAL(&dbConnect);
    LocalDatabaseImageDAL imageDAL(&dbConnect);
    QList<Series*> queryResult;
    DicomMask maskToCountNumberOfImage = seriesMaskToQuery;

    queryResult = seriesDAL.query(seriesMaskToQuery);

    if (seriesDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
    {
        setLastError(seriesDAL.getLastError());
        return queryResult;
    }

    // Consultem el número d'imatges de la Sèrie
    foreach (Series *series, queryResult)
    {
        maskToCountNumberOfImage.setSeriesInstanceUID(series->getInstanceUID());
        series->setNumberOfImages(imageDAL.count(maskToCountNumberOfImage));

        if (imageDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
        {
            break;
        }
    }
    setLastError(imageDAL.getLastError());

    // Carreguem els thumbnails de les series consultades
    loadSeriesThumbnail(seriesMaskToQuery.getStudyInstanceUID(), queryResult);

    return queryResult;
}

QList<Image*> LocalDatabaseManager::queryImage(const DicomMask &imageMaskToQuery)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseImageDAL imageDAL(&dbConnect);
    QList<Image*> queryResult;

    queryResult = imageDAL.query(imageMaskToQuery);
    setLastError(imageDAL.getLastError());

    return queryResult;
}

bool LocalDatabaseManager::existsStudy(Study *study)
{
    DicomMask dicomMask;
    dicomMask.setStudyInstanceUID(study->getInstanceUID());

    return queryStudy(dicomMask).count() > 0;
}

Patient* LocalDatabaseManager::retrieve(const DicomMask &maskToRetrieve)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseStudyDAL studyDAL(&dbConnect);
    Patient *retrievedPatient = NULL;

    // Busquem l'estudi i pacient
    QList<Patient*> patientList = studyDAL.queryPatientStudy(maskToRetrieve, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);

    if (patientList.count() != 1)
    {
        setLastError(studyDAL.getLastError());
        return retrievedPatient;
    }
    else
    {
        retrievedPatient = patientList.at(0);
    }

    // Busquem les series de l'estudi
    LocalDatabaseSeriesDAL seriesDAL(&dbConnect);
    QList<Series*> seriesList = seriesDAL.query(maskToRetrieve);

    if (seriesDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
    {
        setLastError(seriesDAL.getLastError());
        return NULL;
    }

    DicomMask maskImagesToRetrieve;
    LocalDatabaseImageDAL imageDAL(&dbConnect);

    // Busquem les imatges per cada sèrie
    // Estudi del que s'han de cercar les imatges
    maskImagesToRetrieve.setStudyInstanceUID(maskToRetrieve.getStudyInstanceUID());

    foreach (Series *series, seriesList)
    {
        // Específiquem de quina sèrie de l'estudi hem de buscar les imatges
        maskImagesToRetrieve.setSeriesInstanceUID(series->getInstanceUID());

        QList<Image*> images = imageDAL.query(maskImagesToRetrieve);
        if (imageDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
        {
            break;
        }

        foreach (Image *image, images)
        {
            series->addImage(image);
        }

        retrievedPatient->getStudy(maskToRetrieve.getStudyInstanceUID())->addSeries(series);
    }

    if (imageDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
    {
        setLastError(imageDAL.getLastError());
        return new Patient();
    }

    // Actulitzem la última data d'acces de l'estudi
    Study *retrievedStudy = retrievedPatient->getStudy(maskToRetrieve.getStudyInstanceUID());
    studyDAL.update(retrievedStudy, QDate::currentDate());
    setLastError(studyDAL.getLastError());

    return retrievedPatient;
}

void LocalDatabaseManager::deleteStudy(const QString &studyInstanceUIDToDelete)
{
    INFO_LOG("S'esborrara de la base de dades l'estudi: " + studyInstanceUIDToDelete);

    if (!studyInstanceUIDToDelete.isEmpty())
    {
        DatabaseConnection dbConnect;
        dbConnect.open();
        dbConnect.beginTransaction();

        bool status = deleteStudyStructureFromDatabase(&dbConnect, studyInstanceUIDToDelete);
        if (!status)
        {
            setLastError(dbConnect.getLastError());
            return;
        }

        dbConnect.commitTransaction();
        deleteStudyFromHardDisk(studyInstanceUIDToDelete);
    }
}

void LocalDatabaseManager::deleteSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    INFO_LOG("S'esborrara de la base de dades la serie : " + seriesInstanceUID + " de l'estudi " + studyInstanceUID);

    if (!studyInstanceUID.isEmpty() && !seriesInstanceUID.isEmpty())
    {
        DicomMask studyMask;
        studyMask.setStudyInstanceUID(studyInstanceUID);

        if (querySeries(studyMask).count() == 1)
        {
            // Si només té una sèrie esborrem tot l'estudi
            INFO_LOG("L'estudi de la serie a esborrar nomes te aquesta serie, per tant s'esborrara l'estudi sencer.");
            deleteStudy(studyInstanceUID);
        }
        else
        {
            DatabaseConnection dbConnect;
            dbConnect.open();
            dbConnect.beginTransaction();

            bool status = deleteSeriesStructureFromDatabase(&dbConnect, studyInstanceUID, seriesInstanceUID);

            if (!status)
            {
                setLastError(dbConnect.getLastError());
            }
            else
            {
                dbConnect.commitTransaction();
                deleteSeriesFromHardDisk(studyInstanceUID, seriesInstanceUID);
            }
        }
    }
}

void LocalDatabaseManager::deleteOldStudies()
{
    // Comprovem si tenim activada la opció d'esborra estudis vells, sino es així no fem res
    if (Settings().getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        DatabaseConnection dbConnect;
        dbConnect.open();
        LocalDatabaseStudyDAL studyDAL(&dbConnect);

        INFO_LOG("S'esborraran els estudis vells no visualitzats des del dia " + LocalDatabaseManager::LastAccessDateSelectedStudies.addDays(-1)
                 .toString("dd/MM/yyyy"));

        QList<Study*> studyListToDelete = studyDAL.query(DicomMask(), LocalDatabaseManager::LastAccessDateSelectedStudies);

        setLastError(studyDAL.getLastError());

        if (studyDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
        {
            return;
        }

        if (studyListToDelete.count() == 0)
        {
            INFO_LOG("No hi ha estudis vells per esborrar");
        }

        foreach (Study *study, studyListToDelete)
        {
            deleteStudy(study->getInstanceUID());
            if (getLastError() != LocalDatabaseManager::Ok)
            {
                break;
            }

            // Esborrem el punter a study
            delete study;
        }
    }
}

void LocalDatabaseManager::compact()
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseUtilDAL utilDAL(&dbConnect);

    utilDAL.compact();
    setLastError(utilDAL.getLastError());
}

int LocalDatabaseManager::getDatabaseRevision()
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseUtilDAL utilDAL(&dbConnect);

    int databaseRevision = utilDAL.getDatabaseRevision();
    setLastError(utilDAL.getLastError());

    return databaseRevision;
}

void LocalDatabaseManager::setDatabaseRevision(int databaseRevision)
{
    DatabaseConnection dbConnect;
    dbConnect.open();
    LocalDatabaseUtilDAL utilDAL(&dbConnect);

    utilDAL.updateDatabaseRevision(databaseRevision);
    setLastError(utilDAL.getLastError());
}

bool LocalDatabaseManager::thereIsAvailableSpaceOnHardDisk()
{
    HardDiskInformation hardDiskInformation;
    Settings settings;
    quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath());
    quint64 minimumSpaceRequired = quint64(settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toULongLong() * 1024);
    quint64 MbytesToFree;
    quint64 MbytesToEraseWhereNotEnoughSpaceAvailableInHardDisk =
        settings.getValue(InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull).toULongLong() * 1024;

    m_lastError = Ok;

    if (freeSpaceInHardDisk < minimumSpaceRequired)
    {
        INFO_LOG(QString("No hi ha suficient espai lliure per descarregar fitxers. Espai lliure: %1 MB, espai minim necessari: %2 MB")
                    .arg(QString::number(freeSpaceInHardDisk), QString::number(minimumSpaceRequired)));

        if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria).toBool())
        {
            INFO_LOG("s'intentara esborrar estudis vells per alliberar suficient espai");

            // No hi ha suficient espai indiquem quina és la quantitat de MB d'estudis vells que intentem alliberar. Aquest és el número de Mbytes fins arribar
            // l'espai míninm necessari (minimumSpaceRequired - freeSpaceInHardDisk), més una quantitat fixa, per assegurar que disposem de prou espai per
            // descarregar estudis grans, i no haver d'estar en cada descarrega alliberant espai
            MbytesToFree = (minimumSpaceRequired - freeSpaceInHardDisk) + MbytesToEraseWhereNotEnoughSpaceAvailableInHardDisk;

            freeSpaceDeletingStudies(MbytesToFree);
            if (getLastError() != LocalDatabaseManager::Ok)
            {
                ERROR_LOG("S'ha produit un error intentant alliberar espai");
                return false;
            }

            // Tornem a consultar l'espai lliure
            if (hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath()) < minimumSpaceRequired)
            {
                INFO_LOG("No hi ha suficient espai lliure per descarregar (" + QString().setNum(freeSpaceInHardDisk) + " MB)");
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

QString LocalDatabaseManager::getStudyPath(const QString &studyInstanceUID)
{
    return LocalDatabaseManager::getCachePath() + studyInstanceUID;
}

LocalDatabaseManager::LastError LocalDatabaseManager::getLastError()
{
    return m_lastError;
}

bool LocalDatabaseManager::setStudyRetrieving(const QString &studyInstanceUID)
{
    Settings settings;

    if (!settings.contains(InputOutputSettings::RetrievingStudy) && !studyInstanceUID.isEmpty())
    {
        settings.setValue(InputOutputSettings::RetrievingStudy, studyInstanceUID);
        return true;
    }
    else
    {
        return false;
    }
}

void LocalDatabaseManager::setStudyRetrieveFinished()
{
    Settings().remove(InputOutputSettings::RetrievingStudy);
}

void LocalDatabaseManager::checkNoStudiesRetrieving()
{
    if (isStudyRetrieving())
    {
        Settings settings;
        QString studyNotFullRetrieved = settings.getValue(InputOutputSettings::RetrievingStudy).toString();

        INFO_LOG("L'estudi " + studyNotFullRetrieved + " s'estava descarregant al tancar-se la ultima execucio de l'Starviewer, per mantenir la " +
                 "integritat s'esborraran les imatges que se n'havien descarregat fins al moment");

        // Es pot donar el cas que s'hagués arribat a inserir l'estudi i just abans d'indicar que la descàrrega de l'estudi havia finalitzat a través del
        // mètode setStudyRetrieveFinished, s'hagués tancat l'starviewer per tant el mètode el detectaria que l'estudi estés a mig descarregar quan realment
        // està descarregat, per això comprovem si l'estudi existeix i si és el cas l'esborrem per deixar la base de dades en un estat consistent
        DicomMask studyMask;
        studyMask.setStudyInstanceUID(studyNotFullRetrieved);

        if (queryStudy(studyMask).count() > 0)
        {
            deleteStudy(studyNotFullRetrieved);
        }// No s'ha arribat a inserir a la bd
        else
        {
            // Comprovem si el directori existeix de l'estudi, per si no s'hagués arribat a baixar cap imatge, el
            if (QDir().exists(getStudyPath(studyNotFullRetrieved)))
            {
                deleteStudyFromHardDisk(studyNotFullRetrieved);
            }
        }

        settings.remove(InputOutputSettings::RetrievingStudy);
    }
    else
    {
        m_lastError = Ok;
    }
}

bool LocalDatabaseManager::isStudyRetrieving()
{
    return Settings().contains(InputOutputSettings::RetrievingStudy);
}

bool LocalDatabaseManager::saveStudies(DatabaseConnection *dbConnect, QList<Study*> listStudyToSave, const QDate &currentDate, const QTime &currentTime)
{
    bool status = true;

    foreach (Study *studyToSave, listStudyToSave)
    {
        INFO_LOG(QString("S'inserira a la base de dades l'estudi del pacient %1, modalitat %2, data estudi %3, UID estudi %4")
            .arg(studyToSave->getParentPatient()->getFullName(), studyToSave->getModalitiesAsSingleString())
            .arg(studyToSave->getDateTime().toString("dd/MM/yyyy hh:mm:ss"), studyToSave->getInstanceUID()));

        // Primer guardem les sèries
        status = saveSeries(dbConnect, studyToSave->getSeries(), currentDate, currentTime);

        if (!status)
        {
            break;
        }

        studyToSave->setRetrievedDate(currentDate);
        studyToSave->setRetrievedTime(currentTime);

        status = savePatientOfStudy(dbConnect, studyToSave);

        if (!status)
        {
            break;
        }

        // Guardem la sèrie si no s'ha produït cap error
        status = saveStudy(dbConnect, studyToSave);

        if (!status)
        {
            break;
        }
    }

    return status;
}

bool LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, QList<Series*> listSeriesToSave, const QDate &currentDate, const QTime &currentTime)
{
    bool status = true;

    foreach (Series *seriesToSave, listSeriesToSave)
    {
        /// Primer guardem les imatges
        status = saveImages(dbConnect, seriesToSave->getImages(), currentDate, currentTime);

        if (!status)
        {
            break;
        }

        // Guardem la sèrie si no s'ha produït cap error
        seriesToSave->setRetrievedDate(currentDate);
        seriesToSave->setRetrievedTime(currentTime);

        status = saveSeries(dbConnect, seriesToSave);

        if (!status)
        {
            break;
        }
    }

    return status;
}

bool LocalDatabaseManager::saveImages(DatabaseConnection *dbConnect, QList<Image*> listImageToSave, const QDate &currentDate, const QTime &currentTime)
{
    foreach (Image *imageToSave, listImageToSave)
    {
        imageToSave->setRetrievedDate(currentDate);
        imageToSave->setRetrievedTime(currentTime);

        if (!saveImage(dbConnect, imageToSave))
        {
            return false;
        }
    }

    return true;
}

bool LocalDatabaseManager::saveDisplayShutters(DatabaseConnection *dbConnect, QList<DisplayShutter> shuttersList, Image *relatedImage)
{
    LocalDatabaseDisplayShutterDAL displayShutterDAL(dbConnect);
    foreach (const DisplayShutter &shutter, shuttersList)
    {
        if (!displayShutterDAL.insert(shutter, relatedImage))
        {
            return false;
        }
    }

    return true;
}

bool LocalDatabaseManager::savePatientOfStudy(DatabaseConnection *dbConnect, Study *study)
{
    LocalDatabasePatientDAL patientDAL(dbConnect);
    LocalDatabaseStudyDAL studyDAL(dbConnect);
    qlonglong patientID = studyDAL.getPatientIDFromStudyInstanceUID(study->getInstanceUID());

    if (patientID == -1)
    {
        // Si no existeix l'inserim a la BD
        return patientDAL.insert(study->getParentPatient());
    }
    else
    {
        study->getParentPatient()->setDatabaseID(patientID);
        return patientDAL.update(study->getParentPatient());
    }
}

bool LocalDatabaseManager::saveStudy(DatabaseConnection *dbConnect, Study *studyToSave)
{
    LocalDatabaseStudyDAL studyDAL(dbConnect);

    bool status = studyDAL.insert(studyToSave, QDate::currentDate());

    /// Si l'estudi ja existia actualitzem la seva informació
    if (!status && studyDAL.getLastError().nativeErrorCode().toInt() == SQLITE_CONSTRAINT)
    {
        status = studyDAL.update(studyToSave, QDate::currentDate());
    }

    return status;
}

bool LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, Series *seriesToSave)
{
    LocalDatabaseSeriesDAL seriesDAL(dbConnect);

    bool status = seriesDAL.insert(seriesToSave);

    /// Si la serie ja existia actualitzem la seva informació
    if (!status && seriesDAL.getLastError().nativeErrorCode().toInt() == SQLITE_CONSTRAINT)
    {
        status = seriesDAL.update(seriesToSave);
    }

    return status;
}

bool LocalDatabaseManager::saveImage(DatabaseConnection *dbConnect, Image *imageToSave)
{
    LocalDatabaseImageDAL imageDAL(dbConnect);

    bool status = imageDAL.insert(imageToSave);

    /// Si el pacient ja existia actualitzem la seva informació
    if (!status && imageDAL.getLastError().nativeErrorCode().toInt() == SQLITE_CONSTRAINT)
    {
        imageDAL.update(imageToSave);
        // Un cop actualitzades les imatges, actualitzem els corresponents shutters
        LocalDatabaseDisplayShutterDAL shutterDAL(dbConnect);
        shutterDAL.update(imageToSave->getDisplayShutters(), imageToSave);

        // Delete existing VOI LUTs from the image. The new ones (or the same ones) are inserted below.
        status = deleteVoiLuts(dbConnect, imageToSave);

        if (!status)
        {
            return status;
        }
    }
    else
    {
        status = saveDisplayShutters(dbConnect, imageToSave->getDisplayShutters(), imageToSave);
        if (!status)
        {
            return status;
        }
    }

    // Insert VOI LUTs in the database
    return insertVoiLuts(dbConnect, imageToSave);
}

bool LocalDatabaseManager::deleteStudyStructureFromDatabase(DatabaseConnection *dbConnect, const QString &studyInstanceUIDToDelete)
{
    DicomMask maskToDelete;
    maskToDelete.setStudyInstanceUID(studyInstanceUIDToDelete);

    if (!deletePatientOfStudyFromDatabase(dbConnect, maskToDelete))
    {
        return false;
    }

    // Esborrem tots els estudis
    if (!deleteStudyFromDatabase(dbConnect, maskToDelete))
    {
        return false;
    }

    return deleteSeriesStructureFromDatabase(dbConnect, studyInstanceUIDToDelete, "");
}

bool LocalDatabaseManager::deleteSeriesStructureFromDatabase(DatabaseConnection *dbConnect, const QString &studyInstanceUIDToDelete,
                                                            const QString &seriesIntanceUIDToDelete)
{
    DicomMask maskToDelete;

    maskToDelete.setStudyInstanceUID(studyInstanceUIDToDelete);
    maskToDelete.setSeriesInstanceUID(seriesIntanceUIDToDelete);

    // Esborrem totes les series
    if (!deleteSeriesFromDatabase(dbConnect, maskToDelete))
    {
        return false;
    }

    // Esborrem totes les imatges
    return deleteImageFromDatabase(dbConnect, maskToDelete);
}

void LocalDatabaseManager::deleteRetrievedObjects(Patient *failedPatient)
{
    DirectoryUtilities delDirectory;

    foreach (Study *failedStudy, failedPatient->getStudies())
    {
        delDirectory.deleteDirectory(LocalDatabaseManager::getCachePath() + failedStudy->getInstanceUID(), true);
    }
}

void LocalDatabaseManager::deleteRetrievedObjects(Series *failedSeries)
{
    DirectoryUtilities delDirectory;
    QString studyPath = LocalDatabaseManager::getCachePath() + failedSeries->getParentStudy()->getInstanceUID();
    QString seriesDirectory = studyPath + QDir::separator() + failedSeries->getInstanceUID();

    delDirectory.deleteDirectory(seriesDirectory, true);

    if (delDirectory.isDirectoryEmpty(studyPath))
    {
        // Si el directori de l'estudi està buit, vol dir que només s'havia descarregat
        // la sèrie que ha fallat al guardar la base de dades, per tant si només esborrèssim el
        // directori de la sèrie, quedaria el directori pare de l'estudi buit, per això comprovem si aquest
        // directori és buit, i si és així també s'esborra aquest, perquè no ens quedi un directori d'un estudi buit
        delDirectory.deleteDirectory(studyPath, true);
    }
}

bool LocalDatabaseManager::deletePatientOfStudyFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL(dbConnect);

    // Busquem el ID de pacient
    qlonglong patientID = localDatabaseStudyDAL.getPatientIDFromStudyInstanceUID(maskToDelete.getStudyInstanceUID());

    if (localDatabaseStudyDAL.getLastError().nativeErrorCode().toInt() != SQLITE_OK)
    {
        return false;
    }
    else if (patientID == -1)
    {
        ERROR_LOG(QString("No hem trobat el pacient a esborrar de l'estudi amb UID %1").arg(maskToDelete.getStudyInstanceUID()));
        return true;
    }
    else
    {
        return deletePatientFromDatabase(dbConnect, patientID);
    }
}

bool LocalDatabaseManager::deletePatientFromDatabase(DatabaseConnection *dbConnect, qlonglong patientID)
{
    LocalDatabasePatientDAL localDatabasePatientDAL(dbConnect);

    return localDatabasePatientDAL.del(patientID);
}

bool LocalDatabaseManager::deleteStudyFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL(dbConnect);

    return localDatabaseStudyDAL.del(maskToDelete);
}

bool LocalDatabaseManager::deleteSeriesFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseSeriesDAL localDatabaseSeriesDAL(dbConnect);

    return localDatabaseSeriesDAL.del(maskToDelete);
}

bool LocalDatabaseManager::deleteImageFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    // Primer esborrem els shutters que té aquesta imatge -> DELETE CASCADE
    LocalDatabaseDisplayShutterDAL shutterDAL(dbConnect);

    if (!shutterDAL.del(maskToDelete))
    {
        return false;
    }

    // Delete VOI LUTs from this image
    if (!deleteVoiLuts(dbConnect, maskToDelete))
    {
        return false;
    }
    
    // Si no hi ha cap error en esborrar els shutters, esborrem les corresponents imatges
    LocalDatabaseImageDAL localDatabaseImageDAL(dbConnect);
    return localDatabaseImageDAL.del(maskToDelete);
}

void LocalDatabaseManager::freeSpaceDeletingStudies(quint64 MbytesToErase)
{
    QList<Study*> studyListToDelete = queryStudyOrderByLastAccessDate(DicomMask());

    if (getLastError() == LocalDatabaseManager::Ok)
    {
        quint64 MbytesErased = 0;
        int index = 0;

        while (index < studyListToDelete.count() && MbytesErased < MbytesToErase)
        {
            Study *studyToDelete = studyListToDelete.at(index);

            emit studyWillBeDeleted(studyToDelete->getInstanceUID());
            MbytesErased += HardDiskInformation::getDirectorySizeInBytes(LocalDatabaseManager::getCachePath() + studyToDelete->getInstanceUID()) / 1024 / 1024;

            deleteStudy(studyToDelete->getInstanceUID());
            if (getLastError() != LocalDatabaseManager::Ok)
            {
                break;
            }

            index++;
        }

        // Esborrem els estudis de la memòria
        foreach (Study *study, studyListToDelete)
        {
            delete study;
        }
    }
}

void LocalDatabaseManager::deleteStudyFromHardDisk(const QString &studyInstanceToDelete)
{
    DirectoryUtilities deleteDirectory;

    // TODO El Path del directori no s'hauria de calcular aquí
    if (!deleteDirectory.deleteDirectory(getStudyPath(studyInstanceToDelete), true))
    {
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    }
    else
    {
        m_lastError = LocalDatabaseManager::Ok;
    }
}

void LocalDatabaseManager::deleteSeriesFromHardDisk(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    DirectoryUtilities deleteDirectory;

    // TODO El Path del directori no s'hauria de calcular aquí
    if (!deleteDirectory.deleteDirectory(getStudyPath(studyInstanceUID) + QDir::separator() + seriesInstanceUID, true))
    {
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    }
    else
    {
        m_lastError = LocalDatabaseManager::Ok;
    }
}

void LocalDatabaseManager::createStudyThumbnails(Study *studyToGenerateSeriesThumbnails)
{
    foreach (Series *series, studyToGenerateSeriesThumbnails->getSeries())
    {
        createSeriesThumbnail(series);
    }
}

void LocalDatabaseManager::createSeriesThumbnail(Series *seriesToGenerateThumbnail)
{
    ThumbnailCreator thumbnailCreator;
    QString thumbnailFilePath;

    // Només crearem el thumbnail si aquest no s'ha creat encara
    thumbnailFilePath = getSeriesThumbnailPath(seriesToGenerateThumbnail->getParentStudy()->getInstanceUID(), seriesToGenerateThumbnail);
    if (!QFileInfo(thumbnailFilePath).exists())
    {
        thumbnailCreator.getThumbnail(seriesToGenerateThumbnail).save(thumbnailFilePath, "PNG");
    }
}

void LocalDatabaseManager::loadSeriesThumbnail(QString studyInstanceUID, QList<Series*> seriesList)
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

void LocalDatabaseManager::setLastError(const QSqlError &error)
{
    int sqliteLastError = error.nativeErrorCode().toInt();

    // Es tradueixen els errors de Sqlite a errors nostres, per consulta codi d'errors Sqlite http://www.sqlite.org/c3ref/c_abort.html
    if (sqliteLastError == SQLITE_OK)
    {
        m_lastError = Ok;
    }
    else if (sqliteLastError == SQLITE_ERROR)
    {
        m_lastError = SyntaxErrorSQL;
    }
    else if (sqliteLastError == SQLITE_LOCKED || sqliteLastError == SQLITE_BUSY)
    {
        m_lastError = DatabaseLocked;
    }
    else if (sqliteLastError == SQLITE_CORRUPT || sqliteLastError == SQLITE_EMPTY || sqliteLastError == SQLITE_SCHEMA || sqliteLastError == SQLITE_MISMATCH
             || sqliteLastError == SQLITE_NOTADB)
    {
        m_lastError = DatabaseCorrupted;
    }
    else
    {
        m_lastError = DatabaseError;
    }

    // El valor dels errors es pot consultar a http://www.sqlite.org/c3ref/c_abort.html
    if (sqliteLastError != SQLITE_OK)
    {
        ERROR_LOG("Codi error base de dades " + QString().setNum(sqliteLastError));
    }
}

QString LocalDatabaseManager::getSeriesThumbnailPath(QString studyInstanceUID, Series *series)
{
    return getStudyPath(studyInstanceUID) + "/" + series->getInstanceUID() + "/thumbnail.png";
}

QString LocalDatabaseManager::getDatabaseFilePath()
{
    return QDir::toNativeSeparators(Settings().getValue(InputOutputSettings::DatabaseAbsoluteFilePath).toString());
}

QString LocalDatabaseManager::getCachePath()
{
    return QDir::toNativeSeparators(Settings().getValue(InputOutputSettings::CachePath).toString());
}

}
