#include "ambientvoxelshader.h"


namespace udg {


AmbientVoxelShader::AmbientVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 )
{
}


AmbientVoxelShader::~AmbientVoxelShader()
{
    delete[] m_ambientColors;
}


void AmbientVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void AmbientVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


QString AmbientVoxelShader::toString() const
{
    return "AmbientVoxelShader";
}


void AmbientVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
