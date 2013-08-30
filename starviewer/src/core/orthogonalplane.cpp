#include "orthogonalplane.h"
#include <QString>
#include "logging.h"

namespace udg {

OrthogonalPlane::OrthogonalPlane(Plane plane) :
    m_plane(plane)
{
}

void OrthogonalPlane::getXYZIndexes(int &x, int &y, int &z) const
{
    x = getXIndex();
    y = getYIndex();
    z = getZIndex();
}

int OrthogonalPlane::getXIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 0;

        case YZPlane:
            return 1;

        case XZPlane:
            return 0;

        default:
            DEBUG_LOG(QString("Unexpected plane: %1").arg(m_plane));
            ERROR_LOG(QString("Unexpected plane: %1").arg(m_plane));
            return -1;
    }
}

int OrthogonalPlane::getYIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 1;

        case YZPlane:
            return 2;

        case XZPlane:
            return 2;

        default:
            DEBUG_LOG(QString("Unexpected plane: %1").arg(m_plane));
            ERROR_LOG(QString("Unexpected plane: %1").arg(m_plane));
            return -1;
    }
}

int OrthogonalPlane::getZIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 2;

        case YZPlane:
            return 0;

        case XZPlane:
            return 1;

        default:
            DEBUG_LOG(QString("Unexpected plane: %1").arg(m_plane));
            ERROR_LOG(QString("Unexpected plane: %1").arg(m_plane));
            return -1;
    }
}

OrthogonalPlane::operator Plane() const
{
    return m_plane;
}

}
