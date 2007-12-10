/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qlandmarkregistrationextension.h"

#include "toolsactionfactory.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "reglandmark.h"
#include "series.h"
#include "image.h"

//QT
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QSettings>
#include <QSize>
#include <QFileDialog>
#include <QMessageBox>
#include <QCursor>

// VTK
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>
#include <vtkAlgorithmOutput.h>

// ITK
#include <itkRescaleIntensityImageFilter.h>
#include <itkImageRegionConstIterator.h>

namespace udg {

QLandmarkRegistrationExtension::QLandmarkRegistrationExtension(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    m_firstVolume      = 0;
    m_secondVolume     = 0;
    m_registeredVolume = 0;

    m_seedList1.resize(0);

    m_seedSet1 = itk::VectorContainer<int, PointType>::New();
    m_seedSet2 = itk::VectorContainer<int, PointType>::New();

    m_seedLastActor1   = -1;
    m_seedLastActor2   = -1;
    m_seedLastActorReg = -1;

    m_movingSeed = false;

    createActions();
    createToolBars();
    createConnections();

    readSettings();
}

QLandmarkRegistrationExtension::~QLandmarkRegistrationExtension()
{
    writeSettings();
}

void QLandmarkRegistrationExtension::createActions()
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
    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_2DView_2 , SLOT( setVoxelInformationCaptionEnabled(bool) ) );

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_2 , SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateCounterClockWise() ) );
    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ) , m_2DView_2 , SLOT( rotateCounterClockWise() ) );

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

    m_seedAction = new QAction( 0 );
    m_seedAction->setText( tr("SeedTool") );
    m_seedAction->setStatusTip( tr("Enable/Disable seeding tool") );
    m_seedAction->setIcon( QIcon(":/images/seed.png") );
    m_seedAction->setCheckable( true );
    m_seedToolButton->setDefaultAction( m_seedAction );


    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView_2 , SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_seedAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_seedAction->trigger();
}

void QLandmarkRegistrationExtension::createToolBars()
{
}

void QLandmarkRegistrationExtension::createConnections()
{
    connect( m_savePushButton , SIGNAL( clicked() ), SLOT( saveTransform () ) );
    connect( m_loadPushButton , SIGNAL( clicked() ), SLOT( loadTransform () ) );
    connect( m_restorePushButton , SIGNAL( clicked() ), SLOT( restore () ) );
    connect( m_tryAgainPushButton , SIGNAL( clicked() ), SLOT( tryAgain () ) );
    connect( m_applyRegistrationPushButton , SIGNAL( clicked() ), SLOT( applyMethod() ) );

    connect( m_2DView , SIGNAL( eventReceived( unsigned long ) ), SLOT( landmarkEventHandler (unsigned long) ) );
    connect( m_2DView_2 , SIGNAL( eventReceived( unsigned long ) ), SLOT( landmarkEventHandler2 (unsigned long) ) );

    connect( m_opacityOverlaySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );

    connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_sliceViewSlider_2, SIGNAL( valueChanged(int) ) , m_2DView_2 , SLOT( setSlice(int) ) );

    connect( m_seedList1TableWidget, SIGNAL( cellPressed(int, int) ), SLOT( seed1Activated(int, int) ) );
    connect( m_seedList2TableWidget, SIGNAL( cellPressed(int, int) ), SLOT( seed2Activated(int, int) ) );

    connect( m_2DView , SIGNAL( sliceChanged( int ) ), SLOT( sliceChanged1 (int) ) );
    connect( m_2DView_2 , SIGNAL( sliceChanged( int ) ), SLOT( sliceChanged2 (int) ) );

    connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
    connect( m_2DView_2, SIGNAL( volumeChanged(Volume *) ), SLOT( setSecondInput( Volume * ) ) );

    connect( m_seriesSpinBox, SIGNAL( valueChanged(int) ), SLOT( setPhase(int) ) );
    connect( m_seriesSpinBox_2, SIGNAL( valueChanged(int) ), SLOT( setSecondPhase(int) ) );
}

void QLandmarkRegistrationExtension::readSettings()
{
    QSettings settings("GGG", "StarViewer-App-LandmarkRegistration");
    settings.beginGroup("StarViewer-App-LandmarkRegistration");

    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );
    m_verticalSplitter2->restoreState( settings.value("verticalSplitter2").toByteArray() );

    settings.endGroup();
}

void QLandmarkRegistrationExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-LandmarkRegistration");
    settings.beginGroup("StarViewer-App-LandmarkRegistration");

    settings.setValue("verticalSplitter" , m_verticalSplitter->saveState() );
    settings.setValue("verticalSplitter2", m_verticalSplitter2->saveState() );

    settings.endGroup();
}

void QLandmarkRegistrationExtension::setInput( Volume *input )
{
    m_inputVolume = input;

    if(m_inputVolume->getSeries()->getNumberOfPhases()==1)
    {
        m_seriesLabel->setVisible(false);
        m_seriesSpinBox->setVisible(false);
        m_firstVolume = m_inputVolume;
    }
    else
    {
        m_seriesLabel->setVisible(true);
        m_seriesSpinBox->setVisible(true);
        m_seriesSpinBox->setMinimum(0);
        m_seriesSpinBox->setMaximum(input->getSeries()->getNumberOfPhases() -1);
        m_firstVolume = m_inputVolume->getPhaseVolume(m_seriesSpinBox->value());
    }

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_firstVolume );
    m_2DView->setView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();

    int* dim;
    dim = m_firstVolume->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    m_tryAgainPushButton->setEnabled(false);

    //std::cout<<"setInput: NumSlices:"<<dim[2]-1<<std::endl;
    /*m_actionFactory = new ToolsActionFactory( 0 );
    m_windowLevelAction = m_actionFactory->getActionFrom( "WindowLevelTool" );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView , SLOT( setTool(QString) ) );
    m_2DView->enableTools();
    m_windowLevelAction->trigger();
*/
    m_2DView->setCursor(Qt::CrossCursor);
    m_2DView_2->setCursor(Qt::CrossCursor);
    m_2DView->render();
}

