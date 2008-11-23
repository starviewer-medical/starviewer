/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGSPHEREUNIFORMPOINTCLOUDGENERATOR_H
#define UDGSPHEREUNIFORMPOINTCLOUDGENERATOR_H


#include <QVector>

#include "vector3.h"


namespace udg {


/**
 * Aquesta classe genera un núvol de punts distribuïts uniformement sobre la
 * superfície d'una esfera mitjançant la subdivisió recursiva de les cares,
 * començant amb un icosaedre.
 *
 * Es pot decidir el radi de l'esfera i fins a quin nivell es subdivideix.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class SphereUniformPointCloudGenerator {

public:

    /**
     * Crea l'objecte i inicialitza els paràmetres, però no genera els punts.
     *
     * \param radius Radi de l'esfera.
     * \param depth Nivells de recursió. 0 = 12 punts, 1 = 42 punts, 2 = 162
     * punts, 3 = 642 punts, 4 = 2562 punts, etc. (n = 2^(2*n)*10+2 punts).
     */
    SphereUniformPointCloudGenerator( float radius = 10.0, unsigned char depth = 0 );
    ~SphereUniformPointCloudGenerator();

    /// Genera els punts segons els paràmetres definits al constructor.
    void createPOVCloud();
    /// Retorna els punts en coordenades cartesianes.
    const QVector< Vector3 > & getVertices() const;
    /// Retorna els punts en coordenades geogràfiques (radi, latitud, longitud).
    const QVector< Vector3 > & getGeographicVertices() const;

private:

    typedef QVector< unsigned short > FaceList;

    /// Radi de l'esfera.
    float m_radius;
    /// Nivells de recursió.
    unsigned char m_depth;

    /// Punts en coordenades cartesianes.
    QVector< Vector3 > m_vertices;
    /// Cares.
    FaceList m_faces;
    /// Normals dels punts.
    QVector< Vector3 > m_normals;
    /// Punts en coordenades geogràfiques.
    QVector< Vector3 > m_geographicVertices;

    /// Crea els vèrtexs d'un icosaedre (nivell 0).
    void createIcosahedronVertices();
    /// Crea les cares d'un icosaedre (nivell 0).
    void createIcosahedronFaces();
    /// Busca si un vèrtex concret ja existeix, i si existeix en quina posició es troba.
    bool findSphereCloudVertex( const Vector3 & v, unsigned short & position ) const;
    /// Crea una nova cara amb 3 vèrtexs al núvol de punts.
    void createSphereCloudTriangle( const Vector3 & v1, const Vector3 & v2, const Vector3 & v3 );
    /// Subdivideix el núvol de punts recursivament fins la profunditat depth.
    void subdivide( Vector3 v1, Vector3 v2, Vector3 v3, unsigned short depth );
    /// Genera els punts amb coordenades geogràfiques a partir dels cartesians.
    void createGeographicVertices();

};


}


#endif
