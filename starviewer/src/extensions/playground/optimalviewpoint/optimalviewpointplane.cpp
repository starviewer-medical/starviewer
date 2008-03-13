/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointplane.h"

#include <QDir>
#include <QMessageBox>

#include <QVTKWidget.h>

#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleUser.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
#include <vtkMultiThreader.h>

#include "optimalviewpointplanehelper.h"
#include "histogram.h"
#include "optimalviewpointvolume.h"
#include "slicer.h"
#include <vtkPointData.h>
#include "logging.h"

namespace udg {

OptimalViewpointPlane::OptimalViewpointPlane( unsigned short id, unsigned short size )
{
    m_id = id;
    m_size = size;
    m_renderer = vtkRenderer::New();

    m_renderer->SetBackground( 1.0, 1.0, 1.0 );

    m_camera = vtkCamera::New();
    m_camera->ParallelProjectionOn();
    m_camera->SetParallelScale( m_size / 2.0 );
    m_renderer->SetActiveCamera( m_camera );
    m_plane = vtkImageActor::New();
    m_plane->PickableOff();
    m_plane->SetOrigin( (m_size-1)/2.0, (m_size-1)/2.0, 0.0 );
    m_window = new QVTKWidget( 0, Qt::Window );
    m_window->GetRenderWindow()->AddRenderer( m_renderer );
    vtkInteractorStyleUser * style = vtkInteractorStyleUser::New();
    m_window->GetInteractor()->SetInteractorStyle( style );
    style->Delete();
    m_window->setWindowTitle( QString( tr("Plane %1") ).arg( m_id ) );
    m_latitude = 0.0;
    m_longitude = 0.0;

    this->setDistance( 1.0 );

    m_helper = OptimalViewpointPlaneHelper::New();
    m_helper->setOptimalViewpointPlane( this );
    m_renderer->AddObserver( vtkCommand::StartEvent, m_helper );
    m_renderer->AddObserver( vtkCommand::EndEvent, m_helper );

    m_compute = false;
    m_computing = false;

    m_hx = 0.0;
    m_E = 0.0;

    m_numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();


    /// \todo fer la nova implementació multithread
    m_numberOfThreads = 1;
}

OptimalViewpointPlane::~OptimalViewpointPlane()
{
    m_window->close();
    delete m_window;
}

vtkImageActor * OptimalViewpointPlane::getPlane() const
{
    return m_plane;
}

vtkRenderer * OptimalViewpointPlane::getRenderer() const
{
    return m_renderer;
}

void OptimalViewpointPlane::setDistance( double distance )
{
    double latitude = m_latitude;
    double longitude = m_longitude;

    this->setLatitude( 0.0 );
    this->setLongitude( 0.0 );

    m_camera->SetPosition( 0.0, 0.0, distance );

    this->setLatitude( latitude );
    this->setLongitude( longitude );

    double dNear = ( distance > m_size ) ? ( distance - m_size ) : 0.1;
    double dFar = distance + m_size;
    m_camera->SetClippingRange( dNear, dFar );
}

void OptimalViewpointPlane::setLatitude( double latitude )
{
    double longitude = m_longitude;
    this->setLongitude( 0.0 );

    m_camera->Elevation( latitude - m_latitude );
    m_plane->SetPosition( -((m_size-1)/2.0), -((m_size-1)/2.0), 0.0 );
    m_plane->AddPosition( m_camera->GetPosition() );
    m_plane->RotateWXYZ( latitude - m_latitude, -1.0, 0.0, 0.0 );
    m_latitude = latitude;

    this->setLongitude( longitude );
}

void OptimalViewpointPlane::setLongitude( double longitude )
{
    m_camera->Azimuth( longitude - m_longitude );
    m_plane->SetPosition( -((m_size-1)/2.0), -((m_size-1)/2.0), 0.0 );
    m_plane->AddPosition( m_camera->GetPosition() );
    m_plane->RotateWXYZ( longitude - m_longitude, 0.0, 1.0, 0.0 );
    m_longitude = longitude;
}

void OptimalViewpointPlane::update( bool render )
{
    std::cout << "OVP" << m_id << "::update(): start" << std::endl;

    m_window->setMinimumSize( m_size, m_size );
    m_window->setMaximumSize( m_size, m_size );
    //m_window->resize( m_size, m_size );




    if ( render )
    {
        m_window->show();
        m_window->raise();

        vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();
        windowToImageFilter->SetInput( m_window->GetRenderWindow() );
        //windowToImageFilter->Update();

        vtkPNGWriter * pngWriter = vtkPNGWriter::New();
        pngWriter->SetInput( windowToImageFilter->GetOutput() );
        pngWriter->SetFileName( qPrintable( QDir::tempPath().append( "/plane%1.png" ).arg( m_id ) ) );
        pngWriter->Write();

        windowToImageFilter->Delete();
        pngWriter->Delete();

        vtkPNGReader * pngReader = vtkPNGReader::New();
        pngReader->SetFileName( qPrintable( QDir::tempPath().append( "/plane%1.png" ).arg( m_id ) ) );
        //pngReader->Update();
        m_plane->SetInput( pngReader->GetOutput() );
        pngReader->Delete();
    }

//     std::cout << "OVP" << m_id << "::update(): end" << std::endl;


    DEBUG_LOG( "start entropy computing" );
    startEntropyComputing();
    DEBUG_LOG( QString("cast rays (%1)").arg(m_id) );
    castRays();
    DEBUG_LOG( "end entropy computing" );
    endEntropyComputing();



}

void OptimalViewpointPlane::hide()
{
    m_window->hide();
}

void OptimalViewpointPlane::setEntropyL( unsigned char L )
{
    m_L = L;
}

void OptimalViewpointPlane::setEntropyN( unsigned char N )
{
    m_N = N;
}

void OptimalViewpointPlane::startEntropyComputing()
{
    DEBUG_LOG( QString( "m_compute = %1" ).arg( m_compute ) );
    if ( m_compute )
    {
        DEBUG_LOG( QString( "H L-1 size; m_N = %1, m_L = %2" ).arg( m_N ).arg( m_L ) );
        m_histogramL_1Size = static_cast< unsigned short >( pow( (double)m_N, m_L - 1 ) );
//         m_histogramL_1 = new std::vector<unsigned long>( m_histogramL_1Size );
//         m_countL_1 = 0;
        DEBUG_LOG( QString( "H L size" ) );
        m_histogramLSize = static_cast< unsigned short >( pow( (double)m_N, m_L ) );
//         m_histogramL = new std::vector<unsigned long>( m_histogramLSize );
//         m_countL = 0;
//         m_lastLValues = new std::deque<unsigned char>();
//         m_lastLValuesPerRay = new QHash< int, std::deque< unsigned char > >();
        m_computing = true;

        if ( !m_lastLValuesPerThread.isEmpty() )
        {
            QMessageBox::warning( 0, "problema amb els raigs", "last values per thread no buit" );
        }

        DEBUG_LOG( "creem l'slicer" );
        Slicer slicer( m_id );
        DEBUG_LOG( "slicer.setInput( m_volume->getLabeledImage() );" );
        slicer.setInput( m_volume->getLabeledImage() );
        DEBUG_LOG( "slicer.setMatrix" );
        slicer.setMatrix( getTransformMatrix() );
        slicer.setSpacing( m_volume->getImageSampleDistance(), m_volume->getImageSampleDistance(), m_volume->getSampleDistance() );
        DEBUG_LOG( "slicer.reslice" );
        slicer.reslice( false, false );
        DEBUG_LOG( "getReslicedImage" );
        m_planeImage = slicer.getReslicedImage(); m_planeImage->Register( 0 );
        m_planeData = reinterpret_cast< unsigned char * >( m_planeImage->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
        m_planeDataSize = m_planeImage->GetPointData()->GetScalars()->GetSize();
        int dimensions[3];
        m_planeImage->GetDimensions( dimensions );
        m_numberOfRays = dimensions[0] * dimensions[1];
        m_rayLength = dimensions[2];
    }
}

void OptimalViewpointPlane::endEntropyComputing()
{
    if ( m_compute )
    {
        std::cout << "OVP" << m_id << "::endEntropyComputing(): start" << std::endl;
        if ( m_lastLValuesPerThread.size() != m_numberOfThreads )
            QMessageBox::warning( 0, "problema amb els raigs", "problema de concurrència" );
        QHashIterator< int, std::deque< unsigned char > > it( m_lastLValuesPerThread );
        while ( it.hasNext() )
            if ( !it.next().value().empty() )
                QMessageBox::warning( 0, "problema amb els raigs", "queda alguna deque no buida" );

        m_compute = false;
        m_computing = false;

        Histogram histogramL_1;
        QHashIterator< int, Histogram > itHistogramL_1PerThread( m_histogramL_1PerThread );
        histogramL_1 = itHistogramL_1PerThread.next().value();
        while ( itHistogramL_1PerThread.hasNext() )
        {
            histogramL_1.combineWith( itHistogramL_1PerThread.next().value() );
        }
        double H_XL_1_ = 0.0;
        double countL_1 = histogramL_1.count();
        QVectorIterator<quint64> * itHistogramL_1 = histogramL_1.getIterator();
        while ( itHistogramL_1->hasNext() )
        {
            double p_XL_1_ = itHistogramL_1->next() / countL_1;
            if ( p_XL_1_ > 0.0 ) H_XL_1_ -= p_XL_1_ * log( p_XL_1_ );
        }
        H_XL_1_ /= log( 2.0 );
        delete itHistogramL_1;

        Histogram histogramL;
        QHashIterator< int, Histogram > itHistogramLPerThread( m_histogramLPerThread );
        histogramL = itHistogramLPerThread.next().value();
        while ( itHistogramLPerThread.hasNext() )
        {
            histogramL.combineWith( itHistogramLPerThread.next().value() );
        }
        double H_XL_ = 0.0;
        double countL = histogramL.count();
        QVectorIterator<quint64> * itHistogramL = histogramL.getIterator();
        while ( itHistogramL->hasNext() )
        {
            double p_XL_ = itHistogramL->next() / countL;
            if ( p_XL_ > 0.0 ) H_XL_ -= p_XL_ * log( p_XL_ );
        }
        H_XL_ /= log( 2.0 );
        delete itHistogramL;

        m_hx = H_XL_ - H_XL_1_;
        m_E = H_XL_ - m_hx * m_L;

//         delete m_histogramL_1;
        m_histogramL_1PerThread.clear();
//         delete m_histogramL;
        m_histogramLPerThread.clear();
//         delete m_lastLValues;
        m_lastLValuesPerThread.clear();

        std::cout << "Plane " << m_id << ":" << std::endl;
        std::cout << "Entropy Rate = " << m_hx << std::endl;
        std::cout << "Excess Entropy = " << m_E << std::endl;

        emit excessEntropyComputed( m_E );

        // per comptar els canvis //////////////////////////////////////////////
        m_maxNumberOfMaterials = 0;
        m_maxMaterials.clear();
        QHashIterator< int, unsigned short > itMaxNumberOfMaterialsPerThread( m_maxNumberOfMaterialsPerThread );
        while ( itMaxNumberOfMaterialsPerThread.hasNext() )
        {
            itMaxNumberOfMaterialsPerThread.next();
            if ( itMaxNumberOfMaterialsPerThread.value() > m_maxNumberOfMaterials )
            {
                m_maxNumberOfMaterials = itMaxNumberOfMaterialsPerThread.value();
                m_maxMaterials = m_maxMaterialsPerThread[itMaxNumberOfMaterialsPerThread.key()];
            }
        }
        m_lastValuePerThread.clear();
        m_currentNumberOfMaterialsPerThread.clear();
        m_currentMaterialsPerThread.clear();
        m_maxNumberOfMaterialsPerThread.clear();
        m_maxMaterialsPerThread.clear();

        std::cout << "max number of materials: " << m_maxNumberOfMaterials << std::endl;
        std::cout << "max materials:" << std::endl;
        for ( unsigned short i = 0; i < m_maxMaterials.size(); i += 2 )
        {
            std::cout << m_maxMaterials[i] << "(" << m_maxMaterials[i+1] << ") ";
        }
        std::cout << std::endl;
        // per comptar els canvis //////////////////////////////////////////////


        m_planeImage->Delete();
    }
}

void OptimalViewpointPlane::setToRecompute()
{
    m_compute = true;
    emit goingToRecompute();
}

double OptimalViewpointPlane::getEntropyRate() const
{
    return m_hx;
}

double OptimalViewpointPlane::getExcessEntropy() const
{
    return m_E;
}

void OptimalViewpointPlane::updateAndRecompute()
{
    setToRecompute();
    update();
}

void OptimalViewpointPlane::compute( int threadId, unsigned char value )
{
    if ( m_compute && m_computing )
    {
        if ( !m_lastLValuesPerThread.contains( threadId ) )
        {
            m_mutex.lock();
            m_lastLValuesPerThread[threadId];
            m_histogramL_1PerThread[threadId] = Histogram( m_histogramL_1Size );
            m_histogramLPerThread[threadId] = Histogram( m_histogramLSize );

            // per comptar els canvis //////////////////////////////////////////
            m_lastValuePerThread[threadId] = m_N;   // m_N és més gran que qualsevol valor que pugui arribar
            m_currentNumberOfMaterialsPerThread[threadId] = 0;
            m_currentMaterialsPerThread[threadId];
            m_maxNumberOfMaterialsPerThread[threadId] = 0;
            m_maxMaterialsPerThread[threadId];
            // per comptar els canvis //////////////////////////////////////////

            m_mutex.unlock();
        }

        std::deque< unsigned char > & lastLValues = m_lastLValuesPerThread[threadId];

        lastLValues.push_front( value );
        unsigned char L_1 = m_L - 1;

        if ( lastLValues.size() >= L_1 )
        {
            unsigned short i = lastLValues[0];
            unsigned short j, multiplier;

            for ( j = 1, multiplier = m_N; j < L_1; j++, multiplier *= m_N )
                i += lastLValues[j] * multiplier;

            Q_ASSERT_X( i < m_histogramL_1Size, "compute", qPrintable(QString("i = %1, value = %2").arg(i).arg(value)) );
            m_histogramL_1PerThread[threadId].add( i );

            if ( lastLValues.size() == m_L )
            {
                i += lastLValues[j] * multiplier;
                m_histogramLPerThread[threadId].add( i );
                lastLValues.pop_back();
            }
        }

        // per comptar els canvis //////////////////////////////////////////////
        if ( value != m_lastValuePerThread[threadId] )
        {
            m_currentNumberOfMaterialsPerThread[threadId]++;
            m_currentMaterialsPerThread[threadId] << value << 1;
        }
        else
        {
            m_currentMaterialsPerThread[threadId].last()++;
        }
        m_lastValuePerThread[threadId] = value;
        // per comptar els canvis //////////////////////////////////////////////
    }
}

void OptimalViewpointPlane::endLBlock( int threadId )
{
    if ( m_compute && m_computing )
    {
//         m_lastLValues->clear();
        if ( m_lastLValuesPerThread.contains( threadId ) )
        {
            m_lastLValuesPerThread[threadId].clear();

            // per comptar els canvis //////////////////////////////////////////
            m_lastValuePerThread[threadId] = m_N;   // m_N és més gran que qualsevol valor que pugui arribar
            if ( m_currentNumberOfMaterialsPerThread[threadId] > m_maxNumberOfMaterialsPerThread[threadId] )
            {
                m_maxNumberOfMaterialsPerThread[threadId] = m_currentNumberOfMaterialsPerThread[threadId];
                m_maxMaterialsPerThread[threadId] = m_currentMaterialsPerThread[threadId];
            }
            m_currentNumberOfMaterialsPerThread[threadId] = 0;
            m_currentMaterialsPerThread[threadId].clear();
            // per comptar els canvis //////////////////////////////////////////
        }
    }
}



void OptimalViewpointPlane::setNumberOfThreads( unsigned char numberOfThreads )
{
    m_numberOfThreads = numberOfThreads;
}



vtkMatrix4x4 * OptimalViewpointPlane::getTransformMatrix()
{
    return m_plane->GetMatrix();
}


void OptimalViewpointPlane::setVolume( OptimalViewpointVolume * volume )
{
    m_volume = volume;
}


void OptimalViewpointPlane::castRays()
{
    if ( m_compute && m_computing )
    {
        for ( unsigned int i = 0; i < m_numberOfRays; i++ ) // iterate over rays
        {
//             DEBUG_LOG( QString("i = %1").arg(i) );
            /// \todo Per fer-ho bé, seguint el sentit dels raigs, hauria de ser al revés, del final cap al principi
            /// (perquè la Z+ és cap enfora)
            for ( unsigned int j = 0; j < m_rayLength; j++ )    // iterate over current ray
            {
//                 DEBUG_LOG( QString("j = %1").arg(j) );
                Q_ASSERT_X( i + j * m_numberOfRays < m_planeDataSize, "castRays", qPrintable(QString("i = %1, j = %2").arg(i).arg(j)) );
                unsigned char value = m_planeData[i + j * m_numberOfRays];
//                 if ( value == 255 ) value = 0;
                if ( value != 255   // don't count backgroud added by reslicer (255)
//                      && value != 0  // don't count normal background
                   )
                    compute( 0, value );
            }
            endLBlock( 0 );
        }
    }
}



}; // end namespace udg
