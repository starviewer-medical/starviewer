#include "ambientvoxelshader.h"


namespace udg {


AmbientVoxelShader::AmbientVoxelShader()
    : VoxelShader()
{
}


AmbientVoxelShader::~AmbientVoxelShader()
{
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
    for ( int i = 0; i < AMBIENT_COLORS_TABLE_SIZE; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
