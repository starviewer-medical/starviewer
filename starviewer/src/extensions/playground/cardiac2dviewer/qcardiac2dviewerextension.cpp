/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcardiac2dviewerextension.h"

#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "toolsactionfactory.h"
#include <QAction>
#include <QSettings>
#include <QBasicTimer>
#include <QString>
#include <QProgressDialog>
#include <QFileDialog>

// VTK
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>
#include <vtkGenericMovieWriter.h>
#include <vtkMPEG2Writer.h>
// #include <vtkAVIWriter.h>
#include <vtkImageData.h>


#include "slicing2dtool.h"

#include <vector>

namespace udg {

QCardiac2DViewerExtension::QCardiac2DViewerExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;

    m_timer = new QBasicTimer();
    m_nextStep = 1;

    m_spinBox->setValue( 20 );
    m_slider->setPageStep(1);

    readSettings();
    createActions();
    createConnections();

}

QCardiac2DViewerExtension::~QCardiac2DViewerExtension()
{
    writeSettings();
}

void QCardiac2DViewerExtension::createActions()
{
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("&Axial View") );
    m_axialViewAction->setShortcut( tr("Ctrl+A") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    m_axialViewToolButton->setDefaultAction( m_axialViewAction );

    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("&Sagital View") );
    m_sagitalViewAction->setShortcut( tr("Ctrl+S") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Sagital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );
    m_sagitalViewToolButton->setEnabled( false );

    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("&Coronal View") );
    m_coronalViewAction->setShortcut( tr("Ctrl+C") );
    m_coronalViewAction->setStatusTip( tr("Change Current View To Coronal") );
    m_coronalViewAction->setIcon( QIcon(":/images/coronal.png") );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );
    m_coronalViewToolButton->setEnabled( false );

    m_playAction = new QAction( 0 );
    m_playAction->setShortcut( tr("Space") );
    m_playAction->setIcon( QIcon(":/images/player_play32.png") );
    m_ButtonPlay->setDefaultAction( m_playAction );

    // Pseudo-tool \TODO ara mateix no ho integrem dins del framework de tools, però potser que més endavant sí
    m_voxelInformationAction = new QAction( 0 );
    m_voxelInformationAction->setText( tr("Voxel Information") );
    m_voxelInformationAction->setShortcut( tr("Ctrl+I") );
    m_voxelInformationAction->setStatusTip( tr("Enable voxel information over cursor") );
    m_voxelInformationAction->setIcon( QIcon(":/images/voxelInformation.png") );
    m_voxelInformationAction->setCheckable( true );
    m_voxelInformationToolButton->setDefaultAction( m_voxelInformationAction );

    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_2DView , SLOT( setVoxelInformationCaptionEnabled(bool) ) );

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateCounterClockWise() ) );


    // Tools
    m_actionFactory = new ToolsActionFactory( 0 );
    m_slicingAction = m_actionFactory->getActionFrom( "SlicingTool" );
    m_slicingToolButton->setDefaultAction( m_slicingAction );

    m_windowLevelAction = m_actionFactory->getActionFrom( "WindowLevelTool" );
    m_windowLevelToolButton->setDefaultAction( m_windowLevelAction );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = m_actionFactory->getActionFrom( "TranslateTool" );
    m_moveToolButton->setDefaultAction( m_moveAction );

    m_screenShotAction = m_actionFactory->getActionFrom( "ScreenShotTool" );
    m_screenShotToolButton->setDefaultAction( m_screenShotAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_screenShotAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_slicingAction->trigger();
}

void QCardiac2DViewerExtension::createConnections()
{
    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar

    connect( m_slider , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setPhase(int) ) );
    //connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );

    connect( m_playAction , SIGNAL( triggered() ) , this , SLOT( playImages() ) );

    connect( m_ButtonRecord , SIGNAL( clicked() ) , this , SLOT( recordVideo() ) );

    connect( m_spinBox , SIGNAL( valueChanged( int ) ) , this , SLOT( changeVelocity( int ) ) );

    connect( m_ButtonOpenParentesis , SIGNAL( toggled( bool ) ) , this , SLOT( initInterval( bool ) ));
    connect( m_ButtonCloseParentesis , SIGNAL( toggled( bool ) ) , this , SLOT( finishInterval( bool ) ));

    connect( m_ButtonLoop , SIGNAL (toggled( bool )) , this , SLOT ( changeToLoopMode( bool ) ));
    connect( m_ButtonComeBack , SIGNAL (toggled( bool )) , this , SLOT ( changeToComeBackMode( bool ) ));

    connect( m_axialViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToCoronal() ) );


    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView , SLOT( setWindowLevel(double,double) ) );


    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView , SLOT( resetWindowLevelToDefault() ) );

}

