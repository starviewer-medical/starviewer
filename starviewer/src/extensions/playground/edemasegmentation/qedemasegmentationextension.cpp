/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qedemasegmentationextension.h"

#include "strokesegmentationmethod.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "toolmanager.h"
#include "toolproxy.h"
#include "editortool.h"
#include "editortooldata.h"
#include "edemasegmentationsettings.h"
#include "patientbrowsermenu.h"
//Qt
#include <QString>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
// VTK
#include <vtkImageMask.h>
#include <vtkImageThreshold.h>
#include <vtkCommand.h>
#include <vtkMetaImageWriter.h>

namespace udg {

QEdemaSegmentationExtension::QEdemaSegmentationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_lesionMaskVolume(0), m_edemaMaskVolume(0), m_ventriclesMaskVolume(0), m_activedMaskVolume(0), m_imageThreshold(0), m_filteredVolume(0), m_isSeed(false), m_isMask(false), m_cont(0), m_edemaCont(0), m_ventriclesCont(0), m_volume(0.0), m_edemaVolume(0.0)
{
    setupUi( this );
    EdemaSegmentationSettings().init();

    m_seedPosition[0] = 0.0;
    m_seedPosition[1] = 0.0;
    m_seedPosition[2] = 0.0;

    m_segMethod = new StrokeSegmentationMethod();
   
    createActions();
    initializeTools();
    createConnections();
    readSettings();
}

QEdemaSegmentationExtension::~QEdemaSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    //delete m_lesionMaskVolume;//TODO descomentar això per tal d'alliberar el m_lesionMaskVolume (ara peta)

    if(m_filteredVolume)
        delete m_filteredVolume;
    
    if(m_lesionMaskVolume)
        delete m_lesionMaskVolume;

    if(m_ventriclesMaskVolume)
        delete m_ventriclesMaskVolume;

    if(m_edemaMaskVolume)
        delete m_edemaMaskVolume;

    if(m_imageThreshold)
        m_imageThreshold->Delete();
}

void QEdemaSegmentationExtension::createActions()
{
    m_lesionViewAction = new QAction( 0 );
    m_lesionViewAction->setText( tr("Lesion Overlay") );
    m_lesionViewAction->setStatusTip( tr("Enable/Disable lesion View Overlay") );
    m_lesionViewAction->setCheckable( true );
    m_lesionViewAction->setEnabled( false );
    m_lesionViewToolButton->setDefaultAction( m_lesionViewAction );

    m_edemaViewAction = new QAction( 0 );
    m_edemaViewAction->setText( tr("Edema Overlay") );
    m_edemaViewAction->setStatusTip( tr("Enable/Disable edema View Overlay") );
    m_edemaViewAction->setCheckable( true );
    m_edemaViewAction->setEnabled( false );
    m_edemaViewToolButton->setDefaultAction( m_edemaViewAction );

    m_ventriclesViewAction = new QAction( 0 );
    m_ventriclesViewAction->setText( tr("Ventricles Overlay") );
    m_ventriclesViewAction->setStatusTip( tr("Enable/Disable ventricles View Overlay") );
    m_ventriclesViewAction->setCheckable( true );
    m_ventriclesViewAction->setEnabled( false );
    m_ventriclesViewToolButton->setDefaultAction( m_ventriclesViewAction );

    m_viewOverlayActionGroup = new QActionGroup( 0 );
    m_viewOverlayActionGroup->setExclusive( true );
    m_viewOverlayActionGroup->addAction( m_ventriclesViewAction );
    m_viewOverlayActionGroup->addAction( m_edemaViewAction );
    m_viewOverlayActionGroup->addAction( m_lesionViewAction );
}

void QEdemaSegmentationExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_seedToolButton->setDefaultAction( m_toolManager->registerTool("SeedTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_editorToolButton->setDefaultAction( m_toolManager->registerTool("EditorTool") );
    m_editorToolButton->setEnabled(false);
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SlicingTool" << "TranslateTool" << "WindowLevelTool";
    m_toolManager->triggerTools( defaultTools );

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

    // posem a punt les tools
    m_toolManager->setupRegisteredTools( m_2DView );

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool("RotateClockWiseActionTool") );
    m_toolManager->enableRegisteredActionTools( m_2DView );
}

