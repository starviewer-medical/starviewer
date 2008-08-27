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

namespace udg
{

LocalDatabaseManager::LocalDatabaseManager()
{

}

void LocalDatabaseManager::insert(Patient *newPatient)
{
    DeleteDirectory delDirectory;
    StarviewerSettings settings;
 
    int status = SQLITE_OK;

    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    dbConnect->beginTransaction();

    ///Guardem primer els estudis
    if (newPatient->getStudies().count() > 0)
    {
        status = saveStudies(dbConnect, newPatient->getStudies());

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

void LocalDatabaseManager::clear()
{
    DicomMask maskToDelete;
    LocalDatabasePatientDAL patientDAL;
    LocalDatabaseStudyDAL studyDAL;
    LocalDatabaseSeriesDAL seriesDAL;
    LocalDatabaseImageDAL imageDAL;
    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();
    DeleteDirectory delDirectory;

    dbConnect->beginTransaction();

    //esborrem tots els pacients
    patientDAL.setDatabaseConnection(dbConnect);
    patientDAL.del(maskToDelete);

    if (patientDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(patientDAL.getLastError());
        return;
    }

    //esborrem tots els estudis
    studyDAL.setDatabaseConnection(dbConnect);
    studyDAL.del(maskToDelete);

    if (studyDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(studyDAL.getLastError());
        return;
    }

    //esborrem totes les series
    seriesDAL.setDatabaseConnection(dbConnect);
    seriesDAL.del(maskToDelete);

    if (seriesDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(seriesDAL.getLastError());
        return;
    }

    //esborrem totes les imatges 
    imageDAL.setDatabaseConnection(dbConnect);
    imageDAL.del(maskToDelete);

    if (imageDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        setLastError(imageDAL.getLastError());
        return;
    }

    dbConnect->endTransaction();

    //esborrem tots els estudis descarregats
    if (!delDirectory.deleteDirectory(StarviewerSettings().getCacheImagePath(), true)) m_lastError = DeletingFilesError;
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

int LocalDatabaseManager::saveStudies(DatabaseConnection *dbConnect, QList<Study*> listStudyToSave)
{
    int status = SQLITE_OK;

    foreach(Study* studyToSave, listStudyToSave)
    {
        ///primer guardem les sèries
        status = saveSeries(dbConnect, studyToSave->getSeries());

        if (status != SQLITE_OK)
            return status;
        else //Guardem la sèrie si no s'ha produït cap error
            status = saveStudy(dbConnect, studyToSave);

        if (status != SQLITE_OK) return status;
    }

    return status;
}

int LocalDatabaseManager::saveSeries(DatabaseConnection *dbConnect, QList<Series*> listSeriesToSave)
{
    int status = SQLITE_OK;

    foreach(Series* seriesToSave, listSeriesToSave)
    {
        ///primer guardem les imatges
        status = saveImages(dbConnect, seriesToSave->getImages());

        if (status != SQLITE_OK)
            return status;
        else //Guardem la sèrie si no s'ha produït cap error
            status = saveSeries(dbConnect, seriesToSave);

        if (status != SQLITE_OK) return status;
    }

    return status;
}

int LocalDatabaseManager::saveImages(DatabaseConnection *dbConnect, QList<Image*> listImageToSave)
{
    int imageOrderInSeries = 0;
    int status = SQLITE_OK;

    foreach(Image* imageToSave, listImageToSave)
    {
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
