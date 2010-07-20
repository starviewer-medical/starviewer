/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanager.h"

#include <QDir>

#include "patient.h"
#include "study.h"
#include "localdatabaseimagedal.h"
#include "localdatabaseseriesdal.h"
#include "localdatabasestudydal.h"
#include "localdatabasepatientdal.h"
#include "localdatabaseutildal.h"
#include "localdatabasekeyimagenotedal.h"
#include "localdatabasedicomreferencedimagedal.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "sqlite3.h"
#include "logging.h"
#include "deletedirectory.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"
#include "harddiskinformation.h"
#include "thumbnailcreator.h"
#include "keyimagenote.h"
#include "dicomreferencedimage.h"

namespace udg
{
//Nom de la llista de Settings que guardarà els estudis que tenim en aquell moment descarregant
QDate LocalDatabaseManager::LastAccessDateSelectedStudies;

LocalDatabaseManager::LocalDatabaseManager()
{
    Settings settings;

    /* Comprovem si està activada la opció de configuració que indica si s'ha d'esborrar automàticament els estudis vells, sinó
       està activada, construim una data nul·la perquè no s'ha de tenir en compte alhora de fer las cerques*/
    if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        if (!LocalDatabaseManager::LastAccessDateSelectedStudies.isValid())
        {
            LocalDatabaseManager::LastAccessDateSelectedStudies = QDate::currentDate().addDays(-settings.getValue(InputOutputSettings::MinimumDaysUnusedToDeleteStudy).toInt());
        }
    }
    else LocalDatabaseManager::LastAccessDateSelectedStudies = QDate();
}

void LocalDatabaseManager::save(Patient *newPatient)
{
    DatabaseConnection dbConnect;
    int status = SQLITE_OK;

    dbConnect.open();
    dbConnect.beginTransaction(); 

    if (newPatient == NULL)
    {
        ERROR_LOG("No es pot inserir a la base de dades l'estudi que s'ha descarregat, perque el patient es null");
        m_lastError = PatientInconsistent;
        dbConnect.close();
        return;
    }

    ///Guardem primer els estudis
    if (newPatient->getStudies().count() > 0)
    {
        status = saveStudies(&dbConnect, newPatient->getStudies(), QDate::currentDate(), QTime::currentTime());

        if (status != SQLITE_OK) 
        {
            dbConnect.rollbackTransaction();
            deleteRetrievedObjects(newPatient);
            setLastError(status);
            dbConnect.close();
            return;
        }
    }

    status = savePatient(&dbConnect, newPatient);

    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        deleteRetrievedObjects(newPatient);
    }
    else
        dbConnect.endTransaction();

    dbConnect.close();

    foreach(Study *study, newPatient->getStudies())
    {
        createStudyThumbnails(study);
    }

    setLastError(status);
}

void LocalDatabaseManager::save(Series *seriesToSave)
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    DatabaseConnection dbConnect;
    int status = SQLITE_OK;

    dbConnect.open();
    dbConnect.beginTransaction(); 

    if (seriesToSave == NULL)
    {
        ERROR_LOG("No es pot inserir a la base de dades la serie perque te valor null");
        m_lastError = PatientInconsistent;
        dbConnect.close();
        return;
    }

    status = savePatient(&dbConnect, seriesToSave->getParentStudy()->getParentPatient());

    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        deleteRetrievedObjects(seriesToSave);
        setLastError(status);
        dbConnect.close();
        return;
    }

    Study *studyParent = seriesToSave->getParentStudy();
    studyParent->setRetrievedDate(currentDate);
    studyParent->setRetrievedTime(currentTime);

    status = saveStudy(&dbConnect, studyParent);

    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        deleteRetrievedObjects(seriesToSave);
        setLastError(status);
        dbConnect.close();
        return;
    }

    QList<Series*> seriesList;
    seriesList.append(seriesToSave);

    saveSeries(&dbConnect, seriesList, currentDate, currentTime);
    
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        deleteRetrievedObjects(seriesToSave);
        setLastError(status);
        dbConnect.close();
        return;
    }
    else
    {
        dbConnect.endTransaction();
    }

    dbConnect.close();

    createSeriesThumbnail(seriesToSave);

    setLastError(status);
}

QList<Patient*> LocalDatabaseManager::queryPatient(const DicomMask &patientMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabasePatientDAL patientDAL;
    QList<Patient*> queryResult;

    dbConnect.open();
    patientDAL.setDatabaseConnection(&dbConnect);
    queryResult = patientDAL.query(patientMaskToQuery);
    setLastError(patientDAL.getLastError());

    dbConnect.close();

    return queryResult;
}

