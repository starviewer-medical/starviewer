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


QColor ContourVoxelShader::shade( int offset, const Vector3 &direction, const QColor &baseColor ) const
{
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_directionEncoder );

    float *gradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[offset] );
    Vector3 normal( gradient[0], gradient[1], gradient[2] );
    double dotProduct = direction * normal;
    if ( dotProduct < 0.0 ) dotProduct = -dotProduct;

    return dotProduct < m_threshold ? Qt::black : baseColor;
}


QString ContourVoxelShader::toString() const
{
    return "ContourVoxelShader";
}


}
