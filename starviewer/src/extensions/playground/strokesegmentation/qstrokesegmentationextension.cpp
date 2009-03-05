/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qstrokesegmentationextension.h"

#include "strokesegmentationmethod.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "toolmanager.h"
//Qt
#include <QString>
#include <QAction>
#include <QSettings>
#include <QMessageBox>

// VTK
#include <vtkImageMask.h>
#include <vtkImageThreshold.h>
#include <vtkCommand.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkContourGrid.h>

// ITK
#include <itkBinaryThresholdImageFilter.h>

//prova recte
#include "itkRescaleIntensityImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkExtractImageFilter.h"


namespace udg {

QStrokeSegmentationExtension::QStrokeSegmentationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_isSeed(false), m_isMask(false), m_isErase(false), m_isPaint(false), m_isEraseSlice(false), m_isLeftButtonPressed(false), m_cont(0)
{
    setupUi( this );

    m_maskVolume = new Volume();
    m_segMethod = new StrokeSegmentationMethod();

    m_squareActor = vtkActor::New();

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
    m_squareActor->Delete();
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
    
    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // Tool d'slicing per teclat
    QAction *slicingKeyboardTool = m_toolManager->getToolAction("SlicingKeyboardTool");
    slicingKeyboardTool->trigger();

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "SeedTool";
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
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SeedTool" << "VoxelInformationTool";

    m_toolManager->setViewerTools( m_2DView, toolsList );

    // Activació de l'eina d'edició "HOME MADE"
    // TODO Cal fer servir únicament la autèntica tool i treure tota
    // l'edició manual que es fa dins d'aquesta pròpia classe
    m_editorAction = m_toolManager->getToolAction("EditorTool");
    m_editorToolButton->setDefaultAction( m_editorAction );

    m_toolsActionGroup = new QActionGroup( 0 );
    // txapussilla per fer la tool editor(home made) exclusiu de la resta. Amb la tool oficial això seria automàtic
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingToolButton->defaultAction() );
    m_toolsActionGroup->addAction( m_zoomToolButton->defaultAction() );
    m_toolsActionGroup->addAction( m_seedToolButton->defaultAction() );
    m_toolsActionGroup->addAction( m_editorAction );
    // TODO guarrada total! ens veiem forçats a fer això perquè es refresquin les connexions de les tools
    // i no quedi l'editor activat a la vegada amb cap altre tool
    connect( m_editorAction, SIGNAL( triggered() ), m_toolManager, SLOT( undoDisableAllToolsTemporarily() ) );
}

void QStrokeSegmentationExtension::createConnections()
{
    connect( m_applyMethodButton, SIGNAL( clicked() ), SLOT( applyMethod() ) );
    connect( m_eraseButton, SIGNAL( clicked() ), SLOT( setErase() ) );
    connect( m_eraseSliceButton, SIGNAL( clicked() ), SLOT( setEraseSlice() ) );
    connect( m_paintButton, SIGNAL( clicked() ), SLOT( setPaint() ) );
    connect( m_updateVolumeButton, SIGNAL( clicked() ), SLOT( updateVolume() ) );
    connect( m_viewThresholdButton, SIGNAL( clicked() ), SLOT( viewThresholds() ) );
    connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), SLOT( strokeEventHandler(unsigned long) ) );
    connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ), SLOT( setLowerValue(int) ) );
    connect( m_upperValueSlider, SIGNAL( valueChanged(int) ), SLOT( setUpperValue(int) ) );
    connect( m_opacitySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
    connect( m_2DView, SIGNAL( seedChanged() ), SLOT( setSeedPosition() ) );
}

void QStrokeSegmentationExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView->resetView( Q2DViewer::Axial );
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
    if(!m_isSeed || !m_isMask){
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
    m_volume = m_segMethod->applyMethod();
    m_cont = m_segMethod->getNumberOfVoxels();
    //m_cont = m_segMethod->applyMethod3();
    //m_cont = m_segMethod->applyMethod4();
    //m_maskVolume->updateVtkDataFromItkData( );

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_maskVolume);


    //m_2DView->setInput( m_maskVolume );


    //double volume = this->calculateMaskVolume();
    //m_cont està ctualitzat correctament a partir de l'apply method
    //double volume = this->updateMaskVolume();

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    m_resultsLineEdit->setEnabled(true);
    m_resultsLabel->setEnabled(true);
    m_updateVolumeButton->setEnabled(true);
    m_eraseButton->setEnabled(true);
    m_eraseButton->setFlat(false);
    m_paintButton->setEnabled(true);
    m_paintButton->setFlat(true);
    m_eraseSliceButton->setEnabled(true);
    m_eraseSliceButton->setFlat(false);
    m_editorSize->setEnabled(true);
    m_editorAction->trigger();
    m_isPaint = true;
    m_isErase = false;
    m_isEraseSlice = false;
    m_editorAction->setEnabled( true );
    m_2DView->refresh();
    QApplication::restoreOverrideCursor();
    DEBUG_LOG( "Fi Apply method!!" );
  /*
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::cout<<"Inici Apply method!!"<<std::endl;
    typedef itk::Image< float,  3 >   IntermediateImageType;
        typedef itk::Image< Volume::ItkImageType::PixelType,  2 >   Image2DType;
    //typedef itk::CurvatureAnisotropicDiffusionImageFilter< Volume::ItkImageType, IntermediateImageType>  FilterType;
    typedef itk::CurvatureAnisotropicDiffusionImageFilter< Image2DType, Image2DType>  FilterType;

  //Definim la regió molt probablement infartada
  Volume::ItkImageType::Pointer auxVolume = Volume::ItkImageType::New();
  auxVolume->SetRegions( m_mainVolume->getItkData()->GetLargestPossibleRegion() );
  auxVolume->SetSpacing( m_mainVolume->getItkData()->GetSpacing() );
  auxVolume->SetOrigin ( m_mainVolume->getItkData()->GetOrigin() );
  auxVolume->Allocate();
  itk::ImageRegionIterator<Volume::ItkImageType> maskIt( auxVolume, auxVolume->GetBufferedRegion());
  maskIt.GoToBegin();

    // Software Guide : EndCodeSnippet
    const unsigned int numberOfIterations = 5;
    //const double       timeStep = 0.0625; //3D
    const double       timeStep = 0.08; //2D
    const double       conductance = 3.0;
    const bool         useImageSpacing = true;

    for(int i=0;i<m_mainVolume->getDimensions()[2];i++)
    {
        typedef itk::ExtractImageFilter< Volume::ItkImageType, Image2DType > ExtractFilterType;
        ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();

        Volume::ItkImageType::RegionType inputRegion = m_mainVolume->getItkData()->GetLargestPossibleRegion();
        Volume::ItkImageType::SizeType size = inputRegion.GetSize();
        size[2] = 0;
        Volume::ItkImageType::IndexType start = inputRegion.GetIndex();
        start[2] = i;
        Volume::ItkImageType::RegionType desiredRegion;
        desiredRegion.SetSize(  size  );
        desiredRegion.SetIndex( start );
        extractFilter->SetExtractionRegion( desiredRegion );
        extractFilter->SetInput(  m_mainVolume->getItkData() );
        FilterType::Pointer filter = FilterType::New();
        filter->SetInput( extractFilter->GetOutput() );
        filter->SetNumberOfIterations( numberOfIterations );
        filter->SetTimeStep( timeStep );
        filter->SetConductanceParameter( conductance );
        if (useImageSpacing)
            {
            filter->UseImageSpacingOn();
            }
        filter->Update();
        itk::ImageRegionIterator<Image2DType> sliceIt(filter->GetOutput(), filter->GetOutput()->GetBufferedRegion());
        sliceIt.GoToBegin();

        while(!sliceIt.IsAtEnd())
        {
            //std::cout<<sliceIt.Get()<<" "<<std::flush;
            maskIt.Set( sliceIt.Get() );
            ++maskIt;
            ++sliceIt;
        }
        //std::cout<<std::endl;

    }


    filter->SetNumberOfIterations( numberOfIterations );
    filter->SetTimeStep( timeStep );
    filter->SetConductanceParameter( conductance );
    if (useImageSpacing)
        {
        filter->UseImageSpacingOn();
        }
    filter->Update();

    typedef itk::RescaleIntensityImageFilter<Volume::ItkImageType, Volume::ItkImageType > RescaleFilterType;

    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetOutputMinimum(   0 );
    rescaler->SetOutputMaximum( 255 );

    rescaler->SetInput( auxVolume );
    rescaler->Update();


    //m_maskVolume->setData( filter->GetOutput() );
    //m_maskVolume->setData( rescaler->GetOutput() );
    m_maskVolume->setData( auxVolume );


    std::cout<<"Fi ITK method!! --> Init VIsualization"<<std::endl;

    //m_maskVolume->updateVtkDataFromItkData( );

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_maskVolume);
    m_2DView->refresh();
    QApplication::restoreOverrideCursor();
    std::cout<<"Fi Apply method!!"<<std::endl;
    */
}

