/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmprcardiac3dextension.h"

#include "qcardiac3dmprviewer.h"
#include "volume.h"
#include "series.h"
#include "logging.h"
#include "toolmanager.h"
#include "windowlevelpresetstooldata.h"

// Qt
#include <QToolButton>
#include <QMessageBox>
#include <QAction>
#include <QBasicTimer>
#include <QProgressDialog>
#include <QFileDialog>
// VTK
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>
#include <vtkGenericMovieWriter.h>
#include <vtkMPEG2Writer.h>
#include <vtkImageData.h>
// stl
#include <vector>

namespace udg {

QMPRCardiac3DExtension::QMPRCardiac3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_timer = new QBasicTimer();
    m_nextStep = 1;

    m_spinBox->setValue( 20 );
    m_slider->setPageStep(1);

    initializeTools();
    createConnections();

    m_axialViewEnabledButton->setChecked( true );
    m_sagitalViewEnabledButton->setChecked( true );
    m_coronalViewEnabledButton->setChecked( true );
    m_mpr3DView->orientationMarkerOff();

    // ajustaments de window level pel combo box
    m_windowLevelComboBox->setPresetsData( m_mpr3DView->getWindowLevelData() );
    m_windowLevelComboBox->selectPreset( m_mpr3DView->getWindowLevelData()->getCurrentPreset() );
}

QMPRCardiac3DExtension::~QMPRCardiac3DExtension()
{

}

void QMPRCardiac3DExtension::initializeTools()
{
    m_toolManager = new ToolManager( this );

    // obtenim les accions per cada tool
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );
    m_rotate3DToolButton->setDefaultAction( m_toolManager->getToolAction("Rotate3DTool") );

    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_zoomToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_rotate3DToolButton->defaultAction()->trigger();

    // registrem al manager les tools que van amb el viewer principal
    QStringList toolsList;
    toolsList << "ZoomTool" << "TranslateTool" << "Rotate3DTool" << "ScreenShotTool" << "WindowLevelPresetsTool";
    m_toolManager->setViewerTools( m_mpr3DView, toolsList );

    m_toolManager->refreshConnections();
}

void QMPRCardiac3DExtension::createConnections()
{
    connect( m_axialViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setAxialVisibility(bool) ) );
    connect( m_sagitalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setSagitalVisibility(bool) ) );
    connect( m_coronalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setCoronalVisibility(bool) ) );

    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToCoronal() ) );
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToAxial() ) );

    connect( m_slider , SIGNAL( valueChanged(int) ) , m_mpr3DView , SLOT( setSubVolume(int) ) );

    connect( m_ButtonPlay , SIGNAL( clicked() ) , this , SLOT( playImages() ) );
    connect( m_ButtonRecord , SIGNAL( clicked() ) , this , SLOT( recordVideo() ) );

    connect( m_spinBox , SIGNAL( valueChanged( int ) ) , this , SLOT( changeVelocity( int ) ) );

    connect( m_ButtonOpenParentesis , SIGNAL( toggled( bool ) ) , this , SLOT( initInterval( bool ) ));
    connect( m_ButtonCloseParentesis , SIGNAL( toggled( bool ) ) , this , SLOT( finishInterval( bool ) ));

    connect( m_ButtonLoop , SIGNAL (toggled( bool )) , this , SLOT ( changeToLoopMode( bool ) ));
    connect( m_ButtonComeBack , SIGNAL (toggled( bool )) , this , SLOT ( changeToComeBackMode( bool ) ));
}

void QMPRCardiac3DExtension::setInput( Volume *input )
{
    m_volume = input;
    m_mpr3DView->setInput( m_volume );
    double wl[2];
    m_mpr3DView->getCurrentWindowLevel( wl );
    INFO_LOG("QMPRCardiac3DExtension:: Donem Input ");

    m_firstSliceInterval = 0;
    m_lastSliceInterval = m_volume->getSeries()->getNumberOfPhases() - 1;

    m_slider->setMinimum( m_firstSliceInterval );
    m_slider->setMaximum( m_lastSliceInterval );
}

void QMPRCardiac3DExtension::playImages(){

   if ( !m_timer->isActive() )
    {
        m_ButtonPlay->setIcon( QIcon(":/images/player_pause32.png") );
        m_timer->start( ( 1000 / m_spinBox->value() ) , this );
    }
    else
    {
        pauseImages();
    }
}

void QMPRCardiac3DExtension::pauseImages()
{
    m_timer->stop();
    m_ButtonPlay->setIcon( QIcon(":/images/player_play32.png") );
//     m_ButtonPlay->setChecked( false );
}

void QMPRCardiac3DExtension::recordVideo()
{
    int phases = m_volume->getSeries()->getNumberOfPhases();
    int currentSubVolume = m_slider->value();
    std::vector< vtkImageData * > frames;

    for (int i = 0 ; i < phases ; i++ )
    {
        m_mpr3DView->setSubVolume(i);

        vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

        vtkRenderWindow * renderWindow = m_mpr3DView->getRenderWindow();
        renderWindow->OffScreenRenderingOn();

        if( !renderWindow )
            DEBUG_LOG( "La vtkRenderWindow és NUL·LA!" );

        windowToImageFilter->SetInput( renderWindow );

        windowToImageFilter->Update();

        renderWindow->Render();

        vtkImageData * image = windowToImageFilter->GetOutput();
        frames.push_back( image );
    }
    m_mpr3DView->setSubVolume( currentSubVolume );

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
    frames.clear();
}

void QMPRCardiac3DExtension::timerEvent(QTimerEvent *event)
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

void QMPRCardiac3DExtension::changeVelocity( int velocity )
{
    if ( m_timer->isActive() )
    {
            m_timer->start( 1000 / velocity , this );
    }
}

void QMPRCardiac3DExtension::changeToLoopMode( bool checked )
{
    if ( checked )
    {
        m_ButtonComeBack->setChecked( false );
    }
}

void QMPRCardiac3DExtension::changeToComeBackMode( bool checked )
{
    if ( checked )
    {
        m_ButtonLoop->setChecked( false );
    }
}

void QMPRCardiac3DExtension::initInterval( bool checked )
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

void QMPRCardiac3DExtension::finishInterval( bool checked )
{
    if ( checked )
    {
        m_lastSliceInterval = m_slider->value();
    }
    else
    {
        m_lastSliceInterval = m_volume->getSeries()->getNumberOfPhases() - 1;
    }
    m_slider->setMaximum( m_lastSliceInterval );
}

};  // end namespace udg
