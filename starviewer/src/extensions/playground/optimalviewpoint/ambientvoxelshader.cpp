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
}


QColor AmbientVoxelShader::shade( int offset, const QColor &baseColor ) const
{
    Q_UNUSED( baseColor );

    Q_CHECK_PTR( m_data );

    return m_transferFunction.get( m_data[offset] );
}


QString AmbientVoxelShader::toString() const
{
    return "AmbientVoxelShader";
}


}
