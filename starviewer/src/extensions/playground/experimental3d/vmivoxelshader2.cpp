#include "vmivoxelshader2.h"


namespace udg {


VmiVoxelShader2::VmiVoxelShader2()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_dataSize( 0 ), m_ambientColors( 0 ), m_viewedVolume( 0.0f )
{
}


VmiVoxelShader2::~VmiVoxelShader2()
{
    delete[] m_ambientColors;
}


void VmiVoxelShader2::setData( const unsigned short *data, unsigned short maxValue, unsigned int size )
{
    m_data = data;
    m_maxValue = maxValue;
    m_dataSize = size;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void VmiVoxelShader2::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


QVector<float> VmiVoxelShader2::objectProbabilities()
{
    QList<QThread*> keys = m_totalVolumePerThread.keys();
    int nKeys = keys.size();

    float totalVolume = 0.0f;
    for ( int i = 0; i < nKeys; i++ ) totalVolume += m_totalVolumePerThread.value( keys.at(i) );

    QVector<float> objectProbabilities = m_objectVolumePerThread.take( keys.at( 0 ) );
    for ( int i = 1; i < nKeys; i++ )
        for ( unsigned int j = 0; j < m_dataSize; j++ )
            objectProbabilities[j] += m_objectVolumePerThread.value( keys.at( i ) ).at( j );
    for ( unsigned int i = 0; i < m_dataSize; i++ )
        objectProbabilities[i] /= totalVolume;

    m_viewedVolume = totalVolume;

    m_totalVolumePerThread.clear();
    m_objectVolumePerThread.clear();

    return objectProbabilities;
}


float VmiVoxelShader2::viewedVolume() const
{
    return m_viewedVolume;
}


QString VmiVoxelShader2::toString() const
{
    return "VmiVoxelShader2";
}


void VmiVoxelShader2::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
