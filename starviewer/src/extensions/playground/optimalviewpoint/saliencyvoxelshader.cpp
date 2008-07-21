#include "saliencyvoxelshader.h"


namespace udg {


SaliencyVoxelShader::SaliencyVoxelShader()
    : VoxelShader()
{
    m_saliency = 0;
    setScale( 1.0, 0.0 );
    setFilters( 0.0, 2.0 );
}


SaliencyVoxelShader::~SaliencyVoxelShader()
{
}


void SaliencyVoxelShader::setSaliency( const double *saliency )
{
    m_saliency = saliency;
}


void SaliencyVoxelShader::setScale( double a, double b )
{
    m_a = a;
    m_minimum = 1.0 - a; m_maximum = 1.0 + b; m_ab = a + b;
}


void SaliencyVoxelShader::setFilters( double low, double high )
{
    m_lowFilter = low; m_highFilter = high;
}


// TODO el resultat de saliency és diferent de l'oficial perquè el màxim de cada component està capat a 1
//      mentre que a l'oficial pot ser més de 1 i es capa a 1 al final del raig
QColor SaliencyVoxelShader::shade( int offset, const Vector3 &direction, const QColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( m_data );
    Q_CHECK_PTR( m_saliency );

    if ( baseColor.alpha() == 0 ) return baseColor;

    double saliency = 1.0 + m_saliency[offset] * m_ab - m_a;
    if ( saliency < m_lowFilter ) saliency = m_minimum;
    else if ( saliency > m_highFilter ) saliency = m_maximum;

    QColor shaded = baseColor;
    shaded.setAlphaF( qMin( shaded.alphaF() * saliency, 1.0 ) );

    return shaded;
}


QString SaliencyVoxelShader::toString() const
{
    return "SaliencyVoxelShader";
}


}
