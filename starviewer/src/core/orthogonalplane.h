#ifndef ORTHOGONALPLANE_H
#define ORTHOGONALPLANE_H

namespace udg {

/// Class that defines an orthogonal plane
class OrthogonalPlane
{
public:
    /// Definition of the 3 othogonal planes
    enum OrthogonalPlaneType { XYPlane = 2, YZPlane = 0, XZPlane = 1 };

    /// Obté els índexs dels eixos de coordenades corresponents a la vista 2D indicada
    /// Aquests índexs indiquen com hem d'indexar una coordenada 3D per obtenir
    /// la corresponent projecció sobre la vista 2D sobre la que estem operant
    /// En cas que el valor de view no sigui un dels esperats, el valor de l'índex serà -1
    static void getXYZIndexesForView(int &x, int &y, int &z, OrthogonalPlaneType view);
    static int getXIndexForView(OrthogonalPlaneType view);
    static int getYIndexForView(OrthogonalPlaneType view);
    static int getZIndexForView(OrthogonalPlaneType view);

private:
    OrthogonalPlane();
};

}

#endif // ORTHOGONALPLANE_H