void QEdemaSegmentationExtension::createConnections()
{
    connect( m_filterPushButton, SIGNAL( clicked() ), SLOT( applyFilterMainImage() ) );
    connect( m_applyMethodButton, SIGNAL( clicked() ), SLOT( applyMethod() ) );
    connect( m_applyCleanSkullButton, SIGNAL( clicked() ), SLOT( applyCleanSkullMethod() ) );
    connect( m_applyVentriclesMethodButton, SIGNAL( clicked() ), SLOT( applyVentriclesMethod() ) );
    connect( m_applyEdemaMethodButton, SIGNAL( clicked() ), SLOT( applyEdemaMethod() ) );
    connect( m_lesionViewToolButton, SIGNAL( clicked() ), SLOT( viewLesionOverlay() ) );
    connect( m_edemaViewToolButton, SIGNAL( clicked() ), SLOT( viewEdemaOverlay() ) );
    connect( m_ventriclesViewToolButton, SIGNAL( clicked() ), SLOT( viewVentriclesOverlay() ) );
    connect( m_updateVolumeButton, SIGNAL( clicked() ), SLOT( updateVolume() ) );
    connect( m_viewThresholdButton, SIGNAL( clicked() ), SLOT( viewThresholds() ) );
    connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), SLOT( strokeEventHandler(unsigned long) ) );
    connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ), SLOT( setLowerValue(int) ) );
    connect( m_upperValueSlider, SIGNAL( valueChanged(int) ), SLOT( setUpperValue(int) ) );
    connect( m_opacitySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
    // TODO en comptes de tenir aquesta connexió hauríem de connectar el signal de la tool que ens ho hauria de fer el toolmanager
    connect( m_2DView, SIGNAL( seedPositionChanged(double,double,double) ), SLOT( setSeedPosition(double,double,double) ) );

    // Fem que no s'assigni automàticament l'input que s'ha seleccionat amb el menú de pacient, ja que fem tractaments adicionals
    // sobre el volum seleccionat i l'input final del visor pot diferir de l'inicial i és l'extensió qui decideix finalment quin input
    // se li vol donar a cada viewer. Capturem la senyal de quin volum s'ha escollit i a partir d'aquí fem el que calgui
    disconnect( m_2DView->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_2DView, SLOT( setInput(Volume *) ) );
    connect( m_2DView->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), SLOT( setInput(Volume *) ) );

    connect( m_saveMaskPushButton, SIGNAL( clicked() ), SLOT( saveActivedMaskVolume() ) );

    connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ), SLOT( viewThresholds() ) );
    connect( m_upperValueSlider, SIGNAL( valueChanged(int) ), SLOT( viewThresholds() ) );
    
    // cada cop que es modifiqui l'overlay mostrarem un volum diferent (per la edició)
    connect( m_2DView, SIGNAL( overlayModified() ), SLOT( updateVolume() ) );
}

