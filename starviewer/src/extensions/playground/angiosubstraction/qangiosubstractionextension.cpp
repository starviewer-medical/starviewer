/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qangiosubstractionextension.h"

#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "series.h"
#include "image.h"
#include "study.h"
#include "patient.h"
#include "transdifferencetool.h"
#include "transdifferencetooldata.h"
#include "toolproxy.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "patientbrowsermenu.h"
#include "angiosubstractionsettings.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QContextMenuEvent>

// VTK
#include <vtkCommand.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

// ITK
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageRegistrationMethod.h>
#include <itkTranslationTransform.h>
#include <itkMeanSquaresImageToImageMetric.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkExtractImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkResampleImageFilter.h>

namespace udg {

QAngioSubstractionExtension::QAngioSubstractionExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_differenceVolume(0)
{
    setupUi( this );
    AngioSubstractionSettings().init();

    initializeTools();
    createConnections();
    readSettings();
}

QAngioSubstractionExtension::~QAngioSubstractionExtension()
{
    delete m_toolManager;
    writeSettings();
}

void QAngioSubstractionExtension::initializeTools()
{
    m_toolManager = new ToolManager(this);
    // Registrem les tools
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->registerTool("ScreenShotTool") );
    m_transDifferenceToolButton->setDefaultAction( m_toolManager->registerTool("TransDifferenceTool") );
    m_toolManager->registerTool("SynchronizeTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "ScreenShotTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools( defaultTools );

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "TransDifferenceTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // posem a punt les tools pels visors
    m_toolManager->setupRegisteredTools( m_2DView_1->getViewer() );
    m_toolManager->setupRegisteredTools( m_2DView_2->getViewer() );

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool("RotateClockWiseActionTool") );
    m_toolManager->enableRegisteredActionTools( m_2DView_1->getViewer() );
    m_toolManager->enableRegisteredActionTools( m_2DView_2->getViewer() );
}

void QAngioSubstractionExtension::createConnections()
{
	//Només es pot canviar l'input d'un dels viewers (el de l'input)
    disconnect( m_2DView_1->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_2DView_1->getViewer(), SLOT( setInput( Volume * ) ) );
    disconnect( m_2DView_2->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_2DView_2->getViewer(), SLOT( setInput( Volume * ) ) );
    connect( m_2DView_1->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), SLOT( setInput( Volume * ) ) );
    connect( m_imageSelectorSpinBox, SIGNAL( valueChanged(int) ), SLOT( computeDifferenceImage( int ) ) );
    connect( m_2DView_1, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );
    connect( m_2DView_2, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );
    connect( m_autoRegistrationToolButton, SIGNAL( clicked() ), SLOT( computeAutomateSingleImage( ) ) );
    connect( m_applyTranslationToolButton, SIGNAL( clicked() ), SLOT( computeSingleImageDifference( ) ) );
}

void QAngioSubstractionExtension::setInput( Volume *input )
{
    m_mainVolume = input;

	//Desactivem la sincronització perquè si no quan es canvia l'input no funciona correctament
	m_2DView_1->setSynchronized(false);
	m_2DView_2->setSynchronized(false);
	//this->synchronization( m_2DView_1, false );
	//this->synchronization( m_2DView_2, false );

	//eliminem l'anterior m_differenceVolume si n'hi ha
	if(m_differenceVolume){
		delete m_differenceVolume;
		m_differenceVolume = 0;
	}


	//Compute the new difference Volume
	m_2DView_1->getViewer()->setInput(m_mainVolume);
	m_imageSelectorSpinBox->setMinimum(1);
	m_imageSelectorSpinBox->setMaximum(m_mainVolume->getDimensions()[2]);
	m_imageSelectorSpinBox->setValue(1);

	computeDifferenceImage( m_imageSelectorSpinBox->value() );

    //Actualitzem les dades de la transdifference tool
    m_toolManager->triggerTool("TransDifferenceTool");
    TransDifferenceToolData *tdToolData = static_cast<TransDifferenceToolData*> ( m_2DView_2->getViewer()->getToolProxy()->getTool("TransDifferenceTool")->getToolData() );
    tdToolData->setInputVolume(m_mainVolume);
    tdToolData->setDifferenceVolume(m_differenceVolume);
    tdToolData->setReferenceSlice( m_imageSelectorSpinBox->value() );
    m_toolManager->triggerTool("SlicingTool");

	//Només actualitzem l'1 perquè el 2 ja es fa en l'acció computeDifferenceImage
	//Això es fa així perquè l'acció està lligasda a un connect
	m_2DView_1->getViewer()->refresh();

	m_2DView_1->setSynchronized(true);
	m_2DView_2->setSynchronized(true);
}