void QLandmarkRegistrationExtension::setPhase( int phase )
{
    m_firstVolume = m_inputVolume->getPhaseVolume(phase);

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_firstVolume );
    m_2DView->setView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();

    m_2DView->render();

}

void QLandmarkRegistrationExtension::setSecondInput( Volume *input )
{
    m_secondInputVolume = input;

    if(m_inputVolume->getSeries()->getNumberOfPhases()==1)
    {
        m_seriesLabel_2->setVisible(false);
        m_seriesSpinBox_2->setVisible(false);
        m_secondVolume = m_secondInputVolume;
    }
    else
    {
        m_seriesLabel_2->setVisible(true);
        m_seriesSpinBox_2->setVisible(true);
        m_seriesSpinBox_2->setMinimum(0);
        m_seriesSpinBox_2->setMaximum(input->getSeries()->getNumberOfPhases() -1);
        m_secondVolume = m_secondInputVolume->getPhaseVolume(m_seriesSpinBox_2->value());
    }

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView_2->setInput( m_secondVolume );
    m_2DView_2->setView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    int* dim;
    dim = m_secondVolume->getDimensions();
    m_sliceViewSlider_2->setMinimum(0);
    m_sliceViewSlider_2->setMaximum(dim[2]-1);
    m_sliceSpinBox_2->setMinimum(0);
    m_sliceSpinBox_2->setMaximum(dim[2]-1);
    m_sliceViewSlider_2->setValue(m_2DView_2->getCurrentSlice());

    /*m_windowLevelAction_2 = m_actionFactory->getActionFrom( "WindowLevelTool" );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView_2 , SLOT( setTool(QString) ) );
    m_2DView_2->enableTools();
    m_windowLevelAction_2->trigger();
    */

    m_2DView_2->render();
}

void QLandmarkRegistrationExtension::setSecondPhase( int phase )
{
    m_secondVolume = m_secondInputVolume->getPhaseVolume(phase);

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView_2->setInput( m_secondVolume );
    m_2DView_2->setView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    m_2DView_2->render();

}

void QLandmarkRegistrationExtension::applyMethod()
{
    if(m_seedList1.size() != m_seedList2.size())
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "There are not the same number of seeds into images" ) );
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->m_registrationMethod = LandmarkRegistrator::New();
    this->setLandmarks();
    m_registrationMethod->SetFixedLandmarkSet( m_seedSet1 );
    m_registrationMethod->SetMovingLandmarkSet( m_seedSet2 );
    //m_registrationMethod->SetOptimizerScales( m_LandmarkScales );//Deixem les que hi ha per defecte
    m_registrationMethod->SetOptimizerNumberOfIterations( 5000 );
    try
    {
        m_registrationMethod->StartRegistration();
    }
    catch( itk::ExceptionObject &e )
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "The registration process has not obtained successfull results" ) );
        return;
    }
//    std::cout<<"Final Transform: "<< m_registrationMethod->GetFinalParameters()<<std::endl;

  // Agafem la transformació final que s'ha aplicat sobre la imatge mòbil
    landmarkRegTransform = m_registrationMethod->GetTypedTransform();
    //landmarkRegTransform->SetParameters(m_registrationMethod->GetTypedTransform()->GetParameters());
    //landmarkRegTransform->SetCenter(m_registrationMethod->GetMovingCenter());
    //landmarkRegTransform->SetParameters(m_registrationMethod->GetInitialTransformParameters());

    typedef itk::LinearInterpolateImageFunction< Volume::ItkImageType, double > InterpolatorType;
    typedef itk::ResampleImageFilter<Volume::ItkImageType,Volume::ItkImageType> ResampleImageFilterType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    ResampleImageFilterType::Pointer resampleFilter = ResampleImageFilterType::New();

