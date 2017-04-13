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

#include "imageorientation.h"

#include "dicomvaluerepresentationconverter.h"
#include "logging.h"

#include <QStringList>

namespace udg {

ImageOrientation::ImageOrientation()
{
}

ImageOrientation::ImageOrientation(Vector3 rowVector, Vector3 columnVector)
    : m_rowVector(std::move(rowVector)), m_columnVector(std::move(columnVector))
{
    m_normalVector = Vector3::cross(m_rowVector, m_columnVector);
}

bool ImageOrientation::isValid() const
{
    Vector3 nullVector;
    return m_rowVector != nullVector && m_columnVector != nullVector;
}

bool ImageOrientation::setDICOMFormattedImageOrientation(const QString &imageOrientation)
{
    if (imageOrientation.isEmpty())
    {
        m_rowVector = m_columnVector = m_normalVector = Vector3();
        return true;
    }

    // A partir d'aquí comprovem que hi hagi el número correcte d'elements
    QStringList list = imageOrientation.split(DICOMValueRepresentationConverter::ValuesSeparator);

    if (list.size() != 6)
    {
        ERROR_LOG("Image Orientation (Patient) no té els 6 elements esperats. Inconsistència DICOM.");
        return false;
    }

    // Tenim 6 elements, cal comprovar que siguin valors vàlids
    bool validValues;
    QVector<double> convertedValues = DICOMValueRepresentationConverter::decimalStringToDoubleVector(imageOrientation, &validValues);

    if (!validValues)
    {
        ERROR_LOG("Els valors d'Image Orientation (Patient) no són vàlids. Inconsistència DICOM.");   
        return false;
    }

    // Arribats a aquest punt, tenim 6 valors i són vàlids
    // Assignem els valors convertits als corresponents vectors
    Vector3 row(convertedValues.at(0), convertedValues.at(1), convertedValues.at(2));
    Vector3 column(convertedValues.at(3), convertedValues.at(4), convertedValues.at(5));
    setRowAndColumnVectors(row, column);

    return true;
}

QString ImageOrientation::getDICOMFormattedImageOrientation() const
{
    if (!isValid())
    {
        return QString();
    }

    QStringList values;

    for (double v : {m_rowVector.x, m_rowVector.y, m_rowVector.z, m_columnVector.x, m_columnVector.y, m_columnVector.z})
    {
        values.append(QString::number(v));
    }

    return values.join(DICOMValueRepresentationConverter::ValuesSeparator);
}

void ImageOrientation::setRowAndColumnVectors(Vector3 rowVector, Vector3 columnVector)
{
    m_rowVector = std::move(rowVector);
    m_columnVector = std::move(columnVector);
    m_normalVector = Vector3::cross(m_rowVector, m_columnVector);
}

const Vector3& ImageOrientation::getRowVector() const
{
    return m_rowVector;
}

const Vector3& ImageOrientation::getColumnVector() const
{
    return m_columnVector;
}

const Vector3& ImageOrientation::getNormalVector() const
{
    return m_normalVector;
}

bool ImageOrientation::operator==(const ImageOrientation &imageOrientation) const
{
    return (m_rowVector == imageOrientation.m_rowVector) && (m_columnVector == imageOrientation.m_columnVector);
}

} // End namespace udg
