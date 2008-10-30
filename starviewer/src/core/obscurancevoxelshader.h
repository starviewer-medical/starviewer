#ifndef UDGOBSCURANCEVOXELSHADER_H
#define UDGOBSCURANCEVOXELSHADER_H


#include "voxelshader.h"

#include "obscurance.h"
#include "trilinearinterpolator.h"
#include "vector3.h"


namespace udg {


/**
 * Voxel shader que aplica obscurances.
 */
class ObscuranceVoxelShader : public VoxelShader {

public:

    ObscuranceVoxelShader();
    virtual ~ObscuranceVoxelShader();

    /// Assigna l'array d'obscurances.
    void setObscurance( const Obscurance *obscurance );
    /// Assigna el factor pel qual es multipliquen les obscurances.
    void setFactor( double factor );
    /// Assigna els filtres d'obscurances: per sota de \a low es considera 0 i per sobre de \a high es considera 1.
    void setFilters( double low, double high );

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

    const Obscurance *m_obscurance;
    double m_factor;
    double m_lowFilter, m_highFilter;

};


inline HdrColor ObscuranceVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    return nvShade( offset, direction, baseColor );
}


inline HdrColor ObscuranceVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return nvShade( position, direction, interpolator, baseColor );
}


inline HdrColor ObscuranceVoxelShader::nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( m_obscurance );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    double obscurance = m_obscurance->obscurance( offset );
    if ( obscurance < m_lowFilter ) obscurance = 0.0;
    else if ( obscurance > m_highFilter ) obscurance = 1.0;
    obscurance *= m_factor;

    HdrColor shaded = baseColor;
    shaded.multiplyColorBy( obscurance );

    return shaded;
}


inline HdrColor ObscuranceVoxelShader::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_UNUSED( direction );

    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_obscurance );

    if ( baseColor.isTransparent() || baseColor.isBlack() ) return baseColor;

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double obscurance;
    if ( m_obscurance->isDoublePrecision() )
        obscurance = TrilinearInterpolator::interpolate<double>( m_obscurance->doubleObscurance(), offsets, weights );
    else
        obscurance = TrilinearInterpolator::interpolate<float>( m_obscurance->floatObscurance(), offsets, weights );
    if ( obscurance < m_lowFilter ) obscurance = 0.0;
    else if ( obscurance > m_highFilter ) obscurance = 1.0;
    obscurance *= m_factor;

    HdrColor shaded = baseColor;
    shaded.multiplyColorBy( obscurance );

    return shaded;
}


}


#endif