//     //Resample el 2n al 1r
//     typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
//     ConstIterator iter( m_secondVolume->getItkData(), m_secondVolume->getItkData()->GetBufferedRegion() );
//     iter.GoToBegin();
//     Volume::ItkPixelType minValue = iter.Get();
//     Volume::ItkPixelType maxValue = minValue;
//     Volume::ItkPixelType value;
//
//     while ( !iter.IsAtEnd() )
//     {
//         value = iter.Get();
//
//         if ( value < minValue ) { minValue = value; }
//         if ( value > maxValue ) { maxValue = value; }
//
//         ++iter;
//     }
//     double wl1[2], wl2[2];
//     m_2DView->getCurrentWindowLevel( wl1 );
//     m_2DView_2->getCurrentWindowLevel( wl2 );
//     typedef itk::RescaleIntensityImageFilter< Volume::ItkImageType , Volume::ItkImageType > RescaleFilterType;
//     RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
//     rescaler->SetInput( m_secondVolume->getItkData() );
//     Volume::ItkPixelType outputMinimum = (int)((wl1[0]/wl2[0])*((double) minValue - wl2[1]) + wl1[1]);
//     Volume::ItkPixelType outputMaximum = (int)((wl1[0]/wl2[0])*((double) maxValue - wl2[1]) + wl1[1]);
//     //     std::cout<<"Min: "<<minValue<<", Maximum: "<<maxValue<<std::endl;
//     //     std::cout<<"wl1: "<<wl1[0]<<", "<<wl1[1]<<std::endl;
//     //     std::cout<<"wl2: "<<wl2[0]<<", "<<wl2[1]<<std::endl;
//     //     std::cout<<"outputMin: "<<outputMinimum<<", outputMaximum: "<<outputMaximum<<std::endl;
//     rescaler->SetOutputMinimum( outputMinimum );
//     rescaler->SetOutputMaximum( outputMaximum );
//
//     rescaler->Update();
//
//     //interpolator->SetInputImage(m_secondVolume->getItkData());
//     //resampleFilter->SetInput(m_secondVolume->getItkData());
//     //interpolator->SetInputImage(rescaler->GetOutput());
//     resampleFilter->SetInput(rescaler->GetOutput());
//     resampleFilter->SetInterpolator(interpolator.GetPointer());
//     resampleFilter->SetSize(m_firstVolume->getItkData()->GetLargestPossibleRegion().GetSize());
//     resampleFilter->SetOutputOrigin(m_firstVolume->getItkData()->GetOrigin());
//     resampleFilter->SetOutputSpacing(m_firstVolume->getItkData()->GetSpacing());
//     resampleFilter->SetDefaultPixelValue( 100 );
//
//     //prova!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//     std::cout<<landmarkRegTransform<<std::endl;
//     //landmarkRegTransform->SetIdentity();
//
//
//     LandmarkRegTransformType::Pointer landmarkRegInverseTransform =LandmarkRegTransformType::New();
//     landmarkRegTransform->GetInverse(landmarkRegInverseTransform);
//     std::cout<<"Inverse: "<< landmarkRegInverseTransform<<std::endl;
//     resampleFilter->SetTransform(landmarkRegTransform);
//     //resampleFilter->SetTransform(landmarkRegInverseTransform);
//     try
//     {
//         resampleFilter->Update();
//         //rescalerFilter->Update();
//     }
//     catch( itk::ExceptionObject &e )
//     {
//         std::cerr << "ha petat aixo!! " << e << std::endl;
//     }
//
//     m_registeredVolume = new Volume();
//     m_registeredVolume->setData( resampleFilter->GetOutput() );
//


    //Resample 1r al 2n
    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator iter( m_firstVolume->getItkData(), m_firstVolume->getItkData()->GetBufferedRegion() );
    iter.GoToBegin();
    Volume::ItkPixelType minValue = iter.Get();
    Volume::ItkPixelType maxValue = minValue;
    Volume::ItkPixelType value;

    while ( !iter.IsAtEnd() )
    {
        value = iter.Get();

        if ( value < minValue ) { minValue = value; }
        if ( value > maxValue ) { maxValue = value; }

        ++iter;
    }
    double wl1[2], wl2[2];
    m_2DView->getCurrentWindowLevel( wl1 );
    m_2DView_2->getCurrentWindowLevel( wl2 );
    typedef itk::RescaleIntensityImageFilter< Volume::ItkImageType , Volume::ItkImageType > RescaleFilterType;
    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetInput( m_firstVolume->getItkData() );
    Volume::ItkPixelType outputMinimum = (int)((wl2[0]/wl1[0])*((double) minValue - wl1[1]) + wl2[1]);
    Volume::ItkPixelType outputMaximum = (int)((wl2[0]/wl1[0])*((double) maxValue - wl1[1]) + wl2[1]);
    //     std::cout<<"Min: "<<minValue<<", Maximum: "<<maxValue<<std::endl;
    //     std::cout<<"wl1: "<<wl1[0]<<", "<<wl1[1]<<std::endl;
    //     std::cout<<"wl2: "<<wl2[0]<<", "<<wl2[1]<<std::endl;
    //     std::cout<<"outputMin: "<<outputMinimum<<", outputMaximum: "<<outputMaximum<<std::endl;
    rescaler->SetOutputMinimum( outputMinimum );
    rescaler->SetOutputMaximum( outputMaximum );

    rescaler->Update();

    //interpolator->SetInputImage(m_secondVolume->getItkData());
    //resampleFilter->SetInput(m_secondVolume->getItkData());
    //interpolator->SetInputImage(rescaler->GetOutput());
    resampleFilter->SetInput(rescaler->GetOutput());
    resampleFilter->SetInterpolator(interpolator.GetPointer());
    resampleFilter->SetSize(m_secondVolume->getItkData()->GetLargestPossibleRegion().GetSize());
    resampleFilter->SetOutputOrigin(m_secondVolume->getItkData()->GetOrigin());
    resampleFilter->SetOutputSpacing(m_secondVolume->getItkData()->GetSpacing());
    resampleFilter->SetDefaultPixelValue( 100 );

    //prova!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //std::cout<<landmarkRegTransform<<std::endl;
    //landmarkRegTransform->SetIdentity();


    LandmarkRegTransformType::Pointer landmarkRegInverseTransform =LandmarkRegTransformType::New();
    landmarkRegTransform->GetInverse(landmarkRegInverseTransform);
    //std::cout<<"Inverse: "<< landmarkRegInverseTransform<<std::endl;
    //Ara toca l'original --> no inversa!!!
    resampleFilter->SetTransform(landmarkRegTransform);
    //resampleFilter->SetTransform(landmarkRegInverseTransform);
    try
    {
        resampleFilter->Update();
        //rescalerFilter->Update();
    }
    catch( itk::ExceptionObject &e )
    {
        std::cerr << "ha petat aixo!! " << e << std::endl;
    }

    m_registeredVolume = new Volume();
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    m_registeredVolume->setImages( m_firstVolume->getImages() );
    m_registeredVolume->setData( resampleFilter->GetOutput() );



