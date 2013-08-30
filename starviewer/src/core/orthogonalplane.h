#ifndef ORTHOGONALPLANE_H
#define ORTHOGONALPLANE_H

namespace udg {

/**
    Class that defines an orthogonal plane.
 */
class OrthogonalPlane {

public:
    /// Definition of the 3 orthogonal planes.
    enum Plane { XYPlane = 2, YZPlane = 0, XZPlane = 1 };

    OrthogonalPlane(Plane plane = XYPlane);

    /// Returns the indices of the coordinate axes corresponding to this orthogonal plane.
    void getXYZIndexes(int &x, int &y, int &z) const;
    int getXIndex() const;
    int getYIndex() const;
    int getZIndex() const;

    /// Conversion operator: allows to use an OrthogonalPlane in a switch.
    operator Plane() const;

private:
    Plane m_plane;

};

}

#endif // ORTHOGONALPLANE_H
