/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qedemasegmentationextension.h"

#include "strokesegmentationmethod.h"
#include "toolsactionfactory.h"
#include "volume.h"
#include "logging.h"
//#include "qhistogram2d.h"
#include "q2dviewer.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>

// VTK
#include <vtkRenderer.h>
#include <vtkImageMask.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageThreshold.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageIterator.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkContourGrid.h>
#include <vtkCommand.h>
#include <vtkMetaImageWriter.h>
/*#include <vtkDataSetMapper.h>
#include <vtkThreshold.h>
*/

// ITK
#include <itkBinaryThresholdImageFilter.h>

// prova isomètric
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

//prova recte
#include "itkRescaleIntensityImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkExtractImageFilter.h"


namespace udg {

QEdemaSegmentationExtension::QEdemaSegmentationExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume     = 0;
    m_lesionMaskVolume = 0;
    m_edemaMaskVolume  = 0;
    m_ventriclesMaskVolume = 0;
    m_activedMaskVolume = 0;
    m_filteredVolume   = 0;

    m_imageThreshold = 0;
    //m_fusionVolume   = 0;
    //m_vtkFusionImage = 0;

    m_volume = 0.0;
    m_edemaVolume = 0.0;

    m_segMethod = new StrokeSegmentationMethod();

    m_isSeed  = false;
    m_isMask  = false;
    m_editorTool = QEdemaSegmentationExtension::NoEditor;
    m_isLeftButtonPressed = false;
    m_cont = 0;
    m_edemaCont = 0;
    m_ventriclesCont = 0;

    //pointActor = vtkActor::New();
    squareActor = vtkActor::New();

    createActions();
    createToolBars();
    createConnections();

    readSettings();
}

QEdemaSegmentationExtension::~QEdemaSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    //delete m_lesionMaskVolume;//TODO descomentar aix�per tal d'alliberar el m_lesionMaskVolume (ara peta)
    //pointActor  -> Delete();
    squareActor -> Delete();
    if(m_filteredVolume != 0)
    {
        delete m_filteredVolume;
    }
    if(m_lesionMaskVolume != 0)
    {
        delete m_lesionMaskVolume;
    }
    if(m_ventriclesMaskVolume != 0)
    {
        delete m_ventriclesMaskVolume;
    }
    if(m_edemaMaskVolume != 0)
    {
        delete m_edemaMaskVolume;
    }
    if(m_imageThreshold != 0)
    {
        m_imageThreshold->Delete();
    }

}

void QEdemaSegmentationExtension::createActions()
{
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

    m_seedAction = m_actionFactory->getActionFrom( "SeedTool" );
    m_seedAction->setIcon( QIcon(":/images/seed.png") );
    m_seedToolButton->setDefaultAction( m_seedAction );

    m_editorAction = new QAction( 0 );
    m_editorAction->setText( tr("EditorTool") );
    m_editorAction->setStatusTip( tr("Enable/Disable editor tool") );
    m_editorAction->setIcon( QIcon(":/images/pencil.png") );
    m_editorAction->setCheckable( true );
    m_editorAction->setEnabled( false );
    m_editorToolButton->setDefaultAction( m_editorAction );


    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_seedAction );
    m_toolsActionGroup->addAction( m_editorAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_seedAction->trigger();

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
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?

    m_paintEditorAction = new QAction( 0 );
    m_paintEditorAction->setText( tr("Paint Editor Tool") );
    m_paintEditorAction->setStatusTip( tr("Enable/Disable Paint Editor") );
    m_paintEditorAction->setIcon( QIcon(":/images/airbrush.png") );
    m_paintEditorAction->setCheckable( true );
    m_paintEditorAction->setEnabled( false );
    m_paintButton->setDefaultAction( m_paintEditorAction );

    m_eraseEditorAction = new QAction( 0 );
    m_eraseEditorAction->setText( tr("Erase Editor Tool") );
    m_eraseEditorAction->setStatusTip( tr("Enable/Disable Erase Editor") );
    m_eraseEditorAction->setIcon( QIcon(":/images/eraser.png") );
    m_eraseEditorAction->setCheckable( true );
    m_eraseEditorAction->setEnabled( false );
    m_eraseButton->setDefaultAction( m_eraseEditorAction );

    m_eraseSliceEditorAction = new QAction( 0 );
    m_eraseSliceEditorAction->setText( tr("Erase Slice Editor Tool") );
    m_eraseSliceEditorAction->setStatusTip( tr("Enable/Disable Erase Slice Editor") );
    m_eraseSliceEditorAction->setIcon( QIcon(":/images/axial.png") );
    m_eraseSliceEditorAction->setCheckable( true );
    m_eraseSliceEditorAction->setEnabled( false );
    m_eraseSliceButton->setDefaultAction( m_eraseSliceEditorAction );

    m_eraseRegionEditorAction = new QAction( 0 );
    m_eraseRegionEditorAction->setText( tr("Erase Region Editor Tool") );
    m_eraseRegionEditorAction->setStatusTip( tr("Enable/Disable Erase Region Editor") );
    m_eraseRegionEditorAction->setIcon( QIcon(":/images/move.png") );//A falta d'una millor!!!
    m_eraseRegionEditorAction->setCheckable( true );
    m_eraseRegionEditorAction->setEnabled( false );
    m_eraseRegionButton->setDefaultAction( m_eraseRegionEditorAction );

    m_editorToolActionGroup = new QActionGroup( 0 );
    m_editorToolActionGroup->setExclusive( true );
    m_editorToolActionGroup->addAction( m_paintEditorAction );
    m_editorToolActionGroup->addAction( m_eraseEditorAction );
    m_editorToolActionGroup->addAction( m_eraseSliceEditorAction );
    m_editorToolActionGroup->addAction( m_eraseRegionEditorAction );


}

