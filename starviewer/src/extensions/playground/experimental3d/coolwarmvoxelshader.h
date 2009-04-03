#ifndef UDGCOOLWARMVOXELSHADER_H
#define UDGCOOLWARMVOXELSHADER_H


#include "voxelshader.h"

#include <vtkDirectionEncoder.h>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


class vtkEncodedGradientEstimator;


namespace udg {


/**
 * Voxel shader que pinta amb cool-warm shading.
 */
class CoolWarmVoxelShader : public VoxelShader {

public:

    CoolWarmVoxelShader();
    virtual ~CoolWarmVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setBYAlphaBeta( float b, float y, float alpha, float beta );
    /// Assigna l'estimador del gradient.
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );
    void setCombine( bool on );

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

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;
    float m_b, m_y, m_alpha, m_beta;
    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;
    bool m_combine;

};


inline HdrColor CoolWarmVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor CoolWarmVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor CoolWarmVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( m_data );
    Q_ASSERT( m_encodedNormals );
    Q_ASSERT( m_directionEncoder );

    HdrColor color = m_ambientColors[m_data[offset]];

    if ( !color.isTransparent() )
    {
        float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
        Vector3 normal( gradient[0], gradient[1], gradient[2] );
        const double SQRT3_INV = 1.0 / sqrt( 3.0 );
        Vector3 light( SQRT3_INV, SQRT3_INV, SQRT3_INV );
        double dotProduct = light * normal;
        float f = ( 1.0f + dotProduct ) / 2.0f;
        color.red = ( 1.0f - f ) * ( m_y + m_beta * color.red );
        color.green = ( 1.0f - f ) * ( m_y + m_beta * color.green );
        color.blue = f * ( m_b + m_alpha * color.blue );
    }

    return color;
}


inline HdrColor CoolWarmVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );

    HdrColor color = m_ambientColors[static_cast<int>(value)];

    if ( !color.isTransparent() )
    {
        Vector3 normal;

        for ( int i = 0; i < 8; i++ )
        {
            float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offsets[i]] );
            Vector3 localNormal( gradient[0], gradient[1], gradient[2] );
            normal += weights[i] * localNormal;
        }

        const double SQRT3_INV = 1.0 / sqrt( 3.0 );
        Vector3 light( SQRT3_INV, SQRT3_INV, SQRT3_INV );
        double dotProduct = light * normal;
        float f = ( 1.0f + dotProduct ) / 2.0f;
        color.red = ( 1.0f - f ) * ( m_y + m_beta * color.red );
        color.green = ( 1.0f - f ) * ( m_y + m_beta * color.green );
        color.blue = f * ( m_b + m_alpha * color.blue );
    }

    return color;
}


}


#endif
