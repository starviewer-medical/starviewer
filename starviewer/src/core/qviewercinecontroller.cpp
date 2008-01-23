/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qviewercinecontroller.h"

// TODO això és així perquè de moment només suportem Q2DViewer
// a la llarga amb l'interfície de QViewer n'hi hauria d'haver prou
#include "q2dviewer.h"

#include "volume.h"

// Qt
#include <QAction>
#include <QBasicTimer>
#include <QDir>
#include <QTimerEvent>

// vtk
// per grabar el vídeo
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>
#include <vtkMPEG2Writer.h>

namespace udg {

QViewerCINEController::QViewerCINEController(QObject *parent)
  : QObject(parent), m_nextStep(1), m_velocity(1), m_2DViewer(0), m_playing(false), m_cineDimension(TemporalDimension), m_recordFilename( QDir::homePath() + "/cineMovie" )
{
    m_timer = new QBasicTimer();

    m_playAction = new QAction( 0 );
    m_playAction->setShortcut( tr("Space") );
    m_playAction->setIcon( QIcon(":/images/play.png") );
    connect( m_playAction, SIGNAL(triggered()), SLOT(play()) );

    m_recordAction = new QAction( 0 );
    m_recordAction->setIcon( QIcon(":/images/record.png") );
    connect( m_recordAction, SIGNAL(triggered()), SLOT(record()) );

    m_boomerangAction = new QAction( 0 );
    m_boomerangAction->setIcon( QIcon(":/images/boomerang.png") );
    m_boomerangAction->setCheckable( true );
    connect( m_boomerangAction, SIGNAL(triggered(bool)), SLOT( boomerangMode(bool) ) );

    m_loopAction = new QAction( 0 );
    m_loopAction->setIcon( QIcon(":/images/repeat.png") );
    m_loopAction->setCheckable( true );
    connect( m_loopAction, SIGNAL(triggered(bool)), SLOT( loopMode(bool) ) );
}

QViewerCINEController::~QViewerCINEController()
{
}

void QViewerCINEController::setInputViewer( QViewer *viewer )
{
    if( m_2DViewer )
    {
        disconnect( m_2DViewer, 0 , this, 0 );
    }

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
    {
        DEBUG_LOG( "S'HA PROPORCIONAT UN VIEWER NO SUPORTAT. ARA NOMÉS SUPORTEM Q2DVIEWER!!!!!!!!!!!!" );
        return;
    }

    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *)), SLOT( resetCINEInformation(Volume *) ) );
    resetCINEInformation(m_2DViewer->getInput());
}

void QViewerCINEController::setCINEDimension( int dimension )
{
    // checkejem per si el que ens passen és vàlid o no
    if( dimension != SpatialDimension &&  dimension != TemporalDimension )
    {
        DEBUG_LOG("Paràmetre ilegal!");
    }
    else
        m_cineDimension = dimension;
}

QAction *QViewerCINEController::getPlayAction() const
{
    return m_playAction;
}

QAction *QViewerCINEController::getRecordAction() const
{
    return m_recordAction;
}

QAction *QViewerCINEController::getLoopAction() const
{
    return m_loopAction;
}

QAction *QViewerCINEController::getBoomerangAction() const
{
    return m_boomerangAction;
}

void QViewerCINEController::setRecordFilename( const QString filename )
{
    m_recordFilename = filename;
}

void QViewerCINEController::play()
{
    if( !m_playing )
    {
        m_playing = true;
        m_playAction->setIcon( QIcon(":/images/pause.png") );
        emit playing();
        m_timer->start(1000/m_velocity, this);
    }
    else
    {
        pause();
    }
}

void QViewerCINEController::pause()
{
    m_timer->stop();
    m_playing = false;
    m_playAction->setIcon( QIcon(":/images/play.png") );
    emit paused();
}