void QEdemaSegmentationExtension::createToolBars()
{
}

void QEdemaSegmentationExtension::createConnections()
{
  connect( m_filterPushButton , SIGNAL( clicked() ) , this , SLOT( ApplyFilterMainImage() ) );

  connect( m_applyMethodButton , SIGNAL( clicked() ) , this , SLOT( ApplyMethod() ) );

  connect( m_applyCleanSkullButton , SIGNAL( clicked() ) , this , SLOT( ApplyCleanSkullMethod() ) );

  connect( m_applyVentriclesMethodButton , SIGNAL( clicked() ) , this , SLOT( ApplyVentriclesMethod() ) );

  connect( m_applyEdemaMethodButton , SIGNAL( clicked() ) , this , SLOT( ApplyEdemaMethod() ) );

  connect( m_lesionViewToolButton , SIGNAL( clicked() ) , this , SLOT( viewLesionOverlay() ) );

  connect( m_edemaViewToolButton , SIGNAL( clicked() ) , this , SLOT( viewEdemaOverlay() ) );

  connect( m_ventriclesViewToolButton , SIGNAL( clicked() ) , this , SLOT( viewVentriclesOverlay() ) );

  connect( m_eraseButton , SIGNAL( clicked() ) , this , SLOT( setErase() ) );

  connect( m_eraseSliceButton , SIGNAL( clicked() ) , this , SLOT( setEraseSlice() ) );

  connect( m_eraseRegionButton , SIGNAL( clicked() ) , this , SLOT( setEraseRegion() ) );

  connect( m_paintButton , SIGNAL( clicked() ) , this , SLOT( setPaint() ) );

  connect( m_updateVolumeButton , SIGNAL( clicked() ) , this , SLOT( updateVolume() ) );

  connect( m_viewThresholdButton , SIGNAL( clicked() ) , this , SLOT( viewThresholds() ) );

  connect( m_2DView , SIGNAL( eventReceived( unsigned long ) ) , this , SLOT( strokeEventHandler(unsigned long) ) );

  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );

  connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ) , this , SLOT( setLowerValue(int) ) );

  connect( m_upperValueSlider, SIGNAL( valueChanged(int) ) , this , SLOT( setUpperValue(int) ) );

  connect( m_opacitySlider, SIGNAL( valueChanged(int) ) , this , SLOT( setOpacity(int) ) );

  connect( m_2DView, SIGNAL( seedChanged() ) , this , SLOT( setSeedPosition() ) );

  connect( m_saveMaskPushButton, SIGNAL( clicked() ) , this , SLOT( saveActivedMaskVolume() ) );
}

void QEdemaSegmentationExtension::setInput( Volume *input )
{
    m_mainVolume = new Volume();
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
    m_2DView->setView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();
    m_2DView->setOverlayToBlend();

    int* dim;
    dim = m_mainVolume->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getSlice());

    //Posem els nivells de dins i fora de la m�cara els valors l�its del w/l per tal que es vegi correcte
    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_insideValue  = (int) wl[0];
    m_outsideValue = (int) (wl[0] - 2.0*wl[1]);
    m_insideValue  = 255;
    m_outsideValue = 0;

    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator iter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );

    m_minValue = iter.Get();
    m_maxValue = m_minValue;

    Volume::ItkImageType::PixelType value;

    while ( !iter.IsAtEnd() )
    {
        value = iter.Get();

        if ( value < m_minValue ) { m_minValue = value; }
        if ( value > m_maxValue ) { m_maxValue = value; }

        ++iter;
    }
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


    m_2DView->render();

}