void QEdemaSegmentationExtension::setInput( Volume *input )
{
    //m_mainVolume = new Volume();
    //Prova pel mètode de FastMarching
/*    typedef itk::LinearInterpolateImageFunction< Volume::ItkImageType, double > InterpolatorType;
    typedef itk::ResampleImageFilter<Volume::ItkImageType,Volume::ItkImageType> ResampleImageFilterType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    ResampleImageFilterType::Pointer resample    = ResampleImageFilterType::New();

    Volume::ItkImageType::SizeType size = input->getItkData()->GetLargestPossibleRegion().GetSize();
    size[0]=size[0]/2;
    size[1]=size[1]*(input->getItkData()->GetSpacing()[1]/(2*input->getItkData()->GetSpacing()[0]));
    size[2]=size[2]*(input->getItkData()->GetSpacing()[2]/(2*input->getItkData()->GetSpacing()[0]));

    double spacing[3];
    //Posem que l'espaiat sigui el mateix en totes direccions
    spacing[0]=2*input->getItkData()->GetSpacing()[0];
    spacing[1]=2*input->getItkData()->GetSpacing()[0];
    spacing[2]=2*input->getItkData()->GetSpacing()[0];

    std::cout<<"spacing: "<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<std::endl;
    std::cout<<"size: "<<size[0]<<" "<<size[1]<<" "<<size[2]<<std::endl;
    interpolator->SetInputImage(input->getItkData());
    resample->SetInput(input->getItkData());
    resample->SetInterpolator(interpolator.GetPointer());
    resample->SetSize(size);
    resample->SetOutputOrigin(input->getItkData()->GetOrigin());
    resample->SetOutputSpacing(spacing);
    resample->SetDefaultPixelValue( 100 );

    resample->Update();

    m_mainVolume->setData(resample->GetOutput());
    m_mainVolume->getVtkData()->Update();
    */
    //Descomentar això quan no vulguem la prova!!!!!
    m_mainVolume = input;

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView->removeAnnotation(Q2DViewer::ScalarBarAnnotation);
    //m_2DView->resetWindowLevelToDefault();
    m_2DView->setOverlayToBlend();

    int* dim;
    dim = m_mainVolume->getDimensions();
    DEBUG_LOG( QString("dims Vol: %1, %2, %3").arg(dim[0]).arg(dim[1]).arg(dim[2]) );
    DEBUG_LOG("**********************************************************************************************");
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    //Posem els nivells de dins i fora de la màscara els valors del w/l per tal que es vegi correcte
    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_insideValue  = (int) wl[0];
    m_outsideValue = (int) (wl[0] - 2.0*wl[1]);
    m_insideValue  = 255;
    m_outsideValue = 0;

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
    //empirical values!!
    m_lowerValueSlider->setValue(35);
    m_upperValueSlider->setValue(150);

    m_lowerValueVentriclesSpinBox->setMinimum(m_minValue);
    m_lowerValueVentriclesSpinBox->setMaximum(m_maxValue);
    m_upperValueVentriclesSpinBox->setMinimum(m_minValue);
    m_upperValueVentriclesSpinBox->setMaximum(m_maxValue);
    m_lowerValueVentriclesSlider->setMinimum(m_minValue);
    m_lowerValueVentriclesSlider->setMaximum(m_maxValue);
    m_upperValueVentriclesSlider->setMinimum(m_minValue);
    m_upperValueVentriclesSlider->setMaximum(m_maxValue);
    //empirical values!!
    m_lowerValueVentriclesSlider->setValue(15);
    m_upperValueVentriclesSlider->setValue(150);

    //Esborrem els volum per si hem fet un canvi de sèrie
    if(m_filteredVolume)
    {
        delete m_filteredVolume;
        m_filteredVolume = 0;
    }
    if(m_lesionMaskVolume)
    {
        delete m_lesionMaskVolume;
        m_lesionMaskVolume = 0;
    }
    if(m_ventriclesMaskVolume)
    {
        delete m_ventriclesMaskVolume;
        m_ventriclesMaskVolume = 0;
    }

    if(m_imageThreshold)
    {
        m_imageThreshold->Delete();
        m_imageThreshold = 0;
    }
    if(m_edemaMaskVolume)
    {
        delete m_edemaMaskVolume;
        m_edemaMaskVolume = 0;
    }
}

void QEdemaSegmentationExtension::applyFilterMainImage()
{
    if(m_filteredVolume == 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_segMethod->setVolume(m_mainVolume);
        m_filteredVolume = new Volume();
        m_segMethod->applyFilter(m_filteredVolume);
        m_segMethod->setVolume(m_filteredVolume);
        m_2DView->setInput( m_filteredVolume );
        m_2DView->refresh();
        //delete m_mainVolume;
        QApplication::restoreOverrideCursor();
    }
}

void QEdemaSegmentationExtension::applyCleanSkullMethod()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_volume = m_segMethod->applyCleanSkullMethod();
    //std::cout<<"Hem sortit de l'abisme!!!"<<std::endl;
    m_cont = m_segMethod->getNumberOfVoxels();

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));

    m_2DView->refresh();
    QApplication::restoreOverrideCursor();
}

