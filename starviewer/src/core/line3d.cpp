#include "line3d.h"

namespace udg {

Line3D::Line3D()
{
}

Line3D::Line3D(const Point3D &firstPoint, const Point3D &secondPoint)
{
    setFirstPoint(firstPoint);
    setSecondPoint(secondPoint);
}

Line3D::~Line3D()
{
}

void Line3D::setFirstPoint(const Point3D &point)
{
    m_linePoints.first = point;
}

void Line3D::setSecondPoint(const Point3D &point)
{
    m_linePoints.second = point;
}

Point3D Line3D::getFirstPoint() const
{
    return m_linePoints.first;
}

Point3D Line3D::getSecondPoint() const
{
    return m_linePoints.second;
}

Point3D Line3D::getPoint(int i) const
{
    switch (i)
    {
        case 0:
            return getFirstPoint();
            break;
    
        case 1:
            return getSecondPoint();
            break;

        default:
            return Point3D();
            break;
    }
}

} // End namespace udg
