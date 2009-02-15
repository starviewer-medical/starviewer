#ifndef UDGVMIVOXELSHADER1_H
#define UDGVMIVOXELSHADER1_H


#include "voxelshader.h"

#include <QHash>
#include <QMutex>
#include <QThread>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader per la primera passada del càlcul de la VMI. Acumula volum total.
 */
class VmiVoxelShader1 : public VoxelShader
{

public:

    VmiVoxelShader1();
    virtual ~VmiVoxelShader1();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    void initAccumulator();
    const QHash<QThread*, float>& accumulator() const;

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

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;
    QHash<QThread*, float> m_accumulator;
    QMutex m_mutex;

};


inline HdrColor VmiVoxelShader1::shade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VmiVoxelShader1::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                                        const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VmiVoxelShader1::nvShade( const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( position );
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color = m_ambientColors[m_data[offset]];
    QThread *thread = QThread::currentThread();

    if ( !m_accumulator.contains( thread ) )
    {
        m_mutex.lock();
        m_accumulator[thread] = 0.0f;
        m_mutex.unlock();
    }

    m_accumulator[thread] += color.alpha * remainingOpacity;

    return color;
}


inline HdrColor VmiVoxelShader1::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                                          const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_ASSERT( interpolator );
    Q_ASSERT( m_data );

    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );

    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );
    HdrColor color = m_ambientColors[static_cast<int>(value)];

    QThread *thread = QThread::currentThread();

    if ( !m_accumulator.contains( thread ) )
    {
        m_mutex.lock();
        m_accumulator[thread] = 0.0f;
        m_mutex.unlock();
    }

    m_accumulator[thread] += color.alpha * remainingOpacity;

    return color;
}


}


#endif
