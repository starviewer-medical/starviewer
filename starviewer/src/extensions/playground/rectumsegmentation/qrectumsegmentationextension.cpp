/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qrectumsegmentationextension.h"

#include "rectumSegmentationMethod.h"
#include "toolsactionfactory.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "toolmanager.h"

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

// ITK
#include <itkBinaryThresholdImageFilter.h>

// prova isomètric
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

//prova recte
#include "itkRescaleIntensityImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkExtractImageFilter.h"

#include "itkImageFileWriter.h"

namespace udg {

QRectumSegmentationExtension::QRectumSegmentationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_lesionMaskVolume(0), m_imageThreshold(0), m_filteredVolume(0), m_isSeed(false), m_isMask(false),   m_isLeftButtonPressed(false), m_cont(0), m_volume(0.0), m_editorTool(QRectumSegmentationExtension::NoEditor), m_isRegionSet(false), m_isRegionSetting(false)
{
    setupUi( this );

    m_segMethod = new rectumSegmentationMethod();
    squareActor = vtkActor::New();
    squareRegionActor = vtkActor::New();

    createActions();
    createConnections();
    readSettings();

    // creem el tool manager i li assignem les tools. TODO de moment només tenim VoxelInformation, però s'han d'anar afegint la resta
    m_toolManager = new ToolManager(this);
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    QStringList toolsList;
    toolsList << "VoxelInformationTool";
    m_toolManager->setViewerTools( m_2DView, toolsList );
}

QRectumSegmentationExtension::~QRectumSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    squareActor->Delete();
    squareRegionActor->Delete();
    if(m_filteredVolume != 0)
    {
        delete m_filteredVolume;
    }
    if(m_lesionMaskVolume != 0)
    {
        delete m_lesionMaskVolume;
    }
    if(m_imageThreshold != 0)
    {
        m_imageThreshold->Delete();
    }
}

void QRectumSegmentationExtension::createActions()
{
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

    m_regionAction = new QAction( 0 );
    m_regionAction->setText( tr("RegionTool") );
    m_regionAction->setStatusTip( tr("Enable/Disable region tool") );
    m_regionAction->setCheckable( true );
    m_regionAction->setEnabled( true );
    m_regionToolButton->setDefaultAction( m_regionAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_seedAction );
    m_toolsActionGroup->addAction( m_editorAction );
    m_toolsActionGroup->addAction( m_regionAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_seedAction->trigger();

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

void QRectumSegmentationExtension::createConnections()
{
  connect( m_filterPushButton, SIGNAL( clicked() ), SLOT( ApplyFilterMainImage() ) );
  connect( m_applyMethodButton, SIGNAL( clicked() ), SLOT( ApplyMethod() ) );
  connect( m_eraseButton, SIGNAL( clicked() ), SLOT( setErase() ) );
  connect( m_eraseSliceButton, SIGNAL( clicked() ), SLOT( setEraseSlice() ) );
  connect( m_eraseRegionButton, SIGNAL( clicked() ), SLOT( setEraseRegion() ) );
  connect( m_paintButton, SIGNAL( clicked() ), SLOT( setPaint() ) );
  connect( m_updateVolumeButton, SIGNAL( clicked() ), SLOT( updateVolume() ) );
  connect( m_viewThresholdButton, SIGNAL( clicked() ), SLOT( viewThresholds() ) );
  connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), SLOT( strokeEventHandler(unsigned long) ) );
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
  connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ), SLOT( setLowerValue(int) ) );
  connect( m_upperValueSlider, SIGNAL( valueChanged(int) ), SLOT( setUpperValue(int) ) );
  connect( m_opacitySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
  connect( m_2DView, SIGNAL( seedChanged() ), SLOT( setSeedPosition() ) );
  connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
  connect( m_saveMaskPushButton, SIGNAL( clicked() ), SLOT( saveActivedMaskVolume() ) );
  connect( m_save3DPushButton, SIGNAL( clicked() ), SLOT( saveSegmentation3DVolume() ) );
  connect( m_viewROICheckBox, SIGNAL( stateChanged(int) ), SLOT( viewRegionState(int) ) );
  connect( m_toolsActionGroup, SIGNAL( triggered(QAction*) ), SLOT( toolChanged(QAction*) ) );
}

void QRectumSegmentationExtension::setInput( Volume *input )
{
    m_mainVolume = new Volume();
    std::cout<<"Inici d'input"<<std::endl;

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
    m_2DView->resetView( Q2DViewer::Axial );
    m_2DView->removeAnnotation( Q2DViewer::AllAnnotation );
    m_2DView->resetWindowLevelToDefault();
    m_2DView->setOverlayToBlend();

    int* dim;
    dim = m_mainVolume->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

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
    m_upperValueSlider->setValue(170);

    m_2DView->render();

}

