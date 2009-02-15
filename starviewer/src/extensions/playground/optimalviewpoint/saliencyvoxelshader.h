#ifndef UDGSALIENCYVOXELSHADER_H
#define UDGSALIENCYVOXELSHADER_H


#include "voxelshader.h"

#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que aplica efectes de saliency.
 */
class SaliencyVoxelShader : public VoxelShader {

public:

    SaliencyVoxelShader();
    virtual ~SaliencyVoxelShader();

    /// Assigna l'array de saliency.
    void setSaliency( const double *saliency );
    /// Assigna els paràmetres per escalar la saliency: el mínim que s'aplicarà serà 1 - a i el màxim 1 + b.
    void setScale( double a, double b );
    /// Assigna els filtres de saliency: per sota de \a low es considera el mínim i per sobre de \a high es considera el màxim.
    void setFilters( double low, double high );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                            const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                      const HdrColor &baseColor = HdrColor() );
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const double *m_saliency;
    double m_a;
    double m_lowFilter, m_highFilter;
    double m_minimum, m_maximum, m_ab;

};


inline HdrColor SaliencyVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                            const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor SaliencyVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                                            const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor SaliencyVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_CHECK_PTR( m_saliency );

    if ( baseColor.isTransparent() ) return baseColor;

    double saliency = 1.0 + m_saliency[offset] * m_ab - m_a;
    if ( saliency < m_lowFilter ) saliency = m_minimum;
    else if ( saliency > m_highFilter ) saliency = m_maximum;

    HdrColor shaded = baseColor;
    shaded.alpha *= saliency;

    return shaded;
}


inline HdrColor SaliencyVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                              float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

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


}


#endif
