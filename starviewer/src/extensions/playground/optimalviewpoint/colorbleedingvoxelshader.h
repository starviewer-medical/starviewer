#ifndef UDGCOLORBLEEDINGVOXELSHADER_H
#define UDGCOLORBLEEDINGVOXELSHADER_H


#include "voxelshader.h"


namespace udg {


/**
 * Voxel shader que aplica color bleeding.
 */
class ColorBleedingVoxelShader : public VoxelShader {

public:

    ColorBleedingVoxelShader();
    virtual ~ColorBleedingVoxelShader();

    /// Assigna l'array de color bleeding.
    void setColorBleeding( const Vector3 *colorBleeding );
    /// Assigna el factor pel qual es multiplica el color bleeding.
    void setFactor( double factor );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const Vector3 *m_colorBleeding;
    double m_factor;

};


}


#endif
