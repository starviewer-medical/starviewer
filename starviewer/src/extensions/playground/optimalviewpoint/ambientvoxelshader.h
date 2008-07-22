#ifndef UDGAMBIENTVOXELSHADER_H
#define UDGAMBIENTVOXELSHADER_H


#include "voxelshader.h"

#include "transferfunction.h"
#include "vector3.h"


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
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

    static const unsigned int AMBIENT_COLORS_TABLE_SIZE = 256;

    TransferFunction m_transferFunction;
    HdrColor m_ambientColors[AMBIENT_COLORS_TABLE_SIZE];

};


inline HdrColor AmbientVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_CHECK_PTR( m_data );

    //return m_transferFunction.get( m_data[offset] );
    return m_ambientColors[m_data[offset]];
}


}


#endif
