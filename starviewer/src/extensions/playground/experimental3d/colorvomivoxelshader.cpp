#include "colorvomivoxelshader.h"


namespace udg {


ColorVomiVoxelShader::ColorVomiVoxelShader()
 : VoxelShader(), m_maximumColorVomi( 1.0f ), m_colorVomiFactor( 1.0f )
{
}


ColorVomiVoxelShader::~ColorVomiVoxelShader()
{
}


void ColorVomiVoxelShader::setColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float colorVomiFactor )
{
    m_colorVomi = colorVomi;
    m_maximumColorVomi = maximumColorVomi;
    m_colorVomiFactor = colorVomiFactor;
}


QString ColorVomiVoxelShader::toString() const
{
    return "ColorVomiVoxelShader";
}


}
