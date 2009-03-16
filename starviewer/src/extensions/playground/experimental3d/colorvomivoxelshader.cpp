#include "colorvomivoxelshader.h"


namespace udg {


ColorVomiVoxelShader::ColorVomiVoxelShader()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_opacities( 0 ), m_maximumColorVomi( 1.0f ), m_colorVomiFactor( 1.0f ), m_combine( false )
{
}


ColorVomiVoxelShader::~ColorVomiVoxelShader()
{
    delete[] m_opacities;
}


void ColorVomiVoxelShader::setData( const unsigned short *data, unsigned short maxValue )
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_opacities;
    m_opacities = new float[m_maxValue + 1];
}


void ColorVomiVoxelShader::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeOpacities();
}


void ColorVomiVoxelShader::setColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float colorVomiFactor )
{
    m_colorVomi = colorVomi;
    m_maximumColorVomi = maximumColorVomi;
    m_colorVomiFactor = colorVomiFactor;
}


void ColorVomiVoxelShader::setCombine( bool on )
{
    m_combine = on;
}


QString ColorVomiVoxelShader::toString() const
{
    return "ColorVomiVoxelShader";
}


void ColorVomiVoxelShader::precomputeOpacities()
{
    if ( !m_opacities ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_opacities[i] = m_transferFunction.getOpacity( i );
}


}
