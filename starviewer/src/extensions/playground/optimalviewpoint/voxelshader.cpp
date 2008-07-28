#include "voxelshader.h"


namespace udg {


VoxelShader::VoxelShader()
{
    m_data = 0;
}


VoxelShader::~VoxelShader()
{
}


void VoxelShader::setData( const unsigned char *data )
{
    m_data = data;
}


HdrColor VoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return baseColor;
}


QString VoxelShader::toString() const
{
    return "VoxelShader";
}


}
