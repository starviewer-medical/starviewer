#ifndef UDGVMIVOXELSHADER2_H
#define UDGVMIVOXELSHADER2_H


#include "voxelshader2.h"

#include <QHash>
#include <QMutex>
#include <QThread>
#include <QVector>

#include "transferfunction.h"
#include "trilinearinterpolator.h"


namespace udg {


/**
 * Voxel shader per la primera passada del càlcul de la VMI. Acumula volum total.
 */
class VmiVoxelShader2 : public VoxelShader2
{

public:

    VmiVoxelShader2();
    virtual ~VmiVoxelShader2();

    /// Assigna el volum de dades.
    void setData( const unsigned short *data, unsigned short maxValue, unsigned int size );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    /// Retorna el vector de probabilitats p(o|v) i neteja els acumuladors.
    QVector<float> objectProbabilities();

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor = HdrColor() );
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors();

    const unsigned short *m_data;
    unsigned short m_maxValue;
    unsigned int m_dataSize;
    TransferFunction m_transferFunction;
    HdrColor *m_ambientColors;
    QHash< QThread*, QVector<float> > m_objectVolumePerThread;
    QHash<QThread*, float> m_totalVolumePerThread;
    QMutex m_mutex;

};


inline HdrColor VmiVoxelShader2::shade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( offset, direction, remainingOpacity, baseColor );
}


inline HdrColor VmiVoxelShader2::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor )
{
    return nvShade( position, direction, interpolator, remainingOpacity, baseColor );
}


inline HdrColor VmiVoxelShader2::nvShade( int offset, const Vector3 &direction, double remainingOpacity, const HdrColor &baseColor )
{
    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_ASSERT( m_data );

    HdrColor color = m_ambientColors[m_data[offset]];
    QThread *thread = QThread::currentThread();

    if ( !m_objectVolumePerThread.contains( thread ) )
    {
        m_mutex.lock();
        m_objectVolumePerThread[thread] = QVector<float>( m_dataSize );
        m_totalVolumePerThread[thread] = 0.0f;
        m_mutex.unlock();
    }

    float volume = color.alpha * remainingOpacity;
    m_objectVolumePerThread[thread][offset] += volume;
    m_totalVolumePerThread[thread] += volume;

    return color;
}


inline HdrColor VmiVoxelShader2::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, double remainingOpacity, const HdrColor &baseColor )
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

    if ( !m_objectVolumePerThread.contains( thread ) )
    {
        m_mutex.lock();
        m_objectVolumePerThread[thread] = QVector<float>( m_dataSize );
        m_totalVolumePerThread[thread] = 0.0f;
        m_mutex.unlock();
    }

    float volume = color.alpha * remainingOpacity;
    for ( int i = 0; i < 8; i++ ) m_objectVolumePerThread[thread][offsets[i]] += volume * weights[i];
    m_totalVolumePerThread[thread] += volume;

    return color;
}


}


#endif
