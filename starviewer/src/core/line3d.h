#ifndef UDGLINE3D_H
#define UDGLINE3D_H

#include "point3d.h"

#include <QPair>

namespace udg {

/**
    Class to represent a 3D Line. Double precision.
 */
class Line3D {
public:
    Line3D();
    Line3D(const Point3D &firstPoint, const Point3D &secondPoint);
    ~Line3D();

    /// Methods to set the points of the line
    void setFirstPoint(const Point3D &point);
    void setSecondPoint(const Point3D &point);

    /// Methods to get the points of the line
    Point3D getFirstPoint() const;
    Point3D getSecondPoint() const;
    /// Returns the i-th point of the line. If index is out of range (< 0 || > 2) a default constructed value is returned.
    Point3D getPoint(int i) const;

private:
    /// The structure containing the line points
    QPair<Point3D, Point3D> m_linePoints;
};

} // End namespace udg

#endif
