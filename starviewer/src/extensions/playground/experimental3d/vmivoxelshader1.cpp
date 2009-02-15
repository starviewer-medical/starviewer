#include "vmivoxelshader1.h"


namespace udg {


VmiVoxelShader1::VmiVoxelShader1()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 )
{
}


VmiVoxelShader1::~VmiVoxelShader1()
{
    delete[] m_ambientColors;
}


void VmiVoxelShader1::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void VmiVoxelShader1::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


void VmiVoxelShader1::initAccumulator()
{
    m_accumulator.clear();
}


const QHash<QThread*, float>& VmiVoxelShader1::accumulator() const
{
    return m_accumulator;
}


QString VmiVoxelShader1::toString() const
{
    return "VmiVoxelShader1";
}


void VmiVoxelShader1::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
