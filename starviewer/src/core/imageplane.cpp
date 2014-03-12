#include "imageplane.h"
#include "image.h"
#include "mathtools.h"
#include <QString>
// Vtk's
#include <vtkPlane.h>
#include <vtkMatrix4x4.h>

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

    m_spacing = imagePlane->m_spacing;

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

void ImagePlane::setSpacing(const PixelSpacing2D &spacing)
{
    m_spacing = spacing;
}

PixelSpacing2D ImagePlane::getSpacing() const
{
    return m_spacing;
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
    return m_columns * m_spacing.x();
}

double ImagePlane::getColumnLength() const
{
    return m_rows * m_spacing.y();
}

bool ImagePlane::fillFromImage(const Image *image)
{
    if (image)
    {
        this->setImageOrientation(image->getImageOrientationPatient());
        this->setSpacing(image->getPreferredPixelSpacing());
        this->setThickness(image->getSliceThickness());
        this->setRows(image->getRows());
        this->setColumns(image->getColumns());
        this->setOrigin(image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2]);

        return true;
    }
    else
    {
        return false;
    }
}

bool ImagePlane::operator ==(const ImagePlane &imagePlane)
{
    if (m_imageOrientation == imagePlane.m_imageOrientation && m_origin[0] == imagePlane.m_origin[0] && m_origin[1] == imagePlane.m_origin[1] &&
        m_origin[2] == imagePlane.m_origin[2] && m_spacing.isEqual(imagePlane.m_spacing) && m_rows == imagePlane.m_rows && m_columns == imagePlane.m_columns && 
        m_thickness == imagePlane.m_thickness)
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
    
    for (int i = 0; i < 3; i++)
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
    result += QString("\nSpacing: %1, %2").arg(m_spacing.x()).arg(m_spacing.y());
    result += QString("\nThickness: %1").arg(m_thickness);
    result += QString("\nRow length: %1").arg(getRowLength());
    result += QString("\nColumn length: %1").arg(getColumnLength());

    if (verbose)
    {
        QList<QVector<double> > bounds = this->getCentralBounds();
        result += QString("\nTLHC: %1, %2, %3").arg(bounds.at(0)[0]).arg(bounds.at(0)[1]).arg(bounds.at(0)[2]);
        result += QString("\nTRHC: %1, %2, %3").arg(bounds.at(1)[0]).arg(bounds.at(1)[1]).arg(bounds.at(1)[2]);
        result += QString("\nBRHC: %1, %2, %3").arg(bounds.at(2)[0]).arg(bounds.at(2)[1]).arg(bounds.at(2)[2]);
        result += QString("\nBLHC: %1, %2, %3").arg(bounds.at(3)[0]).arg(bounds.at(3)[1]).arg(bounds.at(3)[2]);
    }

    return result;
}

