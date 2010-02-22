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

    void setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor );
    void setYB( float y, float b );

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

    QVector<float> m_vomi;
    float m_maximumVomi;
    float m_vomiFactor;
    float m_y;
    float m_b;

};


inline HdrColor VomiCoolWarmVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VomiCoolWarmVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VomiCoolWarmVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    HdrColor color = baseColor;

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


inline HdrColor VomiCoolWarmVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );

    HdrColor color = baseColor;

    if ( !color.isTransparent() && !color.isBlack() )
    {
        int offsets[8];
        double weights[8];
        interpolator->getOffsetsAndWeights( position, offsets, weights );

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
