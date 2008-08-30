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

#include <QDate>
#include <QTime>

namespace udg
{

LocalDatabaseManager::LocalDatabaseManager()
{

}

void LocalDatabaseManager::insert(Patient *newPatient)
{
    DeleteDirectory delDirectory;
    StarviewerSettings settings;
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();

    int status = SQLITE_OK;

    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    dbConnect->beginTransaction();

    ///Guardem primer els estudis
    if (newPatient->getStudies().count() > 0)
    {
        status = saveStudies(dbConnect, newPatient->getStudies(), currentDate, currentTime);

        if (status != SQLITE_OK) 
        {
            dbConnect->rollbackTransaction();
            deleteRetrievedObjects(newPatient);
            setLastError(status);
            return;
        }
    }

    status = savePatient(dbConnect, newPatient);

    if (status != SQLITE_OK) 
    {
        dbConnect->rollbackTransaction();
        deleteRetrievedObjects(newPatient);
    }
    else
        dbConnect->endTransaction();

    setLastError(status);
}

QList<Patient*> LocalDatabaseManager::queryPatient(DicomMask patientMaskToQuery)
{
    LocalDatabasePatientDAL patientDAL;
    QList<Patient*> queryResult;

    patientDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    queryResult = patientDAL.query(patientMaskToQuery);
    setLastError(patientDAL.getLastError());

    return queryResult;
}

QList<Patient*> LocalDatabaseManager::queryPatientStudy(DicomMask patientStudyMaskToQuery)
{
    LocalDatabaseStudyDAL studyDAL;
    QList<Patient*> queryResult;

    studyDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    queryResult = studyDAL.queryPatientStudy(patientStudyMaskToQuery);
    setLastError(studyDAL.getLastError());

    return queryResult;
}

QList<Study*> LocalDatabaseManager::queryStudy(DicomMask studyMaskToQuery)
{
    LocalDatabaseStudyDAL studyDAL;
    QList<Study*> queryResult;

    studyDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    queryResult = studyDAL.query(studyMaskToQuery);
    setLastError(studyDAL.getLastError());

    return queryResult;
}

QList<Series*> LocalDatabaseManager::querySeries(DicomMask seriesMaskToQuery)
{
    LocalDatabaseSeriesDAL seriesDAL;
    QList<Series*> queryResult;

    seriesDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    queryResult = seriesDAL.query(seriesMaskToQuery);
    setLastError(seriesDAL.getLastError());

    return queryResult;
}

QList<Image*> LocalDatabaseManager::queryImage(DicomMask imageMaskToQuery)
{
    LocalDatabaseImageDAL imageDAL;
    QList<Image*> queryResult;

    imageDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    queryResult = imageDAL.query(imageMaskToQuery);
    setLastError(imageDAL.getLastError());

    return queryResult;
}

Patient* LocalDatabaseManager::retrieve(DicomMask maskToRetrieve)
{
    LocalDatabaseStudyDAL studyDAL;
    LocalDatabaseSeriesDAL seriesDAL;
    LocalDatabaseImageDAL imageDAL;
    QList<Patient*> patientList;
    QList<Series*> seriesList;
    Patient *retrievedPatient;
    Study *retrievedStudy;
    DicomMask maskImagesToRetrieve;
    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    //busquem l'estudi i pacient
    studyDAL.setDatabaseConnection(dbConnect);
    patientList = studyDAL.queryPatientStudy(maskToRetrieve);

    if (patientList.count() != 1) 
    {
        setLastError(studyDAL.getLastError());
        return retrievedPatient;
    }
    else 
        retrievedPatient = patientList.at(0);

    //busquem les series de l'estudi
    seriesDAL.setDatabaseConnection(dbConnect);
    seriesList = seriesDAL.query(maskToRetrieve);

    if (seriesDAL.getLastError() != SQLITE_OK)
    {
        setLastError(seriesDAL.getLastError());
        return new Patient();
    }

    //busquem les imatges per cada sèrie
    maskImagesToRetrieve.setStudyUID(maskToRetrieve.getStudyUID());//estudi del que s'han de cercar les imatges
    imageDAL.setDatabaseConnection(dbConnect);

    foreach(Series *series, seriesList)
    {
        maskImagesToRetrieve.setSeriesUID(series->getInstanceUID());//específiquem de quina sèrie de l'estudi hem de buscar les imatges

        series->setImages(imageDAL.query(maskImagesToRetrieve));
        if (imageDAL.getLastError() != SQLITE_OK) break;

        retrievedPatient->getStudy(maskToRetrieve.getStudyUID())->addSeries(series);
    }

    if (imageDAL.getLastError() != SQLITE_OK)
    {
        setLastError(imageDAL.getLastError());
        return new Patient();
    }

    //Actulitzem la última data d'acces de l'estudi
    retrievedStudy = retrievedPatient->getStudy(maskToRetrieve.getStudyUID());
    studyDAL.update(retrievedStudy, QDate::currentDate());
    setLastError(studyDAL.getLastError());

    return retrievedPatient;
}

void LocalDatabaseManager::del(QString studyInstanceToDelete)
{
    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();
    StarviewerSettings settings;
    DeleteDirectory deleteDirectory;
    DicomMask studyMaskToDelete;
    int status;

    if (studyInstanceToDelete.isEmpty()) 
        return;
    else
        studyMaskToDelete.setStudyUID(studyInstanceToDelete);

    dbConnect->beginTransaction();

    status = delPatientOfStudy(dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    status = delStudy(dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    status = delSeries(dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    status = delImage(dbConnect, studyMaskToDelete);
    if (status != SQLITE_OK) 
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    dbConnect->endTransaction();

    if (!deleteDirectory.deleteDirectory(settings.getCacheImagePath() + studyInstanceToDelete, true))
        m_lastError = LocalDatabaseManager::DeletingFilesError;
    else
       m_lastError = LocalDatabaseManager::Ok;
}

void LocalDatabaseManager::clear()
{
    DicomMask maskToDelete;//creem màscara buida, ho esborrarà tot
    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();
    DeleteDirectory delDirectory;
    int status;

    dbConnect->beginTransaction();

    status = delPatient(dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    //esborrem tots els estudis
    status = delStudy(dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    //esborrem totes les series
    status = delSeries(dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    //esborrem totes les imatges 
    status = delImage(dbConnect, maskToDelete);
    if (status != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(status);
        return;
    }

    dbConnect->endTransaction();

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

    lastDateViewedMinimum = QDate::currentDate().addDays(-settings.getMaximumDaysNotViewedStudy().toInt(NULL, 10));
    oldStudiesMask.setLastAccessDate(lastDateViewedMinimum);

    studyListToDelete = queryStudy(oldStudiesMask);
    if (getLastError() != LocalDatabaseManager::Ok)
        return;

    foreach(Study *study, studyListToDelete)
    {
        del(study->getInstanceUID());
        if (getLastError() != LocalDatabaseManager::Ok)
            break;
    } 
}

void LocalDatabaseManager::compact()
{
    LocalDatabaseUtilDAL utilDAL;

    utilDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());
    utilDAL.compact();
    setLastError(utilDAL.getLastError());
}

LocalDatabaseManager::LastError LocalDatabaseManager::getLastError()
{
    return m_lastError;
}

int LocalDatabaseManager::saveStudies(DatabaseConnection *dbConnect, QList<Study*> listStudyToSave, QDate currentDate, QTime currentTime)
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

int LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, QList<Series*> listSeriesToSave, QDate currentDate, QTime currentTime)
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

int LocalDatabaseManager::saveImages(DatabaseConnection *dbConnect, QList<Image*> listImageToSave, QDate currentDate, QTime currentTime)
{
    int imageOrderInSeries = 0;
    int status = SQLITE_OK;

    foreach(Image* imageToSave, listImageToSave)
    {
        imageToSave->setRetrievedDate(currentDate);
        imageToSave->setRetrievedTime(currentTime);

        status = saveImage(dbConnect, imageToSave, imageOrderInSeries);

        if (status != SQLITE_OK) return status;
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

int LocalDatabaseManager::delPatientOfStudy(DatabaseConnection *dbConnect, DicomMask maskToDelete)
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

int LocalDatabaseManager::delPatient(DatabaseConnection *dbConnect, DicomMask maskToDelete)
{
    LocalDatabasePatientDAL localDatabasePatientDAL;

    localDatabasePatientDAL.setDatabaseConnection(dbConnect);
    localDatabasePatientDAL.del(maskToDelete);

    return localDatabasePatientDAL.getLastError();
}

int LocalDatabaseManager::delStudy(DatabaseConnection *dbConnect, DicomMask maskToDelete)
{
    LocalDatabaseStudyDAL localDatabaseStudyDAL;

    localDatabaseStudyDAL.setDatabaseConnection(dbConnect);
    localDatabaseStudyDAL.del(maskToDelete);

    return localDatabaseStudyDAL.getLastError();
}

int LocalDatabaseManager::delSeries(DatabaseConnection *dbConnect, DicomMask maskToDelete)
{
    LocalDatabaseSeriesDAL localDatabaseSeriesDAL;

    localDatabaseSeriesDAL.setDatabaseConnection(dbConnect);
    localDatabaseSeriesDAL.del(maskToDelete);

    return localDatabaseSeriesDAL.getLastError();
}

int LocalDatabaseManager::delImage(DatabaseConnection *dbConnect, DicomMask maskToDelete)
{
    LocalDatabaseImageDAL localDatabaseImageDAL;

    localDatabaseImageDAL.setDatabaseConnection(dbConnect);
    localDatabaseImageDAL.del(maskToDelete);

    return localDatabaseImageDAL.getLastError();
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

}
