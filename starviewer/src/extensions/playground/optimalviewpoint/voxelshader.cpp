#include "voxelshader.h"


namespace udg {


VoxelShader::VoxelShader()
{
    m_data = 0;
}


VoxelShader::~VoxelShader()
{
}


void VoxelShader::setData( const unsigned char *data ) {
    m_data = data;
}


}
