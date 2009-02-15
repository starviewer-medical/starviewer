#ifndef UDGVOXELSHADER_H
#define UDGVOXELSHADER_H


#include "hdrcolor.h"
#include "vector3.h"


namespace udg {


class TrilinearInterpolator;


/**
 * Aquesta classe implementa els mètodes per retornar el color d'un vòxel. El mètode shade ha de ser implementat per les classes filles.
 */
class VoxelShader {

public:

    VoxelShader();
    virtual ~VoxelShader();

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() ) = 0;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                            const HdrColor &baseColor = HdrColor() ) = 0;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

};


}


#endif