QList<Patient*> LocalDatabaseManager::queryPatientStudy(const DicomMask &patientStudyMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseStudyDAL studyDAL;
    QList<Patient*> queryResult;

    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    queryResult = studyDAL.queryPatientStudy(patientStudyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    dbConnect.close();

    return queryResult;
}

QList<Study*> LocalDatabaseManager::queryStudy(const DicomMask &studyMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseStudyDAL studyDAL;
    QList<Study*> queryResult;

    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    queryResult = studyDAL.query(studyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    dbConnect.close();

    return queryResult;
}

QList<Study*> LocalDatabaseManager::queryStudyOrderByLastAccessDate(const DicomMask &studyMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseStudyDAL studyDAL;
    QList<Study*> queryResult;

    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    queryResult = studyDAL.queryOrderByLastAccessDate(studyMaskToQuery, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);
    setLastError(studyDAL.getLastError());

    dbConnect.close();

    return queryResult;
}

QList<Series*> LocalDatabaseManager::querySeries(const DicomMask &seriesMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseSeriesDAL seriesDAL;
    LocalDatabaseImageDAL imageDAL;
    QList<Series*> queryResult;
    DicomMask maskToCountNumberOfImage = seriesMaskToQuery;

    dbConnect.open();
    seriesDAL.setDatabaseConnection(&dbConnect);
    imageDAL.setDatabaseConnection(&dbConnect);

    queryResult = seriesDAL.query(seriesMaskToQuery);

    if (seriesDAL.getLastError() != SQLITE_OK)
    {
        setLastError(seriesDAL.getLastError());
        dbConnect.close();
        return queryResult;
    }

    // Consultem el número d'imatges de la Sèrie
    foreach(Series *series, queryResult)
    {
        maskToCountNumberOfImage.setSeriesInstanceUID(series->getInstanceUID());
        series->setNumberOfImages(imageDAL.count(maskToCountNumberOfImage));

        if (imageDAL.getLastError() != SQLITE_OK)
        {
            break;
        }
    }
    setLastError(imageDAL.getLastError());

    dbConnect.close();

    //Carreguem els thumbnails de les series consultades
    loadSeriesThumbnail(seriesMaskToQuery.getStudyInstanceUID(), queryResult);

    return queryResult;
}

QList<Image*> LocalDatabaseManager::queryImage(const DicomMask &imageMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseImageDAL imageDAL;
    QList<Image*> queryResult;

    dbConnect.open();
    imageDAL.setDatabaseConnection(&dbConnect);
    queryResult = imageDAL.query(imageMaskToQuery);
    setLastError(imageDAL.getLastError());

    dbConnect.close();

    return queryResult;
}

Patient* LocalDatabaseManager::retrieve(const DicomMask &maskToRetrieve)
{
    LocalDatabaseStudyDAL studyDAL;
    LocalDatabaseSeriesDAL seriesDAL;
    LocalDatabaseImageDAL imageDAL;
    QList<Patient*> patientList;
    QList<Series*> seriesList;
    Patient *retrievedPatient = NULL;
    Study *retrievedStudy;
    DicomMask maskImagesToRetrieve;
    DatabaseConnection dbConnect;

    //busquem l'estudi i pacient
    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    patientList = studyDAL.queryPatientStudy(maskToRetrieve, QDate(), LocalDatabaseManager::LastAccessDateSelectedStudies);

    if (patientList.count() != 1) 
    {
        setLastError(studyDAL.getLastError());
        dbConnect.close();
        return retrievedPatient;
    }
    else 
        retrievedPatient = patientList.at(0);

    //busquem les series de l'estudi
    seriesDAL.setDatabaseConnection(&dbConnect);
    seriesList = seriesDAL.query(maskToRetrieve);

    if (seriesDAL.getLastError() != SQLITE_OK)
    {
        setLastError(seriesDAL.getLastError());
        dbConnect.close();
        return new Patient();
    }

    //busquem les imatges per cada sèrie
    maskImagesToRetrieve.setStudyInstanceUID(maskToRetrieve.getStudyInstanceUID());//estudi del que s'han de cercar les imatges
    imageDAL.setDatabaseConnection(&dbConnect);

    foreach (Series *series, seriesList)
    {
        maskImagesToRetrieve.setSeriesInstanceUID(series->getInstanceUID());//específiquem de quina sèrie de l'estudi hem de buscar les imatges

         if (series->getModality() == "KO")
         {
            int status = queryKeyImageNotes(dbConnect, series, maskImagesToRetrieve);

            if (status != SQLITE_OK)
            {
                break;
            }
         }
         else
         {
            QList<Image*> images = imageDAL.query(maskImagesToRetrieve);
            if (imageDAL.getLastError() != SQLITE_OK) break;

            foreach (Image *image, images)
            {
                series->addImage(image);
            }
         }

         retrievedPatient->getStudy(maskToRetrieve.getStudyInstanceUID())->addSeries(series);
    }

    if (imageDAL.getLastError() != SQLITE_OK)
    {
        setLastError(imageDAL.getLastError());
        dbConnect.close();
        return new Patient();
    }

    //Actulitzem la última data d'acces de l'estudi
    retrievedStudy = retrievedPatient->getStudy(maskToRetrieve.getStudyInstanceUID());
    studyDAL.update(retrievedStudy, QDate::currentDate());
    setLastError(studyDAL.getLastError());

    dbConnect.close();

    //carreguem els thumbnails dels estudis
    foreach(Study *study, retrievedPatient->getStudies())
    {
        loadSeriesThumbnail(study->getInstanceUID(), study->getSeries());
    }

    return retrievedPatient;
}

void LocalDatabaseManager::deleteStudy(const QString &studyInstanceToDelete)
{
    DatabaseConnection dbConnect;
    DicomMask studyMaskToDelete;
    int status;

    INFO_LOG("S'esborrara de la base de dades l'estudi: " + studyInstanceToDelete);

    if (studyInstanceToDelete.isEmpty())
        return;
    else
        studyMaskToDelete.setStudyInstanceUID(studyInstanceToDelete);
	
    dbConnect.open();
    dbConnect.beginTransaction();

    status = deletePatientOfStudyFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = deleteStudyFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = deleteSeriesFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = deleteImageFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = deleteKeyImageNoteFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = deleteDICOMReferencedImageFromDatabase(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    dbConnect.endTransaction();
    dbConnect.close();
    deleteStudyFromHardDisk(studyInstanceToDelete);
}

void LocalDatabaseManager::deleteSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    DatabaseConnection dbConnect;
    DicomMask seriesMaskToDelete, studyMask;
    int status;

    INFO_LOG("S'esborrara de la base de dades la serie : " + seriesInstanceUID + " de l'estudi " + studyInstanceUID);
    if (studyInstanceUID.isEmpty() || seriesInstanceUID.isEmpty())
        return;

    seriesMaskToDelete.setStudyInstanceUID(studyInstanceUID);
    seriesMaskToDelete.setSeriesInstanceUID(seriesInstanceUID);
    studyMask.setStudyInstanceUID(studyInstanceUID);

    if (querySeries(studyMask).count() == 1)
    {
        //Si només té una sèrie esborrem tot l'estudi
        INFO_LOG("L'estudi de la serie a esborrar nomes te aquesta serie, per tant s'esborrara l'estudi sencer.");
        deleteStudy(studyInstanceUID);
    }
    else
    {
        dbConnect.open();
        dbConnect.beginTransaction();

        status = deleteSeriesFromDatabase(&dbConnect, seriesMaskToDelete);
        if (status != SQLITE_OK) 
        {
            dbConnect.rollbackTransaction();
            setLastError(status);
            dbConnect.close();
            return;
        }

        status = deleteImageFromDatabase(&dbConnect, seriesMaskToDelete);
        if (status != SQLITE_OK) 
        {
            dbConnect.rollbackTransaction();
            setLastError(status);
            dbConnect.close();
            return;
        }

        status = deleteKeyImageNoteFromDatabase(&dbConnect, seriesMaskToDelete);
        if (status != SQLITE_OK) 
        {
            dbConnect.rollbackTransaction();
            setLastError(status);
            dbConnect.close();
            return;
        }

        status = deleteDICOMReferencedImageFromDatabase(&dbConnect, seriesMaskToDelete);
        if (status != SQLITE_OK) 
        {
            dbConnect.rollbackTransaction();
            setLastError(status);
            dbConnect.close();
            return;
        }

        dbConnect.endTransaction();
        dbConnect.close();

        deleteSeriesFromHardDisk(studyInstanceUID, seriesInstanceUID);
    }
}

void LocalDatabaseManager::clear()
{
    DicomMask maskToDelete;//creem màscara buida, ho esborrarà tot
    DatabaseConnection dbConnect;
    DeleteDirectory delDirectory;
    int status;

    dbConnect.open();
    dbConnect.beginTransaction();

    status = deletePatientFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem tots els estudis
    status = deleteStudyFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem totes les series
    status = deleteSeriesFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem totes les imatges 
    status = deleteImageFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

     // esborrem tots els key image notes
    status = deleteKeyImageNoteFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    // esborrem tots els DICOM Referenced Images
    status = deleteDICOMReferencedImageFromDatabase(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    dbConnect.endTransaction();
    dbConnect.close();
    //esborrem tots els estudis descarregats, físicament del disc dur
    if (!delDirectory.deleteDirectory(LocalDatabaseManager::getCachePath(), false)) 
        m_lastError = DeletingFilesError;
    else 
        m_lastError = Ok;
}

void LocalDatabaseManager::deleteOldStudies()
{
    Settings settings;
    DicomMask oldStudiesMask;
    QList<Study*> studyListToDelete;
    LocalDatabaseStudyDAL studyDAL;
    DatabaseConnection dbConnect;

    //Comprovem si tenim activada la opció d'esborra estudis vells, sino es així no fem res
    if (!settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool()) return;

    INFO_LOG("S'esborraran els estudis vells no visualitzats des del dia " + LocalDatabaseManager::LastAccessDateSelectedStudies.addDays(-1).toString("dd/MM/yyyy"));

    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    studyListToDelete = studyDAL.query(oldStudiesMask, LocalDatabaseManager::LastAccessDateSelectedStudies);

    setLastError(studyDAL.getLastError());

    if (studyDAL.getLastError() != SQLITE_OK)
    {
        dbConnect.close();
        return;
    }

    if (studyListToDelete.count() == 0)
    {
        INFO_LOG("No hi ha estudis vells per esborrar");
    }

    foreach(Study *study, studyListToDelete)
    {
        deleteStudy(study->getInstanceUID());
        if (getLastError() != LocalDatabaseManager::Ok)
            break;

        //esborrem el punter a study
        delete study;
    } 
    dbConnect.close();
}

void LocalDatabaseManager::compact()
{
    LocalDatabaseUtilDAL utilDAL;
    DatabaseConnection dbConnect;
    
    dbConnect.open();
    utilDAL.setDatabaseConnection(&dbConnect);
    utilDAL.compact();
    setLastError(utilDAL.getLastError());

    dbConnect.close();
}

int LocalDatabaseManager::getDatabaseRevision()
{
    LocalDatabaseUtilDAL utilDAL;
    DatabaseConnection dbConnect;
    int databaseRevision;

    dbConnect.open();
    utilDAL.setDatabaseConnection(&dbConnect);
    databaseRevision = utilDAL.getDatabaseRevision();
    setLastError(utilDAL.getLastError());

    dbConnect.close();

    return databaseRevision;
}

bool LocalDatabaseManager::isDatabaseCorrupted()
{
    LocalDatabaseUtilDAL utilDAL;
    DatabaseConnection dbConnect;
    bool databaseCorrupted;

    dbConnect.open();
    utilDAL.setDatabaseConnection(&dbConnect);
    databaseCorrupted = utilDAL.isDatabaseCorrupted();
    setLastError(utilDAL.getLastError());

    dbConnect.close();

    return databaseCorrupted;
}

bool LocalDatabaseManager::thereIsAvailableSpaceOnHardDisk()
{
    HardDiskInformation hardDiskInformation;
    Settings settings;
    quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath());
    quint64 minimumSpaceRequired = quint64( settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache ).toULongLong() * 1024 );
    quint64 MbytesToFree;
    quint64 MbytesToEraseWhereNotEnoughSpaceAvailableInHardDisk = settings.getValue( InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull ).toULongLong() * 1024;

    m_lastError = Ok;

    if (freeSpaceInHardDisk < minimumSpaceRequired)
    {
        INFO_LOG("No hi ha suficient espai lliure per descarregar (" + QString().setNum(freeSpaceInHardDisk) + " Mb) ");

        if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria).toBool())
        {
            INFO_LOG("s'intentara esborrar estudis vells per alliberar suficient espai");

            //No hi ha suficient espai indiquem quina és la quantitat de Mb d'estudis vells que intentem alliberar. Aquest és el número de Mbytes fins arribar l'espai míninm necessari (minimumSpaceRequired - freeSpaceInHardDisk), més una quantitat fixa, per assegurar que disposem de prou espai per descarregar estudis grans, i no haver d'estar en cada descarrega alliberant espai
            MbytesToFree = (minimumSpaceRequired - freeSpaceInHardDisk) + MbytesToEraseWhereNotEnoughSpaceAvailableInHardDisk;

            freeSpaceDeletingStudies(MbytesToFree);
            if (getLastError() != LocalDatabaseManager::Ok)
            {
                ERROR_LOG("S'ha produit un error intentant alliberar espai");
                return false;
            }

            //Tornem a consultar l'espai lliure
            if (hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath()) < minimumSpaceRequired)
            {
                INFO_LOG("No hi ha suficient espai lliure per descarregar (" + QString().setNum(freeSpaceInHardDisk) + " Mb)");
                return false;
            }
            else return true;
        }
        else return false;
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
    else return false;
}

