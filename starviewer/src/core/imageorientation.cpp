#include "imageorientation.h"

#include <QStringList>

#include "mathtools.h"
#include "logging.h"

namespace udg {

// Constant que defineix quin és el separador
const QString Separator("\\");

ImageOrientation::ImageOrientation()
{
}

ImageOrientation::~ImageOrientation()
{
}

ImageOrientation::ImageOrientation(const QString &dicomFormattedImageOrientationString)
{
    setDICOMFormattedImageOrientation(dicomFormattedImageOrientationString);
}

ImageOrientation::ImageOrientation(const QVector3D &rowVector, const QVector3D &columnVector)
{
    setRowAndColumnVectors(rowVector, columnVector);
}

bool ImageOrientation::setDICOMFormattedImageOrientation(const QString &imageOrientation)
{
    bool ok = false;
    
    QStringList list = imageOrientation.split(Separator);
    if (list.size() == 6)
    {
        m_imageOrientationString = imageOrientation;   
        // Adicionalment convertim l'string en els vectors de doubles
        buildVectorsFromOrientationString();
        
        ok = true;
    }
    else
    {
        DEBUG_LOG("Image Orientation (Patient) no té els 6 elements esperats. Inconsistència DICOM.");
        ERROR_LOG("Image Orientation (Patient) no té els 6 elements esperats. Inconsistència DICOM.");
    }

    return ok;
}

QString ImageOrientation::getDICOMFormattedImageOrientation() const
{
    return m_imageOrientationString;
}

void ImageOrientation::setRowAndColumnVectors(const QVector3D &rowVector, const QVector3D &columnVector)
{
    QString imageOrientationString;
    
    imageOrientationString = QString::number(rowVector.x()) + Separator;
    imageOrientationString += QString::number(rowVector.y()) + Separator;
    imageOrientationString += QString::number(rowVector.z()) + Separator;
    
    imageOrientationString += QString::number(columnVector.x()) + Separator;
    imageOrientationString += QString::number(columnVector.y()) + Separator;
    imageOrientationString += QString::number(columnVector.z());

    setDICOMFormattedImageOrientation(imageOrientationString);
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

bool ImageOrientation::isEmpty() const
{
    return m_imageOrientationString.isEmpty();
}

ImageOrientation& ImageOrientation::operator=(const ImageOrientation &imageOrientation)
{
    m_imageOrientationString = imageOrientation.m_imageOrientationString;
    m_rowVector = imageOrientation.m_rowVector;
    m_columnVector = imageOrientation.m_columnVector;
    m_normalVector = imageOrientation.m_normalVector;
    
    return *this;
}

bool ImageOrientation::operator==(const ImageOrientation &imageOrientation) const
{
    return m_imageOrientationString == imageOrientation.m_imageOrientationString;
}

void ImageOrientation::buildVectorsFromOrientationString()
{
    // Reiniciem els vectors
    initializeVectors();
    
    QStringList list = m_imageOrientationString.split(Separator);
    if (list.size() == 6)
    {    
        m_rowVector.setX(list.at(0).toDouble());
        m_rowVector.setY(list.at(1).toDouble());
        m_rowVector.setZ(list.at(2).toDouble());
        
        m_columnVector.setX(list.at(3).toDouble());
        m_columnVector.setY(list.at(4).toDouble());
        m_columnVector.setZ(list.at(5).toDouble());

        m_normalVector = QVector3D::crossProduct(m_rowVector, m_columnVector);
    }
    else
    {        
        DEBUG_LOG(QString("La cadena d'orientació no conté 6 elements: Elements de la cadena: %1").arg(list.size()));
    }
}

void ImageOrientation::initializeVectors()
{
    // TODO Comprovar si el 'default-constructed value' és 0 pels doubles
    m_rowVector = QVector3D();
    m_columnVector = QVector3D();
    m_normalVector = QVector3D();
}

} // End namespace udg