void QEdemaSegmentationExtension::applyMethod()
{
    if(!m_isSeed || !m_isMask)
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "ERROR: Seed or mask undefined" ) );
        return;
    }

    if( !m_filteredVolume )
    {
        m_segMethod->setVolume(m_mainVolume);
    }
    else
    {
        m_segMethod->setVolume(m_filteredVolume);
    }
    
    if( !m_lesionMaskVolume )
    {
        m_lesionMaskVolume = new Volume();
    }
    m_segMethod->setMask(m_lesionMaskVolume);
    DEBUG_LOG("Inici apply method!!");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_segMethod->setInsideMaskValue ( m_insideValue );
    m_segMethod->setOutsideMaskValue( m_outsideValue );
    m_segMethod->setSeedPosition(m_seedPosition[0], m_seedPosition[1], m_seedPosition[2]);
    m_volume = m_segMethod->applyMethod();
    //m_volume = m_segMethod->applyMethodVTK();//No funciona!!
    m_cont = m_segMethod->getNumberOfVoxels();

    DEBUG_LOG("FI apply filter!!");

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_lesionMaskVolume);

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    m_resultsLineEdit->setEnabled(true);
    m_resultsLabel->setEnabled(true);
    m_updateVolumeButton->setEnabled(true);
    m_applyVentriclesMethodButton->setEnabled(true);
    m_applyCleanSkullButton->setEnabled(true);
    m_toolManager->triggerTool("EditorTool");

    m_lesionViewAction->setEnabled( true );
    m_lesionViewAction->trigger();
    this->viewLesionOverlay();
    m_2DView->refresh();
    QApplication::restoreOverrideCursor();
    DEBUG_LOG("Fi apply method!!");
 }

void QEdemaSegmentationExtension::applyVentriclesMethod()
{
    if( !m_ventriclesMaskVolume )
    {
        m_ventriclesMaskVolume = new Volume();
    }

    if( !m_imageThreshold )
    {
        m_imageThreshold = vtkImageThreshold::New();
    }
    m_imageThreshold->SetInput( m_mainVolume->getVtkData() );
    m_lowerVentriclesValue = m_lowerValueVentriclesSlider->value();
    m_upperVentriclesValue = m_upperValueVentriclesSlider->value();
    m_imageThreshold->ThresholdBetween( m_lowerVentriclesValue,  m_upperVentriclesValue);
    m_imageThreshold->SetInValue( m_outsideValue );   //Inverse mask --> we want < lower or > upper
    m_imageThreshold->SetOutValue( m_insideValue );
    m_imageThreshold->Update();

    m_ventriclesMaskVolume->setData(m_imageThreshold->GetOutput());
    m_ventriclesViewAction->setEnabled( true );
    m_ventriclesViewAction->trigger();
    this->viewVentriclesOverlay();
    m_applyEdemaMethodButton->setEnabled(true);

}

void QEdemaSegmentationExtension::applyEdemaMethod()
{
    //std::cout<<"Init apply filter Edema!!"<<std::endl;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(m_edemaMaskVolume == 0)
    {
        m_edemaMaskVolume  = new Volume();
    }

    if(m_filteredVolume == 0)
    {
        m_segMethod->setVolume(m_mainVolume);
    }
    else
    {
        m_segMethod->setVolume(m_filteredVolume);
    }
    m_segMethod->setMask(m_lesionMaskVolume);
    //m_segMethod->setInitialDistance(m_distanceEdit->text().toDouble());
    m_segMethod->setMean(m_meanEdit->text().toDouble());
    m_segMethod->setVariance(m_varianceEdit->text().toDouble());
    m_segMethod->setConstant(m_constantEdit->text().toDouble());
    m_segMethod->setStoppingTime(m_stoppingTimeEdit->text().toDouble());
    m_segMethod->setMultiplier(m_multiplierEdit->text().toDouble());
    m_segMethod->setAlpha(m_alphaEdit->text().toDouble());
    m_segMethod->setLowerVentriclesThreshold(m_lowerVentriclesValue);
    m_segMethod->setUpperVentriclesThreshold(m_upperVentriclesValue);
    //std::cout<<" Init apply filter Edema!!"<<std::endl;
    m_edemaVolume = m_segMethod->applyMethodEdema(m_edemaMaskVolume);
    m_edemaCont = m_segMethod->getEdemaNumberOfVoxels();
    m_edemaVolumeLineEdit->clear();
    m_edemaVolumeLineEdit->insert(QString("%1").arg(m_edemaVolume, 0, 'f', 2));
    m_edemaViewAction->setEnabled( true );
    m_edemaViewAction->trigger();
    //std::cout<<"FI apply filter Edema, Init view!!"<<std::endl;
    this->viewEdemaOverlay();
    QApplication::restoreOverrideCursor();
    //std::cout<<"Edema cont: "<<m_edemaCont<<", "<<(*m_activedCont)<<std::endl;
    //std::cout<<"Edema vol: "<<m_edemaVolume<<", "<<(*m_activedVolume)<<std::endl;
    //std::cout<<"FI apply filter Edema!!"<<std::endl;
}