//----------------------------------------------------------------------
/*        std::cout << "Init transform!! " << std::endl;
    Volume::ItkImageType::Pointer estimatedImage = Volume::ItkImageType::New();
    estimatedImage->SetRegions( m_firstVolume->getItkData()->GetLargestPossibleRegion() );
    estimatedImage->SetSpacing( m_firstVolume->getItkData()->GetSpacing() );
    estimatedImage->SetOrigin( m_firstVolume->getItkData()->GetOrigin() );
    estimatedImage->Allocate();

    InterpolatorType::Pointer interpolatorResample = InterpolatorType::New();
    interpolatorResample->SetInputImage(estimatedImage);
    Volume::ItkImageType::IndexType index;
    typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
    Iterator it( estimatedImage, estimatedImage->GetBufferedRegion() );
    it.GoToBegin();
    itk::Point<double,3> inputPoint, transformedPoint;

    while ( !it.IsAtEnd() )
    {
        index = it.GetIndex();

        estimatedImage->TransformIndexToPhysicalPoint(index, inputPoint);

        transformedPoint = landmarkRegInverseTransform->TransformPoint(inputPoint);

        if ( interpolatorResample->IsInsideBuffer(transformedPoint) )
        {
            it.Set(interpolatorResample->Evaluate(transformedPoint));
        }
        else
        {
            it.Set(100);
        }
        ++it;
    }

    m_registeredVolume->setData( estimatedImage );
    m_registeredVolume->updateVtkDataFromItkData();
        std::cout << "End transform!! " << std::endl;
*/
//----------------------------------------------------------------------
    itk::Point<double,3> point;
    itk::Point<double,3> pointMoved;
    std::vector<double> cpFixed;
    double pos[3];
    std::list<std::vector<double> >::iterator it2;
    it2 = m_seedList2.begin();
    while(it2 != m_seedList2.end())
    {
        cpFixed  = (*it2);

        point[0] = (double) cpFixed[0];
        point[1] = (double) cpFixed[1];
        point[2] = (double) cpFixed[2];

        pointMoved = landmarkRegTransform->TransformPoint(point);

        pos[0] = (double) pointMoved[0];
        pos[1] = (double) pointMoved[1];
        pos[2] = (double) pointMoved[2];
        //std::cout<<pos[0]<<", "<<pos[1]<<", "<<pos[2]<<std::endl;
        pos[2] = (double) ((int)(pos[2]/m_firstVolume->getSpacing()[2])+ 0.5) * m_firstVolume->getSpacing()[2];
        vtkSphereSource *point = vtkSphereSource::New();
        point->SetRadius(2);
        point-> SetCenter(pos);

        vtkActor *pointActor = vtkActor::New();
        pointActor -> GetProperty()->SetColor(0.14, 0.13, 0.91);
        vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
        pointMapper->SetInput( point->GetOutput() );
        pointActor->SetMapper( pointMapper );
        pointActor->VisibilityOn();

        pointMapper -> Delete();
        point       -> Delete();

        //std::cout<<pos[0]<<", "<<pos[1]<<", "<<pos[2]<<std::endl;
        //std::cout<<"Moved seed Slice: "<<(int)(pos[2]/m_firstVolume->getSpacing()[2])<<std::endl;
        m_seedSliceVectorReg.push_back((int)((pos[2]/m_firstVolume->getSpacing()[2])+1.5));
        //std::cout<<"slice2:"<<(int)((pos[2]/m_firstVolume->getSpacing()[2])+1.5)<<std::endl;
        m_2DView->getRenderer()-> AddActor( pointActor );
        m_seedActorVectorReg.push_back(pointActor);

        it2++;
    }
    /*it2 = m_seedList1.begin();
    std::vector<int>::iterator it;
    it  = m_seedSliceVector1.begin();
    std::cout<<"1"<<std::endl;
    while(it2 != m_seedList1.end())
    {
        cpFixed  = (*it2);
        std::cout<<cpFixed[0]<<", "<<cpFixed[1]<<", "<<cpFixed[2]<<std::endl;
        std::cout<<(int)(cpFixed[2]/m_firstVolume->getSpacing()[2])<<std::endl;
        std::cout<<(*it)<<std::endl;
        std::cout<<"!!!"<<std::endl;
        it2++;
        it++;
    }*/

//------------------------------------------------------------------------------


    //     m_2DView->setOverlayToBlend();
    //     m_2DView->setOpacityOverlay(((double)m_opacityOverlaySlider->value())/100.0);
    //     m_2DView->setOverlayInput(m_registeredVolume);
    //     m_opacityOverlaySlider->setEnabled(true);
    //     m_opacityLabel->setEnabled(true);
    //     m_2DView->getInteractor()->Render();


    m_2DView_2->setOverlayToBlend();
    m_2DView_2->setOpacityOverlay(((double)m_opacityOverlaySlider->value())/100.0);
    m_2DView_2->setOverlayInput(m_registeredVolume);
    m_opacityOverlaySlider->setEnabled(true);
    m_opacityLabel->setEnabled(true);
    m_tryAgainPushButton->setEnabled(true);
    m_2DView_2->getInteractor()->Render();

    QApplication::restoreOverrideCursor();
    DEBUG_LOG("EndApply");
}

void QLandmarkRegistrationExtension::setLandmarks()
{
    std::vector<double> cpFixed;
    std::vector<double> cpMoving;

    std::list<std::vector<double> >::iterator it;
    std::list<std::vector<double> >::iterator it2;

    itk::Point<double,3> pointFixed;
    itk::Point<double,3> pointMoving;

    it  = m_seedList1.begin();
    it2 = m_seedList2.begin();
    int contador = 0;
    while(it != m_seedList1.end())
    {
        cpFixed  = (*it);
        cpMoving = (*it2);

        pointFixed[0] = (double) cpFixed[0];   pointMoving[0] = (double) cpMoving[0];
        pointFixed[1] = (double) cpFixed[1];   pointMoving[1] = (double) cpMoving[1];
        pointFixed[2] = (double) cpFixed[2];   pointMoving[2] = (double) cpMoving[2];

        m_seedSet1->InsertElement(contador, pointFixed);
        m_seedSet2->InsertElement(contador, pointMoving);

        it++;
        it2++;
        contador++;
    }
}

void QLandmarkRegistrationExtension::landmarkEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        mouseMoveEventHandler( 1 );
    break;

    case vtkCommand::LeftButtonPressEvent:
        leftButtonEventHandler( 1 );
    break;

    case vtkCommand::LeftButtonReleaseEvent:
    break;

    case vtkCommand::RightButtonPressEvent:
        rightButtonPressEventHandler( 1 );
    break;

    case vtkCommand::RightButtonReleaseEvent:
        rightButtonReleaseEventHandler( 1 );
    break;

    default:
    break;
    }

}

