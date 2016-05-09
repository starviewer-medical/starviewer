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

#include "encapsulateddocument.h"

#include "series.h"

namespace udg {

EncapsulatedDocument::EncapsulatedDocument(QObject *parent)
    : QObject(parent), m_parentSeries(0)
{
}

EncapsulatedDocument::~EncapsulatedDocument()
{
}

const QString& EncapsulatedDocument::getTransferSyntaxUid() const
{
    return m_transferSyntaxUid;
}

void EncapsulatedDocument::setTransferSyntaxUid(const QString &uid)
{
    m_transferSyntaxUid = uid;
}

const QString& EncapsulatedDocument::getSopInstanceUid() const
{
    return m_sopInstanceUid;
}

void EncapsulatedDocument::setSopInstanceUid(const QString &uid)
{
    m_sopInstanceUid = uid;
}

const QString &EncapsulatedDocument::getInstanceNumber() const
{
    return m_instanceNumber;
}

void EncapsulatedDocument::setInstanceNumber(const QString &instanceNumber)
{
    m_instanceNumber = instanceNumber;
}

const QString& EncapsulatedDocument::getDocumentTitle() const
{
    return m_documentTitle;
}

void EncapsulatedDocument::setDocumentTitle(const QString &title)
{
    m_documentTitle = title;
}

const QString& EncapsulatedDocument::getMimeTypeOfEncapsulatedDocument() const
{
    return m_mimeTypeOfEncapsulatedDocument;
}

void EncapsulatedDocument::setMimeTypeOfEncapsulatedDocument(const QString &mimeType)
{
    m_mimeTypeOfEncapsulatedDocument = mimeType;
}

Series* EncapsulatedDocument::getParentSeries() const
{
    return m_parentSeries;
}

void EncapsulatedDocument::setParentSeries(Series *series)
{
    m_parentSeries = series;
    this->setParent(series);
}

const QString& EncapsulatedDocument::getPath() const
{
    return m_path;
}

void EncapsulatedDocument::setPath(const QString &path)
{
    m_path = path;
}

const DICOMSource& EncapsulatedDocument::getDicomSource() const
{
    return m_dicomSource;
}

void EncapsulatedDocument::setDicomSource(const DICOMSource &dicomSource)
{
    m_dicomSource = dicomSource;
}

QString EncapsulatedDocument::getKeyIdentifier() const
{
    return m_sopInstanceUid;
}

} // namespace udg
