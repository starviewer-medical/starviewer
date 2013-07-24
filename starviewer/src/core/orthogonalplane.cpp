#include "orthogonalplane.h"
#include <QString>
#include "logging.h"

namespace udg {

OrthogonalPlane::OrthogonalPlane()
{
}

void OrthogonalPlane::getXYZIndexesForView(int &x, int &y, int &z, OrthogonalPlane::OrthogonalPlaneType view)
{
    x = OrthogonalPlane::getXIndexForView(view);
    y = OrthogonalPlane::getYIndexForView(view);
    z = OrthogonalPlane::getZIndexForView(view);
}

int OrthogonalPlane::getXIndexForView(OrthogonalPlane::OrthogonalPlaneType view)
{
    switch (view)
    {
        case OrthogonalPlane::XYPlane:
            return 0;

        case OrthogonalPlane::YZPlane:
            return 1;

        case OrthogonalPlane::XZPlane:
            return 0;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}

int OrthogonalPlane::getYIndexForView(OrthogonalPlane::OrthogonalPlaneType view)
{
    switch (view)
    {
        case OrthogonalPlane::XYPlane:
            return 1;

        case OrthogonalPlane::YZPlane:
            return 2;

        case OrthogonalPlane::XZPlane:
            return 2;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}

int OrthogonalPlane::getZIndexForView(OrthogonalPlane::OrthogonalPlaneType view)
{
    switch (view)
    {
        case OrthogonalPlane::XYPlane:
            return 2;

        case OrthogonalPlane::YZPlane:
            return 0;

        case OrthogonalPlane::XZPlane:
            return 1;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}
}
