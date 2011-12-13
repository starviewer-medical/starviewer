#include "imageplane.h"
#include "image.h"
#include "mathtools.h"
#include <QString>
// Vtk's
#include <vtkPlane.h>

namespace udg {

ImagePlane::ImagePlane()
 : m_rows(1), m_columns(1), m_thickness(1.0)
{
    m_imageOrientation.setRowAndColumnVectors(QVector3D(1., 0., 0.), QVector3D(0., 1., 0.));
}

ImagePlane::ImagePlane(ImagePlane *imagePlane)
{
    m_imageOrientation = imagePlane->m_imageOrientation;

    m_origin[0] = imagePlane->m_origin[0];
    m_origin[1] = imagePlane->m_origin[1];
    m_origin[2] = imagePlane->m_origin[2];

    m_center[0] = imagePlane->m_center[0];
    m_center[1] = imagePlane->m_center[1];
    m_center[2] = imagePlane->m_center[2];

    m_spacing[0] = imagePlane->m_spacing[0];
    m_spacing[1] = imagePlane->m_spacing[1];

    m_rows = imagePlane->getRows();
    m_columns = imagePlane->getColumns();
    m_thickness = imagePlane->getThickness();
}

ImagePlane::~ImagePlane()
{
}

void ImagePlane::setImageOrientation(const ImageOrientation &imageOrientation)
{
    m_imageOrientation = imageOrientation;
}

ImageOrientation ImagePlane::getImageOrientation() const
{
    return m_imageOrientation;
}

void ImagePlane::getRowDirectionVector(double vector[3])
{
    QVector3D row = m_imageOrientation.getRowVector();
    vector[0] = row.x();
    vector[1] = row.y();
    vector[2] = row.z();
}

void ImagePlane::getColumnDirectionVector(double vector[3])
{
    QVector3D column = m_imageOrientation.getColumnVector();
    vector[0] = column.x();
    vector[1] = column.y();
    vector[2] = column.z();
}

void ImagePlane::getNormalVector(double vector[3])
{
    QVector3D normal = m_imageOrientation.getNormalVector();
    vector[0] = normal.x();
    vector[1] = normal.y();
    vector[2] = normal.z();
}

void ImagePlane::setOrigin(double origin[3])
{
    setOrigin(origin[0], origin[1], origin[2]);
}

void ImagePlane::setOrigin(double x, double y, double z)
{
    m_origin[0] = x;
    m_origin[1] = y;
    m_origin[2] = z;
    updateCenter();
}

void ImagePlane::getOrigin(double origin[3])
{
    origin[0] = m_origin[0];
    origin[1] = m_origin[1];
    origin[2] = m_origin[2];
}

void ImagePlane::setSpacing(double spacing[2])
{
    setSpacing(spacing[0], spacing[1]);
}

void ImagePlane::setSpacing(double x, double y)
{
    m_spacing[0] = x;
    m_spacing[1] = y;
    updateCenter();
}

void ImagePlane::getSpacing(double spacing[2])
{
    spacing[0] = m_spacing[0];
    spacing[1] = m_spacing[1];
}

void ImagePlane::setThickness(double thickness)
{
    m_thickness = thickness;
    updateCenter();
}

double ImagePlane::getThickness() const
{
    return m_thickness;
}

void ImagePlane::setRows(int rows)
{
    m_rows = rows;
    updateCenter();
}

void ImagePlane::setColumns(int columns)
{
    m_columns = columns;
    updateCenter();
}

int ImagePlane::getRows() const
{
    return m_rows;
}

int ImagePlane::getColumns() const
{
    return m_columns;
}

double ImagePlane::getRowLength() const
{
    return m_columns * m_spacing[0];
}

double ImagePlane::getColumnLength() const
{
    return m_rows * m_spacing[1];
}

bool ImagePlane::fillFromImage(const Image *image)
{
    if(image)
    {
        this->setImageOrientation(image->getImageOrientationPatient());
        this->setSpacing(image->getPixelSpacing()[0], image->getPixelSpacing()[1]);
        this->setThickness(image->getSliceThickness());
        this->setRows(image->getRows());
        this->setColumns(image->getColumns());
        this->setOrigin(image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2]);
        // TODO No tenim implementat cap mètode per guardar la location. De moment no es fa servir desde aquesta classe mai.
        QString location = image->getSliceLocation();

        return true;
    }
    else
    {
        return false;
    }
}

