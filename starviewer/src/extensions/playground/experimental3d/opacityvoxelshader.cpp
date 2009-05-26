#include "opacityvoxelshader.h"


namespace udg {


OpacityVoxelShader::OpacityVoxelShader()
 : VoxelShader(), m_maximum( 1.0f ), m_lowThreshold( 0.0f ), m_lowFactor( 1.0f ), m_highThreshold( 1.0f ), m_highFactor( 1.0f )
{
}


OpacityVoxelShader::~OpacityVoxelShader()
{
}


void OpacityVoxelShader::setData( const QVector<float> &data, float maximum, float lowThreshold, float lowFactor, float highThreshold, float highFactor )
{
    m_data = data;
    m_maximum = maximum;
    m_lowThreshold = lowThreshold;
    m_lowFactor = lowFactor;
    m_highThreshold = highThreshold;
    m_highFactor = highFactor;
}


QString OpacityVoxelShader::toString() const
{
    return "OpacityVoxelShader";
}


}
