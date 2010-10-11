#include "viewpointintensityinformationchannel.h"


#ifdef CUDA_AVAILABLE


#include <QCoreApplication>
#include <QSet>
#include <QThread>

#include "qexperimental3dviewer.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"

#include "../gputesting/camera.h"
#include "cudaviewpointintensityinformationchannel.h"
#include "../gputesting/matrix4.h"


namespace udg {


ViewpointIntensityInformationChannel::ViewpointIntensityInformationChannel(const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer,
                                                                           const TransferFunction &transferFunction)
    : QObject(), m_viewpointGenerator(viewpointGenerator), m_volume(volume), m_viewer(viewer), m_transferFunction(transferFunction)
{
    m_backgroundColor = m_viewer->getBackgroundColor();
    m_viewpoints = m_viewpointGenerator.viewpoints();
}

void ViewpointIntensityInformationChannel::setIntensityClusteringNumberOfClusters(int numberOfClusters)
{
    m_numberOfIntensityClusters = numberOfClusters;
}


void ViewpointIntensityInformationChannel::filterViewpoints( const QVector<bool> &filter )
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


void ViewpointIntensityInformationChannel::compute(bool &viewpointEntropy, bool &entropy, bool &jHVI, bool &vmii, bool &mii, bool &viewpointUnstabilities, bool &imi, bool &intensityClustering, /*bool &viewpointVomi,
                                                   bool &colorVomi, bool &evmiOpacity, bool &evmiVomi, bool &bestViews, bool &guidedTour, bool &exploratoryTour,*/ bool display)
{
    // Si no hi ha res a calcular marxem
    if (!viewpointEntropy && !entropy && !jHVI && !vmii && !mii && !viewpointUnstabilities && !imi && !intensityClustering /*&& !viewpointVomi && !colorVomi && !evmiOpacity && !evmiVomi && !bestViews && !guidedTour
        && !exploratoryTour*/) return;

    bool viewProbabilities = false;
    bool intensityProbabilities = false;

    // Dependències
    if ( entropy ) viewProbabilities = true;
    if ( entropy ) viewpointEntropy = true;
    if (jHVI) viewProbabilities = true;
//    if ( guidedTour ) bestViews = true;
//    if ( bestViews ) mi = true;
//    if ( exploratoryTour ) mi = true;
    if ( mii ) vmii = true;
    if ( vmii ) intensityProbabilities = true;
    if ( viewpointUnstabilities ) viewProbabilities = true;
//    if ( viewpointVomi ) vomi = true;
//    if ( evmiOpacity ) voxelProbabilities = true;
//    if ( evmiVomi ) vomi = true;
    if ( imi ) intensityProbabilities = true;
//    if ( colorVomi ) voxelProbabilities = true;
    if (intensityClustering) intensityProbabilities = true;
    if ( intensityProbabilities ) viewProbabilities = true;

    computeCuda(viewProbabilities, intensityProbabilities, viewpointEntropy, entropy, jHVI, vmii, mii, viewpointUnstabilities, imi, intensityClustering, /*viewpointVomi, colorVomi, evmiOpacity, evmiVomi, bestViews,
                guidedTour, exploratoryTour,*/ display);
}


bool ViewpointIntensityInformationChannel::hasViewedVolume() const
{
    return !m_viewedVolume.isEmpty();
}


const QVector<float>& ViewpointIntensityInformationChannel::viewedVolume() const
{
    return m_viewedVolume;
}


const QVector<float>& ViewpointIntensityInformationChannel::viewpointEntropy() const
{
    return m_viewpointEntropy;
}


float ViewpointIntensityInformationChannel::entropy() const
{
    return m_entropy;
}


float ViewpointIntensityInformationChannel::jHVI() const
{
    return m_jHVI;
}


const QVector<float>& ViewpointIntensityInformationChannel::vmii() const
{
    return m_vmii;
}


float ViewpointIntensityInformationChannel::mii() const
{
    return m_mii;
}


const QVector<float>& ViewpointIntensityInformationChannel::viewpointUnstabilities() const
{
    return m_viewpointUnstabilities;
}


const QVector<float>& ViewpointIntensityInformationChannel::imi() const
{
    return m_imi;
}


float ViewpointIntensityInformationChannel::maximumImi() const
{
    return m_maximumImi;
}


QList< QList<int> > ViewpointIntensityInformationChannel::intensityClusters() const
{
    return m_intensityClusters;
}


QVector<float> ViewpointIntensityInformationChannel::intensityProbabilitiesInView( int i )
{
    return intensityProbabilitiesInViewCuda( i );
}


Matrix4 ViewpointIntensityInformationChannel::viewMatrix( const Vector3 &viewpoint )
{
    Camera camera;
    camera.lookAt( viewpoint, Vector3(), ViewpointGenerator::up( viewpoint ) );
    return camera.getViewMatrix();
}


void ViewpointIntensityInformationChannel::computeCuda(bool computeViewProbabilities, bool computeIntensityProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeJHVI, bool computeVmii,
                                                       bool computeMii, bool computeViewpointUnstabilities, bool computeImi, bool computeIntensityClustering, /*bool computeViewpointVomi, bool computeColorVomi,
                                                       bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour,*/ bool display)
{
    DEBUG_LOG( "computeCuda" );

    // Inicialitzar progrés
    int nSteps = 0;
    if ( computeViewProbabilities ) nSteps++;   // p(V)
    if ( computeIntensityProbabilities ) nSteps++;  // p(I)
    if ( computeImi /*|| computeColorVomi*/ ) nSteps++; // IMI + color VoMI
    // viewpoint entropy + entropy + H(V,I) + VMIi + MIi + viewpoint unstabilities + viewpoint VoMI + EVMI with opacity + EVMI with VoMI
    if (computeViewpointEntropy || computeEntropy || computeJHVI || computeVmii || computeMii || computeViewpointUnstabilities /*|| computeViewpointVomi || computeEvmiOpacity || computeEvmiVomi*/) nSteps++;
//    if ( computeBestViews ) nSteps++;   // best views
//    if ( computeGuidedTour ) nSteps++;  // guided tour
//    if ( computeExploratoryTour ) nSteps++; // exploratory tour
    if (computeIntensityClustering) nSteps++;   // intensity clustering

    emit totalProgressMaximum( nSteps );
    int step = 0;
    emit totalProgress( step );

    // Inicialització de CUDA
    cviicSetupRayCast( m_volume->getImage(), m_transferFunction, 1024, 720, m_backgroundColor, display );
    if ( computeIntensityProbabilities ) cviicSetupIntensityProbabilities();

    // p(V)
    if ( computeViewProbabilities )
    {
        computeViewProbabilitiesCuda();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(I)
    if ( computeIntensityProbabilities )
    {
        computeIntensityProbabilitiesCuda();
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // IMI + color VoMI
    if ( computeImi /*|| computeColorVomi*/ )
    {
        computeImiCuda(/* computeImi, computeColorVomi */);
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // viewpoint entropy + entropy + H(V,I) + VMIi + MIi + viewpoint unstabilities + viewpoint VoMI + EVMI with opacity + EVMI with VoMI
    if (computeViewpointEntropy || computeEntropy || computeJHVI || computeVmii || computeMii || computeViewpointUnstabilities /*|| computeViewpointVomi || computeEvmiOpacity || computeEvmiVomi*/)
    {
        computeViewMeasuresCuda(computeViewpointEntropy, computeEntropy, computeJHVI, computeVmii, computeMii, computeViewpointUnstabilities/*, computeViewpointVomi, computeEvmiOpacity, computeEvmiVomi*/);
        emit totalProgress( ++step );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // intensity clustering
    if (computeIntensityClustering)
    {
        computeIntensityClusteringCuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

//    // best views
//    if ( computeBestViews )
//    {
//        this->computeBestViews();
//        emit totalProgress( ++step );
//        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
//    }
//
//    // guided tour
//    if ( computeGuidedTour )
//    {
//        this->computeGuidedTour();
//        emit totalProgress( ++step );
//        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
//    }
//
//    // exploratory tour
//    if ( computeExploratoryTour )
//    {
//        this->computeExploratoryTour();
//        emit totalProgress( ++step );
//        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
//    }

    // Finalització de CUDA
    if ( computeIntensityProbabilities ) cviicCleanupIntensityProbabilities();
    cviicCleanupRayCast();
}


QVector<float> ViewpointIntensityInformationChannel::intensityProbabilitiesInViewCuda( int i )
{
    QVector<float> pIv = cviicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );   // p(I|v) * viewedVolume
    double viewedVolume = 0.0;
    int nIntensities = m_volume->getRangeMax() + 1;
    for ( int j = 0; j < nIntensities; j++ ) viewedVolume += pIv.at( j );
    Q_ASSERT( viewedVolume == viewedVolume );
    for ( int j = 0; j < nIntensities; j++ ) pIv[j] /= viewedVolume;    // p(I|v)
    return pIv;
}


void ViewpointIntensityInformationChannel::computeViewProbabilitiesCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    m_viewProbabilities.resize( nViewpoints );
    double totalViewedVolume = 0.0;

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> histogram = cviicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) ); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nIntensities; j++ ) viewedVolume += histogram.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        m_viewProbabilities[i] = viewedVolume;
        totalViewedVolume += viewedVolume;

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_viewedVolume = m_viewProbabilities;

    for ( int i = 0; i < nViewpoints; i++ )
    {
        DEBUG_LOG( QString( "volume(v%1) = %2" ).arg( i + 1 ).arg( m_viewProbabilities.at( i ) ) );
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


void ViewpointIntensityInformationChannel::computeIntensityProbabilitiesCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> histogram = cviicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) ); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nIntensities; j++ ) viewedVolume += histogram.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        cviicAccumulateIntensityProbabilities( m_viewProbabilities.at( i ), viewedVolume );

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_intensityProbabilities = cviicGetIntensityProbabilities();

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for ( int j = 0; j < nIntensities; j++ )
    {
        float pi = m_intensityProbabilities.at( j );
        DEBUG_LOG(QString("p(i%1) = %2").arg(j).arg(pi));
        Q_ASSERT( pi == pi );
        Q_ASSERT( pi >= 0.0f && pi <= 1.0f );
        sum += pi;
    }
    DEBUG_LOG( QString( "sum p(i) = %1" ).arg( sum ) );
