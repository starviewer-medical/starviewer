#include "colorbleedingvoxelshader.h"


namespace udg {


ColorBleedingVoxelShader::ColorBleedingVoxelShader()
 : VoxelShader()
{
    m_colorBleeding = 0;
    m_factor = 1.0;
}


ColorBleedingVoxelShader::~ColorBleedingVoxelShader()
{
}


void ColorBleedingVoxelShader::setColorBleeding( const Obscurance *colorBleeding )
{
    m_colorBleeding = colorBleeding;
}


void ColorBleedingVoxelShader::setFactor( double factor )
{
    m_factor = factor;
}


QString ColorBleedingVoxelShader::toString() const
{
    return "ColorBleedingVoxelShader";
}


}