void LocalDatabaseManager::setStudyRetrieveFinished()
{
    Settings settings;

    settings.remove(InputOutputSettings::RetrievingStudy);
}

void LocalDatabaseManager::checkNoStudiesRetrieving()
{
    Settings settings;
    DeleteDirectory deleteDirectory;
    QDir directory;

    if (isStudyRetrieving())
    {
        QString studyNotFullRetrieved = settings.getValue(InputOutputSettings::RetrievingStudy).toString();

        INFO_LOG("L'estudi " + studyNotFullRetrieved + " s'estava descarregant al tancar-se la ultima execucio de l'Starviewer, per mantenir la integritat s'esborraran les imatges que se n'havien descarregat fins al moment");

        //Es pot donarper si es dona el cas que s'hagués arribat a inserir l'estudi i just abans d'indicar que la descàrrega de l'estudi havia finalitzat a través del mètode setStudyRetrieveFinished, s'hagués tancat l'starviewer per tant el mètode el detectaria com que l'estudi estés a mig descarregar quan realment està descarregat, per això comprovem si l'estudi existeix i si és el cas l'esborrem per deixa la base de dades en un estat consistent
        DicomMask studyMask;
        studyMask.setStudyInstanceUID(studyNotFullRetrieved);

        if (queryStudy(studyMask).count() > 0)
        {
            deleteStudy(studyNotFullRetrieved);
        }//No s'ha arribat a inserir a la bd
        else 
        {
            //Comprovem si el directori existeix de l'estudi, per si no s'hagués arribat a baixar cap imatge, el 
            if (directory.exists(getStudyPath(studyNotFullRetrieved)))
                deleteStudyFromHardDisk(studyNotFullRetrieved);
        }

        settings.remove(InputOutputSettings::RetrievingStudy);
    }
    else m_lastError = Ok;
}

