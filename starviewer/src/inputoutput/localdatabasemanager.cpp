/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanager.h"

#include "patient.h"
#include "study.h"
#include "localdatabaseimagedal.h"
#include "localdatabaseseriesdal.h"
#include "localdatabasestudydal.h"
#include "localdatabasepatientdal.h"
#include "localdatabaseutildal.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "testdicomobjects.h"
#include "sqlite3.h"
#include "logging.h"
#include "deletedirectory.h"
#include "starviewersettings.h"
#include "harddiskinformation.h"
#include "thumbnailcreator.h"

#include <QDate>
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QPixmap>

namespace udg
{
//Nom de la llista de QSettings que guardarà els estudis que tenim en aquell moment descarregant
const QString LocalDatabaseManager::qsettingsRetrievingStudy = "/PACS/RetrievingStudy";
QDate LocalDatabaseManager::LastAccessDateSelectedStudies;

LocalDatabaseManager::LocalDatabaseManager()
{
    StarviewerSettings settings;

    if (!LocalDatabaseManager::LastAccessDateSelectedStudies.isValid())
    {
        LocalDatabaseManager::LastAccessDateSelectedStudies = QDate::currentDate().addDays(-settings.getMaximumDaysNotViewedStudy().toInt(NULL, 10));
    }
}

void LocalDatabaseManager::save(Patient *newPatient)
{
    DeleteDirectory delDirectory;
    StarviewerSettings settings;
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    DatabaseConnection dbConnect;
    int status = SQLITE_OK;

    dbConnect.open();
    dbConnect.beginTransaction(); 

    if (newPatient == NULL)
    {
        ERROR_LOG("No es pot inserir a la base de dades l'estudi que s'ha descarregat, perquè el patient és nul");
        m_lastError = PatientInconsistent;
        return;
    }

    ///Guardem primer els estudis
    if (newPatient->getStudies().count() > 0)
    {
        status = saveStudies(&dbConnect, newPatient->getStudies(), currentDate, currentTime);

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
        createSeriesThumbnails(study);
    }

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

QList<Series*> LocalDatabaseManager::querySeries(const DicomMask &seriesMaskToQuery)
{
    DatabaseConnection dbConnect;
    LocalDatabaseSeriesDAL seriesDAL;
    QList<Series*> queryResult;

    dbConnect.open();
    seriesDAL.setDatabaseConnection(&dbConnect);
    queryResult = seriesDAL.query(seriesMaskToQuery);
    setLastError(seriesDAL.getLastError());

    dbConnect.close();

    //Carreguem els thumbnails de les series consultades
    loadSeriesThumbnail(seriesMaskToQuery.getStudyUID(), queryResult);

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
    Patient *retrievedPatient;
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
    maskImagesToRetrieve.setStudyUID(maskToRetrieve.getStudyUID());//estudi del que s'han de cercar les imatges
    imageDAL.setDatabaseConnection(&dbConnect);

    foreach(Series *series, seriesList)
    {
        maskImagesToRetrieve.setSeriesUID(series->getInstanceUID());//específiquem de quina sèrie de l'estudi hem de buscar les imatges

        QList<Image*> images = imageDAL.query(maskImagesToRetrieve);
        if (imageDAL.getLastError() != SQLITE_OK) break;

        foreach(Image *image, images)
        {
            series->addImage(image);
        }

        retrievedPatient->getStudy(maskToRetrieve.getStudyUID())->addSeries(series);
    }

    if (imageDAL.getLastError() != SQLITE_OK)
    {
        setLastError(imageDAL.getLastError());
        dbConnect.close();
        return new Patient();
    }

    //Actulitzem la última data d'acces de l'estudi
    retrievedStudy = retrievedPatient->getStudy(maskToRetrieve.getStudyUID());
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

void LocalDatabaseManager::del(const QString &studyInstanceToDelete)
{
    DatabaseConnection dbConnect;
    StarviewerSettings settings;
    DicomMask studyMaskToDelete;
    int status;

    if (studyInstanceToDelete.isEmpty())
        return;
    else
        studyMaskToDelete.setStudyUID(studyInstanceToDelete);

    dbConnect.open();
    dbConnect.beginTransaction();

    status = delPatientOfStudy(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = delStudy(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = delSeries(&dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    status = delImage(&dbConnect, studyMaskToDelete);
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

void LocalDatabaseManager::clear()
{
    DicomMask maskToDelete;//creem màscara buida, ho esborrarà tot
    DatabaseConnection dbConnect;
    DeleteDirectory delDirectory;
    int status;

    dbConnect.open();
    dbConnect.beginTransaction();

    status = delPatient(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem tots els estudis
    status = delStudy(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem totes les series
    status = delSeries(&dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect.rollbackTransaction();
        setLastError(status);
        dbConnect.close();
        return;
    }

    //esborrem totes les imatges 
    status = delImage(&dbConnect, maskToDelete);
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
    if (!delDirectory.deleteDirectory(StarviewerSettings().getCacheImagePath(), true)) 
        m_lastError = DeletingFilesError;
    else 
        m_lastError = Ok;
}

void LocalDatabaseManager::deleteOldStudies()
{
    QDate lastDateViewedMinimum;
    StarviewerSettings settings;
    DicomMask oldStudiesMask;
    QList<Study*> studyListToDelete;
    LocalDatabaseStudyDAL studyDAL;
    DatabaseConnection dbConnect;

    dbConnect.open();
    studyDAL.setDatabaseConnection(&dbConnect);
    studyListToDelete = studyDAL.query(oldStudiesMask, LocalDatabaseManager::LastAccessDateSelectedStudies);

    setLastError(studyDAL.getLastError());

    if (studyDAL.getLastError() != SQLITE_OK)
    {
        dbConnect.close();
        return;
    }

    foreach(Study *study, studyListToDelete)
    {
        del(study->getInstanceUID());
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

bool LocalDatabaseManager::isEnoughSpace()
{
    HardDiskInformation hardDiskInformation;
    StarviewerSettings settings;
    quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(settings.getCacheImagePath());
    quint64 minimumSpaceRequired = quint64(settings.getMinimumSpaceRequiredToRetrieveInMbytes());
    quint64 MbytesToFree;

    if (freeSpaceInHardDisk < minimumSpaceRequired)
    {
        INFO_LOG("No hi ha suficient espai lliure per descarregar (" + QString().setNum(freeSpaceInHardDisk) + " Mb) " +
                 "s'intentarà esborrar estudis vells per alliberar suficient espai");

        //No hi ha suficient espai indiquem quina és la quantitat de Mb d'estudis vells que intentem alliberar. Aquest és el número de Mbytes fins arribar l'espai míninm necessari (minimumSpaceRequired - freeSpaceInHardDisk), més una quantitat fixa, per assegurar que disposem de prou espai per descarregar estudis grans, i no haver d'estar en cada descarrega alliberant espai
        MbytesToFree = (minimumSpaceRequired - freeSpaceInHardDisk) + MbytesToEraseWhereNotEnoughSpaceRequiredInHardDisk;

        freeSpaceDeletingStudies(MbytesToFree);
        if (getLastError() != LocalDatabaseManager::Ok)
        {
            ERROR_LOG("S'ha produït un error intentant alliberar espai");
            return false;
        }

        freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(settings.getCacheImagePath());//Tornem a consultar l'espai lliure

        if (freeSpaceInHardDisk < minimumSpaceRequired)
        {
            INFO_LOG("No hi ha suficient espai lliure per descarregar (" + QString().setNum(freeSpaceInHardDisk) + " Mb)");
            return false;
        }
        else
            return true;
    }

    return true;
}

QString LocalDatabaseManager::getStudyPath(const QString &studyInstanceUID)
{
    StarviewerSettings settings;

    return settings.getCacheImagePath() + studyInstanceUID;
}

LocalDatabaseManager::LastError LocalDatabaseManager::getLastError()
{
    return m_lastError;
}

bool LocalDatabaseManager::setStudyRetrieving(const QString &studyInstanceUID)
{
    QSettings qsettings;

    if (!qsettings.contains(qsettingsRetrievingStudy) && !studyInstanceUID.isEmpty())
    {
        qsettings.setValue(qsettingsRetrievingStudy, studyInstanceUID);
        return true;
    }
    else return false;
}

void LocalDatabaseManager::setStudyRetrieveFinished()
{
    QSettings qsettings;

    qsettings.remove(qsettingsRetrievingStudy);
}

void LocalDatabaseManager::checkNoStudiesRetrieving()
{
    QSettings qsettings;
    DeleteDirectory deleteDirectory;
    QDir directory;

    if (qsettings.contains(qsettingsRetrievingStudy))
    {
        QString studyNotFullRetrieved = qsettings.value(qsettingsRetrievingStudy).toString();

        INFO_LOG("L'estudi " + studyNotFullRetrieved + " s'estava descarregant al tancar-se la última execució de l'starviewer, per mantenir la integritat s'esborraran les imatges que se n'havien descarregat fins al moment");

        //Es pot donarper si es dona el cas que s'hagués arribat a inserir l'estudi i just abans d'indicar que la descàrrega de l'estudi havia finalitzat a través del mètode setStudyRetrieveFinished, s'hagués tancat l'starviewer per tant el mètode el detectaria com que l'estudi estés a mig descarregar quan realment està descarregat, per això comprovem si l'estudi existeix i si és el cas l'esborrem per deixa la base de dades en un estat consistent
        DicomMask studyMask;
        studyMask.setStudyUID(studyNotFullRetrieved);

        if (queryStudy(studyMask).count() > 0)
        {
            del(studyNotFullRetrieved);
        }//No s'ha arribat a inserir a la bd
        else 
        {
            //Comprovem si el directori existeix de l'estudi, per si no s'hagués arribat a baixar cap imatge, el 
            if (directory.exists(getStudyPath(studyNotFullRetrieved)))
                deleteStudyFromHardDisk(studyNotFullRetrieved);
        }

        qsettings.remove(qsettingsRetrievingStudy);
    }
    else m_lastError = Ok;
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
        ///primer guardem les imatges
        status = saveImages(dbConnect, seriesToSave->getImages(), currentDate, currentTime);

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
    int imageOrderInSeries = 0;
    int status = SQLITE_OK;

    foreach(Image* imageToSave, listImageToSave)
    {
        imageToSave->setRetrievedDate(currentDate);
        imageToSave->setRetrievedTime(currentTime);

        status = saveImage(dbConnect, imageToSave, imageOrderInSeries);

        if (status != SQLITE_OK) 
            return status;

        imageOrderInSeries++;
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

int LocalDatabaseManager::saveImage(DatabaseConnection *dbConnect, Image *imageToSave, int imageOrderInSeries)
{
    LocalDatabaseImageDAL imageDAL;

    imageDAL.setDatabaseConnection(dbConnect);

    imageDAL.insert(imageToSave, imageOrderInSeries);

    ///Si el pacient ja existia actualitzem la seva informació
    if (imageDAL.getLastError() == SQLITE_CONSTRAINT) imageDAL.update(imageToSave, imageOrderInSeries);

    return imageDAL.getLastError();
}

void LocalDatabaseManager::deleteRetrievedObjects(Patient *failedPatient)
{
    DeleteDirectory delDirectory;
    StarviewerSettings settings;

    foreach(Study *failedStudy, failedPatient->getStudies())
    {
        delDirectory.deleteDirectory(settings.getCacheImagePath() + failedStudy->getInstanceUID(), true);
    }
}

int LocalDatabaseManager::delPatientOfStudy(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
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

    if (patientList.count() == 1) 
    {
        patientID = patientList.at(0)->getID();
        delete patientList.at(0);//esborrem el pacient no el necessitem més
    }
    else 
        return -1;

    //busquem quants estudis té el pacient
    numberOfStudies = localDatabaseStudyDAL.countHowManyStudiesHaveAPatient(patientID);
    if (localDatabaseStudyDAL.getLastError() != SQLITE_OK)
        return localDatabaseStudyDAL.getLastError();

    if (numberOfStudies == 1) //si només té un estudi l'esborrem
    {
        DicomMask patientMaskToDelete;
        patientMaskToDelete.setPatientId(patientID);

        return delPatient(dbConnect, patientMaskToDelete);
    }
    else return localDatabaseStudyDAL.getLastError();
}

int LocalDatabaseManager::delPatient(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabasePatientDAL localDatabasePatientDAL;

    localDatabasePatientDAL.setDatabaseConnection(dbConnect);
    localDatabasePatientDAL.del(maskToDelete);

    return localDatabasePatientDAL.getLastError();
}

int LocalDatabaseManager::delStudy(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL;

    localDatabaseStudyDAL.setDatabaseConnection(dbConnect);
    localDatabaseStudyDAL.del(maskToDelete);

    return localDatabaseStudyDAL.getLastError();
}

int LocalDatabaseManager::delSeries(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseSeriesDAL localDatabaseSeriesDAL;

    localDatabaseSeriesDAL.setDatabaseConnection(dbConnect);
    localDatabaseSeriesDAL.del(maskToDelete);

    return localDatabaseSeriesDAL.getLastError();
}

int LocalDatabaseManager::delImage(DatabaseConnection *dbConnect, const DicomMask &maskToDelete)
{
    LocalDatabaseImageDAL localDatabaseImageDAL;

    localDatabaseImageDAL.setDatabaseConnection(dbConnect);
    localDatabaseImageDAL.del(maskToDelete);

    return localDatabaseImageDAL.getLastError();
}

void LocalDatabaseManager::freeSpaceDeletingStudies(quint64 MbytesToErase)
{
    QDate lastDateViewedMinimum;
    StarviewerSettings settings;
    DicomMask oldStudiesMask;
    QList<Study*> studyListToDelete;
    Study *studyToDelete;
    quint64 MbytesErased = 0;
    int index = 0;

    studyListToDelete = queryStudy(oldStudiesMask);
    if (getLastError() != LocalDatabaseManager::Ok)
        return;

    while (index < studyListToDelete.count() && MbytesErased < MbytesToErase)
    {
        studyToDelete = studyListToDelete.at(index);
        MbytesErased += getDirectorySize(settings.getCacheImagePath() + studyToDelete->getInstanceUID()) / 1024 / 1024;

        del(studyToDelete->getInstanceUID());
        if (getLastError() != LocalDatabaseManager::Ok)
            break;

        index++;
    }

    INFO_LOG("S'han alliberat " + QString().setNum(MbytesErased, 10) + " Mb");

    ///Esborrem els estudis de la memòria
    foreach(Study *study, studyListToDelete)
    {
        delete study;
    } 
}

void LocalDatabaseManager::deleteStudyFromHardDisk(const QString &studyInstanceToDelete)
{
    DeleteDirectory deleteDirectory;

    if (!deleteDirectory.deleteDirectory(getStudyPath(studyInstanceToDelete), true))
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    else
       m_lastError = LocalDatabaseManager::Ok;
}

void LocalDatabaseManager::createSeriesThumbnails(Study *studyToGenerateSeriesThumbnails)
{
    ThumbnailCreator thumbnailCreator;

    foreach(Series *series, studyToGenerateSeriesThumbnails->getSeries())
    {
        thumbnailCreator.getThumbnail(series).save(getSeriesThumbnailPath(studyToGenerateSeriesThumbnails->getInstanceUID(), series), "PGM");
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
}

qint64 LocalDatabaseManager::getDirectorySize(const QString &directoryPath)
{
    QDir directory(directoryPath);
    QFileInfoList fileInfoList;
    QStringList directoryList;
    qint64 directorySize = 0;

    fileInfoList =  directory.entryInfoList( QDir::Files );//llista de fitxers del directori

    foreach(QFileInfo fileInfo, fileInfoList)
    {
        directorySize += fileInfo.size();
    }

    directoryList =  directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);//obtenim llistat de subdirectoris

    foreach(QString subdirectory, directoryList) //per cada subdirectori
    {
        directorySize += getDirectorySize(directoryPath + "/" + subdirectory);
    } 

    return directorySize;
}

QString LocalDatabaseManager::getSeriesThumbnailPath(QString studyInstanceUID, Series *series)
{
    return getStudyPath(studyInstanceUID) + "/" + series->getInstanceUID() + "/thumbnail.pgm";
}

}
