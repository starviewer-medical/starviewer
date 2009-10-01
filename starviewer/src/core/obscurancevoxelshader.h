#ifndef UDGOBSCURANCEVOXELSHADER_H
#define UDGOBSCURANCEVOXELSHADER_H


#include "voxelshader.h"

#include "obscurance.h"
#include "transferfunction.h"
#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


/**
 * Voxel shader que aplica obscurances.
 */
class ObscuranceVoxelShader : public VoxelShader {

public:

    ObscuranceVoxelShader();
    virtual ~ObscuranceVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    /// Assigna l'array d'obscurances.
    void setObscurance( const Obscurance *obscurance );
    /// Assigna el factor pel qual es multipliquen les obscurances.
    void setFactor( double factor );
    /// Assigna els filtres d'obscurances: per sota de \a low es considera 0 i per sobre de \a high es considera 1.
    void setFilters( double low, double high );
    void setCombine( bool on );
    void setAdditive( bool on, double weight );

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

    /// Omple la taula d'opacitats.
    void precomputeAmbientColors();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;
    const Obscurance *m_obscurance;
    double m_factor;
    double m_lowFilter, m_highFilter;
    bool m_combine;
    bool m_additive;
    double m_additiveWeight;

};


inline HdrColor ObscuranceVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor ObscuranceVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                              float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor ObscuranceVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                                const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    //Q_ASSERT( m_data );
    Q_ASSERT( m_obscurance );

    HdrColor color( 1.0f, 1.0f, 1.0f );

    if ( m_combine ) color = baseColor;
    else color.alpha = m_ambientColors[m_data[offset]].alpha;

    if ( !color.isTransparent() && !color.isBlack() )
    {
        double obscurance = m_obscurance->obscurance( offset );
        if ( obscurance < m_lowFilter ) obscurance = 0.0;
        else if ( obscurance > m_highFilter ) obscurance = 1.0;
        obscurance *= m_factor;

        if ( !m_additive ) color.multiplyColorBy( obscurance );
        else
        {
            HdrColor obscuranceColor = m_ambientColors[m_data[offset]];
            obscuranceColor.alpha = 0.0f;
            color = color.multiplyColorBy( 1.0 - m_additiveWeight ) + obscuranceColor.multiplyColorBy( m_additiveWeight * obscurance );
        }
    }

    return color;
}


inline HdrColor ObscuranceVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                                float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );
    Q_ASSERT( m_obscurance );

    int offsets[8];
    double weights[8];
    bool offsetsAndWeights = false;

    HdrColor color( 1.0f, 1.0f, 1.0f );

    if ( m_combine ) color = baseColor;
    else
    {
        interpolator->getOffsetsAndWeights( position, offsets, weights );
        offsetsAndWeights = true;
        double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
        color.alpha = m_ambientColors[static_cast<int>(value)].alpha;
    }

    if ( !color.isTransparent() && !color.isBlack() )
    {
        if ( !offsetsAndWeights ) interpolator->getOffsetsAndWeights( position, offsets, weights );

        double obscurance;
        if ( m_obscurance->isDoublePrecision() )
            obscurance = TrilinearInterpolator::interpolate<double>( m_obscurance->doubleObscurance(), offsets, weights );
        else
            obscurance = TrilinearInterpolator::interpolate<float>( m_obscurance->floatObscurance(), offsets, weights );
        if ( obscurance < m_lowFilter ) obscurance = 0.0;
        else if ( obscurance > m_highFilter ) obscurance = 1.0;
        obscurance *= m_factor;

        if ( !m_additive ) color.multiplyColorBy( obscurance );
        else
        {
            double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
            HdrColor obscuranceColor = m_ambientColors[static_cast<int>(value)];
            obscuranceColor.alpha = 0.0f;
            color = color.multiplyColorBy( 1.0 - m_additiveWeight ) + obscuranceColor.multiplyColorBy( m_additiveWeight * obscurance );
        }
    }

    return color;
}


}


#endif
