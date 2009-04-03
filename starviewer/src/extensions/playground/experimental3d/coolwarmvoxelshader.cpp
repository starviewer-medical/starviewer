#include "coolwarmvoxelshader.h"

#include <vtkEncodedGradientEstimator.h>


namespace udg {


CoolWarmVoxelShader::CoolWarmVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_ambientColors( 0 ), m_b( 1.0f ), m_y( 1.0f ), m_alpha( 1.0f ), m_beta( 1.0f ), m_encodedNormals( 0 ), m_directionEncoder( 0 ), m_combine( false )
{
}


CoolWarmVoxelShader::~CoolWarmVoxelShader()
{
    delete[] m_ambientColors;
}


void CoolWarmVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void CoolWarmVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


void CoolWarmVoxelShader::setBYAlphaBeta( float b, float y, float alpha, float beta )
{
    m_b = b;
    m_y = y;
    m_alpha = alpha;
    m_beta = beta;
}


void CoolWarmVoxelShader::setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator )
{
    m_encodedNormals = gradientEstimator->GetEncodedNormals();
    m_directionEncoder = gradientEstimator->GetDirectionEncoder();
}


void CoolWarmVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


QString CoolWarmVoxelShader::toString() const
{
    return "CoolWarmVoxelShader";
}


void CoolWarmVoxelShader::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
