#ifndef UDGAMBIENTVOXELSHADER_H
#define UDGAMBIENTVOXELSHADER_H


#include "voxelshader.h"

#include "transferfunction.h"
#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


/**
 * És un voxel shader que pinta el volum amb il·luminació ambient.
 */
class AmbientVoxelShader : public VoxelShader {

public:

    AmbientVoxelShader();
    virtual ~AmbientVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned char *data );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

    static const unsigned int AMBIENT_COLORS_TABLE_SIZE = 256;

    const unsigned char *m_data;
    TransferFunction m_transferFunction;
    HdrColor m_ambientColors[AMBIENT_COLORS_TABLE_SIZE];

};


inline HdrColor AmbientVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    return nvShade( offset, direction, baseColor );
}


inline HdrColor AmbientVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return nvShade( position, direction, interpolator, baseColor );
}


inline HdrColor AmbientVoxelShader::nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    return m_ambientColors[m_data[offset]];
}


inline HdrColor AmbientVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );

    return m_ambientColors[static_cast<int>(value)];
}


}


#endif
