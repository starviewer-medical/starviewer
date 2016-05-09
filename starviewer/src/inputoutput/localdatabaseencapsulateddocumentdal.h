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

#ifndef UDG_LOCALDATABASEENCAPSULATEDDOCUMENTDAL_H
#define UDG_LOCALDATABASEENCAPSULATEDDOCUMENTDAL_H

#include "localdatabasebasedal.h"

#include <QHash>

namespace udg {

class DicomMask;
class DICOMSource;
class EncapsulatedDocument;
class PacsDevice;

/**
 * @brief The LocalDatabaseEncapsulatedDocumentDAL class is the Data Access Layer class for encapsulated documents in the local cache.
 */
class LocalDatabaseEncapsulatedDocumentDAL : public LocalDatabaseBaseDAL {

public:

    LocalDatabaseEncapsulatedDocumentDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given encapsulated document. Returns true if successful and false otherwise.
    bool insert(const EncapsulatedDocument *document);

    /// Updates in the database the given encapsulated document. Returns true if successful and false otherwise.
    bool update(const EncapsulatedDocument *document);

    /// Deletes from the database the encapsulated documents that match the given mask (only StudyUID, SeriesUID and SOPInstanceUID are considered).
    /// Returns true if successful and false otherwise.
    bool del(const DicomMask &mask);

    /// Retrieves from the database the encapsulated documents that match the given mask (only StudyUID, SeriesUID and SOPInstanceUID are considered)
    /// and returns them in a list.
    QList<EncapsulatedDocument*> query(const DicomMask &mask);

    /// Counts and returns the number of encapsulated documents that match the given mask (only StudyUID, SeriesUID and SOPInstanceUID are considered).
    /// Returns -1 in case of error.
    int count(const DicomMask &mask);

private:

    /// Binds the necessary values of the given query with the information of the given encapsulated document.
    void bindValues(QSqlQuery &query, const EncapsulatedDocument *document);

    /// Creates and returns an encapsulated document with the information of the current row of the given query.
    EncapsulatedDocument* getEncapsulatedDocument(const QSqlQuery &query);

    /// If the given DICOM source contains a PACS, returns its ID in the database. If the PACS doesn't exist in the database, it's inserted.
    /// If the given DICOM source contains multiple PACS, only the first one is considered.
    /// If the given DICOM source doesn't contain a PACS, returns a null QVariant.
    QVariant getDatabasePacsId(const DICOMSource &dicomSource);

    /// Returns the ID of the given PACS in the database. If the PACS doesn't exist in the database, it's inserted.
    /// Keeps a cache of the queried and inserted PACS to avoid subsequent database queries for the same PACS.
    QVariant getDatabasePacsId(const PacsDevice &pacsDevice);

    /// Returns the DICOM source with the PACS with the given ID in the database, or an empty DICOM source if there's no such PACS.
    DICOMSource getDicomSource(const QVariant &retrievedPacsId);

    /// Returns the PACS device with the given ID in the database, or a default PACS device if there's no such PACS.
    /// Keeps a cache of the queried PACS to avoid subsequent database queries for the same PACS.
    PacsDevice getPacsDevice(qlonglong retrievedPacsId);

private:

    /// Hash from IP+AETitle to database PACS id used as a cache to avoid many accesses to the database.
    QHash<QString, QVariant> m_databasePacsIdCache;

    /// Hash from database PACS id to PacsDevice used as a cache to avoid many accesses to the database.
    QHash<qlonglong, PacsDevice> m_pacsDeviceCache;

};

} // namespace udg

#endif // UDG_LOCALDATABASEENCAPSULATEDDOCUMENTDAL_H