void QEdemaSegmentationExtension::ApplyFilterMainImage( )
{
    if(m_filteredVolume == 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_segMethod->setVolume(m_mainVolume);
        m_filteredVolume = new Volume();
        m_segMethod->applyFilter(m_filteredVolume);
        m_segMethod->setVolume(m_filteredVolume);
        m_2DView->setInput( m_filteredVolume );
        m_2DView->getInteractor()->Render();
        //delete m_mainVolume;
        QApplication::restoreOverrideCursor();
    }

}

void QEdemaSegmentationExtension::ApplyCleanSkullMethod( )
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_volume = m_segMethod->applyCleanSkullMethod();
    //std::cout<<"Hem sortit de l'abisme!!!"<<std::endl;
    m_cont = m_segMethod->getNumberOfVoxels();

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));

    m_2DView->getInteractor()->Render();
    QApplication::restoreOverrideCursor();
}

void QEdemaSegmentationExtension::ApplyMethod( )
{
    if(!m_isSeed || !m_isMask){
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "ERROR: no hi ha definida llavor o màscara" ) );
        return;
    }

    if(m_filteredVolume == 0)
    {
        m_segMethod->setVolume(m_mainVolume);
    }
    else
    {
        m_segMethod->setVolume(m_filteredVolume);
    }
    if(m_lesionMaskVolume == 0)
    {
        m_lesionMaskVolume = new Volume();
    }
    m_segMethod->setMask(m_lesionMaskVolume);
    std::cout<<"Inici Apply method!!"<<std::endl;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_segMethod->setInsideMaskValue ( m_insideValue );
    m_segMethod->setOutsideMaskValue( m_outsideValue );
    double pos[3];
    m_2DView->getSeedPosition(pos);
    m_segMethod->setSeedPosition(pos[0],pos[1],pos[2]);
    m_volume = m_segMethod->applyMethod();
    //m_volume = m_segMethod->applyMethodVTK();//No funciona!!
    m_cont = m_segMethod->getNumberOfVoxels();

     std::cout<<"FI Apply filter!!"<<std::endl;

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_lesionMaskVolume);

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    m_resultsLineEdit->setEnabled(true);
    m_resultsLabel->setEnabled(true);
    m_updateVolumeButton->setEnabled(true);
    m_eraseButton->setEnabled(true);
    m_paintButton->setEnabled(true);
    m_eraseSliceButton->setEnabled(true);
    m_eraseRegionButton->setEnabled(true);
    m_editorSize->setEnabled(true);
    m_applyVentriclesMethodButton->setEnabled(true);
    m_applyCleanSkullButton->setEnabled(true);
    m_editorAction->trigger();
    m_2DView->disableTools();
    m_editorAction->setEnabled( true );

    m_paintEditorAction->setEnabled(true);
    m_eraseEditorAction->setEnabled(true);
    m_eraseSliceEditorAction->setEnabled(true);
    m_eraseRegionEditorAction->setEnabled(true);
    m_eraseEditorAction->trigger();
    m_editorTool = QEdemaSegmentationExtension::Erase;

    m_lesionViewAction->setEnabled( true );
    m_lesionViewAction->trigger( );
    this->viewLesionOverlay();
    //m_2DView->getInteractor()->Render();
    QApplication::restoreOverrideCursor();
    std::cout<<"Fi Apply method!!"<<std::endl;
 }

void QEdemaSegmentationExtension::ApplyVentriclesMethod( )
{
    if(m_ventriclesMaskVolume == 0)
    {
        m_ventriclesMaskVolume = new Volume();
    }

    if(m_imageThreshold == 0)
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
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    m_ventriclesMaskVolume->setImages( m_mainVolume->getImages() );
    m_ventriclesViewAction->setEnabled( true );
    m_ventriclesViewAction->trigger( );
    this->viewVentriclesOverlay();
    m_applyEdemaMethodButton->setEnabled(true);

 }


