#ifndef UDGCOLORBLEEDINGVOXELSHADER_H
#define UDGCOLORBLEEDINGVOXELSHADER_H


#include "voxelshader.h"

#include "obscurance.h"
#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


/**
 * Voxel shader que aplica color bleeding.
 */
class ColorBleedingVoxelShader : public VoxelShader {

public:

    ColorBleedingVoxelShader();
    virtual ~ColorBleedingVoxelShader();

    /// Assigna l'array de color bleeding.
    void setColorBleeding( const Obscurance *colorBleeding );
    /// Assigna el factor pel qual es multiplica el color bleeding.
    void setFactor( double factor );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const Obscurance *m_colorBleeding;
    double m_factor;

};


inline HdrColor ColorBleedingVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    return nvShade( offset, direction, baseColor );
}


inline HdrColor ColorBleedingVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return nvShade( position, direction, interpolator, baseColor );
}


inline HdrColor ColorBleedingVoxelShader::nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_ASSERT( m_colorBleeding );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    Vector3 colorBleeding = m_colorBleeding->colorBleeding( offset );
    colorBleeding *= m_factor;

    return baseColor * HdrColor( colorBleeding.x, colorBleeding.y, colorBleeding.z );
}


inline HdrColor ColorBleedingVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_colorBleeding );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    Vector3 colorBleeding;
    if ( m_colorBleeding->isDoublePrecision() )
        colorBleeding = TrilinearInterpolator::interpolate<Vector3Double>( m_colorBleeding->doubleColorBleeding(), offsets, weights );
    else
        colorBleeding = TrilinearInterpolator::interpolate<Vector3Float>( m_colorBleeding->floatColorBleeding(), offsets, weights );
    colorBleeding *= m_factor;

    return baseColor * HdrColor( colorBleeding.x, colorBleeding.y, colorBleeding.z );
}


}


#endif
