#include "contourvoxelshader.h"

#include <vtkDirectionEncoder.h>
#include <vtkEncodedGradientEstimator.h>

#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


ContourVoxelShader::ContourVoxelShader()
    : VoxelShader()
{
    m_encodedNormals = 0;
    m_directionEncoder = 0;
    m_threshold = 0.0;
}


ContourVoxelShader::~ContourVoxelShader()
{
}


void ContourVoxelShader::setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator )
{
    m_encodedNormals = gradientEstimator->GetEncodedNormals();
    m_directionEncoder = gradientEstimator->GetDirectionEncoder();
}


void ContourVoxelShader::setThreshold( double threshold )
{
    m_threshold = threshold;
}


HdrColor ContourVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_directionEncoder );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
    Vector3 normal( gradient[0], gradient[1], gradient[2] );
    double dotProduct = direction * normal;
    if ( dotProduct < 0.0 ) dotProduct = -dotProduct;
    HdrColor black( 0.0, 0.0, 0.0, baseColor.alpha );

    return dotProduct < m_threshold ? black : baseColor;
}


HdrColor ContourVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_directionEncoder );

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


QString ContourVoxelShader::toString() const
{
    return "ContourVoxelShader";
}


}
