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

#ifndef UDGIMAGEPLANE_H
#define UDGIMAGEPLANE_H

#include "imageorientation.h"
#include "pixelspacing2d.h"
#include "vector3.h"

namespace udg {

class Image;

/**
 * @brief The ImagePlane class represents the bounded plane of an image.
 *
 * The plane stores the image orientation, origin, spacing, row and column length, and thickness of the image.
 */
class ImagePlane {

public:
    ImagePlane();

    /// Returns the image plane orientation.
    const ImageOrientation& getImageOrientation() const;
    /// Sets the image plane orientation.
    void setImageOrientation(ImageOrientation imageOrientation);

    /// Returns the top left corner of the plane.
    const Vector3& getOrigin() const;
    /// Sets the top left corner of the plane.
    void setOrigin(Vector3 origin);
    /// Sets the top left corner of the plane.
    void setOrigin(double x, double y, double z);

    /// Returns the pixel spacing.
    const PixelSpacing2D& getSpacing() const;
    /// Sets the pixel spacing.
    void setSpacing(PixelSpacing2D spacing);

    /// Returns the plane width.
    double getRowLength() const;
    /// Sets the plane width.
    void setRowLength(double length);

    /// Returns the plane height.
    double getColumnLength() const;
    /// Sets the plane height.
    void setColumnLength(double length);

    /// Returns the plane thickness.
    double getThickness() const;
    /// Sets the plane thickness.
    void setThickness(double thickness);

    /// Returns the plane center.
    Vector3 getCenter() const;
    /// Modifies the origin so that the given point becomes the center.
    void setCenter(const Vector3 &center);
    /// Modifies the origin so that the given point becomes the center.
    void setCenter(double x, double y, double z);

    /// Fills this ImagePlane from the given Image.
    void fillFromImage(const Image *image);

    bool operator ==(const ImagePlane &imagePlane) const;
    bool operator !=(const ImagePlane &imagePlane) const;

    /// Location of the bounds. Central = middle of the plane, Upper = middle of the plane + thickness/2, Lower = middle of the plane - thickness/2.
    enum CornersLocation { Central, Upper, Lower };

    /// Contains the 4 corners of an ImagePlane at the specified location.
    struct Corners
    {
        CornersLocation location;
        Vector3 topLeft, topRight, bottomRight, bottomLeft;
    };

    /// Returns the corners of the plane in the specified location.
    Corners getCorners(CornersLocation location) const;
    /// Returns the central corners of the plane.
    Corners getCentralCorners() const;
    /// Returns the upper corners of the plane.
    Corners getUpperCorners() const;
    /// Returns the lower corners of the plane.
    Corners getLowerCorners() const;

    /// Returns a string representation of this ImagePlane.
    QString toString(bool verbose = false) const;

    /// Computes the intersection between this ImagePlane and \a planeToIntersect.
    /// \param plane Plane to intersect with this one. It's considered an infinite plane.
    /// \param intersectionPoint1 The first intersection point will be stored here.
    /// \param intersectionPoint2 The second intersection point will be stored here.
    /// \param cornersLocation Specify which corners of this ImagePlane will be used to compute the intersection (upper by default)
    ///                        (\a plane is always used with the central location).
    /// \return True if there are intersections, and false otherwise.
    bool getIntersections(const ImagePlane *plane, Vector3 &intersectionPoint1, Vector3 &intersectionPoint2, CornersLocation cornersLocation = Upper) const;

    /// Returns the distance from the given point to this ImagePlane.
    double getDistanceToPoint(const Vector3 &point) const;
    
    /// Projects the given point onto this ImagePlane and returns the result.
    /// The projection consists in changing the coordinates so that the ImagePlane origin is the new (0,0,0),
    /// and the row and column vector the new X and Y axes, respectively.
    Vector3 projectPoint(Vector3 point, bool vtkReconstructionHack = false) const;

private:
    /// The image plane orientation.
    ImageOrientation m_imageOrientation;
    /// Top left corner of the plane.
    Vector3 m_origin;
    /// Pixel spacing.
    PixelSpacing2D m_spacing;
    /// Plane width.
    double m_rowLength;
    /// Plane height.
    double m_columnLength;
    /// Plane thickness.
    double m_thickness;

};

}

#endif
