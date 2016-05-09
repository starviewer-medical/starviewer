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

#include "localdatabaseencapsulateddocumentdal.h"

#include "dicommask.h"
#include "encapsulateddocument.h"
#include "localdatabasemanager.h"
#include "localdatabasepacsretrievedimagesdal.h"
#include "series.h"
#include "study.h"

#include <QSqlQuery>
#include <QVariant>

namespace udg {

namespace {

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask.
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand)
{
    QString where;

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        where += "StudyInstanceUID = :studyInstanceUID";
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "SeriesInstanceUID = :seriesInstanceUID";
    }
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "SOPInstanceUID = :sopInstanceUID";
    }
    if (!where.isEmpty())
    {
        where = " WHERE " + where;
    }

    query.prepare(sqlCommand + where);

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":studyInstanceUID", mask.getStudyInstanceUID());
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.bindValue(":seriesInstanceUID", mask.getSeriesInstanceUID());
    }
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        query.bindValue(":sopInstanceUID", mask.getSOPInstanceUID());
    }
}

}

LocalDatabaseEncapsulatedDocumentDAL::LocalDatabaseEncapsulatedDocumentDAL(DatabaseConnection &databaseConnection)
    : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseEncapsulatedDocumentDAL::insert(const EncapsulatedDocument *document)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO EncapsulatedDocument (SOPInstanceUID, TransferSyntaxUID, InstanceNumber, DocumentTitle, MimeTypeOfEncapsulatedDocument, "
                                                    "RetrievedPacsID, StudyInstanceUID, SeriesInstanceUID) "
                  "VALUES (:sopInstanceUID, :transferSyntaxUID, :instanceNumber, :documentTitle, :mimeTypeOfEncapsulatedDocument, "
                          ":retrievedPacsId, :studyInstanceUID, :seriesInstanceUID)");
    bindValues(query, document);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseEncapsulatedDocumentDAL::update(const EncapsulatedDocument *document)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE EncapsulatedDocument SET TransferSyntaxUID = :transferSyntaxUID, InstanceNumber = :instanceNumber, DocumentTitle = :documentTitle, "
                                                  "MimeTypeOfEncapsulatedDocument = :mimeTypeOfEncapsulatedDocument, RetrievedPacsID = :retrievedPacsId, "
                                                  "StudyInstanceUID = :studyInstanceUID, SeriesInstanceUID = :seriesInstanceUID "
                  "WHERE SOPInstanceUID = :sopInstanceUID");
    bindValues(query, document);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseEncapsulatedDocumentDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "DELETE FROM EncapsulatedDocument");
    return executeQueryAndLogError(query);
}

QList<EncapsulatedDocument*> LocalDatabaseEncapsulatedDocumentDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    QString select("SELECT SOPInstanceUID, TransferSyntaxUID, InstanceNumber, DocumentTitle, MimeTypeOfEncapsulatedDocument, RetrievedPacsID, "
                          "StudyInstanceUID, SeriesInstanceUID "
                   "FROM EncapsulatedDocument");
    prepareQueryWithMask(query, mask, select);
    QList<EncapsulatedDocument*> documentList;

    if (executeQueryAndLogError(query))
    {
        while (query.next())
        {
            documentList.append(getEncapsulatedDocument(query));
        }
    }

    return documentList;
}

