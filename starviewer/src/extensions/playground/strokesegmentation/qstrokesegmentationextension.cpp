/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qstrokesegmentationextension.h"

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
/*#include <vtkDataSetMapper.h>
#include <vtkThreshold.h>
*/

// ITK
#include <itkBinaryThresholdImageFilter.h>

//prova recte
#include "itkRescaleIntensityImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkExtractImageFilter.h"


namespace udg {

QStrokeSegmentationExtension::QStrokeSegmentationExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume     = 0;
    m_maskVolume     = new Volume();

    //m_fusionVolume   = 0;
    //m_vtkFusionImage = 0;

    m_segMethod = new StrokeSegmentationMethod();

    m_isSeed  = false;
    m_isMask  = false;
    m_isErase = false;
    m_isPaint = false;
    m_isEraseSlice = false;
    m_isLeftButtonPressed = false;
    m_cont = 0;

    //pointActor = vtkActor::New();
    squareActor = vtkActor::New();

    createActions();
    createToolBars();
    createConnections();

    readSettings();
}

QStrokeSegmentationExtension::~QStrokeSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    //delete m_maskVolume;//TODO descomentar aix�per tal d'alliberar el m_maskVolume (ara peta)
    /*if(m_vtkFusionImage!=0)
    {
        m_vtkFusionImage->Delete();
    }*/
    //pointActor  -> Delete();
    squareActor -> Delete();
}

void QStrokeSegmentationExtension::createActions()
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

}

void QStrokeSegmentationExtension::createToolBars()
{
}

void QStrokeSegmentationExtension::createConnections()
{
  connect( m_applyMethodButton , SIGNAL( clicked() ) , this , SLOT( ApplyMethod() ) );

  connect( m_eraseButton , SIGNAL( clicked() ) , this , SLOT( setErase() ) );

  connect( m_eraseSliceButton , SIGNAL( clicked() ) , this , SLOT( setEraseSlice() ) );

  connect( m_paintButton , SIGNAL( clicked() ) , this , SLOT( setPaint() ) );

  connect( m_updateVolumeButton , SIGNAL( clicked() ) , this , SLOT( updateVolume() ) );

  connect( m_viewThresholdButton , SIGNAL( clicked() ) , this , SLOT( viewThresholds() ) );

  connect( m_2DView , SIGNAL( eventReceived( unsigned long ) ) , this , SLOT( strokeEventHandler(unsigned long) ) );

  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );

  connect( m_lowerValueSlider, SIGNAL( valueChanged(int) ) , this , SLOT( setLowerValue(int) ) );

  connect( m_upperValueSlider, SIGNAL( valueChanged(int) ) , this , SLOT( setUpperValue(int) ) );

  connect( m_opacitySlider, SIGNAL( valueChanged(int) ) , this , SLOT( setOpacity(int) ) );

  connect( m_2DView, SIGNAL( seedChanged() ) , this , SLOT( setSeedPosition() ) );
}

void QStrokeSegmentationExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView->setView( Q2DViewer::Axial );
   m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();

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

    m_2DView->render();

}

void QStrokeSegmentationExtension::ApplyMethod( )
{
    if(!m_isSeed || !m_isMask){
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "ERROR: no hi ha definida llavor o màscara" ) );
        return;
    }

    m_segMethod->setVolume(m_mainVolume);
    m_segMethod->setMask(m_maskVolume);
    std::cout<<"Inici Apply method!!"<<std::endl;
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
    //m_cont est�actualitzat correctament a partir de l'apply method
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
    m_2DView->disableTools();
    m_isPaint = true;
    m_isErase = false;
    m_isEraseSlice = false;
    m_editorAction->setEnabled( true );
    m_2DView->getInteractor()->Render();
    QApplication::restoreOverrideCursor();
    std::cout<<"Fi Apply method!!"<<std::endl;
  /*
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::cout<<"Inici Apply method!!"<<std::endl;
    typedef itk::Image< float,  3 >   IntermediateImageType;
        typedef itk::Image< Volume::ItkImageType::PixelType,  2 >   Image2DType;
    //typedef itk::CurvatureAnisotropicDiffusionImageFilter< Volume::ItkImageType, IntermediateImageType>  FilterType;
    typedef itk::CurvatureAnisotropicDiffusionImageFilter< Image2DType, Image2DType>  FilterType;

  //Definim la regi�molt probablement infartada
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
    m_2DView->getInteractor()->Render();
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
        //std::cout<<"Editor Tool"<<std::endl;
        m_2DView->disableTools();
        setEditorPoint(  );
    }
    else
    {
        m_2DView->enableTools();
    }
}

void QStrokeSegmentationExtension::setSeedPosition( )
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

void QStrokeSegmentationExtension::setEditorPoint(  )
{
    double pos[3];
    if(m_isErase || m_isEraseSlice || m_isPaint)
    {
        m_2DView->updateVoxelInformation();
        m_2DView->getCurrentCursorPosition(pos);

        // quan dona una posici�� de (-1, -1, -1) � que estem fora de l'actor
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
            m_2DView->getInteractor()->Render();
        }
    }
}

void QStrokeSegmentationExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}

void QStrokeSegmentationExtension::setOpacity( int op )
{
    m_2DView->setOpacityOverlay(((double)op)/100.0);
    m_2DView->setOverlayInput(m_maskVolume);

    m_2DView->getInteractor()->Render();
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
    if(m_editorToolButton->isChecked())    //Nom� en cas que estiguem en l'editor
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

void QStrokeSegmentationExtension::eraseMask(int size)
{
    int i,j;
    int* value;
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
            value=(int*)m_maskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) != m_outsideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QStrokeSegmentationExtension::paintMask(int size)
{
    int i,j;
    int* value;
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
            value=(int*)m_maskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) == m_outsideValue)
            {
                (*value) = m_insideValue;
                m_cont++;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
}

void QStrokeSegmentationExtension::eraseSliceMask()
{
    int i,j;
    int* value;
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
            value=(int*)m_maskVolume->getVtkData()->GetScalarPointer(index);
            if((*value) != m_outsideValue)
            {
                (*value) = m_outsideValue;
                m_cont--;
            }
        }
    }
    //m_maskVolume->getVtkData()->Update();
    //m_2DView->getInteractor()->Render();
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
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    m_maskVolume->setImages( m_mainVolume->getImages() );

    m_2DView->setOverlayToBlend();
    m_2DView->setOpacityOverlay(((double)m_opacitySlider->value())/100.0);
    m_2DView->setOverlayInput(m_maskVolume);

    m_2DView->getInteractor()->Render();

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
        std::cout<<"Compte!!! Mask Vtk Data Type != INT ("<<m_maskVolume->getVtkData()->GetScalarTypeAsString()<<")"<<std::endl;
    }

    int* value;
    value=(int*)m_maskVolume->getVtkData()->GetScalarPointer();
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
    QSettings settings("GGG", "StarViewer-App-StrokeSegmentation");
    settings.beginGroup("StarViewer-App-StrokeSegmentation");

    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );

    settings.endGroup();
}

void QStrokeSegmentationExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-StrokeSegmentation");
    settings.beginGroup("StarViewer-App-StrokeSegmentation");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );

    settings.endGroup();
}

}
