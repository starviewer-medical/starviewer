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

Status LocalDatabaseManager::insert(Patient *newPatient)
{
    Status state;
    state.setStatus("Normal", true, 0);
    int status = SQLITE_OK;

    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    dbConnect->beginTransaction();

    ///Guardem primer els estudis
    if (newPatient->getStudies().count() > 0)
    {
        status = saveStudies(dbConnect, newPatient->getStudies());

        if (status != SQLITE_OK) return state;
    }

    status = savePatient(dbConnect, newPatient);

    dbConnect->endTransaction();

    return state;
}

QList<Patient*> LocalDatabaseManager::queryPatient(DicomMask patientMaskToQuery)
{
    LocalDatabasePatientDAL patientDAL;

    patientDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());

    return patientDAL.query(patientMaskToQuery);
}

QList<Study*> LocalDatabaseManager::queryStudy(DicomMask studyMaskToQuery)
{
    LocalDatabaseStudyDAL studyDAL;

    studyDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());

    return studyDAL.query(studyMaskToQuery);
}

QList<Series*> LocalDatabaseManager::querySeries(DicomMask seriesMaskToQuery)
{
    LocalDatabaseSeriesDAL seriesDAL;

    seriesDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());

    return seriesDAL.query(seriesMaskToQuery);
}

QList<Image*> LocalDatabaseManager::queryImage(DicomMask imageMaskToQuery)
{
    LocalDatabaseImageDAL imageDAL;

    imageDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());

    return imageDAL.query(imageMaskToQuery);
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
        return;
    }

    //esborrem tots els estudis
    studyDAL.setDatabaseConnection(dbConnect);
    studyDAL.del(maskToDelete);

    if (studyDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        return;
    }
    seriesDAL.setDatabaseConnection(dbConnect);
    seriesDAL.del(maskToDelete);

    if (seriesDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        return;
    }

    //esborrem totes les imatges 
    imageDAL.setDatabaseConnection(dbConnect);
    imageDAL.del(maskToDelete);

    if (imageDAL.getLastError() != SQLITE_OK)
    {
        dbConnect->rollbackTransaction();
        return;
    }

    dbConnect->endTransaction();

    //esborrem tots els estudis descarregats
    delDirectory.deleteDirectory(StarviewerSettings().getCacheImagePath(), false);
}

void LocalDatabaseManager::compact()
{
    LocalDatabaseUtilDAL utilDAL;

    utilDAL.setDatabaseConnection(DatabaseConnection::getDatabaseConnection());

    utilDAL.compact();
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

}
