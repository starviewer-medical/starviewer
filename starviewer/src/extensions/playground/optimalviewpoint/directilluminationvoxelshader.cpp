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


HdrColor DirectIlluminationVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_redDiffuseShadingTable ); Q_CHECK_PTR( m_greenDiffuseShadingTable ); Q_CHECK_PTR( m_blueDiffuseShadingTable );
    Q_CHECK_PTR( m_redSpecularShadingTable ); Q_CHECK_PTR( m_greenSpecularShadingTable ); Q_CHECK_PTR( m_blueSpecularShadingTable );

    HdrColor color = AmbientVoxelShader::shade( offset, direction, baseColor );

    if ( color.isTransparent() ) return color;

    unsigned short normal = m_encodedNormals[offset];
    color.red = color.red * m_redDiffuseShadingTable[normal] + m_redSpecularShadingTable[normal];
    color.green = color.green * m_greenDiffuseShadingTable[normal] + m_greenSpecularShadingTable[normal];
    color.blue = color.blue * m_blueDiffuseShadingTable[normal] + m_blueSpecularShadingTable[normal];

    return color;
}


QString DirectIlluminationVoxelShader::toString() const
{
    return "DirectIlluminationVoxelShader";
}


}
