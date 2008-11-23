#include "contourvoxelshader.h"

#include <vtkEncodedGradientEstimator.h>


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


QString ContourVoxelShader::toString() const
{
    return "ContourVoxelShader";
}


}
