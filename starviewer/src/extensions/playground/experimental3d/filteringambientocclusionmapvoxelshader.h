#ifndef FILTERINGAMBIENTOCCLUSIONMAPVOXELSHADER_H
#define FILTERINGAMBIENTOCCLUSIONMAPVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que pinta amb escala de colors el mapa d'ambient occlusion obtingut a partir d'un filtratge.
 */
class FilteringAmbientOcclusionMapVoxelShader : public VoxelShader {

public:

    FilteringAmbientOcclusionMapVoxelShader();
    virtual ~FilteringAmbientOcclusionMapVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setFilteringAmbientOcclusion( const QVector<float> &filteringAmbientOcclusion, float maximum, float factor );

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
    float m_filteringAmbientOcclusionFactor;

};


inline HdrColor FilteringAmbientOcclusionMapVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionMapVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor FilteringAmbientOcclusionMapVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color;
    color.alpha = m_opacities[m_data[offset]];

    if ( !color.isTransparent() )
    {
        float f = m_filteringAmbientOcclusionFactor * m_filteringAmbientOcclusion.at( offset ) / m_maximumFilteringAmbientOcclusion;
        f = qBound( -1.0f, f, 1.0f );

        if ( f > 0.0f ) // positiu: negre -> groc -> vermell
        {
            color.red = f < 0.5f ? 2.0f * f : 1.0f;
            color.green = 2.0f * ( f < 0.5f ? f : 1.0f - f );

        }
        else    // negatiu: negre -> verd -> blau
        {
            color.green = 2.0f * ( f > -0.5f ? -f : 1.0f + f );
            color.blue = f > -0.5f ? 0.0f : -2.0f * f;
        }
    }

    return color;
}


inline HdrColor FilteringAmbientOcclusionMapVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
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
    HdrColor color;
    color.alpha = m_opacities[static_cast<int>(value)];

    if ( !color.isTransparent() )
    {
        float f = m_filteringAmbientOcclusionFactor * TrilinearInterpolator::interpolate<float>( m_filteringAmbientOcclusion.constData(), offsets, weights ) / m_maximumFilteringAmbientOcclusion;
        f = qBound( -1.0f, f, 1.0f );

        if ( f > 0.0f ) // positiu: negre -> groc -> vermell
        {
            color.red = f < 0.5f ? 2.0f * f : 1.0f;
            color.green = 2.0f * ( f < 0.5f ? f : 1.0f - f );
        }
        else    // negatiu: negre -> verd -> blau
        {
            color.green = 2.0f * ( f > -0.5f ? -f : 1.0f + f );
            color.blue = f > -0.5f ? 0.0f : -2.0f * f;
        }
    }

    return color;
}


} // namespace udg


#endif // FILTERINGAMBIENTOCCLUSIONMAPVOXELSHADER_H
