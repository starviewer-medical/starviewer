/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcardiac2dviewerextension.h"

#include "volume.h"
#include "series.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "toolmanager.h"
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
    initializeTools();
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
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Saggital") );
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
    m_playAction->setIcon( QIcon(":/images/play.png") );
    m_playButton->setDefaultAction( m_playAction );

    m_recordAction = new QAction( 0 );
    m_recordAction->setIcon( QIcon(":/images/record.png") );
    m_recordButton->setDefaultAction( m_recordAction );

    m_boomerangAction = new QAction( 0 );
    m_boomerangAction->setIcon( QIcon(":/images/boomerang.png") );
    m_boomerangAction->setCheckable( true );
    m_boomerangButton->setDefaultAction( m_boomerangAction );

    m_repeatAction = new QAction( 0 );
    m_repeatAction->setIcon( QIcon(":/images/repeat.png") );
    m_repeatAction->setCheckable( true );
    m_repeatButton->setDefaultAction( m_repeatAction );

    m_sequenceBeginAction = new QAction( 0 );
    m_sequenceBeginAction->setIcon( QIcon(":/images/sequenceBegin.png") );
    m_sequenceBeginAction->setCheckable( true );
    m_sequenceBeginButton->setDefaultAction( m_sequenceBeginAction );

    m_sequenceEndAction = new QAction( 0 );
    m_sequenceEndAction->setIcon( QIcon(":/images/sequenceEnd.png") );
    m_sequenceEndAction->setCheckable( true );
    m_sequenceEndButton->setDefaultAction( m_sequenceEndAction );





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
}

void QCardiac2DViewerExtension::createConnections()
{
    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar

    connect( m_slider, SIGNAL( valueChanged(int) ), m_2DView, SLOT( setPhase(int) ) );
    //connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );
    connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );

    connect( m_playAction , SIGNAL( triggered() ), SLOT( playImages() ) );
    connect( m_recordAction, SIGNAL( triggered() ), SLOT( recordVideo() ) );

    connect( m_spinBox, SIGNAL( valueChanged( int ) ), SLOT( changeVelocity( int ) ) );

    connect( m_sequenceBeginAction, SIGNAL( toggled( bool ) ), SLOT( initInterval( bool ) ));
    connect( m_sequenceEndAction, SIGNAL( toggled( bool ) ), SLOT( finishInterval( bool ) ));

    connect( m_repeatAction, SIGNAL(toggled( bool )), SLOT( changeToLoopMode( bool ) ));
    connect( m_boomerangAction, SIGNAL(toggled( bool )), SLOT( changeToComeBackMode( bool ) ));

    connect( m_axialViewAction, SIGNAL( triggered() ), SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction, SIGNAL( triggered() ), SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction, SIGNAL( triggered() ), SLOT( changeViewToCoronal() ) );

    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView , SLOT( resetWindowLevelToDefault() ) );
}

void QCardiac2DViewerExtension::initializeTools()
{
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->getToolAction("SlicingTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );

    // definim els grups exclusius
    QStringList exclusiveTools;
    exclusiveTools << "ZoomTool" << "SlicingTool";
    m_toolManager->addExclusiveToolsGroup("Group1", exclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool";
    m_toolManager->setViewerTools( m_2DView, toolsList );
    m_toolManager->refreshConnections();
}

void QCardiac2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( m_mainVolume );

    m_firstSliceInterval = 0;
    m_lastSliceInterval = m_mainVolume->getSeries()->getNumberOfPhases() - 1;

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

    m_slider->setMaximum( m_mainVolume->getSeries()->getNumberOfPhases() - 1 );
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

void QCardiac2DViewerExtension::playImages()
{
    if( !m_timer->isActive() )
    {
        m_playAction->setIcon( QIcon(":/images/pause.png") );
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
    m_playAction->setIcon( QIcon(":/images/play.png") );
}

void QCardiac2DViewerExtension::recordVideo()
{
    int phases = m_mainVolume->getSeries()->getNumberOfPhases();
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

    DEBUG_LOG("Writing file prova-cor2D.mpg...");
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
    DEBUG_LOG("End writing file prova-cor.mpg...");
    frames.clear();
}

void QCardiac2DViewerExtension::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == m_timer->timerId() )
    {
        // Si estem al final de l'interval
        if (  m_slider->value() == m_lastSliceInterval )
        {
            if ( m_repeatAction->isChecked() )
            {
                m_slider->setValue( m_firstSliceInterval );
            }
            else if ( m_boomerangAction->isChecked() )
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
            // Si tenim algun tipus de repeat activat
            if ( m_repeatAction->isChecked() ||  m_boomerangAction->isChecked() )
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
        m_boomerangAction->setChecked( false );
    }
}

void QCardiac2DViewerExtension::changeToComeBackMode( bool checked )
{
    if ( checked )
    {
        m_repeatAction->setChecked( false );
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
        m_lastSliceInterval = m_mainVolume->getSeries()->getNumberOfPhases() - 1;
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