#endif
}


void ViewpointIntensityInformationChannel::computeViewMeasuresCuda(bool computeViewpointEntropy, bool computeEntropy, bool computeJHVI, bool computeVmii, bool computeMii, bool computeViewpointUnstabilities/*,
                                                                   bool computeViewpointVomi, bool computeEvmiOpacity, bool computeEvmiVomi*/)
{
    /*class ViewpointVomiThread : public QThread {
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
    };*/

    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

//    int nThreads = QThread::idealThreadCount();
//    ViewpointVomiThread **viewpointVomiThreads;

    if ( computeViewpointEntropy ) m_viewpointEntropy.resize( nViewpoints );
    if ( computeEntropy ) m_entropy = 0.0f;
    if (computeJHVI) m_jHVI = 0.0f;
    if ( computeVmii ) m_vmii.resize( nViewpoints );
    if ( computeMii ) m_mii = 0.0f;
    if ( computeViewpointUnstabilities ) m_viewpointUnstabilities.resize( nViewpoints );
//    if ( computeViewpointVomi )
//    {
//        m_viewpointVomi.resize( nViewpoints );
//        m_viewpointVomi.fill( 0.0f );
//        viewpointVomiThreads = new ViewpointVomiThread*[nThreads];
//        int nVoxelsPerThread = nVoxels / nThreads + 1;
//        int start = 0, end = nVoxelsPerThread;
//        for ( int k = 0; k < nThreads; k++ )
//        {
//            viewpointVomiThreads[k] = new ViewpointVomiThread( m_voxelProbabilities, m_vomi, start, end );
//            start += nVoxelsPerThread;
//            end += nVoxelsPerThread;
//            if ( end > nVoxels ) end = nVoxels;
//        }
//    }
//    QVector<float> ppZOpacity;  // p'(Z) (opacitat)
//    if ( computeEvmiOpacity )
//    {
//        m_evmiOpacity.resize( nViewpoints );
//        ppZOpacity.resize( nVoxels );
//        double total = 0.0;
//        unsigned short *voxels = reinterpret_cast<unsigned short*>( m_volume->getImage()->GetScalarPointer() );
//        for ( int j = 0; j < nVoxels; j++ )
//        {
//            ppZOpacity[j] = m_voxelProbabilities.at( j ) * m_evmiOpacityTransferFunction.getOpacity( voxels[j] );
//            total += ppZOpacity.at( j );
//        }
//        for ( int j = 0; j < nVoxels; j++ ) ppZOpacity[j] /= total;
//    }
//    QVector<float> ppZVomi; // p'(Z) (VoMI)
//    if ( computeEvmiVomi )
//    {
//        m_evmiVomi.resize( nViewpoints );
//        ppZVomi.resize( nVoxels );
//        double total = 0.0;
//        for ( int j = 0; j < nVoxels; j++ )
//        {
//            ppZVomi[j] = m_voxelProbabilities.at( j ) * m_vomi.at( j );
//            total += ppZVomi.at( j );
//        }
//        for ( int j = 0; j < nVoxels; j++ ) ppZVomi[j] /= total;
//    }

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for ( int i = 0; i < nViewpoints; i++ )
    {
        QVector<float> intensityProbabilitiesInView = this->intensityProbabilitiesInView( i );  // p(I|v)

        if ( computeViewpointEntropy )
        {
            float viewpointEntropy = InformationTheory::entropy( intensityProbabilitiesInView );
            Q_ASSERT( viewpointEntropy == viewpointEntropy );
            m_viewpointEntropy[i] = viewpointEntropy;
            DEBUG_LOG( QString( "H(I|v%1) = %2" ).arg( i + 1 ).arg( viewpointEntropy ) );
        }

        if ( computeEntropy )
        {
            m_entropy += m_viewProbabilities.at( i ) * m_viewpointEntropy.at( i );
        }

        if (computeJHVI)
        {
            QVector<float> jpIv(intensityProbabilitiesInView);  // p(I,v) <- p(I|v)
            for (int j = 0; j < nIntensities; j++) jpIv[j] *= m_viewProbabilities.at(i);    // p(I,v) = p(I|v) * p(v)
            m_jHVI += InformationTheory::entropy(jpIv);
        }

        if ( computeVmii )
        {
            float vmii = InformationTheory::kullbackLeiblerDivergence( intensityProbabilitiesInView, m_intensityProbabilities );
            Q_ASSERT( vmii == vmii );
            m_vmii[i] = vmii;
            DEBUG_LOG( QString( "VMIi(v%1) = %2" ).arg( i + 1 ).arg( vmii ) );
        }

        if ( computeMii )
        {
            m_mii += m_viewProbabilities.at( i ) * m_vmii.at( i );
        }

        if ( computeViewpointUnstabilities )
        {
            float pvi = m_viewProbabilities.at( i );    // p(vi)

            QVector<int> neighbours = m_viewpointGenerator.neighbours( i );
            int nNeighbours = neighbours.size();
            double viewpointUnstability = 0.0;

            for ( int ij = 0; ij < nNeighbours; ij++ )
            {
                int neighbour = neighbours.at( ij );
                float pvj = m_viewProbabilities.at( neighbour );    // p(vj)
                float pvij = pvi + pvj; // p(v̂)

                if ( pvij == 0.0f ) continue;

                QVector<float> intensityProbabilitiesInNeighbour = this->intensityProbabilitiesInView( neighbour ); // p(I|vj)

                float viewpointDissimilarity = InformationTheory::jensenShannonDivergence( pvi / pvij, pvj / pvij, intensityProbabilitiesInView, intensityProbabilitiesInNeighbour );
                viewpointUnstability += viewpointDissimilarity;
            }

            viewpointUnstability /= nNeighbours;
            m_viewpointUnstabilities[i] = viewpointUnstability;
            DEBUG_LOG( QString( "U(v%1) = %2" ).arg( i + 1 ).arg( viewpointUnstability ) );
        }

//        if ( computeViewpointVomi )
//        {
//            float pv = m_viewProbabilities.at( i );
//
//            if ( pv > 0.0f )
//            {
//                for ( int k = 0; k < nThreads; k++ )
//                {
//                    viewpointVomiThreads[k]->setViewData( pv, voxelProbabilitiesInView );
//                    viewpointVomiThreads[k]->start();
//                }
//
//                double viewpointVomi = 0.0;
//
//                for ( int k = 0; k < nThreads; k++ )
//                {
//                    viewpointVomiThreads[k]->wait();
//                    viewpointVomi += viewpointVomiThreads[k]->viewpointVomi();
//                }
//
//                Q_ASSERT( viewpointVomi == viewpointVomi );
//                m_viewpointVomi[i] = viewpointVomi;
//                DEBUG_LOG( QString( "VVoMI(v%1) = %2" ).arg( i + 1 ).arg( viewpointVomi ) );
//            }
//        }

//        if ( computeEvmiOpacity )
//        {
//            float evmiOpacity = InformationTheory::kullbackLeiblerDivergence( voxelProbabilitiesInView, ppZOpacity, true );
//            Q_ASSERT( evmiOpacity == evmiOpacity );
//            m_evmiOpacity[i] = evmiOpacity;
//            DEBUG_LOG( QString( "EVMI_O(v%1) = %2" ).arg( i + 1 ).arg( evmiOpacity ) );
//        }

//        if ( computeEvmiVomi )
//        {
//            float evmiVomi = InformationTheory::kullbackLeiblerDivergence( voxelProbabilitiesInView, ppZVomi, true );
//            Q_ASSERT( evmiVomi == evmiVomi );
//            m_evmiVomi[i] = evmiVomi;
//            DEBUG_LOG( QString( "EVMI_V(v%1) = %2" ).arg( i + 1 ).arg( evmiVomi ) );
//        }

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if ( computeEntropy )
    {
        DEBUG_LOG( QString( "H(I) = %1" ).arg( m_entropy ) );
    }

    if (computeJHVI)
    {
        Q_ASSERT(!MathTools::isNaN(m_jHVI));
        Q_ASSERT(m_jHVI >= 0.0);
        DEBUG_LOG(QString("H(V,I) = %1").arg(m_jHVI));
    }

    if ( computeMii )
    {
        DEBUG_LOG( QString( "I(V;I) = %1" ).arg( m_mii ) );
    }

//    if ( computeViewpointVomi )
//    {
//        for ( int k = 0; k < nThreads; k++ ) delete viewpointVomiThreads[k];
//        delete[] viewpointVomiThreads;
//    }
}


void ViewpointIntensityInformationChannel::computeImiCuda(/* bool computeVomi, bool computeColorVomi */)
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    cviicSetupImi(/* computeVomi, computeColorVomi */);

    for ( int i = 0; i < nViewpoints; i++ )
    {
        float pv = m_viewProbabilities.at( i );
        if ( pv == 0.0f ) continue;

        QVector<float> histogram = cviicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) ); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for ( int j = 0; j < nIntensities; j++ ) viewedVolume += histogram.at( j );
        Q_ASSERT( viewedVolume == viewedVolume );

        /*if ( computeVomi )*/ cviicAccumulateImi( pv, viewedVolume );
        //if ( computeColorVomi ) cvicAccumulateColorVomi( pv, m_viewpointColors.at( i ), viewedVolume );

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    //if ( computeVomi )
    {
        m_imi = cviicGetImi();
        m_maximumImi = 0.0f;
    }

