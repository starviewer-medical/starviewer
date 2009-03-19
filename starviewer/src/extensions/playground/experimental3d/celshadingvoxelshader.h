#ifndef UDGCELSHADINGVOXELSHADER_H
#define UDGCELSHADINGVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>


namespace udg {


/**
 * Voxel shader que aplica cel-shading
 */
class CelShadingVoxelShader : public VoxelShader {

public:

    CelShadingVoxelShader();
    virtual ~CelShadingVoxelShader();

    /// Assigna el nombre de quantums.
    void setQuantums( int quantums );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    float quantize( float value ) const;

    int m_quantums;
    QVector<float> m_texture;

};


inline HdrColor CelShadingVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor CelShadingVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor CelShadingVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( offset );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    HdrColor color( baseColor );
    color.red = quantize( color.red );
    color.green = quantize( color.green );
    color.blue = quantize( color.blue );
    return color;
}


inline HdrColor CelShadingVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( interpolator );
    Q_UNUSED( remainingOpacity );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    HdrColor color( baseColor );
    color.red = quantize( color.red );
    color.green = quantize( color.green );
    color.blue = quantize( color.blue );
    return color;
}


inline float CelShadingVoxelShader::quantize( float value ) const
{
    if ( value <= 0.0f ) return 0.0f;

//    return ( ceil( value * m_quantums ) - 1.0 ) / ( m_quantums - 1 );

    int i = static_cast<int>( ceil( value * m_quantums ) ) - 1;
    if ( i >= m_quantums ) i = m_quantums - 1;
    return m_texture.at( i );
}


}


#endif
