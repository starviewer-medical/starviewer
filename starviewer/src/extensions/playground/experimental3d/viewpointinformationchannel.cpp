#include "viewpointinformationchannel.h"

#include <QCoreApplication>

#include "qexperimental3dviewer.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"

#ifndef CUDA_AVAILABLE
#include <QTemporaryFile>
#include "mathtools.h"
#else // CUDA_AVAILABLE
#include "../gputesting/camera.h"
#include "cudaviewpointinformationchannel.h"
#include "../gputesting/matrix4.h"
#endif


namespace udg {


ViewpointInformationChannel::ViewpointInformationChannel( const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction )
        : m_viewpointGenerator( viewpointGenerator ), m_volume( volume ), m_viewer( viewer ), m_transferFunction( transferFunction )
{
    m_backgroundColor = m_viewer->getBackgroundColor();
    m_viewpoints = m_viewpointGenerator.viewpoints();
}


void ViewpointInformationChannel::setColorVomiPalette( const QVector<Vector3Float> &colorVomiPalette )
{
    QMap< QPair<int, int>, QVector<int> > viewpointSubSets; // (nPunts,nColors) -> punts-millor-distribuïts
    {
        viewpointSubSets[qMakePair(42, 6)] = ( QVector<int>() << 13 << 17 << 23 << 27 << 33 << 39 );
        viewpointSubSets[qMakePair(162, 6)] = ( QVector<int>() << 16 << 34 << 62 << 82 << 110 << 143 );
    }

    int nViewpoints = m_viewpoints.size();
    m_viewpointColors.resize( nViewpoints );
    m_viewpointColors.fill( Vector3Float() );
    int nColors = colorVomiPalette.size();

    if ( nColors == 1 ) // tots el mateix
    {
        DEBUG_LOG( "tots els colors iguals" );
        for ( int i = 0; i < nViewpoints; i++ ) m_viewpointColors[i] = colorVomiPalette.at( 0 );
    }
    else if ( nViewpoints <= nColors )  // agafem els primers colors
    {
        DEBUG_LOG( "primers colors" );
        for ( int i = 0; i < nViewpoints; i++ ) m_viewpointColors[i] = colorVomiPalette.at( i );
    }
    else    // trobar els més separats -> que la distància mínima entre ells sigui màxima entre totes les mínimes
    {
        QVector<int> bestIndices;
        QPair<int, int> mapIndex( nViewpoints, nColors );

        if ( viewpointSubSets.contains( mapIndex ) )
        {
            DEBUG_LOG( "índexs precalculats" );
            bestIndices = viewpointSubSets.value( mapIndex );
        }
        else
        {
            DEBUG_LOG( "calculem els índexs" );
            double maxMinDistance = 0.0;

            class Combination
            {
                private:
                    int m_n;
                    int m_k;
                    QVector<int> m_data;
                public:
                    Combination( int n, int k )
                    {
                        Q_ASSERT( n >= 0 && k >= 0 );
                        m_n = n;
                        m_k = k;
                        m_data.resize( k );
                        for ( int i = 0; i < k; ++i ) m_data[i] = i;
                    }   // Combination(n,k)
                    QVector<int> data() const
                    {
                        return m_data;
                    }
                    QString toString() const
                    {
                        QString s = "{ ";
                        for ( int i = 0; i < m_k; ++i ) s += QString::number( m_data.at( i ) ) + " ";
                        s += "}";
                        return s;
                    }   // toString()
                    Combination* successor() const
                    {
                        if ( m_data[0] == m_n - m_k) return 0;
                        Combination *ans = new Combination( m_n, m_k );
                        int i;
                        for ( i = 0; i < m_k; ++i ) ans->m_data[i] = m_data.at( i );
                        for ( i = m_k - 1; i > 0 && ans->m_data.at( i ) == m_n - m_k + i; --i );
                        ++ans->m_data[i];
                        for ( int j = i; j < m_k - 1; ++j ) ans->m_data[j+1] = ans->m_data[j] + 1;
                        return ans;
                    }   // successor()
                    static quint64 choose( int n, int k )
                    {
                        if ( n < k ) return 0;  // special case
                        if ( n == k ) return 1;
                        int delta, iMax;
                        if ( k < n - k )    // ex: choose(100,3)
                        {
                            delta = n - k;
                            iMax = k;
                        }
                        else                // ex: choose(100,97)
                        {
                            delta = k;
                            iMax = n - k;
                        }
                        quint64 ans = delta + 1;
                        for ( int i = 2; i <= iMax; ++i ) ans = ( ans * ( delta + i ) ) / i;
                        return ans;
                    }   // choose()
            };  // Combination class

            quint64 nCombinations = Combination::choose( nViewpoints, nColors );
            Combination *c = new Combination( nViewpoints, nColors );
            for ( quint64 i = 0; i < nCombinations; i++ )
            {
                //DEBUG_LOG( QString( "%1: %2" ).arg( i ).arg( c->toString() ) );
                QVector<int> indices = c->data();
                double minDistance = ( m_viewpoints.at( indices.at( 0 ) ) - m_viewpoints.at( indices.at( 1 ) ) ).length();
                bool mayBeBetter = minDistance > maxMinDistance;

                for ( int j = 0; j < nColors - 1 && mayBeBetter; j++ )
                {
                    const Vector3 &v1 = m_viewpoints.at( indices.at( j ) );

                    for ( int k = j + 1; k < nColors && mayBeBetter; k++ )
                    {
                        double distance = ( v1 - m_viewpoints.at( indices.at( k ) ) ).length();

                        if ( distance < minDistance )
                        {
                            minDistance = distance;
                            mayBeBetter = minDistance > maxMinDistance;
                        }
                    }
                }

                if ( minDistance > maxMinDistance )
                {
                    maxMinDistance = minDistance;
                    bestIndices = indices;
                }

                Combination *t = c;
                c = c->successor();
                delete t;
            }
        }

#ifndef QT_NO_DEBUG
        DEBUG_LOG( "best indices:" );
        for ( int i = 0; i < nColors; i++ ) DEBUG_LOG( QString::number( bestIndices.at( i ) ) );
#endif

        for ( int i = 0; i < nColors; i++ ) m_viewpointColors[bestIndices.at(i)] = colorVomiPalette.at( i );

        float maxChange;

        do
        {
            maxChange = 0.0f;

            for ( int i = 0; i < nViewpoints; i++ )
            {
                if ( bestIndices.contains( i ) ) continue;

                Vector3 viewpoint = m_viewpoints.at( i );
                QVector<int> neighbours = m_viewpointGenerator.neighbours( i );
                Vector3Float color;
                double totalWeight = 0.0;
                int nNeighbours = neighbours.size();

                for ( int j = 0; j < nNeighbours; j++ )
                {
                    int neighbourIndex = neighbours.at( j );
                    double distance = ( viewpoint - m_viewpoints.at( neighbourIndex ) ).length();
                    double weight = 1.0 / distance;
                    color += weight * m_viewpointColors.at( neighbourIndex );
                    totalWeight += weight;
                }

                color /= totalWeight;
                float change = ( color - m_viewpointColors.at( i ) ).length();
                m_viewpointColors[i] = color;

                if ( change > maxChange ) maxChange = change;
            }
        } while ( maxChange > 0.1f );
    }

#ifndef QT_NO_DEBUG
    DEBUG_LOG( "colors: " );
    for ( int i = 0; i < nViewpoints; i++ )
    {
        DEBUG_LOG( QString( "v%1: %2 -> %3" ).arg( i+1 ).arg( m_viewpoints.at( i ).toString() ).arg( m_viewpointColors.at( i ).toString() ) );
    }
#endif
}


void ViewpointInformationChannel::filterViewpoints( const QVector<bool> &filter )
{
    if ( m_viewpoints.size() != filter.size() )
    {
        DEBUG_LOG( QString( "Error: mides dels vectors diferents al filtre de punts de vista: %1 punts de vista, %2 filtres" ).arg( m_viewpoints.size() ).arg( filter.size() ) );
        return;
    }

    m_viewpoints.clear();
    QVector<Vector3Float> viewpointColors;

    for ( int i = 0; i < filter.size(); i++ )
    {
        if ( filter.at( i ) )
        {
            m_viewpoints << m_viewpointGenerator.viewpoint( i );
            if ( !m_viewpointColors.isEmpty() ) viewpointColors << m_viewpointColors.at( i );
        }
    }

    m_viewpointColors = viewpointColors;
}


void ViewpointInformationChannel::compute( bool &viewpointEntropy, bool &entropy, bool &vmi, bool &mi, bool &vomi, bool &viewpointVomi, bool &colorVomi )
{
    // Si no hi ha res a calcular marxem
    if ( !viewpointEntropy && !entropy && !vmi && !mi && !vomi && !viewpointVomi && !colorVomi ) return;

    bool viewProbabilities = false;
    bool voxelProbabilities = false;

    // Dependències
    if ( entropy ) viewProbabilities = true;
    if ( entropy ) viewpointEntropy = true;
    if ( mi ) vmi = true;
    if ( vmi ) voxelProbabilities = true;
    if ( viewpointVomi ) vomi = true;
    if ( vomi ) voxelProbabilities = true;
    if ( colorVomi ) voxelProbabilities = true;
    if ( voxelProbabilities ) viewProbabilities = true;

#ifndef CUDA_AVAILABLE
    computeCpu( viewProbabilities, voxelProbabilities, viewpointEntropy, entropy, vmi, mi, vomi, viewpointVomi, colorVomi );
#else // CUDA_AVAILABLE
    computeCuda( viewProbabilities, voxelProbabilities, viewpointEntropy, entropy, vmi, mi, vomi, viewpointVomi );
#endif // CUDA_AVAILABLE
}


const QVector<float>& ViewpointInformationChannel::viewpointEntropy() const
{
    return m_viewpointEntropy;
}


float ViewpointInformationChannel::entropy() const
{
    return m_entropy;
}


const QVector<float>& ViewpointInformationChannel::vmi() const
{
    return m_vmi;
}


float ViewpointInformationChannel::mi() const
{
    return m_mi;
}


const QVector<float>& ViewpointInformationChannel::vomi() const
{
    return m_vomi;
}


float ViewpointInformationChannel::maximumVomi() const
{
    return m_maximumVomi;
}


const QVector<float>& ViewpointInformationChannel::viewpointVomi() const
{
    return m_viewpointVomi;
}


const QVector<Vector3Float>& ViewpointInformationChannel::colorVomi() const
{
    return m_colorVomi;
}


float ViewpointInformationChannel::maximumColorVomi() const
{
    return m_maximumColorVomi;
}


#ifndef CUDA_AVAILABLE


void ViewpointInformationChannel::computeCpu( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeVomi,
                                              bool computeViewpointVomi, bool computeColorVomi )
{
    DEBUG_LOG( "computeCpu" );

    // Inicialitzar progrés
    int nSteps = 1; // ray casting (p(Z|V))
    if ( computeViewProbabilities ) nSteps++;   // p(V)
    if ( computeVoxelProbabilities ) nSteps++;  // p(Z)
    if ( computeVomi || computeColorVomi ) nSteps++;    // VoMI + color VoMI
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi || computeViewpointVomi ) nSteps++;   // viewpoint entropy + entropy + VMI + MI + viewpoint VoMI

