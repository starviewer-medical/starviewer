#include "ambientvoxelshader.h"


namespace udg {


AmbientVoxelShader::AmbientVoxelShader()
 : VoxelShader(), m_data( 0 )
{
}


AmbientVoxelShader::~AmbientVoxelShader()
{
}


void AmbientVoxelShader::setData( const unsigned char *data )
{
    m_data = data;
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
    for ( unsigned int i = 0; i < AMBIENT_COLORS_TABLE_SIZE; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