void QLandmarkRegistrationExtension::landmarkEventHandler2( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        mouseMoveEventHandler( 2 );
    break;

    case vtkCommand::LeftButtonPressEvent:
        leftButtonEventHandler( 2 );
    break;

    case vtkCommand::LeftButtonReleaseEvent:
    break;

    case vtkCommand::RightButtonPressEvent:
        rightButtonPressEventHandler( 2 );
    break;

    case vtkCommand::RightButtonReleaseEvent:
        rightButtonReleaseEventHandler( 2 );
    break;

    default:
    break;
    }

}

void QLandmarkRegistrationExtension::mouseMoveEventHandler( int idVolume )
{
    if(m_movingSeed)
    {
        double pos[3];
        if( idVolume == 1 )
        {
            m_2DView->getCurrentCursorPosition(pos);
        }
        else        // idVolume == 2
        {
            m_2DView_2->getCurrentCursorPosition(pos);
        }

        std::vector<double> posVect(3);
        QString auxX;
        QString auxY;
        QString auxZ;
            //std::cout<<"pos: "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;

        posVect[0]=pos[0];
        posVect[1]=pos[1];
        posVect[2]=pos[2];
        auxX = QString("%1").arg(pos[0], 0, 'f', 1);
        auxY = QString("%1").arg(pos[1], 0, 'f', 1);
        auxZ = QString("%1").arg(pos[2], 0, 'f', 1);
        QTableWidgetItem *newItem = new QTableWidgetItem(auxX);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(auxY);
        QTableWidgetItem *newItem3 = new QTableWidgetItem(auxZ);

        vtkSphereSource* sphere = static_cast<vtkSphereSource*> (m_selectedActor->GetMapper()->GetInput()->GetProducerPort()->GetProducer());
        sphere->SetCenter(pos);
        m_selectedActor-> GetProperty()->SetColor(0.85, 0.13, 0.66);
        if( idVolume == 1 )
        {
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 0, newItem);
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 1, newItem2);
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 2, newItem3);
            m_2DView->getInteractor()->Render();
        }
        else    // idVolume == 2
        {
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 0, newItem);
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 1, newItem2);
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 2, newItem3);
            m_2DView_2->getInteractor()->Render();
        }
    }
}

void QLandmarkRegistrationExtension::leftButtonEventHandler( int idVolume )
{
    if(m_seedToolButton->isChecked())
    {
        //std::cout<<"Seed Tool"<<std::endl;
        m_2DView->disableTools();
        m_2DView_2->disableTools();
        setNewSeedPosition( idVolume );
    }
    else
    {
        m_2DView->enableTools();
        m_2DView_2->enableTools();
    }
}

void QLandmarkRegistrationExtension::setNewSeedPosition( int idVolume )
{
    double pos[3];
    std::vector<double> posVect(3);
    QString auxX;
    QString auxY;
    QString auxZ;
    if( idVolume == 1 )
    {
        m_2DView->getCurrentCursorPosition(pos);
    }
    else        // idVolume == 2
    {
        m_2DView_2->getCurrentCursorPosition(pos);
    }
        //std::cout<<"pos: "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;

    if(!( pos[0] == -1 && pos[1] == -1 && pos[2] == -1))
    {

        posVect[0]=pos[0];
        posVect[1]=pos[1];
        posVect[2]=pos[2];

        auxX = QString("%1").arg(pos[0], 0, 'f', 1);
        auxY = QString("%1").arg(pos[1], 0, 'f', 1);
        auxZ = QString("%1").arg(pos[2], 0, 'f', 1);
        QTableWidgetItem *newItem = new QTableWidgetItem(auxX);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(auxY);
        QTableWidgetItem *newItem3 = new QTableWidgetItem(auxZ);

        vtkSphereSource *point = vtkSphereSource::New();
        point->SetRadius(1.5);
        point-> SetCenter(pos);

        vtkActor *pointActor = vtkActor::New();
        pointActor -> GetProperty()->SetColor(0.85, 0.13, 0.26);
        vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
        pointMapper->SetInput( point->GetOutput() );
        pointActor->SetMapper( pointMapper );
        pointActor->VisibilityOn();


        if( idVolume == 1 )
        {
            m_seedList1.push_back(posVect);
            m_seedSliceVector1.push_back(m_2DView->getCurrentSlice());
            m_seedList1TableWidget->setItem(m_seedList1.size()-1, 0, newItem);
            m_seedList1TableWidget->setItem(m_seedList1.size()-1, 1, newItem2);
            m_seedList1TableWidget->setItem(m_seedList1.size()-1, 2, newItem3);

            m_2DView->getRenderer()-> AddActor( pointActor );
            m_seedActorVector1.push_back(pointActor);
            m_2DView->getInteractor()->Render();

            //connect( m_2DView , SIGNAL( sliceChanged(int) ), SLOT( sliceChanged(int) ) );

        }
        else    // idVolume == 2
        {
            m_seedList2.push_back(posVect);
            m_seedSliceVector2.push_back(m_2DView_2->getCurrentSlice());
            m_seedList2TableWidget->setItem(m_seedList2.size()-1, 0, newItem);
            m_seedList2TableWidget->setItem(m_seedList2.size()-1, 1, newItem2);
            m_seedList2TableWidget->setItem(m_seedList2.size()-1, 2, newItem3);

            m_2DView_2->getRenderer()-> AddActor( pointActor );
            m_seedActorVector2.push_back(pointActor);
            m_2DView_2->getInteractor()->Render();
        }

    pointMapper -> Delete();
    point       -> Delete();

    }
}