bool LocalDatabaseManager::isStudyRetrieving()
{
    return Settings().contains(InputOutputSettings::RetrievingStudy);
}

int LocalDatabaseManager::saveStudies(DatabaseConnection *dbConnect, QList<Study*> listStudyToSave, const QDate &currentDate, const QTime &currentTime)
{
    int status = SQLITE_OK;

    foreach(Study* studyToSave, listStudyToSave)
    {
        ///primer guardem les sèries
        status = saveSeries(dbConnect, studyToSave->getSeries(), currentDate, currentTime);

        if (status != SQLITE_OK)
            break;

        studyToSave->setRetrievedDate(currentDate);
        studyToSave->setRetrievedTime(currentTime);

        //Guardem la sèrie si no s'ha produït cap error
        status = saveStudy(dbConnect, studyToSave);

        if (status != SQLITE_OK) 
            break;
    }

    return status;
}

int LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, QList<Series*> listSeriesToSave, const QDate &currentDate, const QTime &currentTime)
{
    int status = SQLITE_OK;

    foreach(Series* seriesToSave, listSeriesToSave)
    {

        if (seriesToSave->getModality() == "KO")
        {
            status = saveKeyImageNotes(dbConnect, seriesToSave->getKeyImageNotes(), currentDate, currentTime);
        }
        else
        {
            ///primer guardem les imatges
            status = saveImages(dbConnect, seriesToSave->getImages(), currentDate, currentTime);
        }

        if (status != SQLITE_OK)
            break;

        //Guardem la sèrie si no s'ha produït cap error
        seriesToSave->setRetrievedDate(currentDate);
        seriesToSave->setRetrievedTime(currentTime);

        status = saveSeries(dbConnect, seriesToSave);

        if (status != SQLITE_OK) 
            break;
    }

    return status;
}

