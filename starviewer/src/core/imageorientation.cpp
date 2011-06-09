#include "imageorientation.h"

#include <QStringList>

#include "dicomvaluerepresentationconverter.h"
#include "logging.h"

namespace udg {

// Constant per definir el que nosaltres considerem un vector buit
const QVector3D EmptyVector(.0,.0,.0);

ImageOrientation::ImageOrientation()
{
    setVectorValuesToDefault();
}

ImageOrientation::~ImageOrientation()
{
}

ImageOrientation::ImageOrientation(const QVector3D &rowVector, const QVector3D &columnVector)
{
    setRowAndColumnVectors(rowVector, columnVector);
}

bool ImageOrientation::setDICOMFormattedImageOrientation(const QString &imageOrientation)
{
    if (imageOrientation.isEmpty())
    {
        setVectorValuesToDefault();

        return true;
    }

    // A partir d'aquí comprovem que hi hagi el número correcte d'elements
    QStringList list = imageOrientation.split(DICOMValueRepresentationConverter::ValuesSeparator);
    if (list.size() != 6)
    {
        DEBUG_LOG("Image Orientation (Patient) no té els 6 elements esperats. Inconsistència DICOM.");
        ERROR_LOG("Image Orientation (Patient) no té els 6 elements esperats. Inconsistència DICOM.");

        return false;
    }

    // Tenim 6 elements, cal comprovar que siguin valors vàlids
    bool validValues;
    QVector<double> convertedValues = DICOMValueRepresentationConverter::decimalStringToDoubleVector(imageOrientation, &validValues);

    if (!validValues)
    {
        DEBUG_LOG("Els valors d'Image Orientation (Patient) no són vàlids. Inconsistència DICOM.");
        ERROR_LOG("Els valors d'Image Orientation (Patient) no són vàlids. Inconsistència DICOM.");
        
        return false;
    }

    // Arribats a aquest punt, tenim 6 valors i són vàlids
    // Assignem els valors convertits als corresponents vectors
    QVector3D row(convertedValues.at(0), convertedValues.at(1), convertedValues.at(2));
    QVector3D column(convertedValues.at(3), convertedValues.at(4), convertedValues.at(5));
    setRowAndColumnVectors(row, column);

    return true;
}

QString ImageOrientation::getDICOMFormattedImageOrientation() const
{
    if (m_rowVector == EmptyVector && m_columnVector == EmptyVector)
    {
        return QString();
    }
    else
    {
        return this->convertToDICOMFormatString(m_rowVector, m_columnVector);
    }
}

void ImageOrientation::setRowAndColumnVectors(const QVector3D &rowVector, const QVector3D &columnVector)
{
    m_rowVector = rowVector;
    m_columnVector = columnVector;
    m_normalVector = QVector3D::crossProduct(m_rowVector, m_columnVector);
}

QVector3D ImageOrientation::getRowVector() const
{
    return m_rowVector;
}

QVector3D ImageOrientation::getColumnVector() const
{
    return m_columnVector;
}

QVector3D ImageOrientation::getNormalVector() const
{
    return m_normalVector;
}

bool ImageOrientation::operator==(const ImageOrientation &imageOrientation) const
{
    return (m_rowVector == imageOrientation.m_rowVector) && (m_columnVector == imageOrientation.m_columnVector);
}

QString ImageOrientation::convertToDICOMFormatString(const QVector3D &rowVector, const QVector3D &columnVector) const
{
    QString imageOrientationString;

    imageOrientationString = QString::number(rowVector.x()) + DICOMValueRepresentationConverter::ValuesSeparator;
    imageOrientationString += QString::number(rowVector.y()) + DICOMValueRepresentationConverter::ValuesSeparator;
    imageOrientationString += QString::number(rowVector.z()) + DICOMValueRepresentationConverter::ValuesSeparator;

    imageOrientationString += QString::number(columnVector.x()) + DICOMValueRepresentationConverter::ValuesSeparator;
    imageOrientationString += QString::number(columnVector.y()) + DICOMValueRepresentationConverter::ValuesSeparator;
    imageOrientationString += QString::number(columnVector.z());

    return imageOrientationString;
}

void ImageOrientation::setVectorValuesToDefault()
{
    m_rowVector = EmptyVector;
    m_columnVector = EmptyVector;
    m_normalVector = EmptyVector;
}

} // End namespace udg
