#ifndef POVSPHERECLOUD_H
#define POVSPHERECLOUD_H

#include <vector>

class Vector3;

/**
	@author Marc Ruiz <marc@localhost>
*/
class POVSphereCloud
{

public:

    POVSphereCloud( float radius = 10.0, unsigned short depth = 3 );
    ~POVSphereCloud();

    void createPOVCloud();
    const std::vector< Vector3 > & getVertices() const;
    const std::vector< Vector3 > & getGeographicVertices() const;

private:

    typedef std::vector< unsigned short > FacesList;

    double m_radius;
    unsigned short m_depth;

    std::vector< Vector3 > m_vertices;
    FacesList m_faces;
    std::vector< Vector3 > m_normals;
    std::vector< Vector3 > m_geographicVertices;

    void createIcosahedronVertices();
    void createIcosahedronFaces();
    bool findSphereCloudVertex( const Vector3 & v, unsigned short & pos ) const;
    void createSphereCloudTriangle( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 );
    void subdivide( Vector3 v1, Vector3 v2, Vector3 v3, unsigned short depth );
    void createGeographicVertices();

};

#endif
