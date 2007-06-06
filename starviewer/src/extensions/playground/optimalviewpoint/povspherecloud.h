#ifndef POVSPHERECLOUD_H
#define POVSPHERECLOUD_H



#include <vector>



class Vector3;



/**
 * Aquesta classe genera un núvol de punts distribuïts uniformement sobre la
 * superfície d'una esfera mitjançant la subdivisió recursiva de les cares,
 * començant amb un icosaedre.
 *
 * Es pot decidir el radi de l'esfera i fins a quin nivell es subdivideix.
 */
class POVSphereCloud {


public:

    /// Crea l'objecte i inicialitza els paràmetres, però no genera els punts.
    POVSphereCloud( float radius = 10.0, unsigned short depth = 3 );
    /// Destructor.
    ~POVSphereCloud();

    /// Genera els punts segons els paràmetres definits al constructor.
    void createPOVCloud();
    /// Retorna els punts en coordenades cartesianes.
    const std::vector< Vector3 > & getVertices() const;
    /// Retorna els punts en coordenades geogràfiques (radi, latitud, longitud).
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
