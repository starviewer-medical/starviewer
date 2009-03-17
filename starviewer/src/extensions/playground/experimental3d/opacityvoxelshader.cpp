#include "opacityvoxelshader.h"


namespace udg {


OpacityVoxelShader::OpacityVoxelShader()
 : VoxelShader(), m_maximum( 1.0f ), m_factor( 1.0f )
{
}


OpacityVoxelShader::~OpacityVoxelShader()
{
}


void OpacityVoxelShader::setData( const QVector<float> &data, float maximum, float factor )
{
    m_data = data;
    m_maximum = maximum;
    m_factor = factor;
}


QString OpacityVoxelShader::toString() const
{
    return "OpacityVoxelShader";
}


}