//    if ( computeColorVomi )
//    {
//        m_colorVomi = cvicGetColorVomi();
//        m_maximumColorVomi = 0.0f;
//    }

    cviicCleanupImi();

    for ( int j = 0; j < nIntensities; j++ )
    {
        //if ( computeVomi )
        {
            float imi = m_imi.at( j );
            Q_ASSERT( imi == imi );
            Q_ASSERT( imi >= 0.0f );
            if ( imi > m_maximumImi ) m_maximumImi = imi;
        }

//        if ( computeColorVomi )
//        {
//            Vector3Float colorVomi = m_colorVomi.at( j );
//            Q_ASSERT( colorVomi.x == colorVomi.x && colorVomi.y == colorVomi.y && colorVomi.z == colorVomi.z );
//            /// \todo pot ser < 0???
//            //Q_ASSERT( colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f );
//            //Q_ASSERT_X( colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f, "comprovació color vomi", qPrintable( colorVomi.toString() ) );
//            if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
//            if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
//            if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
//        }
    }
}


void ViewpointIntensityInformationChannel::computeIntensityClusteringCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    // Construïm el vector de probabilitats dels clusters p(C): inicialment és igual a p(I)
    QList<float> clusterProbabilities = QList<float>::fromVector(m_intensityProbabilities);    // p(C)

    // Construïm la matriu p(V|C), on cada fila sigui una p(V|c): inicialment és p(V|I), on cada fila és una p(V|i)
    QList< QVector<float> > pVC;
    for (int j = 0; j < nIntensities; j++) pVC << QVector<float>(nViewpoints);  // tota la matriu queda plena de zeros inicialment
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities[i];  // p(v)
        if (pv == 0.0f) continue;
        QVector<float> pIv = intensityProbabilitiesInView(i);   // p(I|v)
        for (int j = 0; j < nIntensities; j++)
        {
            float piv = pIv[j]; // p(i|v)
            if (piv == 0.0f) continue;
            float pi = m_intensityProbabilities[j]; // p(i)
            if (pi == 0.0f) continue;
            float pvi = pv * piv / pi;
            Q_ASSERT(!MathTools::isNaN(pvi));
            pVC[j][i] = pvi;
        }
    }

    // Construïm el clustering inicial: un cluster per cada intensitat.
    QList< QList<int> > clusters;   // cada element és un cluster (un cluster està representat per la llista d'intensitats que el formen)
    for (int j = 0; j < nIntensities; j++) clusters << (QList<int>() << j); // clusters = [[0], [1], ..., [nIntensities-1]]

    // Calculem la pèrdua d'informació mútua per cada possible clustering
    QList<double> mutualInformationDecrease;    // cada element és la pèrdua d'informació mútua resultat de fusionar l'element de la mateixa posició de clusters amb el següent
    for (int j = 0; j < nIntensities - 1; j++)
    {
        float pci = clusterProbabilities[j];
        float pcj = clusterProbabilities[j+1];
        float pcij = pci + pcj;
        mutualInformationDecrease << (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[j], pVC[j+1]));
    }

    while (clusters.size() > m_numberOfIntensityClusters)
    {
        // Trobem la pèrdua mínima d'informació mútua
        int minDecreaseIndex = 0;
        double minDecrease = mutualInformationDecrease[0];
        for (int k = 1; k < mutualInformationDecrease.size(); k++)
        {
            double decrease = mutualInformationDecrease[k];
            if (decrease < minDecrease)
            {
                minDecreaseIndex = k;
                minDecrease = decrease;
            }
        }

        // Fusionem les dues intensitats amb pèrdua mínima d'informació mútua i actualitzem les dades
        clusters[minDecreaseIndex] << clusters[minDecreaseIndex+1]; // integrem el segon cluster c2 al primer c1
        clusters.removeAt(minDecreaseIndex+1);  // esborrem el segon cluster c2
        // integrem p(V|c2) a p(V|c1)
        {
            float pc1 = clusterProbabilities[minDecreaseIndex];     // p(c1)
            float pc2 = clusterProbabilities[minDecreaseIndex+1];   // p(c2)
            float pc12 = pc1 + pc2;                                 // p(c12)
            if (pc12 > 0.0f)    // si p(c12) = 0, llavors p(V|c1) i p(V|c2) estan plens de zeros, per tant no cal fer res
            {
                QVector<float> &pVc1 = pVC[minDecreaseIndex];           // p(V|c1)
                const QVector<float> &pVc2 = pVC[minDecreaseIndex+1];   // p(V|c2)
                for (int i = 0; i < nViewpoints; i++)
                {
                    float pvc1 = pVc1[i];   // p(v|c1)
                    float pvc2 = pVc2[i];   // p(v|c2)
                    pVc1[i] = (pc1 * pvc1 + pc2 * pvc2) / pc12;
                }
            }
        }
        pVC.removeAt(minDecreaseIndex+1);   // esborrem p(V|c2)
        clusterProbabilities[minDecreaseIndex] += clusterProbabilities[minDecreaseIndex+1]; // integrem p(c2) a p(c1)
        clusterProbabilities.removeAt(minDecreaseIndex+1);  // esborrem p(c2)
        mutualInformationDecrease.removeAt(minDecreaseIndex);   // esborrem la pèrdua d'informació mútua associada a la fusió
        // actualitzem les pèrdues d'informació pertinents
        {
            if (minDecreaseIndex > 0)   // anterior
            {
                int i = minDecreaseIndex - 1;
                int j = minDecreaseIndex;
                float pci = clusterProbabilities[i];
                float pcj = clusterProbabilities[j];
                float pcij = pci + pcj;
                mutualInformationDecrease[i] = (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[i], pVC[j]));
            }
            if (minDecreaseIndex < mutualInformationDecrease.size())    // següent
            {
                int i = minDecreaseIndex;
                int j = minDecreaseIndex + 1;
                float pci = clusterProbabilities[i];
                float pcj = clusterProbabilities[j];
                float pcij = pci + pcj;
                mutualInformationDecrease[i] = (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[i], pVC[j]));
            }
        }

#ifndef QT_NO_DEBUG
        if (clusters.size() % 10 == m_numberOfIntensityClusters % 10)
        {
            QString clustersString = "[";
            for (int i = 0; i < clusters.size(); i++)
            {
                if (i > 0) clustersString += ", ";
                clustersString += "[";
                clustersString += QString::number(clusters[i].first());
                if (clusters[i].size() > 1) clustersString += "-" + QString::number(clusters[i].last());
                clustersString += "]";
            }
            clustersString += "]";
            DEBUG_LOG(clustersString);
        }
#endif
    }

    m_intensityClusters = clusters;
}


} // namespace udg


#endif // CUDA_AVAILABLE