int LocalDatabaseManager::saveImages(DatabaseConnection *dbConnect, QList<Image*> listImageToSave, const QDate &currentDate, const QTime &currentTime)
{
    int status = SQLITE_OK;

    foreach(Image* imageToSave, listImageToSave)
    {
        imageToSave->setRetrievedDate(currentDate);
        imageToSave->setRetrievedTime(currentTime);

        status = saveImage(dbConnect, imageToSave);

        if (status != SQLITE_OK) 
            return status;
    }

    return status;
}

int LocalDatabaseManager::saveKeyImageNotes(DatabaseConnection *dbConnect, QList<KeyImageNote*> listKeyImageNoteToSave, const QDate &currentDate, const QTime &currentTime)
{
    int status = SQLITE_OK;

    foreach (KeyImageNote *keyImageNoteToSave, listKeyImageNoteToSave)
    {
        keyImageNoteToSave->setRetrievedDate(currentDate);
        keyImageNoteToSave->setRetrievedTime(currentTime);

        status = saveKeyImageNote(dbConnect, keyImageNoteToSave);

        if (status != SQLITE_OK)
        {
            return status;
        }
    }

    return status;
}

int LocalDatabaseManager::saveDICOMReferencedImages(DatabaseConnection *dbConnect, QList<DICOMReferencedImage*> listDICOMReferencedImageToSave, Series *seriesOfParentObject)
{
    int status = SQLITE_OK;

    foreach (DICOMReferencedImage *DICOMReferencedImageToSave, listDICOMReferencedImageToSave)
    {
        status = saveDICOMReferencedImage(dbConnect, DICOMReferencedImageToSave, seriesOfParentObject);

        if (status != SQLITE_OK)
        {
            return status;
        }
    }

    return status;
}

