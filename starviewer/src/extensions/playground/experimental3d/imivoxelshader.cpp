#include "imivoxelshader.h"


namespace udg {


ImiVoxelShader::ImiVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 ), m_maximumImi( 1.0f ), m_imiFactor( 1.0f ), m_combine( false ), m_additive( false ), m_additiveWeight( 0.0 )
{
}


ImiVoxelShader::~ImiVoxelShader()
{
    delete[] m_ambientColors;
}


void ImiVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void ImiVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


void ImiVoxelShader::setImi( const QVector<float> &imi, float maximumImi, float imiFactor )
{
    m_imi = imi;
    m_maximumImi = maximumImi;
    m_imiFactor = imiFactor;
}


void ImiVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


void ImiVoxelShader::setAdditive( bool on, float weight )
{
    m_additive = on;
    m_additiveWeight = weight;
}


QString ImiVoxelShader::toString() const
{
    return "ImiVoxelShader";
}


void ImiVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
