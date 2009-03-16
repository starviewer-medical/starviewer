#ifndef UDGVOXELSALIENCYVOXELSHADER_H
#define UDGVOXELSALIENCYVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include <vtkDirectionEncoder.h>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


class vtkEncodedGradientEstimator;


namespace udg {


/**
 * Voxel shader que pinta la saliency de cada voxel.
 */
class VoxelSaliencyVoxelShader : public VoxelShader {

public:

    VoxelSaliencyVoxelShader();
    virtual ~VoxelSaliencyVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setVoxelSaliencies( const QVector<float> &voxelSaliencies, float maximumSaliency, float saliencyFactor );

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
    QVector<float> m_voxelSaliencies;
    float m_maximumSaliency;
    float m_saliencyFactor;

};


inline HdrColor VoxelSaliencyVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                                 const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VoxelSaliencyVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                                 float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VoxelSaliencyVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity,
                                                   const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color;
    color.alpha = m_opacities[m_data[offset]];

    if ( !color.isTransparent() )
    {
        float saliency = m_saliencyFactor * m_voxelSaliencies.at( offset ) / m_maximumSaliency;
        color.red = saliency > 0.8f ? 1.0f : saliency > 0.6f ? 5.0f * ( saliency - 0.6f ) : saliency > 0.2f ? 0.0f : 1.0f - 5.0f * saliency;
        color.green = saliency > 0.8f ? 1.0f - 5.0f * ( saliency - 0.8f ) : saliency > 0.4f ? 1.0f : saliency > 0.2f ? 5.0f * ( saliency - 0.2f ) : 0.0f;
        color.blue = saliency > 0.6f ? 0.0f : saliency > 0.4f ? 1.0f - 5.0f * ( saliency - 0.4f ) : 1.0f;
    }

    return color;
}


inline HdrColor VoxelSaliencyVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                                   float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
    HdrColor color;
    color.alpha = m_opacities[static_cast<int>(value)];

    if ( !color.isTransparent() )
    {
        float saliency = m_saliencyFactor * TrilinearInterpolator::interpolate<float>( m_voxelSaliencies.constData(), offsets, weights ) / m_maximumSaliency;
        color.red = saliency > 0.8f ? 1.0f : saliency > 0.6f ? 5.0f * ( saliency - 0.6f ) : saliency > 0.2f ? 0.0f : 1.0f - 5.0f * saliency;
        color.green = saliency > 0.8f ? 1.0f - 5.0f * ( saliency - 0.8f ) : saliency > 0.4f ? 1.0f : saliency > 0.2f ? 5.0f * ( saliency - 0.2f ) : 0.0f;
        color.blue = saliency > 0.6f ? 0.0f : saliency > 0.4f ? 1.0f - 5.0f * ( saliency - 0.4f ) : 1.0f;
    }

    return color;
}


}


#endif
