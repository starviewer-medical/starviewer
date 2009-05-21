#include "viewpointinformationchannel.h"

#include <QCoreApplication>

#include "qexperimental3dviewer.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"

#ifndef CUDA_AVAILABLE
#include <QTemporaryFile>
#include "mathtools.h"
#else
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


void ViewpointInformationChannel::filterViewpoints( const QVector<bool> &filter )
{
    if ( m_viewpoints.size() != filter.size() )
    {
        DEBUG_LOG( QString( "Error: mides dels vectors diferents al filtre de punts de vista: %1 punts de vista, %2 filtres" ).arg( m_viewpoints.size() ).arg( filter.size() ) );
        return;
    }

    m_viewpoints.clear();

    for ( int i = 0; i < filter.size(); i++ ) if ( filter.at( i ) ) m_viewpoints << m_viewpointGenerator.viewpoint( i );
}


void ViewpointInformationChannel::compute( bool &viewpointEntropy, bool &entropy, bool &vmi, bool &mi, bool &vomi, bool &viewpointVomi )
{
    // Si no hi ha res a calcular marxem
    if ( !viewpointEntropy && !entropy && !vmi && !mi && !vomi && !viewpointVomi ) return;

    bool viewProbabilities = false;
    bool voxelProbabilities = false;

    // Dependències
    if ( entropy ) viewProbabilities = true;
    if ( entropy ) viewpointEntropy = true;
    if ( mi ) vmi = true;
    if ( vmi ) voxelProbabilities = true;
    if ( viewpointVomi ) vomi = true;
    if ( vomi ) voxelProbabilities = true;
    if ( voxelProbabilities ) viewProbabilities = true;

#ifndef CUDA_AVAILABLE
    computeCpu( viewProbabilities, voxelProbabilities, viewpointEntropy, entropy, vmi, mi, vomi, viewpointVomi );
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


#ifndef CUDA_AVAILABLE


void ViewpointInformationChannel::computeCpu( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeVomi,
                                              bool computeViewpointVomi )
{
    DEBUG_LOG( "computeCpu" );

    // Inicialitzar progrés
    int nSteps = 1; // ray casting (p(Z|V))
    if ( computeViewProbabilities ) nSteps++;   // p(V)
    if ( computeVoxelProbabilities ) nSteps++;  // p(Z)
    if ( computeVomi ) nSteps++;    // VoMI
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

    // VoMI
    if ( computeVomi )
    {
        computeVomiCpu();
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


void ViewpointInformationChannel::computeVomiCpu()
{
    class VomiThread : public QThread {
        public:
            VomiThread( const QVector<float> &voxelProbabilities, const QVector<float> &voxelProbabilitiesInView, QVector<float> &vomi, int start, int end )
                : m_viewProbability( 0.0f ), m_voxelProbabilities( voxelProbabilities ), m_voxelProbabilitiesInView( voxelProbabilitiesInView ), m_vomi( vomi ), m_start( start ), m_end( end )
            {
            }
            void setViewProbability( float viewProbability )
            {
                m_viewProbability = viewProbability;
            }
        protected:
            virtual void run()
            {
                for ( int i = m_start; i < m_end; i++ )
                {
                    float pz = m_voxelProbabilities.at( i );
                    float pzv = m_voxelProbabilitiesInView.at( i );
                    float pvz = m_viewProbability * pzv / pz;
                    if ( pvz > 0.0f ) m_vomi[i] += pvz * MathTools::logTwo( pvz / m_viewProbability );
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            const QVector<float> &m_voxelProbabilitiesInView;
            QVector<float> &m_vomi;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_vomi.resize( nVoxels );
    m_vomi.fill( 0.0f );
    QVector<float> voxelProbabilitiesInView( nVoxels ); // vector p(Z|v)

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    VomiThread **vomiThreads = new VomiThread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for ( int k = 0; k < nThreads; k++ )
    {
        vomiThreads[k] = new VomiThread( m_voxelProbabilities, voxelProbabilitiesInView, m_vomi, start, end );
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
            vomiThreads[k]->start();
        }

        for ( int k = 0; k < nThreads; k++ ) vomiThreads[k]->wait();

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for ( int k = 0; k < nThreads; k++ ) delete vomiThreads[k];
    delete[] vomiThreads;

    m_maximumVomi = 0.0f;

    for ( int j = 0; j < nVoxels; j++ )
    {
        float vomi = m_vomi.at( j );
        Q_ASSERT( vomi == vomi );
        Q_ASSERT( vomi >= 0.0f );
        if ( vomi > m_maximumVomi ) m_maximumVomi = vomi;
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