bool ImagePlane::operator ==(const ImagePlane &imagePlane)
{
    if(m_imageOrientation == imagePlane.m_imageOrientation &&
        m_origin[0] == imagePlane.m_origin[0] &&
        m_origin[1] == imagePlane.m_origin[1] &&
        m_origin[2] == imagePlane.m_origin[2] &&
        m_spacing[0] == imagePlane.m_spacing[0] &&
        m_spacing[1] == imagePlane.m_spacing[1] &&
        m_rows == imagePlane.m_rows &&
        m_columns == imagePlane.m_columns &&
        m_thickness == imagePlane.m_thickness
      )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ImagePlane::operator !=(const ImagePlane &imagePlane)
{
    return !(*this == imagePlane);
}

QList<QVector<double> > ImagePlane::getBounds(int location)
{
    double factor = 0.0;
    switch (location)
    {
        // Central
        case 0:
            factor = 0.0;
            break;
        // Upper
        case 1:
            factor = m_thickness * 0.5;
            break;
        // Lower
        case 2:
            factor = -m_thickness * 0.5;
            break;
    }

    QList<QVector<double> > boundsList;
    QVector<double> tlhc, trhc, brhc, blhc;

    double rowVector[3];
    double columnVector[3];
    double normalVector[3];
    this->getRowDirectionVector(rowVector);
    this->getColumnDirectionVector(columnVector);
    this->getNormalVector(normalVector);
    
    for(int i = 0; i < 3; i++)
    {
        tlhc << m_origin[i] + normalVector[i] * factor;
        trhc << m_origin[i] + rowVector[i] * this->getRowLength() + normalVector[i] * factor;
        brhc << m_origin[i] + rowVector[i] * this->getRowLength() + columnVector[i] * this->getColumnLength() + normalVector[i] * factor;
        blhc << m_origin[i] + columnVector[i] * this->getColumnLength() + normalVector[i] * factor;
    }
    boundsList << tlhc << trhc << brhc << blhc;
    return boundsList;
}

QList<QVector<double> > ImagePlane::getCentralBounds()
{
    return getBounds(0);
}

QList<QVector<double> > ImagePlane::getUpperBounds()
{
    return getBounds(1);
}

QList<QVector<double> > ImagePlane::getLowerBounds()
{
    return getBounds(2);
}

QString ImagePlane::toString(bool verbose)
{
    QString result;

    result = QString("Origin: %1, %2, %3").arg(m_origin[0]).arg(m_origin[1]).arg(m_origin[2]);
    result += QString("\nRows: %1  Columns: %2").arg(m_rows).arg(m_columns);
    QVector3D rowVector = m_imageOrientation.getRowVector();
    result += QString("\nRow Vector: %1, %2, %3").arg(rowVector.x()).arg(rowVector.y()).arg(rowVector.z());
    QVector3D columnVector = m_imageOrientation.getColumnVector();
    result += QString("\nColumn Vector: %1, %2, %3").arg(columnVector.x()).arg(columnVector.y()).arg(columnVector.z());
    QVector3D normalVector = m_imageOrientation.getNormalVector();
    result += QString("\nNormal Vector: %1, %2, %3").arg(normalVector.x()).arg(normalVector.y()).arg(normalVector.z());
    result += QString("\nCenter: %1, %2, %3").arg(m_center[0]).arg(m_center[1]).arg(m_center[2]);
    result += QString("\nSpacing: %1, %2").arg(m_spacing[0]).arg(m_spacing[1]);
    result += QString("\nThickness: %1").arg(m_thickness);
    result += QString("\nRow length: %1").arg(getRowLength());
    result += QString("\nColumn length: %1").arg(getColumnLength());

    if(verbose)
    {
        QList<QVector<double> > bounds = this->getCentralBounds();
        result += QString("\nTLHC: %1, %2, %3").arg(bounds.at(0)[0]).arg(bounds.at(0)[1]).arg(bounds.at(0)[2]);
        result += QString("\nTRHC: %1, %2, %3").arg(bounds.at(1)[0]).arg(bounds.at(1)[1]).arg(bounds.at(1)[2]);
        result += QString("\nBRHC: %1, %2, %3").arg(bounds.at(2)[0]).arg(bounds.at(2)[1]).arg(bounds.at(2)[2]);
        result += QString("\nBLHC: %1, %2, %3").arg(bounds.at(3)[0]).arg(bounds.at(3)[1]).arg(bounds.at(3)[2]);
    }

    return result;
}

int ImagePlane::getIntersections(ImagePlane *planeToIntersect, double firstIntersectionPoint[3], double secondIntersectionPoint[3])
{
    double t;
    int numberOfIntersections = 0;
    double localizerNormalVector[3], localizerOrigin[3];
    planeToIntersect->getNormalVector(localizerNormalVector);
    planeToIntersect->getOrigin(localizerOrigin);

    QList<QVector<double> > upperPlaneBounds = this->getUpperBounds();

    QVector<double> tlhc = upperPlaneBounds.at(0);
    QVector<double> trhc = upperPlaneBounds.at(1);
    QVector<double> brhc = upperPlaneBounds.at(2);
    QVector<double> blhc = upperPlaneBounds.at(3);

    // Primera "paral·lela" (X)
    if(vtkPlane::IntersectWithLine((double*)tlhc.data(), (double*)trhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint))
    {
        numberOfIntersections++;
    }
    if(vtkPlane::IntersectWithLine((double*)brhc.data(), (double*)blhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint))
    {
        numberOfIntersections++;
    }

    if(numberOfIntersections == 0)
    {
        // Provar amb la segona "paral·lela" (Y)
        if(vtkPlane::IntersectWithLine((double*)trhc.data(), (double*)brhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint))
        {
            numberOfIntersections++;
        }

        if(vtkPlane::IntersectWithLine((double*)blhc.data(), (double*)tlhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint))
        {
            numberOfIntersections++;
        }
    }

    return numberOfIntersections;
}

void ImagePlane::updateCenter()
{
    double rowVector[3];
    double columnVector[3];
    this->getRowDirectionVector(rowVector);
    this->getColumnDirectionVector(columnVector);
    
    for(int i = 0; i < 3; i++)
    {
        m_center[i] = m_origin[i] + 0.5 * (rowVector[i] * this->getRowLength() + columnVector[i] * this->getColumnLength());
    }
}

void ImagePlane::getCenter(double center[3])
{
    center[0] = m_center[0];
    center[1] = m_center[1];
    center[2] = m_center[2];
}

void ImagePlane::setCenter(double x, double y, double z)
{
    double center[3];
    center[0] = x;
    center[1] = y;
    center[2] = z;
    this->setCenter(center);
}

void ImagePlane::setCenter(double center[3])
{
    if (m_center[0] == center[0] && m_center[1] == center[1] && m_center[2] == center[2])
    {
        // No change
        return;
    }
    else
    {
        double rowVector[3];
        double columnVector[3];
        this->getRowDirectionVector(rowVector);
        this->getColumnDirectionVector(columnVector);
        
        for (int i = 0; i < 3; ++i)
        {
            m_center[i] = center[i];
            m_origin[i] = m_center[i] - 0.5 * (rowVector[i] * this->getRowLength() + columnVector[i] * this->getColumnLength());
        }
    }
}

void ImagePlane::push(double distance)
{
    if (distance == 0.0)
    {
        return;
    }

    double normalVector[3];
    this->getNormalVector(normalVector);
    for (int i = 0; i < 3; ++i)
    {
        m_origin[i] += distance * normalVector[i];
    }
    this->updateCenter();
}

}
