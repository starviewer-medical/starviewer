#include "viewpointinformationchannel.h"

#include <QCoreApplication>

// només per cuda? llavors el mètode viewmatrix hauria de ser només per cuda també
#include "../gputesting/camera.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"
// només per cuda? llavors el mètode viewmatrix hauria de ser només per cuda també
#include "../gputesting/matrix4.h"

#ifdef CUDA_AVAILABLE
#include "cudaviewpointinformationchannel.h"
#endif


namespace udg {


ViewpointInformationChannel::ViewpointInformationChannel( const ViewpointGenerator &viewpointGenerator, const Experimental3DVolume *volume, const TransferFunction &transferFunction, const QColor &backgroundColor )
        : m_viewpointGenerator( viewpointGenerator ), m_volume( volume ), m_transferFunction( transferFunction ), m_backgroundColor( backgroundColor )
{
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


void ViewpointInformationChannel::compute( bool &viewpointEntropy, bool &entropy, bool &vmi, bool &mi )
{
    // Si no hi ha res a calcular marxem
    if ( !viewpointEntropy && !entropy && !vmi && !mi ) return;

    bool viewProbabilities = false;
    bool voxelProbabilities = false;

    // Dependències
    if ( entropy ) viewProbabilities = true;
    if ( entropy ) viewpointEntropy = true;
    if ( mi ) vmi = true;
    if ( vmi ) voxelProbabilities = true;
    if ( voxelProbabilities ) viewProbabilities = true;

#ifndef CUDA_AVAILABLE
    computeCpu( viewpointEntropy );
#else // CUDA_AVAILABLE
    computeCuda( viewProbabilities, voxelProbabilities, viewpointEntropy, entropy, vmi, mi );
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


Matrix4 ViewpointInformationChannel::viewMatrix( const Vector3 &viewpoint )
{
    Camera camera;
    camera.lookAt( viewpoint, Vector3(), ViewpointGenerator::up( viewpoint ) );
    return camera.getViewMatrix();
}


#ifndef CUDA_AVAILABLE


void ViewpointInformationChannel::computeCpu( bool computeViewpointEntropy )
{
    DEBUG_LOG( "computeCpu" );

    // Inicialitzar progrés
    int nSteps = 3; // ray casting (p(O|V)), p(V), p(O)
    if ( computeViewpointEntropy ) nSteps++;    // viewpoint entropy

    emit totalProgressMaximum( nSteps );
    emit totalProgress( 0 );
}


#else // CUDA_AVAILABLE


void ViewpointInformationChannel::computeCuda( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi )
{
    DEBUG_LOG( "computeCuda" );

    // Inicialitzar progrés
    int nSteps = 0;
    if ( computeViewProbabilities ) nSteps++;   // p(V)
    if ( computeVoxelProbabilities ) nSteps++;  // p(Z)
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi ) nSteps++;   // viewpoint entropy + entropy + VMI + MI

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

    // viewpoint entropy + entropy + VMI + MI
    if ( computeViewpointEntropy || computeEntropy || computeVmi || computeMi )
    {
        computeViewMeasuresCuda( computeViewpointEntropy, computeEntropy, computeVmi, computeMi );
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


void ViewpointInformationChannel::computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi )
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    if ( computeViewpointEntropy ) m_viewpointEntropy.resize( nViewpoints );
    if ( computeEntropy ) m_entropy = 0.0f;
    if ( computeVmi ) m_vmi.resize( nViewpoints );
    if ( computeMi ) m_mi = 0.0f;

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
}


#endif // CUDA_AVAILABLE


} // namespace udg