void QAngioSubstractionExtension::computeDifferenceImage( int imageid )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
	DEBUG_LOG(QString("Init computeDifferenceImage: %1").arg(imageid));
	int i,j,k;
    int max=0;

    //Allocating memory for the output image
    int ext[6];
    m_mainVolume->getWholeExtent(ext);
    vtkImageData *imdif = vtkImageData::New();
    //imdif->CopyInformation(m_mainVolume->getVtkData());
    imdif->CopyTypeSpecificInformation(m_mainVolume->getVtkData());
    imdif->SetExtent(ext);

    //Creem l'index que ens indica quin és el primer voxel de la imatge de la que restem
    int indexRef[3];
    indexRef[0]=ext[0];
    indexRef[1]=ext[2];
    indexRef[2]=imageid - 1;

    Volume::VoxelType *valueRef, *valueMov, *valueDif;

    valueMov = m_mainVolume->getScalarPointer();
    valueDif = (Volume::VoxelType *)imdif->GetScalarPointer();
    for (k=ext[4];k<=ext[5];k++)
    {
        //Reiniciem el punter a la imatge de referència
        valueRef = m_mainVolume->getScalarPointer(indexRef);
        for (j=ext[2];j<=ext[3];j++)
        {
            for (i=ext[0];i<=ext[1];i++)
            {
                (*valueDif) = (*valueMov) - (*valueRef);
 				if((*valueDif)>max)max=(*valueDif);
				if((*valueDif)<-max)max=-(*valueDif);
                valueRef++;
                valueMov++;
                valueDif++;
            }
        }
    }

	//Converting the VTK data to volume
    if(m_differenceVolume == 0){
        m_differenceVolume = new Volume();
        m_differenceVolume->setImages( m_mainVolume->getImages() );
    }
    m_differenceVolume->setData(imdif);

	m_2DView_2->getViewer()->setInput(m_differenceVolume);
	m_2DView_2->getViewer()->setWindowLevel((double)2*max,0.0);

	m_2DView_2->getViewer()->refresh();
    QApplication::restoreOverrideCursor();
}

void QAngioSubstractionExtension::computeAutomateSingleImage( )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    const    unsigned int          Dimension = 2;
    typedef  Volume::ItkPixelType  PixelType;

    typedef itk::Image< PixelType, Dimension >  FixedImageType;
    typedef itk::Image< PixelType, Dimension >  MovingImageType;
    typedef itk::TranslationTransform< double, Dimension > TransformType;
    typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
    typedef itk::MattesMutualInformationImageToImageMetric< 
                                          FixedImageType, 
                                          MovingImageType >    MetricType;