int LocalDatabaseManager::savePatient(DatabaseConnection *dbConnect, Patient *patientToSave)
{
    LocalDatabasePatientDAL patientDAL;

    patientDAL.setDatabaseConnection(dbConnect);

    patientDAL.insert(patientToSave);

    ///Si el pacient ja existia actualitzem la seva informació
    if (patientDAL.getLastError() == SQLITE_CONSTRAINT) patientDAL.update(patientToSave);

    return patientDAL.getLastError();
}

int LocalDatabaseManager::saveStudy(DatabaseConnection *dbConnect, Study *studyToSave)
{
    LocalDatabaseStudyDAL studyDAL;

    studyDAL.setDatabaseConnection(dbConnect);

    studyDAL.insert(studyToSave, QDate::currentDate());

    ///Si l'estudi ja existia actualitzem la seva informació
    if (studyDAL.getLastError() == SQLITE_CONSTRAINT) studyDAL.update(studyToSave, QDate::currentDate());

    return studyDAL.getLastError();
}

int LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, Series *seriesToSave)
{
    LocalDatabaseSeriesDAL seriesDAL;

    seriesDAL.setDatabaseConnection(dbConnect);

    seriesDAL.insert(seriesToSave);

    ///Si la serie ja existia actualitzem la seva informació
    if (seriesDAL.getLastError() == SQLITE_CONSTRAINT) seriesDAL.update(seriesToSave);

    return seriesDAL.getLastError();
}

int LocalDatabaseManager::saveImage(DatabaseConnection *dbConnect, Image *imageToSave)
{
    LocalDatabaseImageDAL imageDAL;

    imageDAL.setDatabaseConnection(dbConnect);

    imageDAL.insert(imageToSave);

    ///Si el pacient ja existia actualitzem la seva informació
    if (imageDAL.getLastError() == SQLITE_CONSTRAINT) imageDAL.update(imageToSave);

    return imageDAL.getLastError();
}

int LocalDatabaseManager::saveKeyImageNote(DatabaseConnection *dbConnect, KeyImageNote *keyImageNoteToSave)
{
    LocalDatabaseKeyImageNoteDAL keyImageNoteDAL;

    keyImageNoteDAL.setDatabaseConnection(dbConnect);
    keyImageNoteDAL.insert(keyImageNoteToSave);

    int status;
    if (keyImageNoteDAL.getLastError() == SQLITE_OK)
    {
        status = saveDICOMReferencedImages(dbConnect, keyImageNoteToSave->getDICOMReferencedImages(), keyImageNoteToSave->getParentSeries());
    }
    else if (keyImageNoteDAL.getLastError() == SQLITE_CONSTRAINT) 
    {
        keyImageNoteDAL.update(keyImageNoteToSave);
        status = keyImageNoteDAL.getLastError();
    }

    return status;
}

int LocalDatabaseManager::saveDICOMReferencedImage(DatabaseConnection *dbConnect, DICOMReferencedImage *DICOMReferencedImageToSave, Series *seriesOfParentObject)
{
    LocalDatabaseDICOMReferencedImageDAL DICOMReferencedImageDAL;

    DICOMReferencedImageDAL.setDatabaseConnection(dbConnect);

    DICOMReferencedImageDAL.insert(DICOMReferencedImageToSave, seriesOfParentObject);

    return DICOMReferencedImageDAL.getLastError();
}

void LocalDatabaseManager::deleteRetrievedObjects(Patient *failedPatient)
{
    DeleteDirectory delDirectory;

    foreach(Study *failedStudy, failedPatient->getStudies())
    {
        delDirectory.deleteDirectory(LocalDatabaseManager::getCachePath() + failedStudy->getInstanceUID(), true);
    }
}

void LocalDatabaseManager::deleteRetrievedObjects(Series *failedSeries)
{
    DeleteDirectory delDirectory;
    QString studyPath = LocalDatabaseManager::getCachePath() + failedSeries->getParentStudy()->getInstanceUID();
    QString seriesDirectory = studyPath + QDir::separator() + failedSeries->getInstanceUID();

    delDirectory.deleteDirectory(seriesDirectory, true);

    if (delDirectory.isDirectoryEmpty(studyPath))
    {
        /*Si el directori de l'estudi està buit, vol dir que només s'havia descarregat la sèrie que ha fallat al guardar la base de dades,
         per tant si només esborrèssim el directori de la sèrie, quedaria el directori pare de l'estudi buit, per això comprovem si aquest 
         directori és buit, i si és així també s'esborra aquest, perquè no ens quedi un directori d'un estudi buit*/
        delDirectory.deleteDirectory(studyPath, true);
    }
}

