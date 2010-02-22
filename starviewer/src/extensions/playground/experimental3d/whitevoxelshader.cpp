#include "whitevoxelshader.h"


namespace udg {


WhiteVoxelShader::WhiteVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 )
{
}


WhiteVoxelShader::~WhiteVoxelShader()
{
    delete[] m_opacities;
}


void WhiteVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void WhiteVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


QString WhiteVoxelShader::toString() const
{
    return "WhiteVoxelShader";
}


void WhiteVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


}
