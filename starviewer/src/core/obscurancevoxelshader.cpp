#include "obscurancevoxelshader.h"


namespace udg {


ObscuranceVoxelShader::ObscuranceVoxelShader()
    : VoxelShader()
{
    m_obscurance = 0;
    m_factor = 1.0;
    m_lowFilter = 0.0; m_highFilter = 1.0;
}


ObscuranceVoxelShader::~ObscuranceVoxelShader()
{
}


void ObscuranceVoxelShader::setObscurance( const double *obscurance )
{
    m_obscurance = obscurance;
}


void ObscuranceVoxelShader::setFactor( double factor )
{
    m_factor = factor;
}


void ObscuranceVoxelShader::setFilters( double low, double high )
{
    m_lowFilter = low; m_highFilter = high;
}


QString ObscuranceVoxelShader::toString() const
{
    return "ObscuranceVoxelShader";
}


}
