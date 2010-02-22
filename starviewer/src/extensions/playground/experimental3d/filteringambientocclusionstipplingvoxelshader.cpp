#include "filteringambientocclusionstipplingvoxelshader.h"


namespace udg {


FilteringAmbientOcclusionStipplingVoxelShader::FilteringAmbientOcclusionStipplingVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_maximumFilteringAmbientOcclusion( 1.0f ), m_filteringAmbientOcclusionThreshold( 0.0f ), m_filteringAmbientOcclusionFactor( 1.0f )
{
}


FilteringAmbientOcclusionStipplingVoxelShader::~FilteringAmbientOcclusionStipplingVoxelShader()
{
    delete[] m_opacities;
}


void FilteringAmbientOcclusionStipplingVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void FilteringAmbientOcclusionStipplingVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


void FilteringAmbientOcclusionStipplingVoxelShader::setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float threshold, float factor )
{
    m_filteringAmbientOcclusion = filteringAmbientOcclusion;
    m_maximumFilteringAmbientOcclusion = maximum;
    m_filteringAmbientOcclusionThreshold = threshold;
    m_filteringAmbientOcclusionFactor = factor;
}


QString FilteringAmbientOcclusionStipplingVoxelShader::toString() const
{
    return "FilteringAmbientOcclusionStipplingVoxelShader";
}


void FilteringAmbientOcclusionStipplingVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


} // namespace udg