void QCardiac2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( m_mainVolume );

    m_firstSliceInterval = 0;
    m_lastSliceInterval = m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases() - 1;

    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );
    INFO_LOG("QCardiac2DViewerExtension: Donem l'input principal")
    changeViewToAxial();
}


void QCardiac2DViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_slider->setMaximum( m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases() - 1 );
//    m_viewText->setText( tr("XY : Axial") );
    m_2DView->setViewToAxial();
    INFO_LOG("Visor per defecte: Canviem a vista axial (Vista 1)")
    m_2DView->render();
    m_slider->setValue( m_firstSliceInterval );

}

void QCardiac2DViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[0] );
    m_spinBox->setMaximum( extent[1] );
    m_slider->setMaximum( extent[1] );
//     m_viewText->setText( tr("YZ : Sagital") );
    m_2DView->setViewToSagittal();
    INFO_LOG("Visor per defecte: Canviem a vista sagital (Vista 1)")
    m_2DView->render();
}

void QCardiac2DViewerExtension::changeViewToCoronal()
{

    m_currentView = Coronal;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );


    m_spinBox->setMinimum( extent[2] );
    m_spinBox->setMaximum( extent[3] );
    m_slider->setMaximum( extent[3] );
//     m_viewText->setText( tr("XZ : Coronal") );
    m_2DView->setViewToCoronal();
    INFO_LOG("Visor per defecte: Canviem a vista coronal (Vista 1)")
    m_2DView->render();

}

void QCardiac2DViewerExtension::setView( ViewType view )
{
    switch( view )
    {
    case Axial:
        changeViewToAxial();
    break;
    case Sagital:
        changeViewToSagital();
    break;
    case Coronal:
        changeViewToCoronal();
    break;
    }
}

void QCardiac2DViewerExtension::playImages(){

if ( !m_timer->isActive() )
    {
        m_ButtonPlay->setIcon( QIcon(":/images/player_pause32.png") );
        m_timer->start(1000 / m_spinBox->value(), this);
    }
    else
    {
        pauseImages();
    }
}

void QCardiac2DViewerExtension::pauseImages()
{
    m_timer->stop();
    m_ButtonPlay->setIcon( QIcon(":/images/player_play32.png") );
//     m_ButtonPlay->setChecked( false );
}

