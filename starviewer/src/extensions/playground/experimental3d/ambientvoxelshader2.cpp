#include "ambientvoxelshader2.h"

namespace udg {

AmbientVoxelShader2::AmbientVoxelShader2()
    : VoxelShader(), m_data(0), m_maxValue(0), m_alternativeData(0), m_gradientMagnitudes(0), m_extension(0), m_ambientColors(0)
{
}

AmbientVoxelShader2::~AmbientVoxelShader2()
{
    delete[] m_ambientColors;
}

void AmbientVoxelShader2::setData(const unsigned short *data, unsigned short maxValue)
{
    m_data = data;
    m_maxValue = maxValue;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}

void AmbientVoxelShader2::setAlternativeData(const unsigned short *alternativeData)
{
    m_alternativeData = alternativeData;
}

void AmbientVoxelShader2::setGradientMagnitudes(const unsigned char *gradientMagnitudes)
{
    m_gradientMagnitudes = gradientMagnitudes;
}

void AmbientVoxelShader2::setExtension(const QExperimental3DExtension *extension)
{
    m_extension = extension;
}

void AmbientVoxelShader2::setTransferFunction(const TransferFunction &transferFunction)
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}

QString AmbientVoxelShader2::toString() const
{
    return "AmbientVoxelShader2";
}

void AmbientVoxelShader2::precomputeAmbientColors()
{
    if (!m_ambientColors)
    {
        return;
    }

    unsigned int size = m_maxValue + 1;
    for (unsigned int i = 0; i < size; i++)
    {
        m_ambientColors[i] = m_transferFunction.getColor(i);
        m_ambientColors[i].alpha = m_transferFunction.getScalarOpacity(i);
    }
}

} // namespace udg
