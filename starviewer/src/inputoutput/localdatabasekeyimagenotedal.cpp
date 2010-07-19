/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "localdatabasekeyimagenotedal.h"

#include <sqlite3.h>

#include "keyimagenote.h"
#include "series.h"
#include "study.h"
#include "dicommask.h"
#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseKeyImageNoteDAL::LocalDatabaseKeyImageNoteDAL()
{
    m_lastSqliteError = SQLITE_OK;
}

LocalDatabaseKeyImageNoteDAL::~LocalDatabaseKeyImageNoteDAL()
{
}

void LocalDatabaseKeyImageNoteDAL::insert(KeyImageNote *newKeyImageNote)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newKeyImageNote)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
    {
        logError(buildSqlInsert(newKeyImageNote));
    }
}

void LocalDatabaseKeyImageNoteDAL::update(KeyImageNote *keyImageNoteToUpdate)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(keyImageNoteToUpdate)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
    {
        logError(buildSqlUpdate(keyImageNoteToUpdate));
    }
}

void LocalDatabaseKeyImageNoteDAL::del(const DicomMask &keyImageNoteMaskToDelete)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlDelete(keyImageNoteMaskToDelete)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) 
    {
        logError(buildSqlDelete(keyImageNoteMaskToDelete));
    }
}

QList<KeyImageNote*> LocalDatabaseKeyImageNoteDAL::query(const DicomMask &keyImageNoteMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<KeyImageNote*> keyImageNoteList;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(keyImageNoteMask)),
                                    &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(keyImageNoteMask));
        return keyImageNoteList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows; index++)
    {
        keyImageNoteList.append(fillKeyImageNote(reply, index, columns));
    }

    return keyImageNoteList;
}

void LocalDatabaseKeyImageNoteDAL::setDatabaseConnection(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

int LocalDatabaseKeyImageNoteDAL::getLastError() const
{
    return m_lastSqliteError;
}

KeyImageNote* LocalDatabaseKeyImageNoteDAL::fillKeyImageNote(char **reply, int row, int columns)
{
    KeyImageNote *keyImageNote = new KeyImageNote();

    keyImageNote->setInstanceUID(reply[0 + row * columns]);
    keyImageNote->setInstanceNumber(reply[3 + row * columns]);
    keyImageNote->setContentDate(QDate().fromString(reply[4 + row * columns], "yyyyMMdd"));
    keyImageNote->setContentTime(QTime().fromString(reply[5 + row * columns], "hhmmss"));
    keyImageNote->setDocumentTitle(KeyImageNote::DocumentTitle(atoi(reply[6 + row * columns])));
    keyImageNote->setRejectedForQualityReasons(KeyImageNote::RejectedForQualityReasons(atoi(reply[7 + row * columns])));
    keyImageNote->setKeyObjectDescription(reply[8 + row * columns]);
    keyImageNote->setObserverContextType(KeyImageNote::ObserverType(atoi(reply[9 + row * columns])));
    keyImageNote->setObserverContextName(reply[10 + row * columns]);
    keyImageNote->setRetrievedDate(QDate().fromString(reply[11 + row * columns], "yyyyMMdd"));
    keyImageNote->setRetrievedTime(QTime().fromString(reply[12 + row * columns], "hhmmss"));

    return keyImageNote;
}

void LocalDatabaseKeyImageNoteDAL::logError(const QString &sqlSentence)
{
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

QString LocalDatabaseKeyImageNoteDAL::buildSqlInsert(KeyImageNote *newKeyImageNote)
{
    QString insertSentence = QString ("Insert into KeyImageNote   (SOPInstanceUID, StudyInstanceUID, SeriesInstanceUID, "
                                                                   "InstanceNumber, ContentDate, ContentTime, DocumentTitle, "
                                                                   "RejectedForQualityReasons, Description, ObserverContextType, "
                                                                   "ObserverContextName, RetrievedDate, RetrievedTime, State) "
                                                                   "values ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11', "
                                                                   "'%12','%13','%14')")
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getInstanceUID()))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getParentSeries()->getParentStudy()->getInstanceUID()))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getParentSeries()->getInstanceUID()))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getInstanceNumber()))
                                                                    .arg(newKeyImageNote->getContentDate().toString("yyyyMMdd"))
                                                                    .arg(newKeyImageNote->getContentTime().toString("hhmmss"))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString::number(newKeyImageNote->getDocumentTitle())))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString::number(newKeyImageNote->getRejectedForQualityReasons())))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getKeyObjectDescription()))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString::number(newKeyImageNote->getObserverContextType())))
                                                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(newKeyImageNote->getObserverContextName()))
                                                                    .arg(newKeyImageNote->getRetrievedDate().toString("yyyyMMdd"))
                                                                    .arg(newKeyImageNote->getRetrievedTime().toString("hhmmss"))
                                                                    .arg("0");

    return insertSentence;
}