int LocalDatabaseManager::deletePatientOfStudyFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL;
    QList<Patient*> patientList;
    QString patientID;
    int numberOfStudies;

    //Només podem esborrar el pacient si no té cap més estudi que el que s'ha d'esborrar
    localDatabaseStudyDAL.setDatabaseConnection(dbConnect);

    //com de la màscara a esborrar ens passa l'studyUID hem de buscar el patient id per aquell estudi 
    patientList = localDatabaseStudyDAL.queryPatientStudy(maskToDelete);
    if (localDatabaseStudyDAL.getLastError() != SQLITE_OK)
        return localDatabaseStudyDAL.getLastError();

    if (patientList.count() > 0) 
    {
        patientID = patientList.at(0)->getID(); //Per un mateix studyUID no podem trobar més d'un pacient
        delete patientList.at(0);//esborrem el pacient no el necessitem més

        //busquem quants estudis té el pacient
        numberOfStudies = localDatabaseStudyDAL.countHowManyStudiesHaveAPatient(patientID);
        if (localDatabaseStudyDAL.getLastError() != SQLITE_OK)
            return localDatabaseStudyDAL.getLastError();

        if (numberOfStudies == 1) //si només té un estudi l'esborrem
        {
            DicomMask patientMaskToDelete;
            patientMaskToDelete.setPatientId(patientID);

            return deletePatientFromDatabase(dbConnect, patientMaskToDelete);
        }
        else return localDatabaseStudyDAL.getLastError();
    }
    else 
    {
        ERROR_LOG("No s'ha trobat cap pacient a esborrar");
        //No s'ha trobat pacient a esborrar
        return localDatabaseStudyDAL.getLastError();
    }
}

int LocalDatabaseManager::deletePatientFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabasePatientDAL localDatabasePatientDAL;

    localDatabasePatientDAL.setDatabaseConnection(dbConnect);
    localDatabasePatientDAL.del(maskToDelete);

    return localDatabasePatientDAL.getLastError();
}

int LocalDatabaseManager::deleteStudyFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL;

    localDatabaseStudyDAL.setDatabaseConnection(dbConnect);
    localDatabaseStudyDAL.del(maskToDelete);

    return localDatabaseStudyDAL.getLastError();
}

int LocalDatabaseManager::deleteSeriesFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseSeriesDAL localDatabaseSeriesDAL;

    localDatabaseSeriesDAL.setDatabaseConnection(dbConnect);
    localDatabaseSeriesDAL.del(maskToDelete);

    return localDatabaseSeriesDAL.getLastError();
}

int LocalDatabaseManager::deleteImageFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseImageDAL localDatabaseImageDAL;

    localDatabaseImageDAL.setDatabaseConnection(dbConnect);
    localDatabaseImageDAL.del(maskToDelete);

    return localDatabaseImageDAL.getLastError();
}

int LocalDatabaseManager::deleteKeyImageNoteFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseKeyImageNoteDAL localDatabaseKeyImageNoteDAL;

    localDatabaseKeyImageNoteDAL.setDatabaseConnection(dbConnect);
    localDatabaseKeyImageNoteDAL.del(maskToDelete);

    return localDatabaseKeyImageNoteDAL.getLastError();
}

int LocalDatabaseManager::deleteDICOMReferencedImageFromDatabase(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseDICOMReferencedImageDAL localDatabaseDICOMReferencedImageDAL;

    localDatabaseDICOMReferencedImageDAL.setDatabaseConnection(dbConnect);
    localDatabaseDICOMReferencedImageDAL.del(maskToDelete);

    return localDatabaseDICOMReferencedImageDAL.getLastError();
}
void LocalDatabaseManager::freeSpaceDeletingStudies(quint64 MbytesToErase)
{
    DicomMask oldStudiesMask;
    QList<Study*> studyListToDelete;
    Study *studyToDelete;
    quint64 MbytesErased = 0;
    int index = 0;

    studyListToDelete = queryStudyOrderByLastAccessDate(oldStudiesMask);
    if (getLastError() != LocalDatabaseManager::Ok)
        return;

    while (index < studyListToDelete.count() && MbytesErased < MbytesToErase)
    {
        studyToDelete = studyListToDelete.at(index);

		emit studyWillBeDeleted(studyToDelete->getInstanceUID());
        MbytesErased += HardDiskInformation::getDirectorySizeInBytes(LocalDatabaseManager::getCachePath() + studyToDelete->getInstanceUID()) / 1024 / 1024;
		
        deleteStudy(studyToDelete->getInstanceUID());
        if (getLastError() != LocalDatabaseManager::Ok)
            break;

        index++;
    }

    ///Esborrem els estudis de la memòria
    foreach(Study *study, studyListToDelete)
    {
        delete study;
    } 
}

void LocalDatabaseManager::deleteStudyFromHardDisk(const QString &studyInstanceToDelete)
{
    DeleteDirectory deleteDirectory;

    //TODO:El Path del directori no s'hauria de calcular aquí
    if (!deleteDirectory.deleteDirectory(getStudyPath(studyInstanceToDelete), true))
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    else
       m_lastError = LocalDatabaseManager::Ok;
}

