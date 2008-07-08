#include "ambientvoxelshader.h"

#include <QColor>


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


QColor AmbientVoxelShader::shade( int offset ) const
{
    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_colorTable );
    Q_CHECK_PTR( m_opacityTable );

    unsigned char value = m_data[offset], value3 = value * 3;
    //return QColor::fromRgbF( m_colorTable[value3], m_colorTable[value3+1], m_colorTable[value3+2], m_opacityTable[value] );
    return m_transferFunction.get( value );
}


}
