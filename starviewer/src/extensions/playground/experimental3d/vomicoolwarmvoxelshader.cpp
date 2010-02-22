#include "vomicoolwarmvoxelshader.h"


namespace udg {


VomiCoolWarmVoxelShader::VomiCoolWarmVoxelShader()
 : VoxelShader(), m_maximumVomi( 1.0f ), m_vomiFactor( 1.0f ), m_y( 1.0f ), m_b( 1.0f )
{
}


VomiCoolWarmVoxelShader::~VomiCoolWarmVoxelShader()
{
}


void VomiCoolWarmVoxelShader::setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor )
{
    m_vomi = vomi;
    m_maximumVomi = maximumVomi;
    m_vomiFactor = vomiFactor;
}


void VomiCoolWarmVoxelShader::setYB( float y, float b )
{
    m_y = y;
    m_b = b;
}


QString VomiCoolWarmVoxelShader::toString() const
{
    return "VomiCoolWarmVoxelShader";
}


}
