#include "vomicoolwarmvoxelshader.h"


namespace udg {


VomiCoolWarmVoxelShader::VomiCoolWarmVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_maximumVomi( 1.0f ), m_vomiFactor( 1.0f ), m_y( 1.0f ), m_b( 1.0f ), m_combine( false )
{
}


VomiCoolWarmVoxelShader::~VomiCoolWarmVoxelShader()
{
    delete[] m_opacities;
}


void VomiCoolWarmVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void VomiCoolWarmVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
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


void VomiCoolWarmVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


QString VomiCoolWarmVoxelShader::toString() const
{
    return "VomiCoolWarmVoxelShader";
}


void VomiCoolWarmVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


}