void QCardiac2DViewerExtension::recordVideo()
{

    int phases = m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases();
    int currentSlice = m_slider->value();
    std::vector< vtkImageData * > frames;

    // Guardar els fotogrames
    for (int i = 0 ; i < phases ; i++ )
    {
        m_2DView->setPhase(i);

        vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

        vtkRenderWindow * renderWindow = m_2DView->getRenderWindow();
        renderWindow->OffScreenRenderingOn();

        if( !renderWindow )
            DEBUG_LOG( "La vtkRenderWindow és NUL·LA!" );

        windowToImageFilter->SetInput( renderWindow );
        windowToImageFilter->Update();

        renderWindow->Render();

        vtkImageData * image = windowToImageFilter->GetOutput();
        frames.push_back( image );
    }
    m_2DView->setSlice( currentSlice );

    // Guardar el nom del fitxer
    QFileDialog saveAsDialog(0);
    saveAsDialog.setWindowTitle( tr("Save video file as...") );
    saveAsDialog.setDirectory( QDir::homePath() );
    QStringList filters;
    filters << tr("MPEG (*.mpg)") << tr("AVI (*.avi)");
    saveAsDialog.setFilters( filters );
    saveAsDialog.setFileMode( QFileDialog::AnyFile );
    saveAsDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog.setConfirmOverwrite( true );

    QStringList fileNames;
    if( saveAsDialog.exec() )
        fileNames = saveAsDialog.selectedFiles();

    if( fileNames.isEmpty() )
        return;
    QString fileName = fileNames.first();

    vtkGenericMovieWriter *videoWriter;
    QString pattern;
    if( saveAsDialog.selectedFilter() == tr("MPEG (*.mpg)") )
    {
        videoWriter = vtkMPEG2Writer::New();
        pattern = ".mpg";
    }
    else if( saveAsDialog.selectedFilter() == tr("AVI (*.avi)") )
    {
           //\TODO Sembla que les vtk no s'han compilat amb suport per avi. Repassar-ho.
//         videoWriter = vtkAVIWriter::New();
//         pattern = ".avi";
        return;
    }
    else
    {
        DEBUG_LOG("No coincideix cap patró, no es pot desar el video! RETURN!");
        return;
    }

    // Gravar el fitxer
    QProgressDialog *m_progressDialog = new QProgressDialog;

    m_progressDialog->setRange( 0 , frames.size() );
    m_progressDialog->setMinimumDuration( 0 );
    m_progressDialog->setWindowTitle( tr("Making video") );
//     atenció: el missatge triga una miqueta a aparèixer...
//     m_progressDialog->setLabelText( tr("Loading, please wait...") );
    m_progressDialog->setCancelButton( 0 );

    videoWriter->SetFileName( qPrintable( fileName+pattern ) );

    cout << "Writing file prova-cor2D.mpg..." << endl;
    vtkImageData * data = frames[0];
    videoWriter->SetInput( data );
    videoWriter->Start();

    for ( unsigned int i = 0 ; i < frames.size() ; i++ )
    {

        videoWriter->SetInput( frames[i] );

        for ( int j = 0 ; j < 3 ; j++ )
        {
            videoWriter->Write();
        }

        m_progressDialog->setValue(i);
        qApp->processEvents();
    }
    videoWriter->End();
    m_progressDialog->setValue( frames.size() );
    cout << "End writing file prova-cor.mpg..." << endl;
    frames.clear();
}

void QCardiac2DViewerExtension::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == m_timer->timerId() )
    {

        // Si estem al final de l'interval
        if (  m_slider->value() == m_lastSliceInterval )
        {
            if ( m_ButtonLoop->isChecked() )
            {
                m_slider->setValue( m_firstSliceInterval );
            }
            else if ( m_ButtonComeBack->isChecked() )
            {
                m_nextStep = -1;
                m_slider->setValue( m_slider->value() + m_nextStep );
            }
            else
            {
                m_slider->setValue( m_firstSliceInterval );
                pauseImages();
            }
        }
        // Si estem a l'inici de l'interval
        else if ( ( m_slider->value() == m_firstSliceInterval )  )
        {
            // Si tenim algun tipus de loop activat
            if ( m_ButtonLoop->isChecked() ||  m_ButtonComeBack->isChecked() )
            {
                m_nextStep = 1;
                m_slider->setValue( m_slider->value() + m_nextStep );
            }
            else
            {
                // Fins ara reproduia endavant-endarrera
                if ( m_nextStep == -1 )
                {
                    m_nextStep = 1;
                    pauseImages();
                }
                // Inici de la reproduccio
                else
                {
                    m_slider->setValue( m_slider->value() + m_nextStep );
                }
            }
        }
        else
        {
            m_slider->setValue( m_slider->value() + m_nextStep );
        }

    }
}

void QCardiac2DViewerExtension::changeVelocity( int velocity )
{
    if ( m_timer->isActive() )
    {
        m_timer->start( 1000 / velocity , this );
    }
}

void QCardiac2DViewerExtension::changeToLoopMode( bool checked )
{
    if ( checked )
    {
        m_ButtonComeBack->setChecked( false );
    }
}

void QCardiac2DViewerExtension::changeToComeBackMode( bool checked )
{
    if ( checked )
    {
        m_ButtonLoop->setChecked( false );
    }
}

void QCardiac2DViewerExtension::initInterval( bool checked )
{
    if ( checked )
    {
        m_firstSliceInterval = m_slider->value();
    }
    else
    {
        m_firstSliceInterval = 0;
    }
    m_slider->setMinimum( m_firstSliceInterval );
}

void QCardiac2DViewerExtension::finishInterval( bool checked )
{
    if ( checked )
    {
        m_lastSliceInterval = m_slider->value();
    }
    else
    {
        m_lastSliceInterval = m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases() - 1;
    }
    m_slider->setMaximum( m_lastSliceInterval );
}

void QCardiac2DViewerExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
}

void QCardiac2DViewerExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
}

}
