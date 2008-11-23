#include "saliencyvoxelshader.h"

#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


SaliencyVoxelShader::SaliencyVoxelShader()
    : VoxelShader()
{
    m_saliency = 0;
    setScale( 1.0, 0.0 );
    setFilters( 0.0, 2.0 );
}


SaliencyVoxelShader::~SaliencyVoxelShader()
{
}


void SaliencyVoxelShader::setSaliency( const double *saliency )
{
    m_saliency = saliency;
}


void SaliencyVoxelShader::setScale( double a, double b )
{
    m_a = a;
    m_minimum = 1.0 - a; m_maximum = 1.0 + b; m_ab = a + b;
}


void SaliencyVoxelShader::setFilters( double low, double high )
{
    m_lowFilter = low; m_highFilter = high;
}


HdrColor SaliencyVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( m_saliency );

    if ( baseColor.isTransparent() ) return baseColor;

    double saliency = 1.0 + m_saliency[offset] * m_ab - m_a;
    if ( saliency < m_lowFilter ) saliency = m_minimum;
    else if ( saliency > m_highFilter ) saliency = m_maximum;

    HdrColor shaded = baseColor;
    shaded.alpha *= saliency;

    return shaded;
}


HdrColor SaliencyVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_saliency );

    if ( baseColor.isTransparent() ) return baseColor;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double saliency = 1.0 + TrilinearInterpolator::interpolate<double>( m_saliency, offsets, weights ) * m_ab - m_a;
    if ( saliency < m_lowFilter ) saliency = m_minimum;
    else if ( saliency > m_highFilter ) saliency = m_maximum;

    HdrColor shaded = baseColor;
    shaded.alpha *= saliency;

    return shaded;
}


QString SaliencyVoxelShader::toString() const
{
    return "SaliencyVoxelShader";
}


}
