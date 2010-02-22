#ifndef UDGCOLORVOMIVOXELSHADER_H
#define UDGCOLORVOMIVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que pinta la color VoMI de cada voxel.
 */
class ColorVomiVoxelShader : public VoxelShader {

public:

    ColorVomiVoxelShader();
    virtual ~ColorVomiVoxelShader();

    void setColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float colorVomiFactor );

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

    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;
    float m_colorVomiFactor;

};


inline HdrColor ColorVomiVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor ColorVomiVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor ColorVomiVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    HdrColor color = baseColor;

    if ( !color.isTransparent() && !color.isBlack() )
    {
        Vector3Float colorVomi = m_colorVomiFactor * m_colorVomi.at( offset ) / m_maximumColorVomi;
        HdrColor shade( qMax( 1.0f - colorVomi.x, 0.0f ), qMax( 1.0f - colorVomi.y, 0.0f ), qMax( 1.0f - colorVomi.z, 0.0f ) );
        color *= shade;
    }

    return color;
}


inline HdrColor ColorVomiVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
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

        Vector3Float colorVomi = m_colorVomiFactor * TrilinearInterpolator::interpolate<Vector3Float>( m_colorVomi.constData(), offsets, weights ) / m_maximumColorVomi;
        HdrColor shade( qMax( 1.0f - colorVomi.x, 0.0f ), qMax( 1.0f - colorVomi.y, 0.0f ), qMax( 1.0f - colorVomi.z, 0.0f ) );
        color *= shade;
    }

    return color;
}


}


#endif
