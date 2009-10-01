#include "vomivoxelshader.h"


namespace udg {


VomiVoxelShader::VomiVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 ), m_maximumVomi( 1.0f ), m_vomiFactor( 1.0f ), m_combine( false ), m_additive( false ), m_additiveWeight( 0.0 )
{
}


VomiVoxelShader::~VomiVoxelShader()
{
    delete[] m_ambientColors;
}


void VomiVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void VomiVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


void VomiVoxelShader::setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor )
{
    m_vomi = vomi;
    m_maximumVomi = maximumVomi;
    m_vomiFactor = vomiFactor;
}


void VomiVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


void VomiVoxelShader::setAdditive( bool on, float weight )
{
    m_additive = on;
    m_additiveWeight = weight;
}


QString VomiVoxelShader::toString() const
{
    return "VomiVoxelShader";
}


void VomiVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