/*    typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;
    */
    typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;
    typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

    MetricType::Pointer         metric        = MetricType::New();
    TransformType::Pointer      transform     = TransformType::New();
    OptimizerType::Pointer      optimizer     = OptimizerType::New();
    InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    RegistrationType::Pointer   registration  = RegistrationType::New();

    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetTransform(     transform     );
    registration->SetInterpolator(  interpolator  );

    metric->SetNumberOfHistogramBins( 50 );
    metric->SetNumberOfSpatialSamples( 10000 );

    typedef itk::ExtractImageFilter< Volume::ItkImageType, FixedImageType > FilterType;
    
    FilterType::Pointer extractFixedImageFilter = FilterType::New();
    Volume::ItkImageType::RegionType inputRegion = m_mainVolume->getItkData()->GetLargestPossibleRegion();
    Volume::ItkImageType::SizeType size = inputRegion.GetSize();
    //Dividim la mida per dos per tal de quedar-nos només amb la part central
    // ja que si no ens registre el background
    size[0] = size[0] / 2;
    size[1] = size[1] / 2;
    size[2] = 0;
    Volume::ItkImageType::IndexType start = inputRegion.GetIndex();
    const unsigned int sliceReference = m_imageSelectorSpinBox->value();
    //comencem a un quart de la imatge
    start[0] = size[0] / 2;
    start[1] = size[1] / 2;
    start[2] = sliceReference;
    Volume::ItkImageType::RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extractFixedImageFilter->SetExtractionRegion( desiredRegion );
    extractFixedImageFilter->SetInput( m_mainVolume->getItkData() );
    extractFixedImageFilter->Update();

    FilterType::Pointer extractMovingImageFilter = FilterType::New();
    Volume::ItkImageType::IndexType startMoving = inputRegion.GetIndex();
    const unsigned int sliceNumber = m_2DView_1->getViewer()->getCurrentSlice();
    startMoving[0] = size[0] / 2;
    startMoving[1] = size[1] / 2;
    startMoving[2] = sliceNumber;
    Volume::ItkImageType::RegionType desiredMovingRegion;
    desiredMovingRegion.SetSize(  size  );
    desiredMovingRegion.SetIndex( startMoving );
    extractMovingImageFilter->SetExtractionRegion( desiredMovingRegion );
    extractMovingImageFilter->SetInput( m_mainVolume->getItkData() );
    extractMovingImageFilter->Update();

    registration->SetFixedImage( extractFixedImageFilter->GetOutput() );
    registration->SetMovingImage( extractMovingImageFilter->GetOutput() );

    typedef RegistrationType::ParametersType ParametersType;
    ParametersType initialParameters( transform->GetNumberOfParameters() );

    //Potser seria millor posar la transformada que té actualment
    initialParameters[0] = 0.0;  // Initial offset in mm along X
    initialParameters[1] = 0.0;  // Initial offset in mm along Y

    registration->SetInitialTransformParameters( initialParameters );

    optimizer->SetMaximumStepLength( 4.00 );  
    optimizer->SetMinimumStepLength( 0.005 );

    optimizer->SetNumberOfIterations( 200 );

    try 
    { 
        registration->StartRegistration(); 
    } 
    catch( itk::ExceptionObject & err ) 
    { 
        DEBUG_LOG(QString( "ExceptionObject caught !"));
        std::cout<<err<<std::endl;
        return;
    } 
    ParametersType finalParameters = registration->GetLastTransformParameters();

    const double TranslationAlongX = finalParameters[0];
    const double TranslationAlongY = finalParameters[1];

    const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

    const double bestValue = optimizer->GetValue();

    DEBUG_LOG(QString( "Result = " ));
    DEBUG_LOG(QString( " Translation X = %1").arg( TranslationAlongX ) );
    DEBUG_LOG(QString( " Translation Y = %1").arg( TranslationAlongY ) );
    DEBUG_LOG(QString( " Iterations    = %1").arg( numberOfIterations ) );
    DEBUG_LOG(QString( " Metric value  = %1").arg( bestValue ) );

    typedef  unsigned char  OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
    typedef itk::RescaleIntensityImageFilter< FixedImageType, FixedImageType > RescaleFilterType;
    typedef itk::ResampleImageFilter< 
                            FixedImageType, 
                            FixedImageType >    ResampleFilterType;
    typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > CastFilterType;
    typedef itk::ImageFileWriter< OutputImageType >  WriterType;

    WriterType::Pointer      writer =  WriterType::New();
    CastFilterType::Pointer  caster =  CastFilterType::New();
    ResampleFilterType::Pointer resample = ResampleFilterType::New();
    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

    rescaler->SetOutputMinimum(   0 );
    rescaler->SetOutputMaximum( 255 );

    TransformType::Pointer finalTransform = TransformType::New();
    finalTransform->SetParameters( finalParameters );
    resample->SetTransform( finalTransform );
    resample->SetSize( extractMovingImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(  extractMovingImageFilter->GetOutput()->GetOrigin() );
    resample->SetOutputSpacing( extractMovingImageFilter->GetOutput()->GetSpacing() );
    resample->SetDefaultPixelValue( 100 );

    writer->SetFileName( "prova.jpg" );

    rescaler->SetInput( extractMovingImageFilter->GetOutput() );
    resample->SetInput( rescaler->GetOutput() );
    caster->SetInput( resample->GetOutput() );
    writer->SetInput( caster->GetOutput() );
    writer->Update();

    //Pintem la diferència al volume 
    //ho fem amb vtk pq és més ràpid
/*    Volume::VoxelType *valueRef, *valueMov, *valueDif;
    int index[3];

    for(i=-m_editorSize;i<=m_editorSize;i++)
    {
        for(j=-m_editorSize;j<=m_editorSize;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value = m_2DViewer->getOverlayInput()->getScalarPointer(index);
            if(value && ((*value) != m_insideValue) )
            {
                (*value) = m_insideValue;
                m_volumeCont++;
            }
        }
    }
	typedef itk::ImageRegionIteratorWithIndex<Volume::ItkImageType> IteratorWithIndex;
    IteratorWithIndex difIter( difImage, difImage->GetBufferedRegion() );
    IteratorWithIndex imIter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );
    IteratorWithIndex baseIter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );

    Volume::ItkImageType::IndexType initialSlice;
	initialSlice[0] = start[0];
	initialSlice[1] = start[1];
	initialSlice[2] = sliceNumber;
    difIter.SetIndex(initialSlice);
    baseIter.SetIndex(initialSlice);

    initialSlice[2] = sliceNumber;
    imIter.SetIndex(initialSlice);
	int value;
    for (j=0;j<size[1];j++)
    {
        for (i=0;i<size[0];i++)
        {
		    value = imIter.Get()-baseIter.Get();
            difIter.Set(value);
            ++difIter;
            ++imIter;
            ++baseIter;
        }
    }

	//Converting the ITK data to volume
	if(m_differenceVolume == 0){
		m_differenceVolume = new Volume();
		m_differenceVolume->setImages( m_mainVolume->getImages() );
	}
    m_differenceVolume->setData(difImage);

	m_2DView_2->getViewer()->setInput(m_differenceVolume);
	m_2DView_2->getViewer()->setWindowLevel((double)2*max,0.0);

	m_2DView_2->getViewer()->refresh();
*/
    QApplication::restoreOverrideCursor();

}

