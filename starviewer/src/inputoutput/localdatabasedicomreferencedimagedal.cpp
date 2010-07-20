/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "localdatabasedicomreferencedimagedal.h"

#include <sqlite3.h>

#include "databaseconnection.h"
#include "logging.h"
#include "dicomreferencedimage.h"
#include "dicommask.h"
#include "series.h"
#include "study.h"

namespace udg {

LocalDatabaseDICOMReferencedImageDAL::LocalDatabaseDICOMReferencedImageDAL()
{
    m_lastSqliteError = SQLITE_OK;
}

LocalDatabaseDICOMReferencedImageDAL::~LocalDatabaseDICOMReferencedImageDAL()
{
}

void LocalDatabaseDICOMReferencedImageDAL::insert(DICOMReferencedImage *newDICOMReferencedImage, Series *seriesOfParentObject)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newDICOMReferencedImage, seriesOfParentObject)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
    {
        logError(buildSqlInsert(newDICOMReferencedImage, seriesOfParentObject));
    }
}

void LocalDatabaseDICOMReferencedImageDAL::del(const DicomMask &dicomReferencedImageMaskToDelete)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlDelete(dicomReferencedImageMaskToDelete)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
    {
        logError(buildSqlDelete(dicomReferencedImageMaskToDelete));
    }
}

QList<DICOMReferencedImage*> LocalDatabaseDICOMReferencedImageDAL::query(const DicomMask &DICOMReferencedImageMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<DICOMReferencedImage*> DICOMReferencedImageList;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(DICOMReferencedImageMask)),
                                    &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(DICOMReferencedImageMask));
        return DICOMReferencedImageList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows; index++)
    {
        DICOMReferencedImageList.append(fillDICOMReferencedImage(reply, index, columns));
    }

    return DICOMReferencedImageList;
}

void LocalDatabaseDICOMReferencedImageDAL::setDatabaseConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabaseDICOMReferencedImageDAL::getLastError() const
{
    return m_lastSqliteError;
}

DICOMReferencedImage* LocalDatabaseDICOMReferencedImageDAL::fillDICOMReferencedImage(char **reply, int row, int columns)
{
    DICOMReferencedImage *referencedImage = new DICOMReferencedImage();

    referencedImage->setDICOMReferencedImageSOPInstanceUID(reply[0 + row * columns]);
    referencedImage->setFrameNumber(QString(reply[1 + row * columns]).toInt());
    referencedImage->setReferenceParentSOPInstanceUID(reply[2 + row * columns]);

    return referencedImage;
}

void LocalDatabaseDICOMReferencedImageDAL::logError(const QString &sqlSentence)
{
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

QString LocalDatabaseDICOMReferencedImageDAL::buildSqlInsert(DICOMReferencedImage *newDICOMReferencedImage, Series *seriesOfParentObject)
{
    QString insertSentence = QString ("Insert into DICOMReferencedImage   (DICOMReferencedImageSOPInstanceUID, FrameNumber, ReferenceParentSOPInstanceUID, "
                                                                           "ReferenceParentStudyInstanceUID, ReferenceParentSeriesInstanceUID) "
                                                                           "values ('%1','%2','%3','%4','%5')")
                                                                            .arg(DatabaseConnection::formatTextToValidSQLSyntax(newDICOMReferencedImage->getDICOMReferencedImageSOPInstanceUID()))
                                                                            .arg(DatabaseConnection::formatTextToValidSQLSyntax(newDICOMReferencedImage->getFrameNumber()))
                                                                            .arg(DatabaseConnection::formatTextToValidSQLSyntax(newDICOMReferencedImage->getReferenceParentSOPInstanceUID()))
                                                                            .arg(DatabaseConnection::formatTextToValidSQLSyntax(seriesOfParentObject->getParentStudy()->getInstanceUID()))
                                                                            .arg(DatabaseConnection::formatTextToValidSQLSyntax(seriesOfParentObject->getInstanceUID()));

    return insertSentence;
}

QString LocalDatabaseDICOMReferencedImageDAL::buildSqlDelete(const DicomMask &DICOMReferencedImageMaskToDelete)
{
    return "delete from DICOMReferencedImage " + buildWhereSentence(DICOMReferencedImageMaskToDelete);
}

QString LocalDatabaseDICOMReferencedImageDAL::buildSqlSelect(const DicomMask &dicomReferencedImageMaskToSelect)
{
    QString selectSentence = "Select DICOMReferencedImageSOPInstanceUID, FrameNumber, ReferenceParentSOPInstanceUID, "
                                     "ReferenceParentStudyInstanceUID, ReferenceParentSeriesInstanceUID "
                                     "from DICOMReferencedImage ";

    return selectSentence + buildWhereSentence(dicomReferencedImageMaskToSelect);
}

QString LocalDatabaseDICOMReferencedImageDAL::buildWhereSentence(const DicomMask &DICOMReferencedImageMask)
{
    QString whereSentence = "";

    if (!DICOMReferencedImageMask.getStudyInstanceUID().isEmpty()) 
    {
        whereSentence = QString("where ReferenceParentStudyInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(DICOMReferencedImageMask.getStudyInstanceUID()));
    }

    if (!DICOMReferencedImageMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" ReferenceParentSeriesInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(DICOMReferencedImageMask.getSeriesInstanceUID()));
    }

    if (!DICOMReferencedImageMask.getSOPInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty())
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" ReferenceParentSOPInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(DICOMReferencedImageMask.getSOPInstanceUID()));
    }

    return whereSentence;
}
}