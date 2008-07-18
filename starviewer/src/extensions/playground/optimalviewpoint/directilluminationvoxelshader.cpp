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


// TODO el resultat d'especularitat és diferent de l'oficial perquè el màxim de cada component està capat a 1
//      mentre que a l'oficial pot ser més de 1 i es capa a 1 al final del raig
QColor DirectIlluminationVoxelShader::shade( int offset, const Vector3 &direction, const QColor &baseColor ) const
{
    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_encodedNormals );
    Q_CHECK_PTR( m_redDiffuseShadingTable ); Q_CHECK_PTR( m_greenDiffuseShadingTable ); Q_CHECK_PTR( m_blueDiffuseShadingTable );
    Q_CHECK_PTR( m_redSpecularShadingTable ); Q_CHECK_PTR( m_greenSpecularShadingTable ); Q_CHECK_PTR( m_blueSpecularShadingTable );

    //QColor color = m_transferFunction.get( m_data[offset] );
    QColor color = AmbientVoxelShader::shade( offset, direction, baseColor );

    if ( color.alpha() > 0 )
    {
        unsigned short normal = m_encodedNormals[offset];
        color.setRedF( qMin( color.redF() * m_redDiffuseShadingTable[normal] + m_redSpecularShadingTable[normal], 1.0 ) );
        color.setGreenF( qMin( color.greenF() * m_greenDiffuseShadingTable[normal] + m_greenSpecularShadingTable[normal], 1.0 ) );
        color.setBlueF( qMin( color.blueF() * m_blueDiffuseShadingTable[normal] + m_blueSpecularShadingTable[normal], 1.0 ) );
    }

    return color;
}


QString DirectIlluminationVoxelShader::toString() const
{
    return "DirectIlluminationVoxelShader";
}


}