void QLandmarkRegistrationExtension::rightButtonPressEventHandler( int idVolume )
{
    double pos[3];
    double* posCenter;
    int s;
    if( idVolume == 1 )
    {
        m_2DView->getCurrentCursorPosition(pos);
        s = m_2DView->getCurrentSlice();
    }
    else        // idVolume == 2
    {
        m_2DView_2->getCurrentCursorPosition(pos);
        s = m_2DView_2->getCurrentSlice();
    }
        //std::cout<<"pos: "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;


    std::vector<int>::iterator it;
    std::list< std::vector<double> >::iterator itList;
    std::vector<vtkActor*>::iterator itActor;
    int cont = 0;
    if( idVolume == 1 )
    {
        it  = m_seedSliceVector1.begin();
        itActor  = m_seedActorVector1.begin();
        itList = m_seedList1.begin();
        while(it != m_seedSliceVector1.end() && !m_movingSeed)
        {
            if((*it)==s)
            {
                posCenter = (*itActor)->GetMapper()->GetInput()->GetCenter();
                //std::cout<<posCenter[0]<<" "<<posCenter[1]<<" "<<posCenter[2]<<std::endl;
                if((fabs(posCenter[0]-pos[0]) < 4) && (fabs(posCenter[1]-pos[1]) < 4))
                {
                    m_movingSeed=true;
                    m_selectedActor = (*itActor);
                    m_idSelectedSeed = cont;
                    m_itListSelected = itList;
                    (*itActor)-> GetProperty()->SetColor(0.85, 0.13, 0.66);
                }
            }
            cont++;
            itActor++;
            it++;
            itList++;
        }
        m_2DView->getInteractor()->Render();
    }
    else        // idVolume == 2
    {
        it  = m_seedSliceVector2.begin();
        itActor  = m_seedActorVector2.begin();
        itList = m_seedList2.begin();
        while(it != m_seedSliceVector2.end() && !m_movingSeed)
        {
            if((*it)==s)
            {
                posCenter = (*itActor)->GetMapper()->GetInput()->GetCenter();
                //std::cout<<posCenter[0]<<" "<<posCenter[1]<<" "<<posCenter[2]<<std::endl;
                if((fabs(posCenter[0]-pos[0]) < 4) && (fabs(posCenter[1]-pos[1]) < 4))
                {
                    m_movingSeed=true;
                    m_selectedActor = (*itActor);
                    m_idSelectedSeed = cont;
                    m_itListSelected = itList;
                    (*itActor)-> GetProperty()->SetColor(0.85, 0.13, 0.66);
                }
            }
            cont++;
            itActor++;
            itList++;
            it++;
        }
        m_2DView_2->getInteractor()->Render();
    }
}

void QLandmarkRegistrationExtension::rightButtonReleaseEventHandler( int idVolume )
{
    if(m_movingSeed)
    {
        double pos[3];
        std::vector<double> posVect(3);
        QString auxX;
        QString auxY;
        QString auxZ;
        if( idVolume == 1 )
        {
            m_2DView->getCurrentCursorPosition(pos);
        }
        else        // idVolume == 2
        {
            m_2DView_2->getCurrentCursorPosition(pos);
        }
            //std::cout<<"pos: "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;

        posVect[0]=pos[0];
        posVect[1]=pos[1];
        posVect[2]=pos[2];

        (*m_itListSelected) = posVect;

        auxX = QString("%1").arg(pos[0], 0, 'f', 1);
        auxY = QString("%1").arg(pos[1], 0, 'f', 1);
        auxZ = QString("%1").arg(pos[2], 0, 'f', 1);
        QTableWidgetItem *newItem = new QTableWidgetItem(auxX);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(auxY);
        QTableWidgetItem *newItem3 = new QTableWidgetItem(auxZ);

        vtkSphereSource *point = vtkSphereSource::New();
        point->SetRadius(1.5);
        point-> SetCenter(pos);
        vtkActor *pointActor = vtkActor::New();
        pointActor -> GetProperty()->SetColor(0.85, 0.13, 0.26);
        vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
        pointMapper->SetInput( point->GetOutput() );
        pointActor->SetMapper( pointMapper );
        pointActor->VisibilityOn();

        //((vtkSphereSource*)(m_selectedActor->GetMapper()->GetInput()))->SetCenter(pos);
        //m_selectedActor-> GetProperty()->SetColor(0.85, 0.13, 0.66);

        if( idVolume == 1 )
        {
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 0, newItem);
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 1, newItem2);
            m_seedList1TableWidget->setItem(m_idSelectedSeed, 2, newItem3);
            m_2DView->getRenderer()-> RemoveActor( m_selectedActor );
            m_2DView->getRenderer()-> AddActor( pointActor );
            m_seedActorVector1[m_idSelectedSeed] = pointActor;
            m_2DView->getInteractor()->Render();
        }
        else    // idVolume == 2
        {
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 0, newItem);
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 1, newItem2);
            m_seedList2TableWidget->setItem(m_idSelectedSeed, 2, newItem3);
            m_2DView_2->getRenderer()-> RemoveActor( m_selectedActor );
            m_2DView_2->getRenderer()-> AddActor( pointActor );
            m_seedActorVector2[m_idSelectedSeed] = pointActor;
            m_2DView_2->getInteractor()->Render();
        }
        pointMapper -> Delete();
        point       -> Delete();

    }
    m_movingSeed = false;
}

void QLandmarkRegistrationExtension::viewFirstVolume( )
{
/*    QSize mida = layoutWidget_2->size();
    mida.setWidth(1);
    layoutWidget_2->resize(mida);
    m_verticalSplitter->refresh();*/
//    int min, max;
//    m_verticalSplitter->getRange(0,&min,&max);
//    std::cout<<"Splitter: "<<min<<" "<<max<<std::endl;
//    m_verticalSplitter->moveSplitter(max, 0);
}

void QLandmarkRegistrationExtension::viewSecondVolume( )
{
}

void QLandmarkRegistrationExtension::setOpacity( int op )
{
    //     m_2DView->setOpacityOverlay(((double)op)/100.0);
    //     m_2DView->setOverlayInput(m_registeredVolume);
    //     m_2DView->getInteractor()->Render();

    m_2DView_2->setOpacityOverlay(((double)op)/100.0);
    m_2DView_2->setOverlayInput(m_registeredVolume);
    m_2DView_2->render();
}

