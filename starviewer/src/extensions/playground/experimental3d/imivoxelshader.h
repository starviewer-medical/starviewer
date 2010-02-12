#ifndef UDGIMIVOXELSHADER_H
#define UDGIMIVOXELSHADER_H


#include "voxelshader.h"

#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader que pinta l'IMI de cada voxel.
 */
class ImiVoxelShader : public VoxelShader {

public:

    ImiVoxelShader();
    virtual ~ImiVoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void setImi( const QVector<float> &imi, float maximumImi, float imiFactor );
    void setCombine( bool on );
    void setAdditive( bool on, float weight );

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
    void precomputeAmbientColors();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;
    QVector<float> m_imi;
    float m_maximumImi;
    float m_imiFactor;
    bool m_combine;
    bool m_additive;
    float m_additiveWeight;

};


inline HdrColor ImiVoxelShader::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor ImiVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor ImiVoxelShader::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( m_data );

//    HdrColor color( 1.0f, 1.0f, 1.0f );
//
//    if ( m_combine ) color = baseColor;
//    else color.alpha = m_ambientColors[m_data[offset]].alpha;
//
//    if ( !color.isTransparent() && !color.isBlack() )
//    {
//        float imi = m_imiFactor * m_imi.at( m_data[offset] ) / m_maximumImi;
//        float gray = qMax( 1.0f - imi, 0.0f );
//
//        if ( !m_additive ) color.multiplyColorBy( gray );
//        else
//        {
//            HdrColor imiColor = m_ambientColors[m_data[offset]];
//            imiColor.alpha = 0.0f;
//            color = color.multiplyColorBy( 1.0f - m_additiveWeight ) + imiColor.multiplyColorBy( m_additiveWeight * gray );
//        }
//    }
//
//    return color;

    HdrColor color;
    color.alpha = m_ambientColors[m_data[offset]].alpha;

    if ( !color.isTransparent() )
    {
        float imi = m_imiFactor * m_imi.at( m_data[offset] ) / m_maximumImi;
        color.red = imi > 0.8f ? 1.0f : imi > 0.6f ? 5.0f * ( imi - 0.6f ) : imi > 0.2f ? 0.0f : 1.0f - 5.0f * imi;
        color.green = imi > 0.8f ? 1.0f - 5.0f * ( imi - 0.8f ) : imi > 0.4f ? 1.0f : imi > 0.2f ? 5.0f * ( imi - 0.2f ) : 0.0f;
        color.blue = imi > 0.6f ? 0.0f : imi > 0.4f ? 1.0f - 5.0f * ( imi - 0.4f ) : 1.0f;
    }

    return color;
}


inline HdrColor ImiVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( remainingOpacity );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

//    int offsets[8];
//    double weights[8];
//    bool offsetsAndWeights = false;
//
//    HdrColor color( 1.0f, 1.0f, 1.0f );
//
//    if ( m_combine ) color = baseColor;
//    else
//    {
//        interpolator->getOffsetsAndWeights( position, offsets, weights );
//        offsetsAndWeights = true;
//        double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
//        color.alpha = m_ambientColors[static_cast<int>(value)].alpha;
//    }
//
//    if ( !color.isTransparent() && !color.isBlack() )
//    {
//        if ( !offsetsAndWeights ) interpolator->getOffsetsAndWeights( position, offsets, weights );
//
//        double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
//        int iValue = static_cast<int>(value);
//
//        float imi = m_imiFactor * m_imi.at( iValue ) / m_maximumImi;
//        float gray = qMax( 1.0f - imi, 0.0f );
//
//        if ( !m_additive ) color.multiplyColorBy( gray );
//        else
//        {
//            HdrColor imiColor = m_ambientColors[iValue];
//            imiColor.alpha = 0.0f;
//            color = color.multiplyColorBy( 1.0f - m_additiveWeight ) + imiColor.multiplyColorBy( m_additiveWeight * gray );
//        }
//    }
//
//    return color;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
    HdrColor color;
    color.alpha = m_ambientColors[static_cast<int>(value)].alpha;

    if ( !color.isTransparent() )
    {
        float imi = m_imiFactor * m_imi.at( static_cast<int>(value) ) / m_maximumImi;
        color.red = imi > 0.8f ? 1.0f : imi > 0.6f ? 5.0f * ( imi - 0.6f ) : imi > 0.2f ? 0.0f : 1.0f - 5.0f * imi;
        color.green = imi > 0.8f ? 1.0f - 5.0f * ( imi - 0.8f ) : imi > 0.4f ? 1.0f : imi > 0.2f ? 5.0f * ( imi - 0.2f ) : 0.0f;
        color.blue = imi > 0.6f ? 0.0f : imi > 0.4f ? 1.0f - 5.0f * ( imi - 0.4f ) : 1.0f;
    }

    return color;
}


}


#endif
