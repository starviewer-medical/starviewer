#ifndef UDGVIEWPOINTGENERATOR_H
#define UDGVIEWPOINTGENERATOR_H


#include <QVector>


namespace udg {


class Vector3;


/**
 * Un generador de punts de vista distribuïts uniformement o quasi-uniforme.
 */
class ViewpointGenerator {

public:

    /// Crea el generador sense que generi cap punt de vista.
    ViewpointGenerator();
    ~ViewpointGenerator();

    /// Genera 4 punts de vista distribuïts uniformement.
    void setToUniform4( float radius = 1.0 );
    /// Genera 8 punts de vista distribuïts uniformement.
    void setToUniform6( float radius = 1.0 );
    /// Genera 12 punts de vista distribuïts uniformement.
    void setToUniform8( float radius = 1.0 );
    /// Genera 16 punts de vista distribuïts uniformement.
    void setToUniform12( float radius = 1.0 );
    /// Genera 20 punts de vista distribuïts uniformement.
    void setToUniform20( float radius = 1.0 );
    /// Genera 10*4^depth+2 punts de vista distribuïts quasi-uniformement.
    void setToQuasiUniform( unsigned char depth, float radius = 1.0 );
    /// Retorna els punts de vista generats.
    QVector<Vector3> viewpoints() const;
    /// Retorna el punt de vista \a i d'entre els generats.
    Vector3 viewpoint( int i ) const;

private:

    QVector<Vector3> m_viewpoints;

};


}


#endif