void QRectumSegmentationExtension::ApplyFilterMainImage( )
{
    if(m_filteredVolume == 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_segMethod->setVolume(m_mainVolume);
        m_filteredVolume = new Volume();
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_filteredVolume->setImages( m_mainVolume->getImages() );
        m_segMethod->applyFilter(m_filteredVolume);
        m_segMethod->setVolume(m_filteredVolume);
        m_2DView->setInput( m_filteredVolume );
        m_2DView->refresh();
        //delete m_mainVolume;
        QApplication::restoreOverrideCursor();
    }

}

void QRectumSegmentationExtension::ApplyMethod( )
{
    if(!m_isSeed || !m_isMask){
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "ERROR: seed or mask undefined" ) );
        return;
    }
    if(!m_isRegionSet)
    {
        QMessageBox::warning( this , QObject::tr( "Starviewer" ) , QObject::tr( "No definite ROI" ) );
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
    m_segMethod->setMultiplier(m_multiplierEdit->text().toDouble());
    double pos[3];
    m_2DView->getSeedPosition(pos);
    m_segMethod->setSeedPosition(pos[0],pos[1],pos[2]);
    int x[2];
    int y[2];
    if(m_initialRegionPoint[0]<m_finalRegionPoint[0])
    {
        x[0]=(m_initialRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
        y[0]=(m_finalRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
    }else
    {
        x[0]=(m_finalRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
        y[0]=(m_initialRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
    }
    if(m_initialRegionPoint[1]<m_finalRegionPoint[1])
    {
        x[1]=(m_initialRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
        y[1]=(m_finalRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
    }else
    {
        x[1]=(m_finalRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
        y[1]=(m_initialRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
    }
    m_segMethod->setMinROI(x);
    m_segMethod->setMaxROI(y);

    m_volume = m_segMethod->applyMethod();
    m_cont = m_segMethod->getNumberOfVoxels();

    std::cout<<"FI Apply filter!!"<<std::endl;

    this->viewLesionOverlay();
//     m_2DView->setOverlayToBlend();
//     m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
//     m_2DView->setOverlayInput(m_lesionMaskVolume);

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
    m_editorAction->trigger();
    m_2DView->disableTools();
    m_editorAction->setEnabled( true );

    m_paintEditorAction->setEnabled(true);
    m_eraseEditorAction->setEnabled(true);
    m_eraseSliceEditorAction->setEnabled(true);
    m_eraseRegionEditorAction->setEnabled(true);
    m_eraseRegionEditorAction->trigger();
    m_editorTool = QRectumSegmentationExtension::EraseRegion;

/*    this->viewLesionOverlay();
    //m_2DView->refresh();
    m_cont = &m_cont;*/
    QApplication::restoreOverrideCursor();
    std::cout<<"Fi Apply method!!"<<std::endl;
 }


void QRectumSegmentationExtension::strokeEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        onMouseMoveEventHandler();
    break;

    case vtkCommand::LeftButtonPressEvent:
        leftButtonEventHandler();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        leftButtonReleaseHandler();
    break;

    case vtkCommand::RightButtonPressEvent:
    break;

    default:
    break;
    }

}

void QRectumSegmentationExtension::onMouseMoveEventHandler( )
{
    if(m_editorToolButton->isChecked())    //Només en cas que estiguem en l'editor
    {
        setPaintCursor();
    }
    else
    {
        if(m_regionToolButton->isChecked())    //Només en cas que estiguem en la selecció de regió d'interés
        {
            setMovingRegionOfInterest();
        }
    }
}

void QRectumSegmentationExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;

    if((m_editorToolButton->isChecked())||(m_regionToolButton->isChecked()))
    {
        m_2DView->disableTools();
        if(m_editorToolButton->isChecked())
        {
            setEditorPoint(  );
        }
        else
        {
            setRegionOfInterest();
        }
    }
    else
    {
        m_2DView->enableTools();
    }
}

void QRectumSegmentationExtension::leftButtonReleaseHandler( )
{
    setLeftButtonOff();
    if(m_regionToolButton->isChecked())    //Només en cas que estiguem en la selecció de regió d'interés
    {
        setReleaseRegionOfInterest();
    }
}

void QRectumSegmentationExtension::setSeedPosition( )
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

void QRectumSegmentationExtension::setRegionOfInterest( )
{
    std::cout<<"SetRegionOfInterest"<<std::endl;
    double pos[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_initialRegionPoint[0]= pos[0];
    m_initialRegionPoint[1]= pos[1];
    m_isRegionSetting = true;
}

void QRectumSegmentationExtension::setMovingRegionOfInterest( )
{
    if(m_isRegionSetting)
    {
        std::cout<<"isSettingRegionOfInterest"<<std::endl;
        double pos[3];
        double spacing[3];
        m_mainVolume->getSpacing(spacing);
        m_2DView->getCurrentCursorPosition(pos);
        m_finalRegionPoint[0]= pos[0];
        m_finalRegionPoint[1]= pos[1];
        std::cout<<"1"<<std::endl;

        vtkPoints *points = vtkPoints::New();
        points->SetNumberOfPoints(4);
        points->SetPoint(0, m_initialRegionPoint[0], m_initialRegionPoint[1], m_mainVolume->getOrigin()[2]+0.1);
        points->SetPoint(1, m_initialRegionPoint[0], m_finalRegionPoint[1], m_mainVolume->getOrigin()[2]+0.1);
        points->SetPoint(2, m_finalRegionPoint[0], m_finalRegionPoint[1], m_mainVolume->getOrigin()[2]+0.1);
        points->SetPoint(3, m_finalRegionPoint[0], m_initialRegionPoint[1], m_mainVolume->getOrigin()[2]+0.1);

        std::cout<<"Points: ["<<m_initialRegionPoint[0]<<","<<m_initialRegionPoint[1]<<"] ,["<<m_finalRegionPoint[0]<<","<<m_finalRegionPoint[1]<<"]"<<std::endl;

        vtkIdType pointIds[4];

        pointIds[0] = 0;
        pointIds[1] = 1;
        pointIds[2] = 2;
        pointIds[3] = 3;


        std::cout<<"2"<<std::endl;
        vtkUnstructuredGrid*    grid = vtkUnstructuredGrid::New();

        grid->Allocate(1);
        grid->SetPoints(points);

        grid->InsertNextCell(VTK_QUAD,4,pointIds);

        squareRegionActor -> GetProperty()->SetColor(0.45, 0.23, 0.26);
        squareRegionActor -> GetProperty()->SetOpacity(0.2);

        vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
        squareMapper->SetInput( grid );

        squareRegionActor->SetMapper( squareMapper );

        std::cout<<"3"<<std::endl;
        m_2DView->getRenderer()-> AddActor( squareRegionActor );
        m_2DView->refresh();

        squareRegionActor->VisibilityOn();

        squareMapper-> Delete();
        points      -> Delete();
        grid        -> Delete();
    }
}

void QRectumSegmentationExtension::setReleaseRegionOfInterest( )
{
    std::cout<<"FinalRegionOfInterest"<<std::endl;
    m_isRegionSet=true;
    m_isRegionSetting=false;
    m_viewROICheckBox->setEnabled(true);
    m_viewROICheckBox->setChecked(true);
}

void QRectumSegmentationExtension::viewRegionState(int st)
{
    if(st==Qt::Unchecked)
    {
        squareRegionActor->VisibilityOff();
        m_2DView->render();
    }
    else
    {
        squareRegionActor->VisibilityOn();
        m_2DView->render();
    }
}

void QRectumSegmentationExtension::setEditorPoint(  )
{
    double pos[3];
    if(m_editorTool != QRectumSegmentationExtension::NoEditor)
    {
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
            this->updateMaskVolume();
            m_resultsLineEdit->clear();
            m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
            m_2DView->setOverlayInput(m_lesionMaskVolume);
            m_2DView->refresh();
        }
        m_resultsLineEdit->clear();
        m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    }
}

void QRectumSegmentationExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}

void QRectumSegmentationExtension::setOpacity( int op )
{
    if(m_lesionMaskVolume != 0)
    {
        m_2DView->setOpacityOverlay(((double)op)/100.0);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->refresh();
    }
}

void QRectumSegmentationExtension::setLowerValue( int x )
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

void QRectumSegmentationExtension::setUpperValue( int x )
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

void QRectumSegmentationExtension::setErase()
{
    m_editorTool = QRectumSegmentationExtension::Erase;
}

void QRectumSegmentationExtension::setPaint()
{
    m_editorTool = QRectumSegmentationExtension::Paint;
}

void QRectumSegmentationExtension::setEraseSlice()
{
    m_editorTool = QRectumSegmentationExtension::EraseSlice;
}

void QRectumSegmentationExtension::setEraseRegion()
{
    m_editorTool = QRectumSegmentationExtension::EraseRegion;
}


void QRectumSegmentationExtension::setPaintCursor()
{
    if(m_isLeftButtonPressed)
    {
        setEditorPoint();
    }

    if(m_editorTool == QRectumSegmentationExtension::Erase || m_editorTool == QRectumSegmentationExtension::Paint)
    {
        int size = m_editorSize->value();
        double pos[3];
        double spacing[3];
        m_lesionMaskVolume->getSpacing(spacing);
        m_2DView->getCurrentCursorPosition(pos);
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
        m_2DView->refresh();

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

void QRectumSegmentationExtension::eraseMask(int size)
{
    int i,j;
    int* value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_lesionMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_lesionMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getCurrentSlice();

    for(i=-size;i<=size;i++)
    {
        for(j=-size;j<=size;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) == m_insideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //\TODO: Com que no canvia, no s'actualitza!! Ha de canviar
    //m_2DView->setOverlayInput(m_lesionMaskVolume);
}

void QRectumSegmentationExtension::paintMask(int size)
{
    int i,j;
    int* value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_lesionMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_lesionMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getCurrentSlice();
    for(i=-size;i<=size;i++)
    {
        for(j=-size;j<=size;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) != m_insideValue)
            {
                (*value) = m_insideValue;
                m_cont++;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QRectumSegmentationExtension::eraseSliceMask()
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
    m_lesionMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_lesionMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getCurrentSlice();
    //std::cout<<"Esborrant llesca "<<index[2]<<std::endl;
    for(i=ext[0];i<=ext[1];i++)
    {
        for(j=ext[2];j<=ext[3];j++)
        {
            index[0]=i;
            index[1]=j;
            value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) == m_insideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QRectumSegmentationExtension::eraseRegionMask()
{
    double pos[3];
    double origin[3];
    double spacing[3];
    int index[3];
    int ext[6];
    m_lesionMaskVolume->getVtkData()->GetExtent(ext);
    m_2DView->getCurrentCursorPosition(pos);
    m_lesionMaskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_lesionMaskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
    index[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    index[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    //index[2]=(int)(((double)pos[2]-origin[2])/spacing[2]);
    index[2]=m_2DView->getCurrentSlice();
    //std::cout<<"Esborrant llesca "<<index[2]<<std::endl;
    //eraseRegionMaskRecursive(index[0], index[1], index[2]);
    eraseRegionMaskRecursive(index[0],index[1],index[2]);
    //m_lesionMaskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QRectumSegmentationExtension::eraseRegionMaskRecursive(int a, int b, int c)
{
    int ext[6];
    m_lesionMaskVolume->getVtkData()->GetExtent(ext);
    //std::cout<<"Extension: ["<<ext[0]<<" "<<ext[1]<<", "<<ext[2]<<" "<<ext[3]<<", "<<ext[4]<<" "<<ext[5]<<"], Index: ["<<a<<", "<<b<<", "<<c<<"]"<<std::endl;
    if((a>=ext[0])&&(a<=ext[1])&&(b>=ext[2])&&(b<=ext[3])&&(c>=ext[4])&&(c<=ext[5]))
    {
        int index[3];
        index[0]=a;
        index[1]=b;
        index[2]=c;
        int* value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer(index);
        if ((*value) == m_insideValue)
        {
            //std::cout<<m_outsideValue<<" "<<m_insideValue<<"->"<<(*value)<<std::endl;
            (*value)= m_outsideValue;
            m_cont--;
            eraseRegionMaskRecursive( a+1, b, c);
            eraseRegionMaskRecursive( a-1, b, c);
            eraseRegionMaskRecursive( a, b+1, c);
            eraseRegionMaskRecursive( a, b-1, c);
        }
    }
}

void QRectumSegmentationExtension::updateVolume()
{
    m_volume = this->calculateMaskVolume();
    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
}

void QRectumSegmentationExtension::viewThresholds()
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
    imageThreshold->Update();

    m_lesionMaskVolume->setData(imageThreshold->GetOutput());

    this->viewLesionOverlay();

//     m_2DView->setOverlayToBlend();
//     m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
//     m_2DView->setOverlayInput(m_lesionMaskVolume);
//     m_2DView->refresh();

}


void QRectumSegmentationExtension::viewLesionOverlay()
{
    if(m_lesionMaskVolume != 0)
    {
        m_lesionMaskVolume = m_lesionMaskVolume;
        m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlayToBlend();
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->refresh();
    }
}


double QRectumSegmentationExtension::calculateMaskVolume()
{

    if ( m_lesionMaskVolume == 0 ) return 0.0;

    int ext[6];
    int i,j,k;
    m_cont = 0;
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
                    m_cont++;
                }
                value++;
            }
        }
    }
    m_volume = volume*(double)m_cont;

    return m_volume;

}


double QRectumSegmentationExtension::updateMaskVolume()
{

    if ( m_lesionMaskVolume == 0 ) return 0.0;

    double spacing[3];
    m_lesionMaskVolume->getSpacing(spacing);
    double volume = 1;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }


    m_volume = volume*(double)m_cont;
    //std::cout<<"vtkvolume ="<<volume<<std::endl;
    //std::cout<<"updateMask-> Rectum cont: "<<m_rectumCont<<", "<<m_cont<<std::endl;
    //std::cout<<"updateMask-> Rectum cont: "<<m_rectumVolume<<", "<<m_volume<<", "<<volume<<std::endl;

    return m_volume;

}

void QRectumSegmentationExtension::saveActivedMaskVolume()
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
        imageThreshold->SetInput( m_lesionMaskVolume->getVtkData() );
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

void QRectumSegmentationExtension::saveSegmentation3DVolume()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)") );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).suffix() != "mhd" )
        {
            fileName += ".mhd";
        }
        itk::Image<unsigned char, 3>::Pointer outputImage = itk::Image<unsigned char, 3>::New();
        outputImage->SetRegions( m_mainVolume->getItkData()->GetBufferedRegion() );
        outputImage->SetSpacing( m_mainVolume->getItkData()->GetSpacing() );
        outputImage->SetOrigin( m_mainVolume->getItkData()->GetOrigin() );
        outputImage->Allocate();

        typedef itk::ImageRegionIterator<Volume::ItkImageType> ItkIterator;
        ItkIterator iter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );
        ItkIterator itLesion( m_lesionMaskVolume->getItkData(), m_lesionMaskVolume->getItkData()->GetBufferedRegion() );
        typedef itk::ImageRegionIterator< itk::Image<unsigned char, 3> > OutIterator;
        OutIterator itOut( outputImage, outputImage->GetBufferedRegion() );

        double pos[3];
        m_2DView->getSeedPosition(pos);
        int seedSl=(int) (pos[2]-m_mainVolume->getOrigin()[2])/m_mainVolume->getSpacing()[2];

        int i,j,k;
        std::cout<<"m_seedSlice: "<<m_seedSlice<<", pos[2]: "<<pos[2]<<", seedSl: "<<seedSl<<endl;
        for(k=0;k<m_mainVolume->getDimensions()[2];k++)
        {
            for(j=0;j<m_mainVolume->getDimensions()[1];j++)
            {
                for(i=0;i<m_mainVolume->getDimensions()[0];i++)
                {
                    if(k<seedSl)
                    {
                        if(itLesion.Get()!=m_insideValue)
                        {
                            itOut.Set(0);
                        }else{
                            itOut.Set(255);
                        }
                    }else{
                        if(itLesion.Get()!=m_insideValue)
                        {
                            //Ho reescalem de 1 fins a 254
                            itOut.Set((((iter.Get()-m_minValue)*253)/m_maxValue)+1);
                        }else{
                            itOut.Set(255);
                        }
                    }
                    ++itOut;
                    ++iter;
                    ++itLesion;
                }
            }
        }

        typedef itk::ImageFileWriter< itk::Image<unsigned char, 3> > ExternalWriterType;

        ExternalWriterType::Pointer mapWriter = ExternalWriterType::New();
        mapWriter->SetInput( outputImage );
        mapWriter->SetFileName(qPrintable( fileName ));
        mapWriter->Update();

        m_savingMaskDirectory = QFileInfo( fileName ).absolutePath();
    }
}

void QRectumSegmentationExtension::toolChanged( QAction* ac)
{
    if(ac==m_seedAction || ac==m_regionAction)
    {
        m_2DView->disableTools();
    }
    else
    {
        m_2DView->enableTools();
    }
}


void QRectumSegmentationExtension::readSettings()
{
    QSettings settings("GGG", "StarViewer-App-RectumSegmentation");
    settings.beginGroup("StarViewer-App-RectumSegmentation");

    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );
    m_savingMaskDirectory = settings.value( "savingDirectory", "." ).toString();
    //std::cout<<"Saving directory: "<<qPrintable( m_savingMaskDirectory )<<std::endl;
    settings.endGroup();
}

void QRectumSegmentationExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-RectumSegmentation");
    settings.beginGroup("StarViewer-App-RectumSegmentation");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );
    settings.setValue("savingDirectory", m_savingMaskDirectory );

    settings.endGroup();
}

}
