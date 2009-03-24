#ifndef UDGVOMICOOLWARMVOXELSHADER_H
#define UDGVOMICOOLWARMVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que pinta la VoMI de cada voxel.
 */
class VomiCoolWarmVoxelShader : public VoxelShader {

public:

    VomiCoolWarmVoxelShader();
    virtual ~VomiCoolWarmVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor );
    void setYB( float y, float b );
    void setCombine( bool on );

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
    void precomputeOpacities();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    float *m_opacities;
    QVector<float> m_vomi;
    float m_maximumVomi;
    float m_vomiFactor;
    float m_y;
    float m_b;
    bool m_combine;

};


inline HdrColor VomiCoolWarmVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                                const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VomiCoolWarmVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                                float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VomiCoolWarmVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                                  const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( m_data );

    HdrColor color( 1.0f, 1.0f, 1.0f );

    if ( m_combine ) color = baseColor;
    else color.alpha = m_opacities[m_data[offset]];

    if ( !color.isTransparent() && !color.isBlack() )
    {
        float vomi = m_vomiFactor * m_vomi.at( offset ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );
        color.red *= m_y * gray;
        color.green *= m_y * gray;
        color.blue *= m_b * ( 1.0f - gray );
    }

    return color;
}


inline HdrColor VomiCoolWarmVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                                  float remainingOpacity, const HdrColor &baseColor )
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
        color.alpha = m_opacities[static_cast<int>(value)];
    }

    if ( !color.isTransparent() && !color.isBlack() )
    {
        if ( !offsetsAndWeights ) interpolator->getOffsetsAndWeights( position, offsets, weights );

        float vomi = m_vomiFactor * TrilinearInterpolator::interpolate<float>( m_vomi.constData(), offsets, weights ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );
        color.red *= m_y * gray;
        color.green *= m_y * gray;
        color.blue *= m_b * ( 1.0f - gray );
    }

    return color;
}


}


#endif
