#include "filteringambientocclusionmapvoxelshader.h"


namespace udg {


FilteringAmbientOcclusionMapVoxelShader::FilteringAmbientOcclusionMapVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_maximumFilteringAmbientOcclusion( 1.0f ), m_filteringAmbientOcclusionFactor( 1.0f )
{
}


FilteringAmbientOcclusionMapVoxelShader::~FilteringAmbientOcclusionMapVoxelShader()
{
    delete[] m_opacities;
}


void FilteringAmbientOcclusionMapVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void FilteringAmbientOcclusionMapVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


void FilteringAmbientOcclusionMapVoxelShader::setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float factor )
{
    m_filteringAmbientOcclusion = filteringAmbientOcclusion;
    m_maximumFilteringAmbientOcclusion = maximum;
    m_filteringAmbientOcclusionFactor = factor;
}


QString FilteringAmbientOcclusionMapVoxelShader::toString() const
{
    return "FilteringAmbientOcclusionMapVoxelShader";
}


void FilteringAmbientOcclusionMapVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


} // namespace udg
