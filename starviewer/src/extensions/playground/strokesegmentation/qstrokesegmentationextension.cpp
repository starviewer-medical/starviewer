/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qstrokesegmentationextension.h"

#include "strokesegmentationmethod.h"
#include "logging.h"
#include "q2dviewer.h"
#include "toolmanager.h"
#include "editortool.h"
#include "editortooldata.h"
#include "toolproxy.h"
//Qt
#include <QAction>
#include <QSettings>
#include <QMessageBox>
// VTK
#include <vtkImageThreshold.h>
#include <vtkCommand.h>

namespace udg {

QStrokeSegmentationExtension::QStrokeSegmentationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_isSeed(false), m_isMask(false)
{
    setupUi( this );

    m_maskVolume = new Volume();
    m_segMethod = new StrokeSegmentationMethod();

    createActions();
    initializeTools();
    createConnections();
    readSettings();
}

QStrokeSegmentationExtension::~QStrokeSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    //TODO descomentar això per tal d'alliberar el m_maskVolume (ara peta)
    //delete m_maskVolume;
}

void QStrokeSegmentationExtension::createActions()
{
    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateClockWise() ) );
}

void QStrokeSegmentationExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->getToolAction("SlicingTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_seedToolButton->setDefaultAction( m_toolManager->getToolAction("SeedTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    m_editorToolButton->setDefaultAction( m_toolManager->getToolAction("EditorTool") );
    m_editorToolButton->setEnabled( false );

    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // Tool d'slicing per teclat
    QAction *slicingKeyboardTool = m_toolManager->getToolAction("SlicingKeyboardTool");
    slicingKeyboardTool->trigger();

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "SeedTool" << "EditorTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    // inicialitzem totes les tools
    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SeedTool" << "VoxelInformationTool" <<  "EditorTool";

    m_toolManager->setViewerTools( m_2DView, toolsList );
}

void QStrokeSegmentationExtension::createConnections()
{
    connect( m_applyMethodButton, SIGNAL( clicked() ), SLOT( applyMethod() ) );
    connect( m_updateVolumeButton, SIGNAL( clicked() ), SLOT( updateVolume() ) );
    connect( m_viewThresholdButton, SIGNAL( clicked() ), SLOT( viewThresholds() ) );
    connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ), SLOT( setLowerValue(int) ) );
    connect( m_upperValueSlider, SIGNAL( valueChanged(int) ), SLOT( setUpperValue(int) ) );
    connect( m_opacitySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
    connect( m_2DView, SIGNAL( seedChanged() ), SLOT( setSeedPosition() ) );
    connect( m_2DView, SIGNAL( overlayChanged( ) ), SLOT( updateVolume() ) );
}

void QStrokeSegmentationExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView->removeAnnotation(Q2DViewer::ScalarBarAnnotation);
    
    int* dim;
    dim = m_mainVolume->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    //Posem els nivells de dins i fora de la màcara els valors l?its del w/l per tal que es vegi correcte
    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_insideValue  = (int) wl[0];
    m_outsideValue = (int) (wl[0] - 2.0*wl[1]);

    // obtenim els valors mínim i màxim del volum
    m_minValue = m_mainVolume->getVtkData()->GetScalarRange()[0];
    m_maxValue = m_mainVolume->getVtkData()->GetScalarRange()[1];

    m_lowerValueSpinBox->setMinimum(m_minValue);
    m_lowerValueSpinBox->setMaximum(m_maxValue);
    m_upperValueSpinBox->setMinimum(m_minValue);
    m_upperValueSpinBox->setMaximum(m_maxValue);
    m_lowerValueSlider->setMinimum(m_minValue);
    m_lowerValueSlider->setMaximum(m_maxValue);
    m_upperValueSlider->setMinimum(m_minValue);
    m_upperValueSlider->setMaximum(m_maxValue);
}