void LocalDatabaseManager::deleteSeriesFromHardDisk(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    DeleteDirectory deleteDirectory;

    //TODO:El Path del directori no s'hauria de calcular aquí
    if (!deleteDirectory.deleteDirectory(getStudyPath(studyInstanceUID) + QDir::separator() + seriesInstanceUID, true))
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    else
       m_lastError = LocalDatabaseManager::Ok;
}

void LocalDatabaseManager::createStudyThumbnails(Study *studyToGenerateSeriesThumbnails)
{
    foreach(Series *series, studyToGenerateSeriesThumbnails->getSeries())
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
    if( !QFileInfo( thumbnailFilePath ).exists() )
    {
        thumbnailCreator.getThumbnail(seriesToGenerateThumbnail).save( thumbnailFilePath, "PGM" );
    }
}

void LocalDatabaseManager::loadSeriesThumbnail(QString studyInstanceUID, QList<Series*> seriesList)
{
    QString thumbnailPath;

    foreach(Series *series, seriesList)
    {
        thumbnailPath = getSeriesThumbnailPath(studyInstanceUID, series);
        QFileInfo thumbnailFile(thumbnailPath);

        if (thumbnailFile.exists())
        {
            series->setThumbnail(QPixmap(thumbnailPath));
        }
    }
}

void LocalDatabaseManager::setLastError(int sqliteLastError)
{
    //Es tradueixen els errors de Sqlite a errors nostres, per consulta codi d'errors Sqlite http://www.sqlite.org/c3ref/c_abort.html
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
    else if (sqliteLastError == SQLITE_CORRUPT || sqliteLastError == SQLITE_EMPTY ||
             sqliteLastError == SQLITE_SCHEMA || sqliteLastError == SQLITE_MISMATCH ||
             sqliteLastError == SQLITE_NOTADB)
    {
        m_lastError = DatabaseCorrupted;
    }
    else m_lastError = DatabaseError;

    //El valor dels errors es pot consultar a http://www.sqlite.org/c3ref/c_abort.html
    if (sqliteLastError != SQLITE_OK)
    {
        ERROR_LOG("Codi error base de dades " + QString().setNum(sqliteLastError));
    }
}

QString LocalDatabaseManager::getSeriesThumbnailPath(QString studyInstanceUID, Series *series)
{
    return getStudyPath(studyInstanceUID) + "/" + series->getInstanceUID() + "/thumbnail.pgm";
}

QString LocalDatabaseManager::getDatabaseFilePath() 
{
    Settings settings;
    
    return QDir::toNativeSeparators( settings.getValue( InputOutputSettings::DatabaseAbsoluteFilePath ).toString() );
}

QString LocalDatabaseManager::getCachePath() 
{
    Settings settings;

    return QDir::toNativeSeparators( settings.getValue( InputOutputSettings::CachePath ).toString() );
}

int LocalDatabaseManager::queryDICOMReferencedImageInKeyImageNote(DatabaseConnection &dbConnect, KeyImageNote *keyImageNote) 
{
    LocalDatabaseDICOMReferencedImageDAL localDataBaseDICOMReferencedImageDAL;
    localDataBaseDICOMReferencedImageDAL.setDatabaseConnection(&dbConnect);

    DicomMask maskDICOMReferencedImagesToRetrieve;
    maskDICOMReferencedImagesToRetrieve.setSOPInstanceUID(keyImageNote->getInstanceUID());

    QList<DICOMReferencedImage*> DICOMReferencedImages = localDataBaseDICOMReferencedImageDAL.query(maskDICOMReferencedImagesToRetrieve);

    if (localDataBaseDICOMReferencedImageDAL.getLastError() == SQLITE_OK)
    {
        keyImageNote->setDICOMReferencedImages(DICOMReferencedImages);
    }

    return localDataBaseDICOMReferencedImageDAL.getLastError();
}
int LocalDatabaseManager::queryKeyImageNotes(DatabaseConnection &dbConnect, Series *series, DicomMask &maskToRetrieve)
{
    LocalDatabaseKeyImageNoteDAL keyImageNoteDAL;
    keyImageNoteDAL.setDatabaseConnection(&dbConnect);

    QList<KeyImageNote*> keyImageNotes = keyImageNoteDAL.query(maskToRetrieve);

    if (keyImageNoteDAL.getLastError() != SQLITE_OK) 
    {
        return keyImageNoteDAL.getLastError();
    }

    int status = SQLITE_OK;
    foreach (KeyImageNote *keyImageNote, keyImageNotes)
    {
        series->addKeyImageNote(keyImageNote);
        status = queryDICOMReferencedImageInKeyImageNote(dbConnect, keyImageNote);

        if (status != SQLITE_OK)
        {
            break;
        }
    }

    return status;
}
}