int LocalDatabaseEncapsulatedDocumentDAL::count(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT count(*) FROM EncapsulatedDocument");

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

void LocalDatabaseEncapsulatedDocumentDAL::bindValues(QSqlQuery &query, const EncapsulatedDocument *document)
{
    query.bindValue(":sopInstanceUID", document->getSopInstanceUid());
    query.bindValue(":transferSyntaxUID", document->getTransferSyntaxUid());
    query.bindValue(":instanceNumber", document->getInstanceNumber());
    query.bindValue(":documentTitle", document->getDocumentTitle());
    query.bindValue(":mimeTypeOfEncapsulatedDocument", document->getMimeTypeOfEncapsulatedDocument());
    query.bindValue(":retrievedPacsId", getDatabasePacsId(document->getDicomSource()));
    query.bindValue(":studyInstanceUID", document->getParentSeries()->getParentStudy()->getInstanceUID());
    query.bindValue(":seriesInstanceUID", document->getParentSeries()->getInstanceUID());
}

EncapsulatedDocument* LocalDatabaseEncapsulatedDocumentDAL::getEncapsulatedDocument(const QSqlQuery &query)
{
    EncapsulatedDocument *document = new EncapsulatedDocument();
    document->setSopInstanceUid(query.value("SOPInstanceUID").toString());
    document->setTransferSyntaxUid(query.value("TransferSyntaxUID").toString());
    document->setInstanceNumber(query.value("InstanceNumber").toString());
    document->setDocumentTitle(query.value("DocumentTitle").toString());
    document->setMimeTypeOfEncapsulatedDocument(query.value("MimeTypeOfEncapsulatedDocument").toString());
    document->setDicomSource(getDicomSource(query.value("RetrievedPACSID")));
    document->setPath(LocalDatabaseManager::getCachePath() + query.value("StudyInstanceUID").toString() + "/" + query.value("SeriesInstanceUID").toString() +
                      "/" + query.value("SOPInstanceUID").toString());
    return document;
}

QVariant LocalDatabaseEncapsulatedDocumentDAL::getDatabasePacsId(const DICOMSource &dicomSource)
{
    if (dicomSource.getRetrievePACS().isEmpty())
    {
        return QVariant(QVariant::LongLong);
    }

    return getDatabasePacsId(dicomSource.getRetrievePACS().first());
}

QVariant LocalDatabaseEncapsulatedDocumentDAL::getDatabasePacsId(const PacsDevice &pacsDevice)
{
    QString key = pacsDevice.getAddress() + QString::number(pacsDevice.getQueryRetrieveServicePort());

    if (m_databasePacsIdCache.contains(key))
    {
        return m_databasePacsIdCache[key];
    }

    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDeviceRetrievedFromDatabase = localDatabasePACSRetrievedImagesDAL.query(pacsDevice.getAETitle(), pacsDevice.getAddress(),
                                                                                           pacsDevice.getQueryRetrieveServicePort());

    if (!pacsDeviceRetrievedFromDatabase.getID().isEmpty())
    {
        // PACS is in the database
        m_databasePacsIdCache[key] = pacsDeviceRetrievedFromDatabase.getID().toLongLong();
        return pacsDeviceRetrievedFromDatabase.getID();
    }
    else
    {
        // PACS is not in the database. Let's insert it
        qlonglong databasePacsId = localDatabasePACSRetrievedImagesDAL.insert(pacsDevice);

        if (databasePacsId >= 0)
        {
            m_databasePacsIdCache[key] = databasePacsId;
            return m_databasePacsIdCache[key];
        }
        else
        {
            return QVariant(QVariant::LongLong);
        }
    }
}

DICOMSource LocalDatabaseEncapsulatedDocumentDAL::getDicomSource(const QVariant &retrievedPacsId)
{
    DICOMSource dicomSource;

    if (!retrievedPacsId.isNull())
    {
        PacsDevice pacsDevice = getPacsDevice(retrievedPacsId.toLongLong());

        if (!pacsDevice.getID().isEmpty())
        {
            dicomSource.addRetrievePACS(pacsDevice);
        }
    }

    return dicomSource;
}

PacsDevice LocalDatabaseEncapsulatedDocumentDAL::getPacsDevice(qlonglong retrievedPacsId)
{
    if (m_pacsDeviceCache.contains(retrievedPacsId))
    {
        return m_pacsDeviceCache[retrievedPacsId];
    }

    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDevice = localDatabasePACSRetrievedImagesDAL.query(retrievedPacsId);

    if (!pacsDevice.getID().isEmpty())
    {
        m_pacsDeviceCache[retrievedPacsId] = pacsDevice;
    }

    return pacsDevice;
}

} // namespace udg
