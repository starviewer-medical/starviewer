#ifndef UDGVOXELSHADER_H
#define UDGVOXELSHADER_H


#include "hdrcolor.h"


namespace udg {


class TrilinearInterpolator;
class Vector3;


/**
 * Aquesta classe implementa els mètodes per retornar el color d'un vòxel. El mètode shade ha de ser implementat per les classes filles.
 */
class VoxelShader {

public:

    VoxelShader();
    virtual ~VoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned char *data );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const = 0;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const unsigned char *m_data;

};


}


#endif