void QViewerCINEController::record()
{
    //ens curem en salut
    if( !m_2DViewer )
        return;
    if( !m_2DViewer->getInput() )
        return;

    int phases = m_2DViewer->getInput()->getNumberOfPhases();
    int currentSlice = m_2DViewer->getCurrentSlice();
    QVector< vtkImageData * > frames;

    // Guardar els fotogrames
    for( int i = 0 ; i < phases ; i++ )
    {
        // TODO això es podria fer amb la crida QViewer::grabCurrentView()
        m_2DViewer->setPhase(i);

        vtkWindowToImageFilter *windowToImageFilter = vtkWindowToImageFilter::New();

        vtkRenderWindow *renderWindow = m_2DViewer->getRenderWindow();
        renderWindow->OffScreenRenderingOn();

        windowToImageFilter->SetInput( renderWindow );
        windowToImageFilter->Update();

        renderWindow->Render();

        vtkImageData *image = windowToImageFilter->GetOutput();
        frames << ( image );
    }
    m_2DViewer->setSlice( currentSlice );

    // Fer la gravació
    // TODO ara només gravem en mpg, hauríem d'incoporar mètodes per gravar en altres formats
    vtkGenericMovieWriter *videoWriter = vtkMPEG2Writer::New();
    videoWriter->SetFileName( qPrintable( m_recordFilename+".mpg" ) );

    vtkImageData *data = frames[0];
    videoWriter->SetInput( data );
    videoWriter->Start();

    int progressIncrement = static_cast<int>( (1.0/(double)frames.size()) * 100 );
    int progress = 0;
    for( unsigned int i = 0; i < frames.size(); i++ )
    {
        videoWriter->SetInput( frames[i] );

        for( int j = 0; j < 3; j++ ) // TODO perquè un loop de 3?
        {
            videoWriter->Write();
        }
        progress += progressIncrement;
        emit recording( progress );
    }
    videoWriter->End();
    frames.clear();
}

void QViewerCINEController::setVelocity( int imagesPerSecond )
{
    m_velocity = imagesPerSecond;
    emit velocityChanged( m_velocity );
    if( m_playing )
    {
        m_timer->start(1000/m_velocity,this);
    }
}

void QViewerCINEController::loopMode( bool enable )
{
    m_loopAction->setChecked( enable );
    if( enable )
        m_boomerangAction->setChecked( false );
}

void QViewerCINEController::boomerangMode( bool enable )
{
    m_boomerangAction->setChecked( enable );
    if( enable )
    {
        m_loopAction->setChecked( false );
    }
    else
        m_nextStep = 1;
}

void QViewerCINEController::setPlayInterval( int firstImage, int lastImage )
{
    m_firstSliceInterval = firstImage;
    m_lastSliceInterval = lastImage;
}

void QViewerCINEController::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == m_timer->timerId() )
    {
        handleCINETimerEvent();
    }
}

void QViewerCINEController::handleCINETimerEvent()
{
    if( !m_2DViewer )
        return;

    int currentImageIndex;
    int nextImageIndex;

    if( m_cineDimension == TemporalDimension )
        currentImageIndex = m_2DViewer->getCurrentPhase();
    else
        currentImageIndex = m_2DViewer->getCurrentSlice();

    // Si estem al final de l'interval
    if( currentImageIndex == m_lastSliceInterval )
    {
        if( m_loopAction->isChecked() )
        {
            nextImageIndex = m_firstSliceInterval;
        }
        else if( m_boomerangAction->isChecked() )
        {
            m_nextStep = -1;
            nextImageIndex = currentImageIndex + m_nextStep;
        }
        else
        {
            nextImageIndex = m_firstSliceInterval;
            pause();
        }
    }
    // Si estem a l'inici de l'interval
    else if( ( currentImageIndex == m_firstSliceInterval )  )
    {
        // Si tenim algun tipus de repeat activat
        if( m_loopAction->isChecked() ||  m_boomerangAction->isChecked() )
        {
            m_nextStep = 1;
            nextImageIndex = currentImageIndex + m_nextStep;
        }
        else
        {
            // Fins ara reproduia endavant-endarrera
            if( m_nextStep == -1 )
            {
                m_nextStep = 1;
                pause();
            }
            // Inici de la reproduccio
            else
            {
                nextImageIndex = currentImageIndex + m_nextStep;
            }
        }
    }
    else
    {
        nextImageIndex = currentImageIndex + m_nextStep;
    }

    if( m_cineDimension == TemporalDimension )
        m_2DViewer->setPhase( nextImageIndex );
    else
        m_2DViewer->setSlice( nextImageIndex );
}

void QViewerCINEController::resetCINEInformation(Volume *input)
{
    if( input )
    {
        int phases = input->getNumberOfPhases();
        if( phases > 1 )
        {
            setCINEDimension( TemporalDimension ); // si tenim fases, per defecte treballem sota la temporal
            setVelocity( input->getNumberOfPhases() );
            m_firstSliceInterval = 0;
            m_lastSliceInterval = input->getNumberOfPhases() - 1;
        }
        else
        {
            // TODO potser seria més correcte si s'interrogués a partir d'input!
            setCINEDimension( SpatialDimension ); // si no tenim fases, només podem treballar sobre la dim espaial
            setVelocity( 10 ); // li donarem una velocitat de 10 img/sec
            m_firstSliceInterval = 0;
            m_lastSliceInterval = m_2DViewer->getNumberOfSlices() - 1;
        }

    }
}

}
