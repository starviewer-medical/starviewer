#ifndef FILTERINGAMBIENTOCCLUSIONVOXELSHADER_H
#define FILTERINGAMBIENTOCCLUSIONVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que aplica una ambient occlusion a partir d'un filtratge.
 */
class FilteringAmbientOcclusionVoxelShader : public VoxelShader {

public:

    FilteringAmbientOcclusionVoxelShader();
    virtual ~FilteringAmbientOcclusionVoxelShader();

    void setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float lambda );

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

    QVector<float> m_filteringAmbientOcclusion;
    float m_maximum;
    float m_lambda;

};


inline HdrColor FilteringAmbientOcclusionVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    HdrColor color( baseColor );

    if ( !color.isTransparent() )
    {
        float ao = m_filteringAmbientOcclusion.at( offset ) / m_maximum * m_lambda;
        ao = qBound( -1.0f, ao, 1.0f ); // clipping
        color.red += ao;
        color.green += ao;
        color.blue += ao;
    }

    return color;
}


inline HdrColor FilteringAmbientOcclusionVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );

    HdrColor color( baseColor );

    if ( !color.isTransparent() )
    {
        int offsets[8];
        double weights[8];
        interpolator->getOffsetsAndWeights( position, offsets, weights );

        float ao = TrilinearInterpolator::interpolate<float>( m_filteringAmbientOcclusion.constData(), offsets, weights ) / m_maximum * m_lambda;
        ao = qBound( -1.0f, ao, 1.0f ); // clipping
        color.red += ao;
        color.green += ao;
        color.blue += ao;
    }

    return color;
}


} // namespace udg


#endif // FILTERINGAMBIENTOCCLUSIONVOXELSHADER_H
