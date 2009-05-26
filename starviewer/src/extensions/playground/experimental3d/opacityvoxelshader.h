#ifndef UDGOPACITYVOXELSHADER_H
#define UDGOPACITYVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que modifica l'opacitat d'un voxel segons la VoMI o la saliency.
 */
class OpacityVoxelShader : public VoxelShader {

public:

    OpacityVoxelShader();
    virtual ~OpacityVoxelShader();

    /// Assigna les dades (VoMI o saliency).
    void setData( const QVector<float> &data, float maximum, float lowThreshold, float lowFactor, float highThreshold, float highFactor );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// VoMI o saliency.
    QVector<float> m_data;
    float m_maximum;
    float m_lowThreshold;
    float m_lowFactor;
    float m_highThreshold;
    float m_highFactor;

};


inline HdrColor OpacityVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor OpacityVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor OpacityVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    if ( baseColor.isTransparent() ) return baseColor;

    HdrColor color( baseColor );
    float value = m_data.at( offset ) / m_maximum;
    if ( value < m_lowThreshold ) color.alpha *= m_lowFactor * value;
    else if ( value > m_highThreshold ) color.alpha *= m_highFactor * value;
    return color;
}


inline HdrColor OpacityVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );

    if ( baseColor.isTransparent() ) return baseColor;

    HdrColor color( baseColor );
    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );
    float value = TrilinearInterpolator::interpolate<float>( m_data.constData(), offsets, weights ) / m_maximum;
    if ( value < m_lowThreshold ) color.alpha *= m_lowFactor * value;
    else if ( value > m_highThreshold ) color.alpha *= m_highFactor * value;
    return color;
}


}


#endif
