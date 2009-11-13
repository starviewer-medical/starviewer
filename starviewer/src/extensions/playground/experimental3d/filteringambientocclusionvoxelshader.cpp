#include "filteringambientocclusionvoxelshader.h"


namespace udg {


FilteringAmbientOcclusionVoxelShader::FilteringAmbientOcclusionVoxelShader()
 : VoxelShader(), m_maximum( 0.0f ), m_lambda( 0.0f )
{
}


FilteringAmbientOcclusionVoxelShader::~FilteringAmbientOcclusionVoxelShader()
{
}


void FilteringAmbientOcclusionVoxelShader::setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float lambda )
{
    m_filteringAmbientOcclusion = filteringAmbientOcclusion;
    m_maximum = maximum;
    m_lambda = lambda;
}


QString FilteringAmbientOcclusionVoxelShader::toString() const
{
    return "FilteringAmbientOcclusionVoxelShader";
}


} // namespace udg
