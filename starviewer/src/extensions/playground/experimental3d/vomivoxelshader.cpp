#include "vomivoxelshader.h"

#include <vtkEncodedGradientEstimator.h>


namespace udg {


VomiVoxelShader::VomiVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_maximumVomi( 1.0f ), m_vomiFactor( 1.0f ), m_diffuseLighting( false ),
   m_encodedNormals( 0 ), m_directionEncoder( 0 )
{
}


VomiVoxelShader::~VomiVoxelShader()
{
    delete[] m_opacities;
}


void VomiVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void VomiVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


void VomiVoxelShader::setVomi( const QVector<float> &vomi, float maximumVomi, float vomiFactor )
{
    m_vomi = vomi;
    m_maximumVomi = maximumVomi;
    m_vomiFactor = vomiFactor;
}


void VomiVoxelShader::setDiffuseLighting( bool on )
{
    m_diffuseLighting = on;
}


void VomiVoxelShader::setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator )
{
    m_encodedNormals = gradientEstimator->GetEncodedNormals();
    m_directionEncoder = gradientEstimator->GetDirectionEncoder();
}


QString VomiVoxelShader::toString() const
{
    return "VomiVoxelShader";
}


void VomiVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


}
