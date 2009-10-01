#ifndef UDGVOMIVOXELSHADER_H
#define UDGVOMIVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que pinta la VoMI de cada voxel.
 */
class VomiVoxelShader : public VoxelShader {

public:

    VomiVoxelShader();
    virtual ~VomiVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor );
    void setCombine( bool on );
    void setAdditive( bool on, float weight );

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
    QVector<float> m_vomi;
    float m_maximumVomi;
    float m_vomiFactor;
    bool m_combine;
    bool m_additive;
    float m_additiveWeight;

};


inline HdrColor VomiVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VomiVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VomiVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( m_data );

    HdrColor color( 1.0f, 1.0f, 1.0f );

    if ( m_combine ) color = baseColor;
    else color.alpha = m_ambientColors[m_data[offset]].alpha;

    if ( !color.isTransparent() && !color.isBlack() )
    {
        float vomi = m_vomiFactor * m_vomi.at( offset ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );

        if ( !m_additive ) color.multiplyColorBy( gray );
        else
        {
            HdrColor vomiColor = m_ambientColors[m_data[offset]];
            vomiColor.alpha = 0.0f;
            color = color.multiplyColorBy( 1.0f - m_additiveWeight ) + vomiColor.multiplyColorBy( m_additiveWeight * gray );
        }
    }

    return color;
}


inline HdrColor VomiVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

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

        float vomi = m_vomiFactor * TrilinearInterpolator::interpolate<float>( m_vomi.constData(), offsets, weights ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );

        if ( !m_additive ) color.multiplyColorBy( gray );
        else
        {
            double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
            HdrColor vomiColor = m_ambientColors[static_cast<int>(value)];
            vomiColor.alpha = 0.0f;
            color = color.multiplyColorBy( 1.0f - m_additiveWeight ) + vomiColor.multiplyColorBy( m_additiveWeight * gray );
        }
    }

    return color;
}


}


#endif