void QLandmarkRegistrationExtension::seed1Activated( int row, int aux)
{
    //std::cout<<"Row1: "<<row<<std::endl;
    if( row < (int)m_seedActorVectorReg.size() )
    {
        m_seedActorVectorReg[row]->GetProperty()->SetColor(0.13, 0.92, 0.26);
        if(m_seedLastActorReg != -1 && m_seedLastActorReg != row)   //no � la primera ni la que estem seleccionant
        {
            m_seedActorVectorReg[m_seedLastActorReg]->GetProperty()->SetColor(0.14, 0.13, 0.91);
        }
        m_seedLastActorReg = row;
        //m_2DView->setSlice(m_seedSliceVector1[row]);
    }
    if( row < (int)m_seedActorVector1.size() )
    {
        m_seedActorVector1[row]->GetProperty()->SetColor(0.13, 0.92, 0.26);
        if(m_seedLastActor1 != -1 && m_seedLastActor1 != row)   //no � la primera ni la que estem seleccionant
        {
            m_seedActorVector1[m_seedLastActor1]->GetProperty()->SetColor(0.85, 0.13, 0.26);
        }
        m_seedLastActor1 = row;
        //m_2DView->setSlice(m_seedSliceVector1[row]);
        m_sliceViewSlider->setValue(m_seedSliceVector1[row]);
        m_2DView->getInteractor()->Render();
    }
    if(aux != -1)
    {
       seed2Activated(row,-1);
    }
}

void QLandmarkRegistrationExtension::seed2Activated( int row, int aux)
{
    //std::cout<<"Row2: "<<row<<std::endl;
    if( row < (int)m_seedActorVector2.size() )
    {
        m_seedActorVector2[row]->GetProperty()->SetColor(0.13, 0.92, 0.26);
        if(m_seedLastActor2 != -1 && m_seedLastActor2 != row)   //no � la primera ni la que estem seleccionant
        {
            m_seedActorVector2[m_seedLastActor2]->GetProperty()->SetColor(0.85, 0.13, 0.26);
        }
        m_seedLastActor2 = row;
        m_2DView_2->setSlice(m_seedSliceVector2[row]);
        m_2DView_2->getInteractor()->Render();
    }
    if(aux != -1)
    {
        seed1Activated(row,-1);
    }
}

void QLandmarkRegistrationExtension::sliceChanged1( int s )
{
    m_sliceViewSlider->setValue(s);
    std::vector<int>::iterator it;
    std::vector<vtkActor*>::iterator itActor;
    it  = m_seedSliceVector1.begin();
    itActor  = m_seedActorVector1.begin();
    while(it != m_seedSliceVector1.end())
    {
        if((*it)==s)
        {
            (*itActor)->VisibilityOn();
        }
        else
        {
            (*itActor)->VisibilityOff();
        }
        itActor++;
        it++;
    }
    //Llavors del registre
    it  = m_seedSliceVectorReg.begin();
    itActor  = m_seedActorVectorReg.begin();
    while(it != m_seedSliceVectorReg.end())
    {
        if((*it)==s)
        {
            (*itActor)->VisibilityOn();
        }
        else
        {
            (*itActor)->VisibilityOff();
        }
        itActor++;
        it++;
    }
    m_2DView->getInteractor()->Render();
    //std::cout<<"CurrentSlice:"<<m_2DView->getCurrentSlice()<<std::endl;
/*    double pos[3];
    std::cout<<"ChapusSlice:"<<(int)((pos[2]/m_firstVolume->getSpacing()[2])+0.5)<<std::endl;*/
}

void QLandmarkRegistrationExtension::sliceChanged2( int s )
{
    m_sliceViewSlider_2->setValue(s);
    std::vector<int>::iterator it;
    std::vector<vtkActor*>::iterator itActor;
    it  = m_seedSliceVector2.begin();
    itActor  = m_seedActorVector2.begin();
    while(it != m_seedSliceVector2.end())
    {
        if((*it)==s)
        {
            (*itActor)->VisibilityOn();
        }
        else
        {
            (*itActor)->VisibilityOff();
        }
        itActor++;
        it++;
    }
    m_2DView_2->getInteractor()->Render();
}

void QLandmarkRegistrationExtension::saveTransform(  )
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Transform file") , QDir::homePath() , tr("all Files (*)") );
    if ( !fileName.isEmpty() )
    {
        ofstream fout(qPrintable( fileName ));
        DEBUG_LOG(qPrintable( fileName  ));
        for(unsigned int i=0;i<landmarkRegTransform->GetNumberOfParameters();i++)
        {
            fout<<landmarkRegTransform->GetParameters()[i]<<std::endl;
        }
        //fout<<landmarkRegTransform->GetParameters()<<std::endl;
        fout<<landmarkRegTransform->GetCenter()[0]<<" "<<landmarkRegTransform->GetCenter()[1]<<" "<<landmarkRegTransform->GetCenter()[2]<<" "<<std::endl;
        fout.close();
    }

}

