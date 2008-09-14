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


HdrColor DirectIlluminationVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_redDiffuseShadingTable ); Q_CHECK_PTR( m_greenDiffuseShadingTable ); Q_CHECK_PTR( m_blueDiffuseShadingTable );
    Q_CHECK_PTR( m_redSpecularShadingTable ); Q_CHECK_PTR( m_greenSpecularShadingTable ); Q_CHECK_PTR( m_blueSpecularShadingTable );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );

    HdrColor color = m_ambientColors[static_cast<int>(value)];

    if ( color.isTransparent() ) return color;

    int normals[8];
    for ( int i = 0; i < 8; i++ ) normals[i] = m_encodedNormals[offsets[i]];

    double diffuseRed = TrilinearInterpolator::interpolate<double>( m_redDiffuseShadingTable, normals, weights );
    double diffuseGreen = TrilinearInterpolator::interpolate<double>( m_greenDiffuseShadingTable, normals, weights );
    double diffuseBlue = TrilinearInterpolator::interpolate<double>( m_blueDiffuseShadingTable, normals, weights );
    double specularRed = TrilinearInterpolator::interpolate<double>( m_redSpecularShadingTable, normals, weights );
    double specularGreen = TrilinearInterpolator::interpolate<double>( m_greenSpecularShadingTable, normals, weights );
    double specularBlue = TrilinearInterpolator::interpolate<double>( m_blueSpecularShadingTable, normals, weights );

    color.red = color.red * diffuseRed + specularRed;
    color.green = color.green * diffuseGreen + specularGreen;
    color.blue = color.blue * diffuseBlue + specularBlue;

    return color;
}


QString DirectIlluminationVoxelShader::toString() const
{
    return "DirectIlluminationVoxelShader";
}


}
