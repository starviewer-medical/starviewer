#ifndef UDGVOMIVOXELSHADER_H
#define UDGVOMIVOXELSHADER_H


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
class VomiVoxelShader : public VoxelShader {

public:

    VomiVoxelShader();
    virtual ~VomiVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor );
    void setDiffuseLighting( bool on );
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );

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
    bool m_diffuseLighting;
    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;

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
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color;
    color.alpha = m_opacities[m_data[offset]];

    if ( !color.isTransparent() )
    {
        float vomi = m_vomiFactor * m_vomi.at( offset ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );

        if ( m_diffuseLighting )
        {
            float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
            Vector3 normal( gradient[0], gradient[1], gradient[2] );
            double dotProduct = direction * normal;
            gray *= -dotProduct;
        }

        color.red = color.green = color.blue = gray;
    }

    return color;
}


inline HdrColor VomiVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
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
        float vomi = m_vomiFactor * TrilinearInterpolator::interpolate<float>( m_vomi.constData(), offsets, weights ) / m_maximumVomi;
        float gray = qMax( 1.0f - vomi, 0.0f );

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
            gray *= -dotProduct;
        }

        color.red = color.green = color.blue = gray;
    }

    return color;
}


}


#endif