void QEdemaSegmentationExtension::ApplyEdemaMethod( )
{
    //std::cout<<"Init Apply filter Edema!!"<<std::endl;
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
    //std::cout<<" Init Apply filter Edema!!"<<std::endl;
    m_edemaVolume = m_segMethod->applyMethodEdema(m_edemaMaskVolume);
    m_edemaCont = m_segMethod->getEdemaNumberOfVoxels();
    m_edemaVolumeLineEdit->clear();
    m_edemaVolumeLineEdit->insert(QString("%1").arg(m_edemaVolume, 0, 'f', 2));
    m_edemaViewAction->setEnabled( true );
    m_edemaViewAction->trigger( );
    //std::cout<<"FI Apply filter Edema, Init view!!"<<std::endl;
    this->viewEdemaOverlay();
    QApplication::restoreOverrideCursor();
    //std::cout<<"Edema cont: "<<m_edemaCont<<", "<<(*m_activedCont)<<std::endl;
    //std::cout<<"Edema vol: "<<m_edemaVolume<<", "<<(*m_activedVolume)<<std::endl;
    //std::cout<<"FI Apply filter Edema!!"<<std::endl;
}


void QEdemaSegmentationExtension::strokeEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        setPaintCursor();
    break;

    case vtkCommand::LeftButtonPressEvent:
        leftButtonEventHandler();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        setLeftButtonOff();
    break;

    case vtkCommand::RightButtonPressEvent:
    break;

    default:
    break;
    }

}

void QEdemaSegmentationExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;

    if(m_editorToolButton->isChecked())
    {
        //std::cout<<"Editor Tool"<<std::endl;
        m_2DView->disableTools();
        setEditorPoint(  );
    }
    else
    {
        m_2DView->enableTools();
    }
}

void QEdemaSegmentationExtension::setSeedPosition( )
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

void QEdemaSegmentationExtension::setEditorPoint(  )
{
    double pos[3];
    if(m_editorTool != QEdemaSegmentationExtension::NoEditor)
    {
        m_2DView->updateVoxelInformation();
        m_2DView->getCurrentCursorPosition(pos);

        // quan dona una posici�� de (-1, -1, -1) � que estem fora de l'actor
        if(!( pos[0] == -1 && pos[1] == -1 && pos[2] == -1) )
        {
            switch( m_editorTool)
            {
                case Erase:
                {
                    this->eraseMask(m_editorSize->value());
                    break;
                }
                case Paint:
                {
                    this->paintMask(m_editorSize->value());
                    break;
                }
                case EraseSlice:
                {
                    this->eraseSliceMask();
                    break;
                }
                case EraseRegion:
                {
                    this->eraseRegionMask();
                    break;
                }
            }
            double volume = this->updateMaskVolume();
            m_resultsLineEdit->clear();
            m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
            m_edemaVolumeLineEdit->clear();
            m_edemaVolumeLineEdit->insert(QString("%1").arg(m_edemaVolume, 0, 'f', 2));
            m_2DView->setOverlayInput(m_activedMaskVolume);
            m_2DView->getInteractor()->Render();
        }
    }
}

void QEdemaSegmentationExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}