    emit totalProgressMaximum( nSteps );
    int step = 0;
    emit totalProgress( step );

    createVoxelProbabilitiesPerViewFiles();
    if ( m_voxelProbabilitiesPerViewFiles.isEmpty() ) return;   // caldria llançar alguna excepció o retornar error

    float totalViewedVolume;

    // p(Z|V) (i acumulació de p(V))
    {
        totalViewedVolume = rayCastingCpu( computeViewProbabilities );
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(V)
    if ( computeViewProbabilities )
    {
        computeViewProbabilitiesCpu( totalViewedVolume );
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(Z)
    if ( computeVoxelProbabilities )
    {
        computeVoxelProbabilitiesCpu();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI + color VoMI
    if ( computeVomi || computeColorVomi )
    {
        computeVomiCpu( computeVomi, computeColorVomi );
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // viewpoint entropy + entropy + VMI + MI + viewpoint VoMI
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi || computeViewpointVomi )
    {
        computeViewMeasuresCpu( computeViewpointEntropy, computeEntropy, computeVmi, computeMi, computeViewpointVomi );
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    deleteVoxelProbabilitiesPerViewFiles();
}


void ViewpointInformationChannel::createVoxelProbabilitiesPerViewFiles()
{
    DEBUG_LOG( "Creem p(Z|V)" );

    int nViewpoints = m_viewpoints.size();
    m_voxelProbabilitiesPerViewFiles.resize( nViewpoints );

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_voxelProbabilitiesPerViewFiles[i] = new QTemporaryFile( "pZvXXXXXX.tmp" );    // els fitxers temporals es creen al directori de treball

        if ( !m_voxelProbabilitiesPerViewFiles.at( i )->open() )
        {
            DEBUG_LOG( QString( "No s'ha pogut obrir el fitxer: error %1" ).arg( m_voxelProbabilitiesPerViewFiles.at( i )->errorString() ) );
            for ( int j = 0; j < i; j++ ) m_voxelProbabilitiesPerViewFiles.at( j )->close();
            m_voxelProbabilitiesPerViewFiles.clear();   // el deixarem buit si hi ha hagut problemes
            break;
        }
    }
}


void ViewpointInformationChannel::readVoxelProbabilitiesInView( int i, QVector<float> &voxelProbabilitiesInView )
{
    m_voxelProbabilitiesPerViewFiles.at( i )->reset();  // reset per tornar al principi
    m_voxelProbabilitiesPerViewFiles.at( i )->read( reinterpret_cast<char*>( voxelProbabilitiesInView.data() ), voxelProbabilitiesInView.size() * sizeof(float) ); // llegim...
    m_voxelProbabilitiesPerViewFiles.at( i )->reset();  // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)
}


void ViewpointInformationChannel::deleteVoxelProbabilitiesPerViewFiles()
{
    DEBUG_LOG( "Destruïm p(Z|V)" );

    for ( int i = 0; i < m_voxelProbabilitiesPerViewFiles.size(); i++ )
    {
        m_voxelProbabilitiesPerViewFiles.at( i )->close();
        delete m_voxelProbabilitiesPerViewFiles.at( i );
    }

    m_voxelProbabilitiesPerViewFiles.clear();
}


float ViewpointInformationChannel::rayCastingCpu( bool computeViewProbabilities )
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();
    double totalViewedVolume = 0.0;

    if ( computeViewProbabilities ) m_viewProbabilities.resize( nViewpoints );

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_volume->startVmiMode();

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_volume->startVmiSecondPass();
        m_viewer->setCamera( m_viewpoints.at( i ), Vector3(), m_viewpointGenerator.up( m_viewpoints.at( i ) ) );    // render

        // p(Z|V)
        QVector<float> voxelProbabilitiesInView = m_volume->finishVmiSecondPass();  // p(Z|v)
        m_voxelProbabilitiesPerViewFiles.at( i )->write( reinterpret_cast<const char*>( voxelProbabilitiesInView.data() ), nVoxels * sizeof(float) );

        // p(V)
        if ( computeViewProbabilities )
        {
            float viewedVolume = m_volume->viewedVolumeInVmiSecondPass();
            m_viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;
        }

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    return totalViewedVolume;
}


void ViewpointInformationChannel::computeViewProbabilitiesCpu( float totalViewedVolume )
{
    int nViewpoints = m_viewpoints.size();

    if ( totalViewedVolume > 0.0f )
    {
        emit partialProgress( 0 );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_viewProbabilities[i] /= totalViewedVolume;
            Q_ASSERT( m_viewProbabilities.at( i ) == m_viewProbabilities.at( i ) );
            DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( m_viewProbabilities.at( i ) ) );

            emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
            QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
        }
    }

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for ( int i = 0; i < nViewpoints; i++ )
    {
        float pv = m_viewProbabilities.at( i );
        Q_ASSERT( pv == pv );
        Q_ASSERT( pv >= 0.0f && pv <= 1.0f );
        sum += pv;
    }
    DEBUG_LOG( QString( "sum p(v) = %1" ).arg( sum ) );
#endif
}


