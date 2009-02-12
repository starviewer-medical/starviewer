#ifndef UDGVOXELSALIENCYVOXELSHADER_H
#define UDGVOXELSALIENCYVOXELSHADER_H


#include "voxelshader2.h"

#include <QVector>

#include <vtkDirectionEncoder.h>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


class vtkEncodedGradientEstimator;


namespace udg {


/**
 * Voxel shader que pinta la saliency de cada voxel.
 */
class VoxelSaliencyVoxelShader : public VoxelShader2 {

public:

    VoxelSaliencyVoxelShader();
    virtual ~VoxelSaliencyVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setVoxelSaliencies( const QVector<float> &voxelSaliencies, float maximumSaliency, float saliencyFactor );
    void setDiffuseLighting( bool on );
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
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
    bool m_diffuseLighting;
    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;

};


inline HdrColor VoxelSaliencyVoxelShader::shade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VoxelSaliencyVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VoxelSaliencyVoxelShader::nvShade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor )
{
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

        if ( m_diffuseLighting )
        {
            float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
            Vector3 normal( gradient[0], gradient[1], gradient[2] );
            double dotProduct = direction * normal;
            color.multiplyColorBy( -dotProduct );
        }
    }

    return color;
}


inline HdrColor VoxelSaliencyVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor )
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

        if ( m_diffuseLighting )
        {
            Vector3 normal;

            for ( int i = 0; i < 8; i++ )
            {
                float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offsets[i]] );
                Vector3 localNormal( gradient[0], gradient[1], gradient[2] );
                normal += weights[i] * localNormal;
            }

            double dotProduct = direction * normal;
            color.multiplyColorBy( -dotProduct );
        }
    }

    return color;
}


}


#endif
