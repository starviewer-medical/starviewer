#ifndef UDGAMBIENTVOXELSHADER2_H
#define UDGAMBIENTVOXELSHADER2_H

#include "voxelshader.h"

#include "transferfunction.h"
#include "trilinearinterpolator.h"
#include "qexperimental3dextension.h"

namespace udg {

/**
    És un voxel shader que pinta el volum amb il·luminació ambient.
    TODO Això és una marranada per interpolar sobre el volum original en l'optimització de funcions de transferència 2D. Cal pensar com fer-ho ben fet.
 */
class AmbientVoxelShader2 : public VoxelShader {

public:

    AmbientVoxelShader2();
    virtual ~AmbientVoxelShader2();

    /// Assigna el volum de dades.
    void setData(const unsigned short *data, unsigned short maxValue);
    /// Assigna el volum de dades alternatiu.
    void setAlternativeData(const unsigned short *alternativeData);
    /// Assigna els gradients.
    void setGradientMagnitudes(const unsigned char *gradientMagnitudes);
    /// Assigna l'extensió.
    void setExtension(const QExperimental3DExtension *extension);
    /// Assigna la funció de transferència.
    void setTransferFunction(const TransferFunction &transferFunction);

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade(const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor());
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade(const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                           const HdrColor &baseColor = HdrColor());
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade(const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor());
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade(const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                     const HdrColor &baseColor = HdrColor());
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

protected:

    const unsigned short *m_data;
    unsigned short m_maxValue;
    const unsigned short *m_alternativeData;
    const unsigned char *m_gradientMagnitudes;
    const QExperimental3DExtension *m_extension;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;

};

inline HdrColor AmbientVoxelShader2::shade(const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor)
{
    return nvShade(position, offset, direction, remainingOpacity, baseColor);
}

inline HdrColor AmbientVoxelShader2::shade(const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                                           const HdrColor &baseColor)
{
    return nvShade(position, direction, interpolator, remainingOpacity, baseColor);
}

inline HdrColor AmbientVoxelShader2::nvShade(const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor)
{
    Q_UNUSED(position);
    Q_UNUSED(direction);
    Q_UNUSED(remainingOpacity);
    Q_UNUSED(baseColor);

    Q_ASSERT(m_data);

    return m_ambientColors[m_data[offset]];
}

inline HdrColor AmbientVoxelShader2::nvShade(const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator,
                                             float remainingOpacity, const HdrColor &baseColor)
{
    Q_UNUSED(direction);
    Q_UNUSED(remainingOpacity);
    Q_UNUSED(baseColor);

    Q_ASSERT(interpolator);
    Q_ASSERT(m_data);

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights(position, offsets, weights);

    double value;
    if (m_alternativeData)
    {
        double intensity = TrilinearInterpolator::interpolate<double>(m_alternativeData, offsets, weights);
        double gradient = TrilinearInterpolator::interpolate<double>(m_gradientMagnitudes, offsets, weights);
        value = m_extension->cluster2DIndex(intensity, gradient);
    }
    else
    {
        value = TrilinearInterpolator::interpolate<double>(m_data, offsets, weights);
    }

    return m_ambientColors[static_cast<int>(value)];
}

} // namespace udg

#endif // UDGAMBIENTVOXELSHADER2_H