void QEdemaSegmentationExtension::setSeedPosition(double x,double y, double z)
{
    m_seedPosition[0] = x;
    m_seedPosition[1] = y;
    m_seedPosition[2] = z;

    m_seedXLineEdit->setText( QString::number(m_seedPosition[0], 'f', 1) );
    m_seedYLineEdit->setText( QString::number(m_seedPosition[1], 'f', 1) );
    m_seedZLineEdit->setText( QString::number(m_seedPosition[2], 'f', 1) );
    m_isSeed=true;
    if(m_isMask)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QEdemaSegmentationExtension::setOpacity( int op )
{
    if(m_activedMaskVolume)
    {
        m_2DView->setOpacityOverlay(((double)op)/100.0);
        m_2DView->setOverlayInput(m_activedMaskVolume);
        m_2DView->refresh();
    }
}

void QEdemaSegmentationExtension::setLowerValue( int x )
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

void QEdemaSegmentationExtension::setUpperValue( int x )
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

void QEdemaSegmentationExtension::updateVolume()
{
    m_volume = this->calculateMaskVolume();
    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
}

void QEdemaSegmentationExtension::viewThresholds()
{
    if(m_lesionMaskVolume == 0)
    {
        m_lesionMaskVolume = new Volume();
        DEBUG_LOG("*");
    }
    DEBUG_LOG( QString("Extent Vol: %1 %2 %3 %4 %5 %6").arg( m_mainVolume->getWholeExtent()[0] ).arg( m_mainVolume->getWholeExtent()[1] ).arg( m_mainVolume->getWholeExtent()[2] ).arg( m_mainVolume->getWholeExtent()[3] ).arg( m_mainVolume->getWholeExtent()[4] ).arg( m_mainVolume->getWholeExtent()[5] ) );

    DEBUG_LOG( QString("Extent Vol Lesion: %1 %2 %3 %4 %5 %6").arg( m_lesionMaskVolume->getWholeExtent()[0] ).arg( m_lesionMaskVolume->getWholeExtent()[1] ).arg( m_lesionMaskVolume->getWholeExtent()[2] ).arg( m_lesionMaskVolume->getWholeExtent()[3] ).arg( m_lesionMaskVolume->getWholeExtent()[4] ).arg( m_lesionMaskVolume->getWholeExtent()[5] ) );

    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_mainVolume->getVtkData() );
    imageThreshold->ThresholdBetween( m_lowerValueSlider->value(),  m_upperValueSlider->value());
    imageThreshold->SetInValue( m_insideValue );
    imageThreshold->SetOutValue( m_outsideValue );
    DEBUG_LOG( QString("min: %1, mout %2").arg(m_insideValue).arg(m_outsideValue) );
    imageThreshold->Update();
    DEBUG_LOG( QString("min: %1, mout %2").arg(m_insideValue).arg(m_outsideValue) );

    m_lesionMaskVolume->setData(imageThreshold->GetOutput() );
    DEBUG_LOG( QString("min: %1, mout %2").arg(m_insideValue).arg(m_outsideValue) );

    this->viewLesionOverlay();
    DEBUG_LOG( QString("min: %1, mout %2").arg(m_insideValue).arg(m_outsideValue) );
    imageThreshold->Delete();

    m_2DView->refresh();
}

void QEdemaSegmentationExtension::viewEdemaOverlay()
{
    if(m_edemaMaskVolume)
    {
        m_activedMaskVolume = m_edemaMaskVolume;
        m_activedCont = &m_edemaCont;
        m_activedVolume = &m_edemaVolume;
        m_2DView->setOverlayToBlend();
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayInput(m_edemaMaskVolume);
        m_2DView->refresh();
    }
}

