#ifndef UDGWHITEVOXELSHADER_H
#define UDGWHITEVOXELSHADER_H


#include "voxelshader.h"

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * És un voxel shader que pinta el volum de color blanc amb l'opacitat segons la funció de transferència.
 */
class WhiteVoxelShader : public VoxelShader {

public:

    WhiteVoxelShader();
    virtual ~WhiteVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                            const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                      const HdrColor &baseColor = HdrColor() );
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula d'opacitats.
    void precomputeOpacities();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    float *m_opacities;

};


inline HdrColor WhiteVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor WhiteVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                                           const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor WhiteVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    return HdrColor( 1.0f, 1.0f, 1.0f, m_opacities[m_data[offset]] );
}


inline HdrColor WhiteVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );

    return HdrColor( 1.0f, 1.0f, 1.0f, m_opacities[static_cast<int>(value)] );
}


}


#endif
