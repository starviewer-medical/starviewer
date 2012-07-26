#include "localdatabasedisplayshutterdal.h"

#include <sqlite3.h>

#include "displayshutter.h"
#include "dicommask.h"
#include "databaseconnection.h"
#include "image.h"

namespace udg {

LocalDatabaseDisplayShutterDAL::LocalDatabaseDisplayShutterDAL(DatabaseConnection *dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

void LocalDatabaseDisplayShutterDAL::insert(const DisplayShutter &shutter, Image *shuttersImage)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSQLInsert(shutter, shuttersImage)), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(buildSQLInsert(shutter, shuttersImage));
    }
}

void LocalDatabaseDisplayShutterDAL::update(const QList<DisplayShutter> &shuttersList, Image *shuttersImage)
{
    // Fem un delete de tots els shutters existents i fem un insert de nou,
    // ja que la classe DisplayShuter no té l'ID de la taula DisplayShutter de la BdD
    DicomMask mask;
    mask.setImageNumber(QString::number(shuttersImage->getFrameNumber()));
    mask.setSOPInstanceUID(shuttersImage->getSOPInstanceUID());
    del(mask);
    foreach (const DisplayShutter &shutter, shuttersList)
    {
        insert(shutter, shuttersImage);
    }
}

void LocalDatabaseDisplayShutterDAL::del(const DicomMask &mask)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSQLDelete(mask)), 0, 0, 0);

    if (getLastError() != SQLITE_OK)
    {
        logError(buildSQLDelete(mask));
    }
}

QList<DisplayShutter> LocalDatabaseDisplayShutterDAL::query(const DicomMask &mask)
{
    int columns;
    int rows;
    char **reply = NULL;
    char **error = NULL;
    QList<DisplayShutter> shutterList;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(), qPrintable(buildSQLSelect(mask)), &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError(buildSQLSelect(mask));
        return shutterList;
    }

    // index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows; index++)
    {
        shutterList << fillDisplayShutter(reply, index, columns);
    }

    sqlite3_free_table(reply);

    return shutterList;
}

DisplayShutter LocalDatabaseDisplayShutterDAL::fillDisplayShutter(char **reply, int row, int columns)
{
    DisplayShutter shutter;

    QString shape = reply[0 + row * columns];
    if (shape == "RECTANGULAR")
    {
        shutter.setShape(DisplayShutter::RectangularShape);
    }
    else if (shape == "CIRCULAR")
    {
        shutter.setShape(DisplayShutter::CircularShape);
    }
    else if (shape == "POLYGONAL")
    {
        shutter.setShape(DisplayShutter::PolygonalShape);
    }
    else
    {
        shutter.setShape(DisplayShutter::UndefinedShape);
    }

    QString shutterPoints = reply[2 + row * columns];
    shutter.setPoints(shutterPoints);

    shutter.setShutterValue(QString("%1").arg(reply[1 + row * columns]).toUShort());

    return shutter;
}

QString LocalDatabaseDisplayShutterDAL::buildSQLSelect(const DicomMask &mask)
{
    QString selectSentence = "SELECT Shape, ShutterValue, PointsList FROM DisplayShutter ";

    return selectSentence + buildWhereSentence(mask);
}

QString LocalDatabaseDisplayShutterDAL::buildSQLInsert(const DisplayShutter &shutter, Image *shuttersImage)
{
    QString insertSentence = "INSERT INTO DisplayShutter ";
    insertSentence += "(Shape, ShutterValue, PointsList, ImageInstanceUID, ImageFrameNumber) ";
    insertSentence += QString("VALUES ('%1', %2, '%3', '%4', %5)")
        .arg(DatabaseConnection::formatTextToValidSQLSyntax(shutter.getShapeAsDICOMString()))
        .arg(shutter.getShutterValue())
        .arg(DatabaseConnection::formatTextToValidSQLSyntax(shutter.getPointsAsString()))
        .arg(DatabaseConnection::formatTextToValidSQLSyntax(shuttersImage->getSOPInstanceUID()))
        .arg(shuttersImage->getFrameNumber());

    return insertSentence;
}

QString LocalDatabaseDisplayShutterDAL::buildSQLDelete(const DicomMask &mask)
{
    return "DELETE FROM DisplayShutter " + buildWhereSentence(mask);
}

QString LocalDatabaseDisplayShutterDAL::buildWhereSentence(const DicomMask &mask)
{
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        QString whereSentence = QString("WHERE ImageInstanceUID = '%1' ").arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getSOPInstanceUID()));
        if (!mask.getImageNumber().isEmpty())
        {
            whereSentence += QString("AND ImageFrameNumber = %1").arg(mask.getImageNumber());
        }

        return whereSentence;
    }
    
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE SeriesInstanceUID = '%1')")
            .arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getSeriesInstanceUID()));
    }
    
    if (!mask.getStudyInstanceUID().isEmpty())
    {
        return QString("WHERE ImageInstanceUID IN (SELECT SOPInstanceUID FROM Image WHERE StudyInstanceUID = '%1')")
            .arg(DatabaseConnection::formatTextToValidSQLSyntax(mask.getStudyInstanceUID()));
    }
    
    return QString();
}

}
