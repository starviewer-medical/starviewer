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


QColor AmbientVoxelShader::shade( int offset ) const
{
    Q_CHECK_PTR( m_data );

    unsigned char value = m_data[offset];
    return QColor( value, value, value, value );
}


}
