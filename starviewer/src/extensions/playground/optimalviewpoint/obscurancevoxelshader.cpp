#include "obscurancevoxelshader.h"


namespace udg {


ObscuranceVoxelShader::ObscuranceVoxelShader()
    : VoxelShader()
{
    m_obscurance = 0;
    m_factor = 1.0;
    m_lowFilter = 0.0; m_highFilter = 1.0;
}


ObscuranceVoxelShader::~ObscuranceVoxelShader()
{
}


void ObscuranceVoxelShader::setObscurance( const double * obscurance )
{
    m_obscurance = obscurance;
}


void ObscuranceVoxelShader::setFactor( double factor )
{
    m_factor = factor;
}


void ObscuranceVoxelShader::setFilters( double low, double high )
{
    m_lowFilter = low; m_highFilter = high;
}


// TODO el resultat d'obscurances és diferent de l'oficial (un pèl més fosc) perquè el màxim de cada component està capat a 1
//      mentre que a l'oficial pot ser més de 1 i es capa a 1 al final del raig
QColor ObscuranceVoxelShader::shade( int offset, const Vector3 &direction, const QColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_obscurance );

    if ( baseColor.alpha() == 0 ) return baseColor;

    double obscurance = m_obscurance[offset];
    if ( obscurance < m_lowFilter ) obscurance = 0.0;
    else if ( obscurance > m_highFilter ) obscurance = 1.0;
    obscurance *= m_factor;

    QColor shaded = baseColor;
    shaded.setRedF( qMin( baseColor.redF() * obscurance, 1.0 ) );
    shaded.setGreenF( qMin( baseColor.greenF() * obscurance, 1.0 ) );
    shaded.setBlueF( qMin( baseColor.blueF() * obscurance, 1.0 ) );

    return shaded;
}


QString ObscuranceVoxelShader::toString() const
{
    return "ObscuranceVoxelShader";
}


}