void QStrokeSegmentationExtension::applyMethod( )
{
    if(!m_isSeed || !m_isMask)
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "ERROR: no hi ha definida llavor o màscara" ) );
        return;
    }

    m_segMethod->setVolume(m_mainVolume);
    m_segMethod->setMask(m_maskVolume);
    DEBUG_LOG( "Inici Apply method!!" );
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_segMethod->setInsideMaskValue ( m_insideValue );
    m_segMethod->setOutsideMaskValue( m_outsideValue );
    double pos[3];
    m_2DView->getSeedPosition(pos);
    m_segMethod->setSeedPosition(pos[0],pos[1],pos[2]);
    double segmentationVolume = m_segMethod->applyMethod();
    
    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_maskVolume);

    m_editorToolButton->defaultAction()->trigger();
    m_editorToolButton->setEnabled( true );

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(segmentationVolume, 0, 'f', 2));
    m_resultsLineEdit->setEnabled(true);
    m_resultsLabel->setEnabled(true);
    m_updateVolumeButton->setEnabled(true);
    
    m_2DView->refresh();
    QApplication::restoreOverrideCursor();
    DEBUG_LOG( "Fi Apply method!!" );
}

void QStrokeSegmentationExtension::setSeedPosition()
{
    double pos[3];
    QString aux;
    m_2DView->getSeedPosition(pos);
    m_seedXLineEdit->clear();
    m_seedYLineEdit->clear();
    m_seedZLineEdit->clear();
    aux = QString("%1").arg(pos[0], 0, 'f', 1);
    m_seedXLineEdit->insert(aux);
    aux = QString("%1").arg(pos[1], 0, 'f', 1);
    m_seedYLineEdit->insert(aux);
    aux = QString("%1").arg(pos[2], 0, 'f', 1);
    m_seedZLineEdit->insert(aux);
    m_isSeed=true;
    if(m_isMask)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QStrokeSegmentationExtension::setOpacity( int op )
{
    m_2DView->setOpacityOverlay(((double)op)/100.0);
    m_2DView->setOverlayInput(m_maskVolume);

    m_2DView->refresh();
}

void QStrokeSegmentationExtension::setLowerValue( int x )
{
    if(x>m_upperValueSlider->value())
    {
        m_upperValueSlider->setValue(x);
    }

    m_segMethod->setHistogramLowerLevel (x);
    m_isMask=true;
    if(m_isSeed)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QStrokeSegmentationExtension::setUpperValue( int x )
{
    if(x<m_lowerValueSlider->value())
    {
        m_lowerValueSlider->setValue(x);
    }
    m_segMethod->setHistogramUpperLevel (x);
    m_isMask=true;
    if(m_isSeed)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QStrokeSegmentationExtension::updateVolume()
{
    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg( this->calculateMaskVolume() , 0, 'f', 2));
}

void QStrokeSegmentationExtension::viewThresholds()
{
    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_mainVolume->getVtkData() );
    imageThreshold->ThresholdBetween( m_lowerValueSlider->value(),  m_upperValueSlider->value());
    imageThreshold->SetInValue( m_insideValue );
    imageThreshold->SetOutValue( m_outsideValue );
    imageThreshold->Update();

    m_maskVolume->setData(imageThreshold->GetOutput() );

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_maskVolume);

    m_2DView->refresh();

}

double QStrokeSegmentationExtension::calculateMaskVolume()
{
    if ( !m_maskVolume ) 
        return 0.0;

    double spacing[3];
    m_maskVolume->getSpacing(spacing);
    double volume = 1.0;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }
    // obtenim el volum de la màscara
    int cont;
    Tool *editorTool = m_2DView->getToolProxy()->getTool("EditorTool");
    if( editorTool )
    {
        EditorToolData *editorToolData = static_cast<EditorToolData *> ( editorTool->getToolData() );
        if( editorToolData )
        {
            cont = editorToolData->getVolumeVoxels();
        }
        else
        {
            DEBUG_LOG("No existeixen deades de l'editor tool. Comprovar aquest problema");
            cont = 0;
        }
    }
    else
    {
        DEBUG_LOG("No existeix la editor tool. No s'han inicialitzat les tools adequadament");
            cont = 0;
    }

    volume = volume*(double)cont;

    return volume;
}

void QStrokeSegmentationExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("StarViewer-App-StrokeSegmentation");

    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );

    settings.endGroup();
}

void QStrokeSegmentationExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("StarViewer-App-StrokeSegmentation");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );

    settings.endGroup();
}

}
