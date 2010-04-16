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
#include <itkMutualInformationImageToImageMetric.h>
#include <itkGradientDescentOptimizer.h>
#include <itkNormalizeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

namespace udg {

QAngioSubstractionExtension::QAngioSubstractionExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_differenceVolume(0), m_tdToolData(0)
{
    setupUi( this );
    AngioSubstractionSettings().init();

    //De moment amaguem el botó d'autoregistration perquè no acaba de funcionar
    m_autoRegistrationToolButton->setVisible(false);

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
    //connect( m_autoRegistrationToolButton, SIGNAL( clicked() ), SLOT( computeAutomateSingleImage( ) ) );
}

void QAngioSubstractionExtension::setInput( Volume *input )
{
    m_mainVolume = input;

	//Desactivem la sincronització perquè si no quan es canvia l'input no funciona correctament
	m_2DView_1->setSynchronized(false);
	m_2DView_2->setSynchronized(false);

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

	//Només actualitzem l'1 perquè el 2 ja es fa en l'acció computeDifferenceImage
	//Això es fa així perquè l'acció està lligada a un connect
	m_2DView_1->getViewer()->refresh();

	m_2DView_1->setSynchronized(true);
	m_2DView_2->setSynchronized(true);

    m_2DView_2->getViewer()->disableContextMenu();
    m_2DView_1->getViewer()->removeAnnotation( Q2DViewer::ScalarBarAnnotation );
    m_2DView_2->getViewer()->removeAnnotation( Q2DViewer::ScalarBarAnnotation );
}


void QAngioSubstractionExtension::computeDifferenceImage( int imageid )
{
    if(m_mainVolume == 0)
    {
        DEBUG_LOG("ERROR: Estem inicialitzant la diferència sense tenir input!");
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );
	//DEBUG_LOG(QString("Init computeDifferenceImage: %1").arg(imageid));
    
    //Actualitzem les dades de la transdifference tool
    m_toolManager->triggerTool("TransDifferenceTool");
    TransDifferenceTool* tdTool = static_cast<TransDifferenceTool*> ( m_2DView_2->getViewer()->getToolProxy()->getTool("TransDifferenceTool"));
    if(m_tdToolData == 0){
        m_tdToolData = static_cast<TransDifferenceToolData*> ( tdTool->getToolData() );
    }
    if(m_tdToolData->getInputVolume() != m_mainVolume){
        m_tdToolData->setInputVolume(m_mainVolume);
    }
    m_tdToolData->setReferenceSlice( imageid );
    tdTool->initializeDifferenceImage();
    m_toolManager->triggerTool("SlicingTool");
    
    QApplication::restoreOverrideCursor();
}

void QAngioSubstractionExtension::computeAutomateSingleImage( )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    const    unsigned int          Dimension = 2;
    typedef  Volume::ItkPixelType  PixelType;

    typedef itk::Image< PixelType, Dimension >  FixedImageType;
    typedef itk::Image< PixelType, Dimension >  MovingImageType;
    typedef   float     InternalPixelType;
    typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

    typedef itk::TranslationTransform< double, Dimension > TransformType;
    typedef itk::GradientDescentOptimizer                  OptimizerType;
    typedef itk::LinearInterpolateImageFunction< 
                                    InternalImageType,
                                    double             > InterpolatorType;
    typedef itk::ImageRegistrationMethod< 
                                    InternalImageType, 
                                    InternalImageType >  RegistrationType;
    typedef itk::MutualInformationImageToImageMetric< 
                                          InternalImageType, 
                                          InternalImageType >    MetricType;

    TransformType::Pointer      transform     = TransformType::New();
    OptimizerType::Pointer      optimizer     = OptimizerType::New();
    InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    RegistrationType::Pointer   registration  = RegistrationType::New();

    registration->SetOptimizer(     optimizer     );
    registration->SetTransform(     transform     );
    registration->SetInterpolator(  interpolator  );

    MetricType::Pointer         metric        = MetricType::New();
    registration->SetMetric( metric  );
    metric->SetFixedImageStandardDeviation(  0.4 );
    metric->SetMovingImageStandardDeviation( 0.4 );
    metric->SetNumberOfSpatialSamples( 50 );

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

    typedef itk::NormalizeImageFilter< 
                                FixedImageType, 
                                InternalImageType 
                                        > FixedNormalizeFilterType;

    typedef itk::NormalizeImageFilter< 
                                MovingImageType, 
                                InternalImageType 
                                              > MovingNormalizeFilterType;

