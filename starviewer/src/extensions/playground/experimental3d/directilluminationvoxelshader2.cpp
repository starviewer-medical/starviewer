#include "directilluminationvoxelshader2.h"

namespace udg {

DirectIlluminationVoxelShader2::DirectIlluminationVoxelShader2()
    : AmbientVoxelShader2()
{
    m_encodedNormals = 0;
    m_redDiffuseShadingTable = m_greenDiffuseShadingTable = m_blueDiffuseShadingTable = 0;
    m_redSpecularShadingTable = m_greenSpecularShadingTable = m_blueSpecularShadingTable = 0;
}

DirectIlluminationVoxelShader2::~DirectIlluminationVoxelShader2()
{
}

void DirectIlluminationVoxelShader2::setEncodedNormals(const unsigned short *encodedNormals)
{
    m_encodedNormals = encodedNormals;
}

void DirectIlluminationVoxelShader2::setDiffuseShadingTables(const float *red, const float *green, const float *blue)
{
    m_redDiffuseShadingTable = red;
    m_greenDiffuseShadingTable = green;
    m_blueDiffuseShadingTable = blue;
}

void DirectIlluminationVoxelShader2::setSpecularShadingTables(const float *red, const float *green, const float *blue)
{
    m_redSpecularShadingTable = red;
    m_greenSpecularShadingTable = green;
    m_blueSpecularShadingTable = blue;
}

QString DirectIlluminationVoxelShader2::toString() const
{
    return "DirectIlluminationVoxelShader2";
}

} // namespace udg
