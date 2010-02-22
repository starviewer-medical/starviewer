#ifndef FILTERINGAMBIENTOCCLUSIONSTIPPLINGVOXELSHADER_H
#define FILTERINGAMBIENTOCCLUSIONSTIPPLINGVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que fa stippling segons l'ambient occlusion obtinguta a partir d'un filtratge.
 */
class FilteringAmbientOcclusionStipplingVoxelShader : public VoxelShader {

public:

    FilteringAmbientOcclusionStipplingVoxelShader();
    virtual ~FilteringAmbientOcclusionStipplingVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float threshold, float factor );

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

     /// Omple la taula d'opacitats.
    void precomputeOpacities();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    float *m_opacities;
    QVector<float> m_filteringAmbientOcclusion;
    float m_maximumFilteringAmbientOcclusion;
    float m_filteringAmbientOcclusionThreshold;
    float m_filteringAmbientOcclusionFactor;

};


inline HdrColor FilteringAmbientOcclusionStipplingVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionStipplingVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionStipplingVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color( 1.0f, 1.0f, 1.0f, 1.0f );
    color.alpha = m_opacities[m_data[offset]];

    if ( !color.isTransparent() )
    {
        float ao = m_filteringAmbientOcclusion.at( offset ) / m_maximumFilteringAmbientOcclusion;

        if ( ao > m_filteringAmbientOcclusionThreshold )
        {
            float f = m_filteringAmbientOcclusionFactor * ao;
            f = qBound( 0.0f, f, 1.0f );

            if ( f > qrand() / (double)RAND_MAX ) color.red = color.green = color.blue = 0.0f;
            else color.red = color.green = color.blue = 1.0f;
        }
    }

    return color;
}


inline HdrColor FilteringAmbientOcclusionStipplingVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
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
    HdrColor color( 1.0f, 1.0f, 1.0f, 1.0f );;
    color.alpha = m_opacities[static_cast<int>(value)];

    if ( !color.isTransparent() )
    {
        float ao = TrilinearInterpolator::interpolate<float>( m_filteringAmbientOcclusion.constData(), offsets, weights ) / m_maximumFilteringAmbientOcclusion;

        if ( ao > m_filteringAmbientOcclusionThreshold )
        {
            float f = m_filteringAmbientOcclusionFactor * ao;
            f = qBound( 0.0f, f, 1.0f );

            if ( f > qrand() / (double)RAND_MAX ) color.red = color.green = color.blue = 0.0f;
            else color.red = color.green = color.blue = 1.0f;
        }
    }

    return color;
}


} // namespace udg


#endif // FILTERINGAMBIENTOCCLUSIONSTIPPLINGVOXELSHADER_H
