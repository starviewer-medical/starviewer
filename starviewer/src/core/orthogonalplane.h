#ifndef ORTHOGONALPLANE_H
#define ORTHOGONALPLANE_H

namespace udg {

/// Class that defines an orthogonal plane
class OrthogonalPlane
{
public:
    /// Definition of the 3 othogonal planes
    enum OrthogonalPlaneType { XYPlane = 2, YZPlane = 0, XZPlane = 1 };

private:
    OrthogonalPlane();
};

}

#endif // ORTHOGONALPLANE_H
