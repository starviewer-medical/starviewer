#ifndef UDGCOLORVOMIVOXELSHADER_H
#define UDGCOLORVOMIVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include <vtkDirectionEncoder.h>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


class vtkEncodedGradientEstimator;


namespace udg {


/**
 * Voxel shader que pinta la color VoMI de cada voxel.
 */
class ColorVomiVoxelShader : public VoxelShader {

public:

    ColorVomiVoxelShader();
    virtual ~ColorVomiVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float colorVomiFactor );
    void setCombine( bool on );
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );

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

    /// Omple la taula d'opacitats.
    void precomputeOpacities();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    float *m_opacities;
    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;
    float m_colorVomiFactor;
    bool m_combine;
    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;

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

    Q_ASSERT( m_data );

    HdrColor color( 1.0f, 1.0f, 1.0f );

    if ( m_combine ) color = baseColor;
    else color.alpha = m_opacities[m_data[offset]];

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

        Vector3Float colorVomi = m_colorVomiFactor * TrilinearInterpolator::interpolate<Vector3Float>( m_colorVomi.constData(), offsets, weights ) / m_maximumColorVomi;
        HdrColor shade( qMax( 1.0f - colorVomi.x, 0.0f ), qMax( 1.0f - colorVomi.y, 0.0f ), qMax( 1.0f - colorVomi.z, 0.0f ) );
        color *= shade;
    }

    return color;
}


}


#endif