void QEdemaSegmentationExtension::viewLesionOverlay()
{
    if(m_lesionMaskVolume)
    {
        m_activedMaskVolume = m_lesionMaskVolume;
        m_activedCont = &m_cont;
        m_activedVolume = &m_volume;
        m_2DView->setOverlayToBlend();
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->refresh();
        DEBUG_LOG( QString("Extent les: %1 %2 %3 %4 %5 %6").arg( m_lesionMaskVolume->getWholeExtent()[0] ).arg( m_lesionMaskVolume->getWholeExtent()[1] ).arg( m_lesionMaskVolume->getWholeExtent()[2] ).arg( m_lesionMaskVolume->getWholeExtent()[3] ).arg( m_lesionMaskVolume->getWholeExtent()[4] ).arg( m_lesionMaskVolume->getWholeExtent()[5] ) );
        DEBUG_LOG( QString("Extent Vol: %1 %2 %3 %4 %5 %6").arg( m_mainVolume->getWholeExtent()[0] ).arg( m_mainVolume->getWholeExtent()[1] ).arg( m_mainVolume->getWholeExtent()[2] ).arg( m_mainVolume->getWholeExtent()[3] ).arg( m_mainVolume->getWholeExtent()[4] ).arg( m_mainVolume->getWholeExtent()[5] ) );
    }
}

void QEdemaSegmentationExtension::viewVentriclesOverlay()
{
    if(m_ventriclesMaskVolume != 0)
    {
        m_activedMaskVolume = m_ventriclesMaskVolume;
        m_activedCont = &m_ventriclesCont;
        m_activedVolume = &m_ventriclesVolume;
        m_2DView->setOverlayToBlend();
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayInput(m_ventriclesMaskVolume);
        m_2DView->refresh();
    }
}

double QEdemaSegmentationExtension::calculateMaskVolume()
{
    if( !m_lesionMaskVolume )
        return 0.0;

    double spacing[3];
    m_lesionMaskVolume->getSpacing(spacing);
    double volume = 1.0;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    int cont;
    EditorToolData *editorToolData = static_cast<EditorToolData*> ( m_2DView->getToolProxy()->getTool("EditorTool")->getToolData() );
    if( editorToolData!=0 )
    {
        cont = editorToolData->getVolumeVoxels();
    }
    else
    {
        DEBUG_LOG("No existeix la editor tool");
        cont = 0;
    }

    volume = volume*(double)cont;

    return volume;
}

void QEdemaSegmentationExtension::saveActivedMaskVolume()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)") );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).suffix() != "mhd" )
        {
            fileName += ".mhd";
        }
        //Forcem que la màscara que gaurdem el dins sigui 255 i el fora 0
        vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
        imageThreshold->SetInput( m_activedMaskVolume->getVtkData() );
        imageThreshold->ThresholdBetween( m_insideValue , m_insideValue); // només els que valen m_insideValue
        imageThreshold->SetInValue( 255 );
        imageThreshold->SetOutValue( 0 );

        m_savingMaskDirectory = QFileInfo( fileName ).absolutePath();
        vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
        writer->SetFileName(qPrintable( fileName ));
        writer->SetFileDimensionality(3);
        writer->SetInput(imageThreshold->GetOutput());
        writer->Write();

        writer->Delete();
        imageThreshold->Delete();
    }
}

void QEdemaSegmentationExtension::readSettings()
{
    Settings settings;

    settings.restoreGeometry( EdemaSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter );
    settings.restoreGeometry( EdemaSegmentationSettings::VerticalSplitterGeometry, m_verticalSplitter );
    m_savingMaskDirectory = settings.getValue( EdemaSegmentationSettings::SavedMaskPath ).toString();
    
}

void QEdemaSegmentationExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry( EdemaSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter );
    settings.saveGeometry( EdemaSegmentationSettings::VerticalSplitterGeometry, m_verticalSplitter );
    settings.setValue( EdemaSegmentationSettings::SavedMaskPath, m_savingMaskDirectory );
}

}
