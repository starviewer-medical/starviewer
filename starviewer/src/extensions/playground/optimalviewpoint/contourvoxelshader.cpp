#include "contourvoxelshader.h"

#include <vtkDirectionEncoder.h>
#include <vtkEncodedGradientEstimator.h>

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


QString ContourVoxelShader::toString() const
{
    return "ContourVoxelShader";
}


}
