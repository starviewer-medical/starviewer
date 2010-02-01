#include "vomigammavoxelshader.h"


namespace udg {


VomiGammaVoxelShader::VomiGammaVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 ), m_maximumVomi( 1.0f ), m_vomiFactor( 1.0f ), m_gamma( 1.0f ), m_combine( false ), m_additive( false ), m_additiveWeight( 0.0 )
{
}


VomiGammaVoxelShader::~VomiGammaVoxelShader()
{
    delete[] m_ambientColors;
}


void VomiGammaVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void VomiGammaVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


void VomiGammaVoxelShader::setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor, float gamma )
{
    m_vomi = vomi;
    m_maximumVomi = maximumVomi;
    m_vomiFactor = vomiFactor;
    m_gamma = gamma;
}


void VomiGammaVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


void VomiGammaVoxelShader::setAdditive( bool on, float weight )
{
    m_additive = on;
    m_additiveWeight = weight;
}


QString VomiGammaVoxelShader::toString() const
{
    return "VomiGammaVoxelShader";
}


void VomiGammaVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
