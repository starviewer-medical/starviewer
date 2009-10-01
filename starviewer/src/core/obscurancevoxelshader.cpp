#include "obscurancevoxelshader.h"


namespace udg {


ObscuranceVoxelShader::ObscuranceVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 ), m_obscurance( 0 ), m_factor( 1.0 ), m_lowFilter( 0.0 ), m_highFilter( 1.0 ), m_combine( true ), m_additive( false ), m_additiveWeight( 0.0 )
{
}


ObscuranceVoxelShader::~ObscuranceVoxelShader()
{
    delete[] m_ambientColors;
}


void ObscuranceVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void ObscuranceVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
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


void ObscuranceVoxelShader::setAdditive( bool on, double weight )
{
    m_additive = on;
    m_additiveWeight = weight;
}


QString ObscuranceVoxelShader::toString() const
{
    return "ObscuranceVoxelShader";
}


void ObscuranceVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
