#include "obscurancevoxelshader.h"


namespace udg {


ObscuranceVoxelShader::ObscuranceVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_obscurance( 0 ), m_factor( 1.0 ), m_lowFilter( 0.0 ), m_highFilter( 1.0 ), m_combine( true )
{
}


ObscuranceVoxelShader::~ObscuranceVoxelShader()
{
    delete[] m_opacities;
}


void ObscuranceVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void ObscuranceVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


void ObscuranceVoxelShader::setObscurance( const Obscurance *obscurance )
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


void ObscuranceVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


QString ObscuranceVoxelShader::toString() const
{
    return "ObscuranceVoxelShader";
}


void ObscuranceVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


}
