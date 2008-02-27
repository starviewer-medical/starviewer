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
#include "windowlevelpresetstooldata.h"
#include "qviewercinecontroller.h"
// Qt
#include <QAction>
#include <QString>
#include <QProgressDialog>
#include <QFileDialog>

namespace udg {

QCardiac2DViewerExtension::QCardiac2DViewerExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;

    m_viewerCineController = new QViewerCINEController(this);
    m_viewerCineController->setInputViewer( m_2DView );

    m_spinBox->setValue( 20 );
    m_slider->setPageStep(1);

    createActions();
    createConnections();
    initializeTools();
    // activem les dades de ww/wl de la combo box
    m_windowLevelComboBox->setPresetsData( m_2DView->getWindowLevelData() );
    m_windowLevelComboBox->selectPreset( m_2DView->getWindowLevelData()->getCurrentPreset() );
}

QCardiac2DViewerExtension::~QCardiac2DViewerExtension()
{
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

    m_playAction = m_viewerCineController->getPlayAction();
    m_playButton->setDefaultAction( m_playAction );

    m_recordAction = new QAction(0);
    m_recordAction->setIcon( QIcon(":/images/record.png") );
    connect( m_recordAction, SIGNAL( triggered() ), this, SLOT(recordVideo()) );
    m_recordButton->setDefaultAction( m_recordAction );

    m_boomerangAction = m_viewerCineController->getBoomerangAction();
    m_boomerangButton->setDefaultAction( m_boomerangAction );

    m_repeatAction = m_viewerCineController->getLoopAction();
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
    connect( m_slider, SIGNAL( valueChanged(int) ), m_2DView, SLOT( setPhase(int) ) );
    connect( m_2DView , SIGNAL( phaseChanged(int) ) , m_slider , SLOT( setValue(int) ) );
    connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );

    connect( m_spinBox, SIGNAL( valueChanged( int ) ), m_viewerCineController, SLOT( setVelocity( int ) ) );

    connect( m_sequenceBeginAction, SIGNAL( toggled( bool ) ), SLOT( initInterval( bool ) ));
    connect( m_sequenceEndAction, SIGNAL( toggled( bool ) ), SLOT( finishInterval( bool ) ));

    connect( m_axialViewAction, SIGNAL( triggered() ), SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction, SIGNAL( triggered() ), SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction, SIGNAL( triggered() ), SLOT( changeViewToCoronal() ) );
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

    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // definim els grups exclusius
    QStringList exclusiveTools;
    exclusiveTools << "ZoomTool" << "SlicingTool";
    m_toolManager->addExclusiveToolsGroup("Group1", exclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool" << "WindowLevelPresetsTool";
    m_toolManager->setViewerTools( m_2DView, toolsList );
}

void QCardiac2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( m_mainVolume );
    m_viewerCineController->resetCINEInformation( m_mainVolume );

    m_firstSliceInterval = 0;
    m_lastSliceInterval = m_mainVolume->getSeries()->getNumberOfPhases() - 1;

    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
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
    m_2DView->resetViewToAxial();
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
    m_2DView->resetViewToSagital();
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
    m_2DView->resetViewToCoronal();
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

void QCardiac2DViewerExtension::recordVideo()
{
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

    QString pattern;
    if( saveAsDialog.selectedFilter() == tr("MPEG (*.mpg)") )
    {
        pattern = ".mpg";
    }
    else if( saveAsDialog.selectedFilter() == tr("AVI (*.avi)") )
    {
           //\TODO Sembla que les vtk no s'han compilat amb suport per avi. Repassar-ho.
        pattern = ".avi";
        return;
    }
    else
    {
        DEBUG_LOG("No coincideix cap patró, no es pot desar el video! RETURN!");
        return;
    }
    m_viewerCineController->setRecordFilename( fileName );

    // connectem el progrés
    QProgressDialog *progressDialog = new QProgressDialog;

    progressDialog->setRange( 0 , 100 );
    progressDialog->setMinimumDuration( 0 );
    progressDialog->setWindowTitle( tr("Making video") );
//     atenció: el missatge triga una miqueta a aparèixer...
//     m_progressDialog->setLabelText( tr("Loading, please wait...") );
    progressDialog->setCancelButton( 0 );
    connect( m_viewerCineController, SIGNAL( recording(int) ), progressDialog, SLOT( setValue(int) ) );
    qApp->processEvents();
    m_viewerCineController->record();
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
    m_viewerCineController->setPlayInterval( m_firstSliceInterval, m_lastSliceInterval );
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
    m_viewerCineController->setPlayInterval( m_firstSliceInterval, m_lastSliceInterval );
}

}