void QStrokeSegmentationExtension::strokeEventHandler( unsigned long id )
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

void QStrokeSegmentationExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;

    if(m_editorToolButton->isChecked())
    {
        setEditorPoint();
    }
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

void QStrokeSegmentationExtension::setEditorPoint()
{
    double pos[3];
    if(m_isErase || m_isEraseSlice || m_isPaint)
    {
        m_2DView->getCurrentCursorPosition(pos);

        // quan dona una posició de (-1, -1, -1) que estem fora de l'actor
        if(!( pos[0] == -1 && pos[1] == -1 && pos[2] == -1) )
        {
            if( m_isErase )
            {
                this->eraseMask(m_editorSize->value());
            }
            else if( m_isPaint )
            {
                this->paintMask(m_editorSize->value());
            }
            else    //eraseSlice is true
            {
                this->eraseSliceMask();
            }
            double volume = this->updateMaskVolume();
            m_resultsLineEdit->clear();
            m_resultsLineEdit->insert(QString("%1").arg(volume, 0, 'f', 2));
            m_2DView->setOverlayInput(m_maskVolume);
            m_2DView->refresh();
        }
    }
}

void QStrokeSegmentationExtension::setLeftButtonOff()
{
    m_isLeftButtonPressed = false;
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

void QStrokeSegmentationExtension::setErase()
{
    if(m_isErase==false)
    {
        m_isErase=true;
        m_eraseButton->setFlat(true);
        if(m_isPaint==true)
        {
            m_isPaint=false;
            m_paintButton->setFlat(false);
        }
        else if(m_isEraseSlice==true)
        {
            m_isEraseSlice=false;
            m_eraseSliceButton->setFlat(false);
        }
    }
    else
    {
        m_isErase=false;
        m_eraseButton->setFlat(false);
    }
}

void QStrokeSegmentationExtension::setPaint()
{
    if(m_isPaint==false)
    {
        m_isPaint=true;
        m_paintButton->setFlat(true);
        if(m_isErase==true)
        {
            m_isErase=false;
            m_eraseButton->setFlat(false);
        }
        else if(m_isEraseSlice==true)
        {
            m_isEraseSlice=false;
            m_eraseSliceButton->setFlat(false);
        }
    }
    else
    {
        m_isPaint=false;
        m_paintButton->setFlat(false);
    }
}

void QStrokeSegmentationExtension::setEraseSlice()
{
    if(m_isEraseSlice==false)
    {
        m_isEraseSlice=true;
        m_eraseSliceButton->setFlat(true);
        if(m_isErase==true)
        {
            m_isErase=false;
            m_eraseButton->setFlat(false);
        }
        else if(m_isPaint==true)
        {
            m_isPaint=false;
            m_paintButton->setFlat(false);
        }
    }
    else
    {
        m_isEraseSlice=false;
        m_eraseSliceButton->setFlat(false);
    }
}

void QStrokeSegmentationExtension::setPaintCursor()
{
    if(m_editorToolButton->isChecked())    //Només en cas que estiguem en l'editor
    {
        if(m_isLeftButtonPressed)
        {
            setEditorPoint();
        }

        if(m_isErase || m_isPaint)
        {
            int size = m_editorSize->value();
            double pos[3];
            double spacing[3];
            m_maskVolume->getSpacing(spacing);
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


            vtkUnstructuredGrid *grid = vtkUnstructuredGrid::New();

            grid->Allocate(1);
            grid->SetPoints(points);

            grid->InsertNextCell(VTK_QUAD,4,pointIds);

            m_squareActor->GetProperty()->SetColor(0.15, 0.83, 0.26);
            m_squareActor->GetProperty()->SetOpacity(0.2);

            vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
            squareMapper->SetInput( grid );

            m_squareActor->SetMapper( squareMapper );

            m_2DView->getRenderer()->AddViewProp( m_squareActor );
            m_2DView->refresh();

            m_squareActor->VisibilityOn();

            squareMapper->Delete();
            points->Delete();
            grid->Delete();
        }
        else
        {
            m_squareActor->VisibilityOff();
        }
    }
}

void QStrokeSegmentationExtension::eraseMask(int size)
{
    int i,j;
    Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_maskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_maskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
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
            value = m_maskVolume->getScalarPointer(index);
            if((*value) != m_outsideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QStrokeSegmentationExtension::paintMask(int size)
{
    int i,j;
    Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DView->getCurrentCursorPosition(pos);
    m_maskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_maskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
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
            value = m_maskVolume->getScalarPointer(index);
            if((*value) == m_outsideValue)
            {
                (*value) = m_insideValue;
                m_cont++;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QStrokeSegmentationExtension::eraseSliceMask()
{
    int i,j;
    Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    int ext[6];
    m_maskVolume->getVtkData()->GetExtent(ext);
    m_2DView->getCurrentCursorPosition(pos);
    m_maskVolume->getVtkData()->GetSpacing(spacing[0],spacing[1],spacing[2]);
    m_maskVolume->getVtkData()->GetOrigin(origin[0],origin[1],origin[2]);
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
            value = m_maskVolume->getScalarPointer(index);
            if((*value) != m_outsideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->refresh();
}

void QStrokeSegmentationExtension::updateVolume()
{
    m_volume = this->calculateMaskVolume();
    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
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

    if ( m_maskVolume == 0 ) return 0.0;

    int ext[6];
    int i,j,k;
    m_cont = 0;
    m_maskVolume->getWholeExtent(ext);

    double spacing[3];
    m_maskVolume->getSpacing(spacing);
    double volume = 1;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    if(m_maskVolume->getVtkData()->GetScalarType()!=6)
    {
        DEBUG_LOG( QString("Compte!!! Mask Vtk Data Type != INT (%1) - POTSER aquest missatge de debug ha quedat obsolet!!!!").arg( m_maskVolume->getVtkData()->GetScalarTypeAsString() ) );
    }

    Volume::VoxelType *value = m_maskVolume->getScalarPointer();
    for(i=ext[0];i<=ext[1];i++)
    {
        for(j=ext[2];j<=ext[3];j++)
        {
            for(k=ext[4];k<=ext[5];k++)
            {
                if ((*value) != m_outsideValue)
                {
                    m_cont++;
                }
                value++;
            }
        }
    }
    volume = volume*(double)m_cont;

    return volume;

}


double QStrokeSegmentationExtension::updateMaskVolume()
{

    if ( m_maskVolume == 0 ) return 0.0;

    double spacing[3];
    m_maskVolume->getSpacing(spacing);
    double volume = 1;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    volume = volume*(double)m_cont;
    //std::cout<<"vtkvolume ="<<volume<<std::endl;

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
