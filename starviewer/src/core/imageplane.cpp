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

#include "imageplane.h"

#include "image.h"

#include <vtkMatrix4x4.h>
#include <vtkPlane.h>

namespace udg {

ImagePlane::ImagePlane()
 : m_rowLength(0.0), m_columnLength(0.0), m_thickness(1.0)
{
    m_imageOrientation.setRowAndColumnVectors(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
}

const ImageOrientation& ImagePlane::getImageOrientation() const
{
    return m_imageOrientation;
}

void ImagePlane::setImageOrientation(ImageOrientation imageOrientation)
{
    m_imageOrientation = std::move(imageOrientation);
}

const Vector3& ImagePlane::getOrigin() const
{
    return m_origin;
}

void ImagePlane::setOrigin(Vector3 origin)
{
    m_origin = std::move(origin);
}

void ImagePlane::setOrigin(double x, double y, double z)
{
    m_origin = Vector3(x, y, z);
}

const PixelSpacing2D& ImagePlane::getSpacing() const
{
    return m_spacing;
}

void ImagePlane::setSpacing(PixelSpacing2D spacing)
{
    m_spacing = std::move(spacing);
}

double ImagePlane::getRowLength() const
{
    return m_rowLength;
}

void ImagePlane::setRowLength(double length)
{
    m_rowLength = length;
}

double ImagePlane::getColumnLength() const
{
    return m_columnLength;
}

void ImagePlane::setColumnLength(double length)
{
    m_columnLength = length;
}

double ImagePlane::getThickness() const
{
    return m_thickness;
}

void ImagePlane::setThickness(double thickness)
{
    m_thickness = thickness;
}

Vector3 ImagePlane::getCenter() const
{
    return m_origin + 0.5 * (Vector3(m_imageOrientation.getRowVector()) * getRowLength() + Vector3(m_imageOrientation.getColumnVector()) * getColumnLength());
}

void ImagePlane::setCenter(const Vector3 &center)
{
    m_origin = center - 0.5 * (Vector3(m_imageOrientation.getRowVector()) * getRowLength() + Vector3(m_imageOrientation.getColumnVector()) * getColumnLength());
}

void ImagePlane::setCenter(double x, double y, double z)
{
    setCenter(Vector3(x, y, z));
}

void ImagePlane::fillFromImage(const Image *image)
{
    if (image)
    {
        this->setImageOrientation(image->getImageOrientationPatient());
        this->setOrigin(image->getImagePositionPatient());
        this->setSpacing(image->getPreferredPixelSpacing());
        this->setRowLength(image->getColumns() * m_spacing.x());
        this->setColumnLength(image->getRows() * m_spacing.y());
        this->setThickness(image->getSliceThickness());
    }
}

bool ImagePlane::operator ==(const ImagePlane &imagePlane) const
{
    return m_imageOrientation == imagePlane.m_imageOrientation && m_origin == imagePlane.m_origin && m_spacing.isEqual(imagePlane.m_spacing)
            && m_rowLength == imagePlane.m_rowLength && m_columnLength == imagePlane.m_columnLength && m_thickness == imagePlane.m_thickness;
}

bool ImagePlane::operator !=(const ImagePlane &imagePlane) const
{
    return !(*this == imagePlane);
}

ImagePlane::Corners ImagePlane::getCorners(CornersLocation location) const
{
    double factor = 0.0;

    switch (location)
    {
        case Central: factor = 0.0; break;
        case Upper: factor = m_thickness * 0.5; break;
        case Lower: factor = -m_thickness * 0.5; break;
    }

    Vector3 rowVector = m_imageOrientation.getRowVector();
    Vector3 columnVector = m_imageOrientation.getColumnVector();
    Vector3 normalVector = m_imageOrientation.getNormalVector();

    Corners corners;
    corners.location = location;
    corners.topLeft = m_origin + normalVector * factor;
    corners.topRight =  m_origin + rowVector * this->getRowLength() + normalVector * factor;
    corners.bottomRight = m_origin + rowVector * this->getRowLength() + columnVector * this->getColumnLength() + normalVector * factor;
    corners.bottomLeft = m_origin + columnVector * this->getColumnLength() + normalVector * factor;

    return corners;
}

ImagePlane::Corners ImagePlane::getCentralCorners() const
{
    return getCorners(Central);
}

ImagePlane::Corners ImagePlane::getUpperCorners() const
{
    return getCorners(Upper);
}

ImagePlane::Corners ImagePlane::getLowerCorners() const
{
    return getCorners(Lower);
}

QString ImagePlane::toString(bool verbose) const
{
    QString string = QString("Orientation:\n"
                             "- Row vector: %1\n"
                             "- Column vector: %2\n"
                             "- Normal vector: %3\n"
                             "Origin: %4\n"
                             "Spacing: (%5, %6)\n"
                             "Row length: %7\n"
                             "Column length: %8\n"
                             "Thickness: %9")
            .arg(Vector3(m_imageOrientation.getRowVector()))
            .arg(Vector3(m_imageOrientation.getColumnVector()))
            .arg(Vector3(m_imageOrientation.getNormalVector()))
            .arg(m_origin)
            .arg(m_spacing.x()).arg(m_spacing.y())
            .arg(m_rowLength)
            .arg(m_columnLength)
            .arg(m_thickness);

    if (verbose)
    {
        auto corners = this->getCentralCorners();
        string += QString("\n"
                          "Central corners:\n"
                          "- Top left: %1\n"
                          "- Top right: %2\n"
                          "- Bottom right: %3\n"
                          "- Bottom left: %4")
                .arg(corners.topLeft)
                .arg(corners.topRight)
                .arg(corners.bottomRight)
                .arg(corners.bottomLeft);
    }

    return string;
}

bool ImagePlane::getIntersections(const ImagePlane *plane, Vector3 &intersectionPoint1, Vector3 &intersectionPoint2, CornersLocation cornersLocation) const
{
    Corners corners = this->getCorners(cornersLocation);
    std::array<double, 3> topLeft = corners.topLeft, topRight = corners.topRight, bottomRight = corners.bottomRight, bottomLeft = corners.bottomLeft;
    std::array<double, 3> otherNormal = Vector3(plane->getImageOrientation().getNormalVector());
    std::array<double, 3> otherOrigin = plane->getOrigin();
    double t;
    double p1[3], p2[3];
    bool foundIntersection = true;

    // First try intersection with horizontal borders
    foundIntersection &= vtkPlane::IntersectWithLine(topLeft.data(), topRight.data(), otherNormal.data(), otherOrigin.data(), t, p1) != 0
            || t != VTK_DOUBLE_MAX;
    foundIntersection &= vtkPlane::IntersectWithLine(bottomRight.data(), bottomLeft.data(), otherNormal.data(), otherOrigin.data(), t, p2) != 0
            || t != VTK_DOUBLE_MAX;

    if (!foundIntersection)
    {
        foundIntersection = true;
        // Now try intersection with vertical borders.
        foundIntersection &= vtkPlane::IntersectWithLine(topRight.data(), bottomRight.data(), otherNormal.data(), otherOrigin.data(), t, p1) != 0
                || t != VTK_DOUBLE_MAX;
        foundIntersection &= vtkPlane::IntersectWithLine(bottomLeft.data(), topLeft.data(), otherNormal.data(), otherOrigin.data(), t, p2) != 0
                || t != VTK_DOUBLE_MAX;
    }

    if (foundIntersection)
    {
        intersectionPoint1 = p1;
        intersectionPoint2 = p2;
    }

    return foundIntersection;
}

double ImagePlane::getDistanceToPoint(const Vector3 &point) const
{
    // TODO An alternative method would be computing distance with QVector3D::distanceToPlane()
    return vtkPlane::DistanceToPlane(point.toArray().data(), Vector3(m_imageOrientation.getNormalVector()).toArray().data(), m_origin.toArray().data());
}

Vector3 ImagePlane::projectPoint(Vector3 point, bool vtkReconstructionHack) const
{
    // First gather the needed plane data to project the point
    std::array<double, 3> rowVector = Vector3(m_imageOrientation.getRowVector());
    std::array<double, 3> columnVector = Vector3(m_imageOrientation.getColumnVector());
    std::array<double, 3> normalVector = Vector3(m_imageOrientation.getNormalVector());

    // Then create the projection matrix
    auto projectionMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    projectionMatrix->Identity();

    for (int column = 0; column < 3; column++)
    {
        projectionMatrix->SetElement(0, column, rowVector[column]);
        projectionMatrix->SetElement(1, column, columnVector[column]);
        projectionMatrix->SetElement(2, column, normalVector[column]);
    }

    // Check if we have to apply vtk hack on projection matrix
    if (vtkReconstructionHack)
    {
        // HACK Patch for the cranium cases that are not working properly (gantry tilt).
        // TODO This is a first aproximation, must be thoroughly tested
        projectionMatrix->SetElement(0, 0, 0);
        projectionMatrix->SetElement(0, 1, 1);
        projectionMatrix->SetElement(0, 2, 0);
    }

    // Once we have the matrix, we can project the point in homogeneous coordinates
    // We must shift the point to project to the plane origin
    point -= m_origin;
    double homogeneousPoint[4] = { point.x, point.y, point.z, 1.0 };

    // Project the point with the matrix
    double homogeneousProjectedPoint[4];
    projectionMatrix->MultiplyPoint(homogeneousPoint, homogeneousProjectedPoint);

    // Convert the homogeneous projected point to the 3D projected point to return
    return homogeneousProjectedPoint;
}

}