void ViewpointInformationChannel::computeVoxelProbabilitiesCpu()
{
    class PZThread : public QThread {
        public:
            PZThread( QVector<float> &voxelProbabilities, const QVector<float> &voxelProbabilitiesInView, int start, int end )
                : m_viewProbability( 0.0f ), m_voxelProbabilities( voxelProbabilities ), m_voxelProbabilitiesInView( voxelProbabilitiesInView ), m_start( start ), m_end( end )
            {
            }
            void setViewProbability( float viewProbability )
            {
                m_viewProbability = viewProbability;
            }
        protected:
            virtual void run()
            {
                for ( int i = m_start; i < m_end; i++ ) m_voxelProbabilities[i] += m_viewProbability * m_voxelProbabilitiesInView[i];
            }
        private:
            float m_viewProbability;
            QVector<float> &m_voxelProbabilities;
            const QVector<float> &m_voxelProbabilitiesInView;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_voxelProbabilities.resize( nVoxels );
    m_voxelProbabilities.fill( 0.0f );
    QVector<float> voxelProbabilitiesInView( nVoxels ); // vector p(Z|v)

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    PZThread **pzThreads = new PZThread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for ( int k = 0; k < nThreads; k++ )
    {
        pzThreads[k] = new PZThread( m_voxelProbabilities, voxelProbabilitiesInView, start, end );
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if ( end > nVoxels ) end = nVoxels;
    }

    for ( int i = 0; i < nViewpoints; i++ )
    {
        readVoxelProbabilitiesInView( i, voxelProbabilitiesInView );

        for ( int k = 0; k < nThreads; k++ )
        {
            pzThreads[k]->setViewProbability( m_viewProbabilities.at( i ) );
            pzThreads[k]->start();
        }

        for ( int k = 0; k < nThreads; k++ ) pzThreads[k]->wait();

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for ( int k = 0; k < nThreads; k++ ) delete pzThreads[k];
    delete[] pzThreads;

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for ( int j = 0; j < nVoxels; j++ )
    {
        float pz = m_voxelProbabilities.at( j );
        Q_ASSERT( pz == pz );
        Q_ASSERT( pz >= 0.0f && pz <= 1.0f );
        sum += pz;
    }
    DEBUG_LOG( QString( "sum p(z) = %1" ).arg( sum ) );
#endif
}


void ViewpointInformationChannel::computeViewMeasuresCpu( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointVomi )
{
    class ViewpointVomiThread : public QThread {
        public:
            ViewpointVomiThread( const QVector<float> &voxelProbabilities, const QVector<float> &voxelProbabilitiesInView, const QVector<float> &vomi, int start, int end )
                : m_viewProbability( 0.0f ), m_voxelProbabilities( voxelProbabilities ), m_voxelProbabilitiesInView( voxelProbabilitiesInView ), m_vomi( vomi ), m_viewpointVomi( 0.0f ), m_start( start ), m_end( end )
            {
            }
            void setViewProbability( float viewProbability )
            {
                m_viewProbability = viewProbability;
            }
            float viewpointVomi() const
            {
                return m_viewpointVomi;
            }
        protected:
            virtual void run()
            {
                double viewpointVomi = 0.0;
                for ( int i = m_start; i < m_end; i++ )
                {
                    float pz = m_voxelProbabilities.at( i );
                    float pzv = m_voxelProbabilitiesInView.at( i );
                    float pvz = m_viewProbability * pzv / pz;
                    if ( pvz > 0.0f ) viewpointVomi += pvz * m_vomi.at( i );
                }
                m_viewpointVomi = viewpointVomi;
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            const QVector<float> &m_voxelProbabilitiesInView;
            const QVector<float> &m_vomi;
            float m_viewpointVomi;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    QVector<float> voxelProbabilitiesInView( nVoxels ); // vector p(Z|v)

    int nThreads = QThread::idealThreadCount();
    ViewpointVomiThread **viewpointVomiThreads;

    if ( computeViewpointEntropy ) m_viewpointEntropy.resize( nViewpoints );
    if ( computeEntropy ) m_entropy = 0.0f;
    if ( computeVmi ) m_vmi.resize( nViewpoints );
    if ( computeMi ) m_mi = 0.0f;
    if ( computeViewpointVomi )
    {
        m_viewpointVomi.resize( nViewpoints );
        m_viewpointVomi.fill( 0.0f );
        viewpointVomiThreads = new ViewpointVomiThread*[nThreads];
        int nVoxelsPerThread = nVoxels / nThreads + 1;
        int start = 0, end = nVoxelsPerThread;
        for ( int k = 0; k < nThreads; k++ )
        {
            viewpointVomiThreads[k] = new ViewpointVomiThread( m_voxelProbabilities, voxelProbabilitiesInView, m_vomi, start, end );
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if ( end > nVoxels ) end = nVoxels;
        }
    }

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        readVoxelProbabilitiesInView( i, voxelProbabilitiesInView );

        if ( computeViewpointEntropy )
        {
            float viewpointEntropy = InformationTheory<float>::entropy( voxelProbabilitiesInView );
            Q_ASSERT( viewpointEntropy == viewpointEntropy );
            m_viewpointEntropy[i] = viewpointEntropy;
            DEBUG_LOG( QString( "H(Z|v%1) = %2" ).arg( i + 1 ).arg( viewpointEntropy ) );
        }

        if ( computeEntropy )
        {
            m_entropy += m_viewProbabilities.at( i ) * m_viewpointEntropy.at( i );
        }

        if ( computeVmi )
        {
            float vmi = InformationTheory<float>::kullbackLeiblerDivergence( voxelProbabilitiesInView, m_voxelProbabilities );
            Q_ASSERT( vmi == vmi );
            m_vmi[i] = vmi;
            DEBUG_LOG( QString( "VMI(v%1) = %2" ).arg( i + 1 ).arg( vmi ) );
        }

        if ( computeMi )
        {
            m_mi += m_viewProbabilities.at( i ) * m_vmi.at( i );
        }

        if ( computeViewpointVomi )
        {
            float pv = m_viewProbabilities.at( i );

            if ( pv > 0.0f )
            {
                for ( int k = 0; k < nThreads; k++ )
                {
                    viewpointVomiThreads[k]->setViewProbability( pv );
                    viewpointVomiThreads[k]->start();
                }

                double viewpointVomi = 0.0;

                for ( int k = 0; k < nThreads; k++ )
                {
                    viewpointVomiThreads[k]->wait();
                    viewpointVomi += viewpointVomiThreads[k]->viewpointVomi();
                }

                Q_ASSERT( viewpointVomi == viewpointVomi );
                m_viewpointVomi[i] = viewpointVomi;
                DEBUG_LOG( QString( "VVoMI(v%1) = %2" ).arg( i + 1 ).arg( viewpointVomi ) );
            }
        }

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if ( computeEntropy )
    {
        DEBUG_LOG( QString( "H(Z) = %1" ).arg( m_entropy ) );
    }

    if ( computeMi )
    {
        DEBUG_LOG( QString( "I(V;Z) = %1" ).arg( m_mi ) );
    }

    if ( computeViewpointVomi )
    {
        for ( int k = 0; k < nThreads; k++ ) delete viewpointVomiThreads[k];
        delete[] viewpointVomiThreads;
    }
}


void ViewpointInformationChannel::computeVomiCpu( bool computeVomi, bool computeColorVomi )
{
    class VomiThread : public QThread {
        public:
            VomiThread( const QVector<float> &voxelProbabilities, const QVector<float> &voxelProbabilitiesInView, bool computeVomi, QVector<float> &vomi, bool computeColorVomi, QVector<Vector3Float> &colorVomi,
                        int start, int end )
                : m_viewProbability( 0.0f ), m_voxelProbabilities( voxelProbabilities ), m_voxelProbabilitiesInView( voxelProbabilitiesInView ), m_computeVomi( computeVomi ), m_vomi( vomi ),
                  m_computeColorVomi( computeColorVomi ), m_viewColor( 1.0f, 1.0f, 1.0f ), m_colorVomi( colorVomi ), m_start( start ), m_end( end )
            {
            }
            void setViewProbability( float viewProbability )
            {
                m_viewProbability = viewProbability;
            }
            void setViewColor( const Vector3Float &viewColor )
            {
                m_viewColor = viewColor;
            }
        protected:
            virtual void run()
            {
                Vector3Float color = Vector3Float( 1.0f, 1.0f, 1.0f ) - m_viewColor;
                for ( int i = m_start; i < m_end; i++ )
                {
                    float pz = m_voxelProbabilities.at( i );
                    float pzv = m_voxelProbabilitiesInView.at( i );
                    float pvz = m_viewProbability * pzv / pz;
                    if ( pvz > 0.0f )
                    {
                        if ( m_computeVomi ) m_vomi[i] += pvz * MathTools::logTwo( pvz / m_viewProbability );
                        if ( m_computeColorVomi ) m_colorVomi[i] += pvz * MathTools::logTwo( pvz / m_viewProbability ) * color;
                    }
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            const QVector<float> &m_voxelProbabilitiesInView;
            bool m_computeVomi;
            QVector<float> &m_vomi;
            bool m_computeColorVomi;
            Vector3Float m_viewColor;
            QVector<Vector3Float> &m_colorVomi;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    if ( computeVomi )
    {
        m_vomi.resize( nVoxels );
        m_vomi.fill( 0.0f );
    }

    if ( computeColorVomi )
    {
        m_colorVomi.resize( nVoxels );
        m_colorVomi.fill( Vector3Float() );
    }

    QVector<float> voxelProbabilitiesInView( nVoxels ); // vector p(Z|v)

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    VomiThread **vomiThreads = new VomiThread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for ( int k = 0; k < nThreads; k++ )
    {
        vomiThreads[k] = new VomiThread( m_voxelProbabilities, voxelProbabilitiesInView, computeVomi, m_vomi, computeColorVomi, m_colorVomi, start, end );
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if ( end > nVoxels ) end = nVoxels;
    }

    for ( int i = 0; i < nViewpoints; i++ )
    {
        float pv = m_viewProbabilities.at( i );
        if ( pv == 0.0f ) continue;

        readVoxelProbabilitiesInView( i, voxelProbabilitiesInView );

        for ( int k = 0; k < nThreads; k++ )
        {
            vomiThreads[k]->setViewProbability( m_viewProbabilities.at( i ) );
            if ( computeColorVomi ) vomiThreads[k]->setViewColor( m_viewpointColors.at( i ) );
            vomiThreads[k]->start();
        }

        for ( int k = 0; k < nThreads; k++ ) vomiThreads[k]->wait();

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for ( int k = 0; k < nThreads; k++ ) delete vomiThreads[k];
    delete[] vomiThreads;

    if ( computeVomi ) m_maximumVomi = 0.0f;
    if ( computeColorVomi ) m_maximumColorVomi = 0.0f;

    for ( int j = 0; j < nVoxels; j++ )
    {
        if ( computeVomi )
        {
            float vomi = m_vomi.at( j );
            Q_ASSERT( vomi == vomi );
            Q_ASSERT( vomi >= 0.0f );
            if ( vomi > m_maximumVomi ) m_maximumVomi = vomi;
        }

        if ( computeColorVomi )
        {
            Vector3Float colorVomi = m_colorVomi.at( j );
            Q_ASSERT( colorVomi.x == colorVomi.x && colorVomi.y == colorVomi.y && colorVomi.z == colorVomi.z );
            /// \todo pot ser < 0???
            //Q_ASSERT( colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f );
            //Q_ASSERT_X( colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f, "comprovació color vomi", qPrintable( colorVomi.toString() ) );
            if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
            if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
            if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
        }
    }
}


#else // CUDA_AVAILABLE


Matrix4 ViewpointInformationChannel::viewMatrix( const Vector3 &viewpoint )
{
    Camera camera;
    camera.lookAt( viewpoint, Vector3(), ViewpointGenerator::up( viewpoint ) );
    return camera.getViewMatrix();
}


void ViewpointInformationChannel::computeCuda( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeVomi,
                                               bool computeViewpointVomi )
{
    DEBUG_LOG( "computeCuda" );

    // Inicialitzar progrés
    int nSteps = 0;
    if ( computeViewProbabilities ) nSteps++;   // p(V)
    if ( computeVoxelProbabilities ) nSteps++;  // p(Z)
    if ( computeVomi ) nSteps++;    // VoMI
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi || computeViewpointVomi ) nSteps++;   // viewpoint entropy + entropy + VMI + MI + viewpoint VoMI

    emit totalProgressMaximum( nSteps );
    int step = 0;
    emit totalProgress( step );

    // Inicialització de CUDA
    cvicSetupRayCast( m_volume->getImage(), m_transferFunction, 1024, 720, m_backgroundColor, true );
    if ( computeVoxelProbabilities ) cvicSetupVoxelProbabilities();

    // p(V)
    if ( computeViewProbabilities )
    {
        computeViewProbabilitiesCuda();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(Z)
    if ( computeVoxelProbabilities )
    {
        computeVoxelProbabilitiesCuda();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI
    if ( computeVomi )
    {
        computeVomiCuda();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // viewpoint entropy + entropy + VMI + MI + viewpoint VoMI
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi || computeViewpointVomi )
    {
        computeViewMeasuresCuda( computeViewpointEntropy, computeEntropy, computeVmi, computeMi, computeViewpointVomi );
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // Finalització de CUDA
    if ( computeVoxelProbabilities ) cvicCleanupVoxelProbabilities();
    cvicCleanupRayCast();
}


void ViewpointInformationChannel::computeViewProbabilitiesCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_viewProbabilities.resize( nViewpoints );
    double totalViewedVolume = 0.0;

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> voxelProbabilitiesInView = cvicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nVoxels; j++ ) viewedVolume += voxelProbabilitiesInView.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        m_viewProbabilities[i] = viewedVolume;
        totalViewedVolume += viewedVolume;

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_viewProbabilities[i] /= totalViewedVolume;
        DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i+1 ).arg( m_viewProbabilities.at(i) ) );
    }

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for ( int i = 0; i < nViewpoints; i++ )
    {
        float pv = m_viewProbabilities.at( i );
        Q_ASSERT( pv == pv );
        Q_ASSERT( pv >= 0.0f && pv <= 1.0f );
        sum += pv;
    }
    DEBUG_LOG( QString( "sum p(v) = %1" ).arg( sum ) );
#endif
}


void ViewpointInformationChannel::computeVoxelProbabilitiesCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> voxelProbabilitiesInView = cvicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nVoxels; j++ ) viewedVolume += voxelProbabilitiesInView.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        cvicAccumulateVoxelProbabilities( m_viewProbabilities.at( i ), viewedVolume );

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_voxelProbabilities = cvicGetVoxelProbabilities();

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for ( int j = 0; j < nVoxels; j++ )
    {
        float pz = m_voxelProbabilities.at( j );
        Q_ASSERT( pz == pz );
        Q_ASSERT( pz >= 0.0f && pz <= 1.0f );
        sum += pz;
    }
    DEBUG_LOG( QString( "sum p(z) = %1" ).arg( sum ) );
#endif
}


void ViewpointInformationChannel::computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointVomi )
{
    class ViewpointVomiThread : public QThread {
        public:
            ViewpointVomiThread( const QVector<float> &voxelProbabilities, const QVector<float> &vomi, int start, int end )
                : m_viewProbability( 0.0f ), m_voxelProbabilities( voxelProbabilities ), m_vomi( vomi ), m_viewpointVomi( 0.0f ), m_start( start ), m_end( end )
            {
            }
            void setViewData( float viewProbability, const QVector<float> &voxelProabilitiesInView )
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProabilitiesInView;
            }
            float viewpointVomi() const
            {
                return m_viewpointVomi;
            }
        protected:
            virtual void run()
            {
                double viewpointVomi = 0.0;
                for ( int i = m_start; i < m_end; i++ )
                {
                    float pz = m_voxelProbabilities.at( i );
                    float pzv = m_voxelProbabilitiesInView.at( i );
                    float pvz = m_viewProbability * pzv / pz;
                    if ( pvz > 0.0f ) viewpointVomi += pvz * m_vomi.at( i );
                }
                m_viewpointVomi = viewpointVomi;
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            const QVector<float> &m_vomi;
            float m_viewpointVomi;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    int nThreads = QThread::idealThreadCount();
    ViewpointVomiThread **viewpointVomiThreads;

    if ( computeViewpointEntropy ) m_viewpointEntropy.resize( nViewpoints );
    if ( computeEntropy ) m_entropy = 0.0f;
    if ( computeVmi ) m_vmi.resize( nViewpoints );
    if ( computeMi ) m_mi = 0.0f;
    if ( computeViewpointVomi )
    {
        m_viewpointVomi.resize( nViewpoints );
        m_viewpointVomi.fill( 0.0f );
        viewpointVomiThreads = new ViewpointVomiThread*[nThreads];
        int nVoxelsPerThread = nVoxels / nThreads + 1;
        int start = 0, end = nVoxelsPerThread;
        for ( int k = 0; k < nThreads; k++ )
        {
            viewpointVomiThreads[k] = new ViewpointVomiThread( m_voxelProbabilities, m_vomi, start, end );
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if ( end > nVoxels ) end = nVoxels;
        }
    }

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> voxelProbabilitiesInView = cvicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nVoxels; j++ ) viewedVolume += voxelProbabilitiesInView.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );
        for ( int j = 0; j < nVoxels; j++ ) voxelProbabilitiesInView[j] /= viewedVolume;    // p(Z|v)

        if ( computeViewpointEntropy )
        {
            float viewpointEntropy = InformationTheory<float>::entropy( voxelProbabilitiesInView );
            Q_ASSERT( viewpointEntropy == viewpointEntropy );
            m_viewpointEntropy[i] = viewpointEntropy;
            DEBUG_LOG( QString( "H(Z|v%1) = %2" ).arg( i + 1 ).arg( viewpointEntropy ) );
        }

        if ( computeEntropy )
        {
            m_entropy += m_viewProbabilities.at( i ) * m_viewpointEntropy.at( i );
        }

        if ( computeVmi )
        {
            float vmi = InformationTheory<float>::kullbackLeiblerDivergence( voxelProbabilitiesInView, m_voxelProbabilities );
            Q_ASSERT( vmi == vmi );
            m_vmi[i] = vmi;
            DEBUG_LOG( QString( "VMI(v%1) = %2" ).arg( i + 1 ).arg( vmi ) );
        }

        if ( computeMi )
        {
            m_mi += m_viewProbabilities.at( i ) * m_vmi.at( i );
        }

        if ( computeViewpointVomi )
        {
            float pv = m_viewProbabilities.at( i );

            if ( pv > 0.0f )
            {
                for ( int k = 0; k < nThreads; k++ )
                {
                    viewpointVomiThreads[k]->setViewData( pv, voxelProbabilitiesInView );
                    viewpointVomiThreads[k]->start();
                }

                double viewpointVomi = 0.0;

                for ( int k = 0; k < nThreads; k++ )
                {
                    viewpointVomiThreads[k]->wait();
                    viewpointVomi += viewpointVomiThreads[k]->viewpointVomi();
                }

                Q_ASSERT( viewpointVomi == viewpointVomi );
                m_viewpointVomi[i] = viewpointVomi;
                DEBUG_LOG( QString( "VVoMI(v%1) = %2" ).arg( i + 1 ).arg( viewpointVomi ) );
            }
        }

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if ( computeEntropy )
    {
        DEBUG_LOG( QString( "H(Z) = %1" ).arg( m_entropy ) );
    }

    if ( computeMi )
    {
        DEBUG_LOG( QString( "I(V;Z) = %1" ).arg( m_mi ) );
    }

    if ( computeViewpointVomi )
    {
        for ( int k = 0; k < nThreads; k++ ) delete viewpointVomiThreads[k];
        delete[] viewpointVomiThreads;
    }
}


void ViewpointInformationChannel::computeVomiCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    cvicSetupVomi();

    for ( int i = 0; i < nViewpoints; i++ )
    {
        float pv = m_viewProbabilities.at( i );
        if ( pv == 0.0f ) continue;

        QVector<float> voxelProbabilitiesInView = cvicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nVoxels; j++ ) viewedVolume += voxelProbabilitiesInView.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        cvicAccumulateVomi( pv, viewedVolume );

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_vomi = cvicCleanupVomi();
    m_maximumVomi = 0.0f;

    for ( int j = 0; j < nVoxels; j++ )
    {
        float vomi = m_vomi.at( j );
        Q_ASSERT( vomi == vomi );
        Q_ASSERT( vomi >= 0.0f );
        if ( vomi > m_maximumVomi ) m_maximumVomi = vomi;
    }
}


#endif // CUDA_AVAILABLE


} // namespace udg
