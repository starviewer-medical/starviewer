#include "saliencyvoxelshader.h"


namespace udg {


SaliencyVoxelShader::SaliencyVoxelShader()
    : VoxelShader()
{
    m_saliency = 0;
    setScale( 1.0, 0.0 );
    setFilters( 0.0, 2.0 );
}


SaliencyVoxelShader::~SaliencyVoxelShader()
{
}


void SaliencyVoxelShader::setSaliency( const double *saliency )
{
    m_saliency = saliency;
}


void SaliencyVoxelShader::setScale( double a, double b )
{
    m_a = a;
    m_minimum = 1.0 - a; m_maximum = 1.0 + b; m_ab = a + b;
}


void SaliencyVoxelShader::setFilters( double low, double high )
{
    m_lowFilter = low; m_highFilter = high;
}


QString SaliencyVoxelShader::toString() const
{
    return "SaliencyVoxelShader";
}


}
