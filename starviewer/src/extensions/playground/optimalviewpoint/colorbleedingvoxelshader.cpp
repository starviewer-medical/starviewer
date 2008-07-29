#include "colorbleedingvoxelshader.h"

#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


ColorBleedingVoxelShader::ColorBleedingVoxelShader()
    : VoxelShader()
{
    m_colorBleeding = 0;
    m_factor = 1.0;
}


ColorBleedingVoxelShader::~ColorBleedingVoxelShader()
{
}


void ColorBleedingVoxelShader::setColorBleeding( const Vector3 *colorBleeding )
{
    m_colorBleeding = colorBleeding;
}


void ColorBleedingVoxelShader::setFactor( double factor )
{
    m_factor = factor;
}


HdrColor ColorBleedingVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_colorBleeding );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    Vector3 colorBleeding = m_colorBleeding[offset];
    colorBleeding *= m_factor;

    return baseColor * HdrColor( colorBleeding.x, colorBleeding.y, colorBleeding.z );
}


HdrColor ColorBleedingVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_colorBleeding );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    Vector3 colorBleeding = TrilinearInterpolator::interpolate<Vector3>( m_colorBleeding, offsets, weights );
    colorBleeding *= m_factor;

    return baseColor * HdrColor( colorBleeding.x, colorBleeding.y, colorBleeding.z );
}


QString ColorBleedingVoxelShader::toString() const
{
    return "ColorBleedingVoxelShader";
}


}