void QLandmarkRegistrationExtension::loadTransform(  )
{
    if(landmarkRegTransform.IsNull() )
    {
        landmarkRegTransform = LandmarkRegTransformType::New();
    }

    QString fileName = QFileDialog::getOpenFileName( this , tr("Chose a transform filename") , QDir::homePath() , tr("all Files (*)") );
    if ( !fileName.isEmpty() )
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        ifstream fin(qPrintable( fileName ));
        //std::cout<<qPrintable( fileName  )<<std::endl;
        LandmarkRegTransformType::ParametersType parameters(landmarkRegTransform->GetNumberOfParameters());
        LandmarkRegTransformType::InputPointType center;
        for(unsigned int i=0;i<landmarkRegTransform->GetNumberOfParameters();i++)
        {
            fin>>parameters[i];
        }
        //std::cout<<landmarkRegTransform;
        landmarkRegTransform->SetParameters(parameters);
        //fin>>center[0]>>" "center[1]>>" "center[2];
        for(unsigned int i=0;i<3;i++)
        {
            fin>>center[i];
        }
        landmarkRegTransform->SetCenter(center);

        fin.close();
        //std::cout<<landmarkRegTransform;
        //std::cout<<landmarkRegTransform->GetParameters()<<std::endl;
        //std::cout<<landmarkRegTransform->GetCenter()[0]<<" "<<landmarkRegTransform->GetCenter()[1]<<" "<<landmarkRegTransform->GetCenter()[2] <<"  "<<std::endl;


        typedef itk::LinearInterpolateImageFunction< Volume::ItkImageType, double > InterpolatorType;
        typedef itk::ResampleImageFilter<Volume::ItkImageType,Volume::ItkImageType> ResampleImageFilterType;

        InterpolatorType::Pointer interpolator = InterpolatorType::New();
        ResampleImageFilterType::Pointer resampleFilter = ResampleImageFilterType::New();


        //Resample 1r al 2n
        typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
        ConstIterator iter( m_firstVolume->getItkData(), m_firstVolume->getItkData()->GetBufferedRegion() );
        iter.GoToBegin();
        Volume::ItkPixelType minValue = iter.Get();
        Volume::ItkPixelType maxValue = minValue;
        Volume::ItkPixelType value;

        while ( !iter.IsAtEnd() )
        {
            value = iter.Get();

            if ( value < minValue ) { minValue = value; }
            if ( value > maxValue ) { maxValue = value; }

            ++iter;
        }
        double wl1[2], wl2[2];
        m_2DView->getCurrentWindowLevel( wl1 );
        m_2DView_2->getCurrentWindowLevel( wl2 );
        typedef itk::RescaleIntensityImageFilter< Volume::ItkImageType , Volume::ItkImageType > RescaleFilterType;
        RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
        rescaler->SetInput( m_firstVolume->getItkData() );
        Volume::ItkPixelType outputMinimum = (int)((wl2[0]/wl1[0])*((double) minValue - wl1[1]) + wl2[1]);
        Volume::ItkPixelType outputMaximum = (int)((wl2[0]/wl1[0])*((double) maxValue - wl1[1]) + wl2[1]);
        rescaler->SetOutputMinimum( outputMinimum );
        rescaler->SetOutputMaximum( outputMaximum );

        rescaler->Update();

        resampleFilter->SetInput(rescaler->GetOutput());
        resampleFilter->SetInterpolator(interpolator.GetPointer());
        resampleFilter->SetSize(m_secondVolume->getItkData()->GetLargestPossibleRegion().GetSize());
        resampleFilter->SetOutputOrigin(m_secondVolume->getItkData()->GetOrigin());
        resampleFilter->SetOutputSpacing(m_secondVolume->getItkData()->GetSpacing());
        resampleFilter->SetDefaultPixelValue( 100 );


        resampleFilter->SetTransform(landmarkRegTransform);

        try
        {
            resampleFilter->Update();
        }
        catch( itk::ExceptionObject &e )
        {
            DEBUG_LOG( "ha petat aixo!! ");
        }

        m_registeredVolume = new Volume();
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_registeredVolume->setImages( m_firstVolume->getImages() );
        m_registeredVolume->setData( resampleFilter->GetOutput() );

        m_2DView_2->setOverlayToBlend();
        m_2DView_2->setOpacityOverlay(((double)m_opacityOverlaySlider->value())/100.0);
        m_2DView_2->setOverlayInput(m_registeredVolume);
        m_opacityOverlaySlider->setEnabled(true);
        m_opacityLabel->setEnabled(true);
        m_2DView_2->getInteractor()->Render();

        QApplication::restoreOverrideCursor();
        //std::cout<<"EndApply"<<std::endl;
    }
}

void QLandmarkRegistrationExtension::restore(  )
{
    std::vector<vtkActor*>::iterator itActor;
    itActor  = m_seedActorVector1.begin();
    while(itActor != m_seedActorVector1.end())
    {
        m_2DView->getRenderer()-> RemoveActor( *itActor );
        itActor++;
    }
    itActor  = m_seedActorVector2.begin();
    while(itActor != m_seedActorVector2.end())
    {
        m_2DView_2->getRenderer()-> RemoveActor( *itActor );
        itActor++;
    }
    itActor  = m_seedActorVectorReg.begin();
    while(itActor != m_seedActorVectorReg.end())
    {
        m_2DView->getRenderer()-> RemoveActor( *itActor );
        itActor++;
    }
    m_seedList1TableWidget->clear();
    m_seedList2TableWidget->clear();
    m_seedSet1->CastToSTLContainer( ).clear();
    m_seedSet2->CastToSTLContainer( ).clear();
    m_seedList1.clear();
    m_seedList2.clear();
    m_seedActorVector1.clear();
    m_seedActorVector2.clear();
    m_seedActorVectorReg.clear();
    m_seedSliceVector1.clear();
    m_seedSliceVector2.clear();
    m_seedSliceVectorReg.clear();
    m_seedLastActor1 = -1;
    m_seedLastActor2 = -1;
    m_seedLastActorReg = -1;

    if(m_registeredVolume != 0)
    {
        m_2DView_2->setNoOverlay();
        m_opacityOverlaySlider->setEnabled(false);
        m_opacityLabel->setEnabled(false);
    }

    m_tryAgainPushButton->setEnabled(false);

    m_2DView->render();
    m_2DView_2->render();

}

void QLandmarkRegistrationExtension::tryAgain(  )
{
    std::vector<vtkActor*>::iterator itActor;
    itActor  = m_seedActorVectorReg.begin();
    while(itActor != m_seedActorVectorReg.end())
    {
        m_2DView->getRenderer()-> RemoveActor( *itActor );
        itActor++;
    }
    m_seedActorVectorReg.clear();
    m_seedSliceVectorReg.clear();
    if(m_registeredVolume != 0)
    {
        m_2DView_2->setNoOverlay();
        m_opacityOverlaySlider->setEnabled(false);
        m_opacityLabel->setEnabled(false);
    }

    m_2DView->render();
    m_2DView_2->render();

}
}