    FixedNormalizeFilterType::Pointer fixedNormalizer = 
                                            FixedNormalizeFilterType::New();

    MovingNormalizeFilterType::Pointer movingNormalizer =
                                            MovingNormalizeFilterType::New();
    typedef itk::DiscreteGaussianImageFilter<
                                      InternalImageType, 
                                      InternalImageType
                                                    > GaussianFilterType;

    GaussianFilterType::Pointer fixedSmoother  = GaussianFilterType::New();
    GaussianFilterType::Pointer movingSmoother = GaussianFilterType::New();

    fixedSmoother->SetVariance( 2.0 );
    movingSmoother->SetVariance( 2.0 );
    fixedNormalizer->SetInput(  extractFixedImageFilter->GetOutput() );
    movingNormalizer->SetInput( extractMovingImageFilter->GetOutput() );

    fixedSmoother->SetInput( fixedNormalizer->GetOutput() );
    movingSmoother->SetInput( movingNormalizer->GetOutput() );

    registration->SetFixedImage(    fixedSmoother->GetOutput()    );
    registration->SetMovingImage(   movingSmoother->GetOutput()   );

    fixedNormalizer->Update();
    registration->SetFixedImageRegion( 
       fixedNormalizer->GetOutput()->GetBufferedRegion() );

    typedef RegistrationType::ParametersType ParametersType;
    ParametersType initialParameters( transform->GetNumberOfParameters() );

    initialParameters[0] = 0.0;  // Initial offset in mm along X
    initialParameters[1] = 0.0;  // Initial offset in mm along Y

    registration->SetInitialTransformParameters( initialParameters );

    optimizer->SetLearningRate( 20.0 );
    optimizer->SetNumberOfIterations( 200 );
    optimizer->MaximizeOn();

    try 
    { 
        registration->StartRegistration(); 
    } 
    catch( itk::ExceptionObject & err ) 
    { 
        std::cout << "ExceptionObject caught !" << std::endl; 
        std::cout << err << std::endl; 
        return;
    } 

    ParametersType finalParameters = registration->GetLastTransformParameters();

    double TranslationAlongX = finalParameters[0];
    double TranslationAlongY = finalParameters[1];

    unsigned int numberOfIterations = optimizer->GetCurrentIteration();

    double bestValue = optimizer->GetValue();


    // Print out results
    //
    DEBUG_LOG(QString( "Result = " ));
    DEBUG_LOG(QString( " Translation X = %1").arg( TranslationAlongX ) );
    DEBUG_LOG(QString( " Translation Y = %1").arg( TranslationAlongY ) );
    DEBUG_LOG(QString( " Iterations    = %1").arg( numberOfIterations ) );
    DEBUG_LOG(QString( " Metric value  = %1").arg( bestValue ) );
    double spacing[3];
    m_mainVolume->getSpacing( spacing );
    DEBUG_LOG(QString( " Translation X (in px) = %1").arg( TranslationAlongX / spacing[0] ) );
    DEBUG_LOG(QString( " Translation Y (in px) = %1").arg( TranslationAlongY / spacing[1] ) );

    //Actualitzem les dades de la transdifference tool
    m_toolManager->triggerTool("TransDifferenceTool");
    TransDifferenceTool* tdTool = static_cast<TransDifferenceTool*> ( m_2DView_2->getViewer()->getToolProxy()->getTool("TransDifferenceTool"));
    if(m_tdToolData == 0){
        m_tdToolData = static_cast<TransDifferenceToolData*> ( tdTool->getToolData() );
    }
    if(m_tdToolData->getInputVolume() != m_mainVolume){
        m_tdToolData->setInputVolume(m_mainVolume);
    }
    tdTool->setSingleDifferenceImage(TranslationAlongX / spacing[0],TranslationAlongY / spacing[1]);
    m_toolManager->triggerTool("SlicingTool");
    

/*    typedef itk::Image< PixelType, Dimension >  FixedImageType;
    typedef itk::Image< PixelType, Dimension >  MovingImageType;
    typedef itk::TranslationTransform< double, Dimension > TransformType;
    typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
    typedef itk::MattesMutualInformationImageToImageMetric< 
                                          FixedImageType, 
                                          MovingImageType >    MetricType;
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
*/

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

void QAngioSubstractionExtension::readSettings()
{
//    Settings settings;
}

void QAngioSubstractionExtension::writeSettings()
{
//    Settings settings;
}

}
