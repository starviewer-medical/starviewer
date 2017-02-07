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

#ifndef UDG_PLANE_H
#define UDG_PLANE_H

#include "vector3.h"

namespace udg {

/**
 * @brief The Plane class represents an infinite plane defined by a normal and a point.
 */
class Plane
{

public:

    /// Creates a plane with the given normal and point.
    Plane(Vector3 normal, Vector3 point);

    /// Returns the normal.
    const Vector3& getNormal() const;
    /// Sets the normal.
    void setNormal(Vector3 normal);

    /// Returns the point.
    const Vector3& getPoint() const;
    /// Sets the point.
    void setPoint(Vector3 point);

    /// Returns true if both planes have the same normal and point and false otherwise.
    bool operator==(const Plane &plane) const;
    /// Returns true if both planes don't have the same normal and point and false otherwise.
    bool operator!=(const Plane &plane) const;

    /// Returns the signed distance between this plane and the given point.
    /// The signed distance is positive in the direction of the normal and negative in the opposite direction.
    double signedDistanceToPoint(const Vector3 &point) const;

private:

    /// The normal of this plane.
    Vector3 m_normal;
    /// A point in this plane.
    Vector3 m_point;

};

} // namespace udg

#endif // UDG_PLANE_H