QString LocalDatabaseKeyImageNoteDAL::buildSqlUpdate(KeyImageNote *keyImageNoteToUpdate)
{
    QString updateSentence = QString ("Update KeyImageNote Set StudyInstanceUID = '%1', " 
                                                        "SeriesInstanceUID = '%2', "
                                                        "InstanceNumber = '%3', "
                                                        "ContentDate = '%4', " 
                                                        "ContentTime = '%5', "
                                                        "DocumentTitle = '%6', "
                                                        "RejectedForQualityReasons = '%7', "
                                                        "Description = '%8', "
                                                        "ObserverContextType = '%9', "
                                                        "ObserverContextName = '%10', "
                                                        "RetrievedDate = '%11', "
                                                        "RetrievedTime = '%12', "
                                                        "State = '%13' "
                                                 "Where SOPInstanceUID = '%14'")
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getParentSeries()->getParentStudy()->getInstanceUID()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getParentSeries()->getInstanceUID()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getInstanceNumber()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString("contentDate")))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString("contentTime")))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString("documentTitle")))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString("rejected")))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getKeyObjectDescription()))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(QString("Observer Type")))
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getObserverContextName()))
                                    .arg(keyImageNoteToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                                    .arg(keyImageNoteToUpdate->getRetrievedTime().toString("hhmmss"))
                                    .arg("0")
                                    .arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteToUpdate->getInstanceUID()));
    
    return updateSentence;
}

QString LocalDatabaseKeyImageNoteDAL::buildSqlSelect(const DicomMask &keyImageNoteMaskToSelect)
{
    QString selectSentence = "Select SOPInstanceUID, StudyInstanceUID, SeriesInstanceUID, "
                                     "InstanceNumber, ContentDate, ContentTime, DocumentTitle, "
                                     "RejectedForQualityReasons, Description, ObserverContextType, "
                                     "ObserverContextName, RetrievedDate, RetrievedTime, State "
                            "from KeyImageNote ";

    return selectSentence + buildWhereSentence(keyImageNoteMaskToSelect);
}

QString LocalDatabaseKeyImageNoteDAL::buildSqlDelete(const DicomMask &keyImageNoteMaskToDelete)
{
    return "delete from KeyImageNote " + buildWhereSentence(keyImageNoteMaskToDelete);
}

QString LocalDatabaseKeyImageNoteDAL::buildWhereSentence(const DicomMask &keyImageNoteMask)
{
    QString whereSentence = "";

    if (!keyImageNoteMask.getStudyInstanceUID().isEmpty()) 
    {
        whereSentence = QString("where StudyInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteMask.getStudyInstanceUID()));
    }

    if (!keyImageNoteMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" SeriesInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteMask.getSeriesInstanceUID()));
    }

    if (!keyImageNoteMask.getSOPInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty())
        {
            whereSentence = "where";
        }
        else
        {   
            whereSentence += " and ";
        }

        whereSentence += QString(" SOPInstanceUID = '%1'").arg(DatabaseConnection::formatTextToValidSQLSyntax(keyImageNoteMask.getSOPInstanceUID()));
    }

    return whereSentence;
}

}