#ifndef UDGAMBIENTVOXELSHADER_H
#define UDGAMBIENTVOXELSHADER_H


#include <voxelshader.h>


namespace udg {


/**
 * És un voxel shader que pinta el volum amb il·luminació ambient.
 */
class AmbientVoxelShader : public VoxelShader
{

public:

    AmbientVoxelShader();
    virtual ~AmbientVoxelShader();

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset ) const;

};


}


#endif
