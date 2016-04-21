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

#ifndef UDG_ENCAPSULATEDDOCUMENT_H
#define UDG_ENCAPSULATEDDOCUMENT_H

#include <QObject>

#include "dicomsource.h"

namespace udg {

class Series;

/**
 * @brief The EncapsulatedDocument class represents the DICOM Encapsulated Document IOD.
 */
class EncapsulatedDocument : public QObject {

    Q_OBJECT

public:

    explicit EncapsulatedDocument(QObject *parent = 0);
    virtual ~EncapsulatedDocument();

    /// Returns the transfer syntax UID.
    const QString& getTransferSyntaxUid() const;
    /// Sets the transfer syntax UID.
    void setTransferSyntaxUid(const QString &uid);

    /// Returns the SOP Instance UID.
    const QString& getSopInstanceUid() const;
    /// Sets the SOP Instance UID.
    void setSopInstanceUid(const QString &uid);

    /// Returns the instance number.
    const QString& getInstanceNumber() const;
    /// Sets the instance number.
    void setInstanceNumber(const QString &instanceNumber);

    /// Returns the document title.
    const QString& getDocumentTitle() const;
    /// Sets the document title.
    void setDocumentTitle(const QString &title);

    /// Returns the MIME type of the encapsulated document.
    const QString& getMimeTypeOfEncapsulatedDocument() const;
    /// Sets the MIME type of the encapsulated document.
    void setMimeTypeOfEncapsulatedDocument(const QString &mimeType);

    /// Returns the parent series.
    Series* getParentSeries() const;
    /// Sets the parent series.
    void setParentSeries(Series *series);

    /// Returns the path of the file of this object.
    const QString& getPath() const;
    /// Sets the path of the file of this object.
    void setPath(const QString &path);

    /// Returns the DICOM source.
    const DICOMSource& getDicomSource() const;
    /// Sets the DICOM source.
    void setDicomSource(const DICOMSource &dicomSource);

    /// Returns an identifier for this object that can be used as a key.
    QString getKeyIdentifier() const;

private:

    /// Uniquely identifies the Transfer Syntax used to encode the following Data Set (0002,0010).
    QString m_transferSyntaxUid;
    /// Uniquely identifies the SOP Instance (0008,0018).
    QString m_sopInstanceUid;
    /// A number that identifies this Composite object instance (0020,0013).
    QString m_instanceNumber;
    /// The title of the document (0042,0010).
    QString m_documentTitle;
    /// The type of the encapsulated document stream described using the MIME Media Type (0042,0012).
    QString m_mimeTypeOfEncapsulatedDocument;

    /// The series to which this object belongs.
    Series *m_parentSeries;

    /// Path of the file of this object.
    QString m_path;

    /// Where this object has been obtained from.
    DICOMSource m_dicomSource;

};

} // namespace udg

#endif // UDG_ENCAPSULATEDDOCUMENT_H
