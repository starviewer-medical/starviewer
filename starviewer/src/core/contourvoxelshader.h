#ifndef UDGCONTOURVOXELSHADER_H
#define UDGCONTOURVOXELSHADER_H


#include "voxelshader.h"

#include <vtkDirectionEncoder.h>

#include "trilinearinterpolator.h"


class vtkEncodedGradientEstimator;


namespace udg {


/**
 * És un voxel shader que pinta un contorn negre en funció d'un paràmetre.
 */
class ContourVoxelShader : public VoxelShader {

public:

    ContourVoxelShader();
    virtual ~ContourVoxelShader();

    /// Assigna l'estimador del gradient.
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );
    /// Assigna el llindar a partir del qual s'aplica el contorn.
    void setThreshold( double threshold );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;
    double m_threshold;

};


inline HdrColor ContourVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    return nvShade( offset, direction, baseColor );
}


inline HdrColor ContourVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return nvShade( position, direction, interpolator, baseColor );
}


inline HdrColor ContourVoxelShader::nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_ASSERT( m_encodedNormals );
    Q_ASSERT( m_directionEncoder );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
    Vector3 normal( gradient[0], gradient[1], gradient[2] );
    double dotProduct = direction * normal;
    if ( dotProduct < 0.0 ) dotProduct = -dotProduct;
    HdrColor black( 0.0, 0.0, 0.0, baseColor.alpha );

    return dotProduct < m_threshold ? black : baseColor;
}


inline HdrColor ContourVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_ASSERT( interpolator );
    Q_ASSERT( m_encodedNormals );
    Q_ASSERT( m_directionEncoder );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    Vector3 normal;

    for ( int i = 0; i < 8; i++ )
    {
        float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offsets[i]] );
        Vector3 localNormal( gradient[0], gradient[1], gradient[2] );
        normal += weights[i] * localNormal;
    }

    double dotProduct = direction * normal;
    if ( dotProduct < 0.0 ) dotProduct = -dotProduct;
    HdrColor black( 0.0, 0.0, 0.0, baseColor.alpha );

    return dotProduct < m_threshold ? black : baseColor;
}


}


#endif
