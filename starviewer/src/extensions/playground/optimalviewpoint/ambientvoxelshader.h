#ifndef UDGAMBIENTVOXELSHADER_H
#define UDGAMBIENTVOXELSHADER_H


#include "voxelshader.h"

#include "transferfunction.h"


namespace udg {


/**
 * És un voxel shader que pinta el volum amb il·luminació ambient.
 */
class AmbientVoxelShader : public VoxelShader {

public:

    AmbientVoxelShader();
    virtual ~AmbientVoxelShader();

    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset, const QColor &baseColor = Qt::white ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    TransferFunction m_transferFunction;

};


}


#endif
