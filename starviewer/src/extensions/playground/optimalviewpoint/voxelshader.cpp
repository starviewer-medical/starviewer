#include "voxelshader.h"


namespace udg {


VoxelShader::VoxelShader()
{
    m_data = 0;
    m_colorTable = 0;
    m_opacityTable = 0;
}


VoxelShader::~VoxelShader()
{
}


void VoxelShader::setData( const unsigned char *data ) {
    m_data = data;
}


void VoxelShader::setColorTable( const float *colorTable )
{
    m_colorTable = colorTable;
}


void VoxelShader::setOpacityTable( const float *opacityTable )
{
    m_opacityTable = opacityTable;
}


}