int ImagePlane::getIntersections(ImagePlane *planeToIntersect, double firstIntersectionPoint[3], double secondIntersectionPoint[3], int bounds)
{
    double t;
    int numberOfIntersections = 0;
    double planeToIntersectNormalVector[3], planeToIntersectOrigin[3];
    planeToIntersect->getNormalVector(planeToIntersectNormalVector);
    planeToIntersect->getOrigin(planeToIntersectOrigin);

    QList<QVector<double> > planeBounds;
    switch (bounds)
    {
        case 0:
            planeBounds = this->getUpperBounds();
            break;

        case 1:
            planeBounds = this->getLowerBounds();
            break;

        case 2:
            planeBounds = this->getCentralBounds();
            break;

        default:
            planeBounds = this->getUpperBounds();
            break;
    }

    double *currentPlaneTopLeftPoint = (double*)planeBounds.at(0).data();
    double *currentPlaneTopRightPoint = (double*)planeBounds.at(1).data();
    double *currentPlaneBottomRightPoint = (double*)planeBounds.at(2).data();
    double *currentPlaneBottomLetfPoint = (double*)planeBounds.at(3).data();

    // Primera "paral·lela" (X)
    if (vtkPlane::IntersectWithLine(currentPlaneTopLeftPoint, currentPlaneTopRightPoint, planeToIntersectNormalVector, planeToIntersectOrigin, t, firstIntersectionPoint))
    {
        numberOfIntersections++;
    }
    if (vtkPlane::IntersectWithLine(currentPlaneBottomRightPoint, currentPlaneBottomLetfPoint, planeToIntersectNormalVector, planeToIntersectOrigin, t, secondIntersectionPoint))
    {
        numberOfIntersections++;
    }

    if (numberOfIntersections == 0)
    {
        // Provar amb la segona "paral·lela" (Y)
        if (vtkPlane::IntersectWithLine(currentPlaneTopRightPoint, currentPlaneBottomRightPoint, planeToIntersectNormalVector, planeToIntersectOrigin, t, firstIntersectionPoint))
        {
            numberOfIntersections++;
        }

        if (vtkPlane::IntersectWithLine(currentPlaneBottomLetfPoint, currentPlaneTopLeftPoint, planeToIntersectNormalVector, planeToIntersectOrigin, t, secondIntersectionPoint))
        {
            numberOfIntersections++;
        }
    }

    return numberOfIntersections;
}

double ImagePlane::getDistanceToPoint(double point[3])
{
    double currentNormalVector[3];
    getNormalVector(currentNormalVector);
    
    // TODO An alternative method would be computing distance with QVector3D::distanceToPlane()
    return vtkPlane::DistanceToPlane(point, currentNormalVector, m_origin);
}

void ImagePlane::projectPoint(const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack)
{
    // First gather the needed plane data to project the point
    double currentPlaneRowVector[3];
    double currentPlaneColumnVector[3];
    double currentPlaneNormalVector[3];
    double currentPlaneOrigin[3];
    getRowDirectionVector(currentPlaneRowVector);
    getColumnDirectionVector(currentPlaneColumnVector);
    getNormalVector(currentPlaneNormalVector);
    getOrigin(currentPlaneOrigin);

    // Then create the projection matrix
    vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
    projectionMatrix->Identity();
    for (int column = 0; column < 3; column++)
    {
        projectionMatrix->SetElement(0, column, currentPlaneRowVector[column]);
        projectionMatrix->SetElement(1, column, currentPlaneColumnVector[column]);
        projectionMatrix->SetElement(2, column, currentPlaneNormalVector[column]);
    }

    // Once we have the matrix, we can project the point i homogeneous coordinates
    double homogeneousPointToProject[4];
    for (int i = 0; i < 3; i++)
    {
        // We must shift the point to project to the plane origin
        homogeneousPointToProject[i] = pointToProject[i] - currentPlaneOrigin[i];
    }
    homogeneousPointToProject[3] = 1.0;

    // Project the point with the matrix
    double homogeneousProjectedPoint[4];
    projectionMatrix->MultiplyPoint(homogeneousPointToProject, homogeneousProjectedPoint);
    
    // Check if we have to apply vtk hack on projection matrix
    if (vtkReconstructionHack)
    {
        // HACK Patch for the cranium cases that are not working properly (gantry tilt).
        // TODO This is a first aproximation, must be more tested still
        projectionMatrix->SetElement(0, 0, 0);
        projectionMatrix->SetElement(0, 1, 1);
        projectionMatrix->SetElement(0, 2, 0);
        // Project the point with the matrix
        projectionMatrix->MultiplyPoint(homogeneousPointToProject, homogeneousProjectedPoint);
    }
    
    projectionMatrix->Delete();

    // Convert the homogeneous projected point to the 3D projected point to return
    for (int i = 0; i < 3; ++i)
    {
        projectedPoint[i] = homogeneousProjectedPoint[i];
    }
}

void ImagePlane::updateCenter()
{
    double rowVector[3];
    double columnVector[3];
    this->getRowDirectionVector(rowVector);
    this->getColumnDirectionVector(columnVector);
    
    for (int i = 0; i < 3; i++)
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
