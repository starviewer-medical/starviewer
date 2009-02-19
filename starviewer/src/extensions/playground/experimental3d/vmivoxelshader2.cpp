#include "vmivoxelshader2.h"


namespace udg {


VmiVoxelShader2::VmiVoxelShader2()
 : VoxelShader(), m_data( 0 ), m_maxValue( 0 ), m_dataSize( 0 ), m_ambientColors( 0 ), m_viewedVolume( 0.0f )
{
}


VmiVoxelShader2::~VmiVoxelShader2()
{
    delete[] m_ambientColors;
}


void VmiVoxelShader2::setData( const unsigned short *data, unsigned short maxValue, unsigned int size )
{
    m_data = data;
    m_maxValue = maxValue;
    m_dataSize = size;
    delete[] m_ambientColors;
    m_ambientColors = new HdrColor[m_maxValue + 1];
}


void VmiVoxelShader2::setTransferFunction( const TransferFunction &transferFunction )
{
    m_transferFunction = transferFunction;
    precomputeAmbientColors();
}


QVector<float> VmiVoxelShader2::objectProbabilities()
{
    QList<QThread*> keys = m_totalVolumePerThread.keys();
    int nKeys = keys.size();

    float totalVolume = 0.0f;
    for ( int i = 0; i < nKeys; i++ ) totalVolume += m_totalVolumePerThread.value( keys.at( i ) );

    class MergeThread : public QThread {
        public:
            MergeThread( QVector<float> &objectProbabilities, const QHash< QThread*, QVector<float> > &objectVolumePerThread, float totalVolume, int start, int end )
                : m_objectProbabilities( objectProbabilities ), m_objectVolumePerThread( objectVolumePerThread ), m_totalVolume( totalVolume ), m_start( start ), m_end( end )
            {
            }
            virtual void run()
            {
                QList<QThread*> keys = m_objectVolumePerThread.keys();
                int nKeys = keys.size();
                for ( int i = 0; i < nKeys; i++ )
                    for ( int j = m_start; j < m_end; j++ )
                        m_objectProbabilities[j] += m_objectVolumePerThread.value( keys.at( i ) ).at( j );
                for ( int i = m_start; i < m_end; i++ )
                    m_objectProbabilities[i] /= m_totalVolume;
            }
        private:
            QVector<float> &m_objectProbabilities;
            const QHash< QThread*, QVector<float> > &m_objectVolumePerThread;
            float m_totalVolume;
            int m_start, m_end;
    };

    QVector<float> objectProbabilities = m_objectVolumePerThread.take( keys.at( 0 ) );
    int nThreads = QThread::idealThreadCount();
    int nObjectsPerThread = m_dataSize / nThreads + 1;
    QList<QThread*> mergeThreads;
    int start = 0, end = nObjectsPerThread;
    for ( int i = 0; i < nThreads; i++ )
    {
        mergeThreads << new MergeThread( objectProbabilities, m_objectVolumePerThread, totalVolume, start, end );
        mergeThreads[i]->start();
        start += nObjectsPerThread;
        end += nObjectsPerThread;
        if ( end > static_cast<int>( m_dataSize ) ) end = m_dataSize;
    }
    for ( int i = 0; i < nThreads; i++ )
    {
        mergeThreads[i]->wait();
        delete mergeThreads[i];
    }

    m_viewedVolume = totalVolume;

    m_totalVolumePerThread.clear();
    m_objectVolumePerThread.clear();

    return objectProbabilities;
}


float VmiVoxelShader2::viewedVolume() const
{
    return m_viewedVolume;
}


QString VmiVoxelShader2::toString() const
{
    return "VmiVoxelShader2";
}


void VmiVoxelShader2::precomputeAmbientColors()
{
    if ( !m_ambientColors ) return;

    unsigned int size = m_maxValue + 1;

    for ( unsigned int i = 0; i < size; i++ ) m_ambientColors[i] = m_transferFunction.get( i );
}


}
