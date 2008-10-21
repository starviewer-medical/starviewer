#include "directilluminationvoxelshader.h"


namespace udg {


DirectIlluminationVoxelShader::DirectIlluminationVoxelShader()
    : AmbientVoxelShader()
{
    m_encodedNormals = 0;
    m_redDiffuseShadingTable = m_greenDiffuseShadingTable = m_blueDiffuseShadingTable = 0;
    m_redSpecularShadingTable = m_greenSpecularShadingTable = m_blueSpecularShadingTable = 0;
}


DirectIlluminationVoxelShader::~DirectIlluminationVoxelShader()
{
}


void DirectIlluminationVoxelShader::setEncodedNormals( const unsigned short *encodedNormals )
{
    m_encodedNormals = encodedNormals;
}


void DirectIlluminationVoxelShader::setDiffuseShadingTables( const float *red, const float *green, const float *blue )
{
    m_redDiffuseShadingTable = red;
    m_greenDiffuseShadingTable = green;
    m_blueDiffuseShadingTable = blue;
}


void DirectIlluminationVoxelShader::setSpecularShadingTables( const float *red, const float *green, const float *blue )
{
    m_redSpecularShadingTable = red;
    m_greenSpecularShadingTable = green;
    m_blueSpecularShadingTable = blue;
}


QString DirectIlluminationVoxelShader::toString() const
{
    return "DirectIlluminationVoxelShader";
}


}