void QEdemaSegmentationExtension::setOpacity( int op )
{
    if(m_activedMaskVolume != 0)
    {
        m_2DView->setOpacityOverlay(((double)op)/100.0);
        m_2DView->setOverlayInput(m_activedMaskVolume);
        m_2DView->getInteractor()->Render();
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

void QEdemaSegmentationExtension::setErase()
{
    m_editorTool = QEdemaSegmentationExtension::Erase;
}

void QEdemaSegmentationExtension::setPaint()
{
    m_editorTool = QEdemaSegmentationExtension::Paint;
}

void QEdemaSegmentationExtension::setEraseSlice()
{
    m_editorTool = QEdemaSegmentationExtension::EraseSlice;
}

void QEdemaSegmentationExtension::setEraseRegion()
{
    m_editorTool = QEdemaSegmentationExtension::EraseRegion;
}


void QEdemaSegmentationExtension::setPaintCursor()
{
    if(m_editorToolButton->isChecked())    //Nom� en cas que estiguem en l'editor
    {
        if(m_isLeftButtonPressed)
        {
            setEditorPoint();
        }

        double pos[3];
        m_2DView->getCurrentCursorPosition(pos);
        if((m_editorTool == QEdemaSegmentationExtension::Erase || m_editorTool == QEdemaSegmentationExtension::Paint)&&(!( pos[0] == -1 && pos[1] == -1 && pos[2] == -1) ))
        {
            int size = m_editorSize->value();
            double spacing[3];
            m_lesionMaskVolume->getSpacing(spacing);

            vtkPoints *points = vtkPoints::New();
            points->SetNumberOfPoints(4);

            double sizeView[2];
            sizeView[0]=(double)size*spacing[0];
            sizeView[1]=(double)size*spacing[1];

            points->SetPoint(0, pos[0] - sizeView[0], pos[1] - sizeView[1], pos[2]-1);
            points->SetPoint(1, pos[0] + sizeView[0], pos[1] - sizeView[1], pos[2]-1);
            points->SetPoint(2, pos[0] + sizeView[0], pos[1] + sizeView[1], pos[2]-1);
            points->SetPoint(3, pos[0] - sizeView[0], pos[1] + sizeView[1], pos[2]-1);


            vtkIdType pointIds[4];

            pointIds[0] = 0;
            pointIds[1] = 1;
            pointIds[2] = 2;
            pointIds[3] = 3;


            vtkUnstructuredGrid*    grid = vtkUnstructuredGrid::New();

            grid->Allocate(1);
            grid->SetPoints(points);

            grid->InsertNextCell(VTK_QUAD,4,pointIds);

            squareActor -> GetProperty()->SetColor(0.15, 0.83, 0.26);
            squareActor -> GetProperty()->SetOpacity(0.2);

            vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
            squareMapper->SetInput( grid );

            squareActor->SetMapper( squareMapper );

            m_2DView->getRenderer()-> AddActor( squareActor );
            m_2DView->getInteractor()->Render();

            squareActor->VisibilityOn();

            squareMapper-> Delete();
            points      -> Delete();
            grid        -> Delete();
        }
        else
        {
            squareActor->VisibilityOff();
        }
    }
}

void QEdemaSegmentationExtension::eraseMask(int size)
{
    int i,j;
    int* value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_activedMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_activedMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getSlice();

    for(i=-size;i<=size;i++)
    {
        for(j=-size;j<=size;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value=(int*)m_activedMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) == m_insideValue)
            {
                (*value) = m_outsideValue;
                (*m_activedCont)--;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QEdemaSegmentationExtension::paintMask(int size)
{
    int i,j;
    int* value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_activedMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_activedMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getSlice();
    for(i=-size;i<=size;i++)
    {
        for(j=-size;j<=size;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value=(int*)m_activedMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) != m_insideValue)
            {
                (*value) = m_insideValue;
                (*m_activedCont)++;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QEdemaSegmentationExtension::eraseSliceMask()
{
    int i,j;
    int* value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    int ext[6];
    m_lesionMaskVolume->getVtkData()->GetExtent(ext);
    m_2DView->getCurrentCursorPosition(pos);
    m_activedMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_activedMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getSlice();
    //std::cout<<"Esborrant llesca "<<index[2]<<std::endl;
    for(i=ext[0];i<=ext[1];i++)
    {
        for(j=ext[2];j<=ext[3];j++)
        {
            index[0]=i;
            index[1]=j;
            value=(int*)m_activedMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) == m_insideValue)
            {
                (*value) = m_outsideValue;
                (*m_activedCont)--;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QEdemaSegmentationExtension::eraseRegionMask()
{
    double pos[3];
    double origin[3];
    double spacing[3];
    int index[3];
    int ext[6];
    m_lesionMaskVolume->getVtkData()->GetExtent(ext);
    m_2DView->getCurrentCursorPosition(pos);
    m_activedMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_activedMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    index[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    index[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getSlice();
    //std::cout<<"Esborrant llesca "<<index[2]<<std::endl;
    //eraseRegionMaskRecursive(index[0], index[1], index[2]);
    eraseRegionMaskRecursive(index[0],index[1],index[2]);
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QEdemaSegmentationExtension::eraseRegionMaskRecursive(int a, int b, int c)
{
    int ext[6];
    m_activedMaskVolume->getVtkData()->GetExtent(ext);
    //std::cout<<"Extension: ["<<ext[0]<<" "<<ext[1]<<", "<<ext[2]<<" "<<ext[3]<<", "<<ext[4]<<" "<<ext[5]<<"], Index: ["<<a<<", "<<b<<", "<<c<<"]"<<std::endl;
    if((a>=ext[0])&&(a<=ext[1])&&(b>=ext[2])&&(b<=ext[3])&&(c>=ext[4])&&(c<=ext[5]))
    {
        int index[3];
        index[0]=a;
        index[1]=b;
        index[2]=c;
        int* value=(int*)m_activedMaskVolume->getVtkData()->GetScalarPointer(index);
        if ((*value) == m_insideValue)
        {
            //std::cout<<m_outsideValue<<" "<<m_insideValue<<"->"<<(*value)<<std::endl;
            (*value)= m_outsideValue;
            (*m_activedCont)--;
            eraseRegionMaskRecursive( a+1, b, c);
            eraseRegionMaskRecursive( a-1, b, c);
            eraseRegionMaskRecursive( a, b+1, c);
            eraseRegionMaskRecursive( a, b-1, c);
        }
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
    }
    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_mainVolume->getVtkData() );
    imageThreshold->ThresholdBetween( m_lowerValueSlider->value(),  m_upperValueSlider->value());
    imageThreshold->SetInValue( m_insideValue );
    imageThreshold->SetOutValue( m_outsideValue );
    std::cout<<"min: "<<m_insideValue<<", mout: "<<m_outsideValue<<std::endl;
    imageThreshold->Update();

    m_lesionMaskVolume->setData(imageThreshold->GetOutput() );
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    m_lesionMaskVolume->setImages( m_mainVolume->getImages() );

    this->viewLesionOverlay();

}

void QEdemaSegmentationExtension::viewEdemaOverlay()
{
    if(m_edemaMaskVolume != 0)
    {
        m_activedMaskVolume = m_edemaMaskVolume;
        m_activedCont = &m_edemaCont;
        m_activedVolume = &m_edemaVolume;
        m_2DView->setOverlayToBlend();
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayInput(m_edemaMaskVolume);
        m_2DView->getInteractor()->Render();
    }
}

void QEdemaSegmentationExtension::viewLesionOverlay()
{
    if(m_lesionMaskVolume != 0)
    {
        m_activedMaskVolume = m_lesionMaskVolume;
        m_activedCont = &m_cont;
        m_activedVolume = &m_volume;
        m_2DView->setOverlayToBlend();
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->getInteractor()->Render();
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
        m_2DView->getInteractor()->Render();
    }
}

double QEdemaSegmentationExtension::calculateMaskVolume()
{

    if ( m_lesionMaskVolume == 0 ) return 0.0;

    int ext[6];
    int i,j,k;
    (*m_activedCont) = 0;
    m_lesionMaskVolume->getWholeExtent(ext);

    double spacing[3];
    m_lesionMaskVolume->getSpacing(spacing);
    double volume = 1;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    if(m_lesionMaskVolume->getVtkData()->GetScalarType()!=6)
    {
        std::cout<<"Compte!!! Mask Vtk Data Type != INT ("<<m_lesionMaskVolume->getVtkData()->GetScalarTypeAsString()<<")"<<std::endl;
    }

    int* value;
    value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer();
    for(i=ext[0];i<=ext[1];i++)
    {
        for(j=ext[2];j<=ext[3];j++)
        {
            for(k=ext[4];k<=ext[5];k++)
            {
                if ((*value) == m_insideValue)
                {
                    (*m_activedCont)++;
                }
                value++;
            }
        }
    }
    volume = volume*(double)(*m_activedCont);

    return volume;

}


double QEdemaSegmentationExtension::updateMaskVolume()
{

    if ( m_lesionMaskVolume == 0 ) return 0.0;

    double spacing[3];
    m_lesionMaskVolume->getSpacing(spacing);
    double volume = 1;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }


    (*m_activedVolume) = volume*(double)(*m_activedCont);
    //std::cout<<"vtkvolume ="<<volume<<std::endl;
    //std::cout<<"updateMask-> Edema cont: "<<m_edemaCont<<", "<<(*m_activedCont)<<std::endl;
    //std::cout<<"updateMask-> Edema cont: "<<m_edemaVolume<<", "<<(*m_activedVolume)<<", "<<volume<<std::endl;

    return (*m_activedVolume);

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
    QSettings settings("GGG", "StarViewer-App-EdemaSegmentation");
    settings.beginGroup("StarViewer-App-EdemaSegmentation");

    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );
    m_savingMaskDirectory = settings.value( "savingDirectory", "." ).toString();
    //std::cout<<"Saving directory: "<<qPrintable( m_savingMaskDirectory )<<std::endl;
    settings.endGroup();
}

void QEdemaSegmentationExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-EdemaSegmentation");
    settings.beginGroup("StarViewer-App-EdemaSegmentation");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );
    settings.setValue("savingDirectory", m_savingMaskDirectory );

    settings.endGroup();
}

}