void QAngioSubstractionExtension::computeSingleImageDifference(  )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    //Simplifiquem dient que la translació només pot ser per múltiples del píxel
    //Pintem la diferència al volume a la llesca "slice"
    //ho fem amb vtk pq és més ràpid
    Volume::VoxelType *valueRef, *valueMov, *valueDif;
    int indexRef[3];
    int indexMov[3];
    int indexDif[3];

    //De moment ho treiem de l'spinbox
    int tx = m_xTranslationSpinBox->value();
    int ty = m_yTranslationSpinBox->value();

    indexRef[2]=m_imageSelectorSpinBox->value();
    indexMov[2]=m_2DView_1->getViewer()->getCurrentSlice();
    indexDif[2]=m_2DView_1->getViewer()->getCurrentSlice();

    Volume::ItkImageType::SizeType size = m_mainVolume->getItkData()->GetBufferedRegion().GetSize();

    int i,j;
    int imax,imin;
    int jmax,jmin;

    imin = tx < 0 ? 0 : tx ;
    imax = tx < 0 ? size[0]+tx : size[0] ;
    jmin = ty < 0 ? 0 : ty ;
    jmax = ty < 0 ? size[1]+ty : size[1] ;

    indexRef[0]=imin;
    indexMov[0]=imin-tx;
    indexDif[0]=imin;
    for(j=jmin;j<jmax;j++)
    {
        indexRef[1]=j;
        indexMov[1]=j-ty;
        indexDif[1]=j;
        valueRef = m_mainVolume->getScalarPointer(indexRef);
        valueMov = m_mainVolume->getScalarPointer(indexMov);
        valueDif = m_differenceVolume->getScalarPointer(indexDif);
        for(i=imin;i<imax;i++)
        {
            (*valueDif) = (*valueMov) - (*valueRef);
            valueRef++;
            valueMov++;
            valueDif++;
        }
    }
    //Això ho fem perquè ens refresqui la imatge diferència que hem modificat
    m_2DView_2->getViewer()->getWindowLevelMapper()->Modified();
    m_2DView_2->getViewer()->refresh();
    QApplication::restoreOverrideCursor();
}


void QAngioSubstractionExtension::synchronization( Q2DViewerWidget * viewer, bool active )
{
    if( active )
    {
        // Per defecte sincronitzem només la tool de slicing
        ToolConfiguration *synchronizeConfiguration = new ToolConfiguration();
        synchronizeConfiguration->addAttribute( "Slicing", QVariant( true ) );
        m_toolManager->setViewerTool( viewer->getViewer(), "SynchronizeTool", synchronizeConfiguration );
        m_toolManager->activateTool("SynchronizeTool");
    }
    else
    {
        m_toolManager->removeViewerTool( viewer->getViewer(), "SynchronizeTool" );
    }
}

void QAngioSubstractionExtension::angioEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        //setPaintCursor();
    break;

    case vtkCommand::LeftButtonPressEvent:
        //leftButtonEventHandler();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        //setLeftButtonOff();
    break;

    case vtkCommand::RightButtonPressEvent:
    break;

    default:
    break;
    }

}

void QAngioSubstractionExtension::readSettings()
{
//    Settings settings;
}

void QAngioSubstractionExtension::writeSettings()
{
//    Settings settings;
}

}
