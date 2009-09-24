/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qperfusionmapreconstructionextension.h"

#include "perfusionmapcalculatormainthread.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "series.h"
#include "image.h"
#include "study.h"
#include "patient.h"
#include "toolmanager.h"
#include "patientbrowsermenu.h"
#include "drawer.h"
#include "drawerpoint.h"
#include "hoverpoints.h"
#include "mathtools.h" // pel PI
#include "qgraphicplotwidget.h"
#include "perfusionmapreconstructionsettings.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

//Qt
#include <QString>
#include <QVector>
#include <QPair>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QMultiMap>
#include <QTextStream>
// VTK
#include <vtkCommand.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>
// ITK
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkCastImageFilter.h>
#include "itkMinimumMaximumImageCalculator.h"
#include <itkVnlFFTRealToComplexConjugateImageFilter.h>
#include <itkVnlFFTComplexConjugateToRealImageFilter.h>
//Fourier Transform
//#include <fftw3.h>
//#include <complex>
#include <cmath> // pel ceil

namespace udg {

// definició de constants
const double QPerfusionMapReconstructionExtension::TE = 25.0;
const double QPerfusionMapReconstructionExtension::TR = 1.5;

QPerfusionMapReconstructionExtension::QPerfusionMapReconstructionExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_DSCVolume(0), m_isLeftButtonPressed(false), m_mapCalculator(0), m_aifDrawPoint(0)
{
    setupUi( this );
    PerfusionMapReconstructionSettings().init();

    m_mapCalculator = new PerfusionMapCalculatorMainThread;

    initializeTools();
    createConnections();
    readSettings();

    m_graphicplot->setTitle("DeltaR signal");
    m_graphicplot->setAutoLimits();
    m_aifplot->setTitle("AIF");
    m_aifplot->setAutoLimits();
}

QPerfusionMapReconstructionExtension::~QPerfusionMapReconstructionExtension()
{
    delete m_toolManager;
    writeSettings();
}

void QPerfusionMapReconstructionExtension::initializeTools()
{
    // Tools
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->registerTool("ScreenShotTool") );
    m_toolManager->registerTool("SynchronizeTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "ScreenShotTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools( defaultTools );

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // registrem al manager les tools que van amb el viewer principal
    m_toolManager->setupRegisteredTools( m_2DView );

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool("RotateClockWiseActionTool") );
    m_toolManager->enableRegisteredActionTools( m_2DView );
}

void QPerfusionMapReconstructionExtension::createConnections()
{
  connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), SLOT( eventHandler(unsigned long) ) );
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
  connect( m_2DView, SIGNAL( sliceChanged(int) ) , m_sliceViewSlider , SLOT( setValue(int) ) );
  connect( m_2DView, SIGNAL( sliceChanged(int) ) , SLOT( paintMeanSlice(int) ) );
  connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
  connect( m_chooseDSCPushButton, SIGNAL( clicked() ), SLOT( contextMenuDSCRelease() ) );
  connect( m_computePerfusionPushButton, SIGNAL( clicked() ), SLOT( computePerfusionMap() ) );
  //connect( m_filterPushButton, SIGNAL( clicked() ), SLOT( applyFilterMapImage() ) );
  connect( m_mapViewComboBox, SIGNAL( currentIndexChanged (int) ), SLOT( changeMap(int) ) );
  connect( m_mapCalculator, SIGNAL( computed() ), SLOT( paintMap() ) );
  //connect( m_2DView, SIGNAL( windowLevelChanged( double,double ) ), SLOT( createColorMap( double, double ) ) );
}

void QPerfusionMapReconstructionExtension::setInput( Volume *input )
{
    m_mainVolume = input;

    if (this->findProbableSeries( ) )
    {
        //std::cout<<"Tot ok!!"<<std::endl;
    }
}


void QPerfusionMapReconstructionExtension::changeMap( int value )
{
    //Com que al fer un setInput hem destruit tots els draws del viewer, hem de tornar a reservar memòria
    //m_aifDrawPoint = new DrawerPoint();
    //double pos[3];

    switch(value)
    {
    case 0:     //CBV
        m_2DView->getDrawer()->removeAllPrimitives();
        m_2DView->setInput( m_mapCalculator->getCBVVolume() );
        this->createColorMap(m_mapCalculator->getCBVImage(), m_2DView);
        // Suposem que tots els volums reconstruits tenen les mateixes mides, que en principi és correcte
/*        pos[0] = m_aifIndex[0]*m_map0Volume->getSpacing()[0] + m_map0Volume->getOrigin()[0];
        pos[1] = m_aifIndex[1]*m_map0Volume->getSpacing()[1] + m_map0Volume->getOrigin()[1];
        pos[2] = m_aifIndex[2]*m_map0Volume->getSpacing()[2] + m_map0Volume->getOrigin()[2];*/
       break;
    case 1:     //CBF
        m_2DView->getDrawer()->removeAllPrimitives();
        m_2DView->setInput( m_mapCalculator->getCBFVolume() );
        this->createColorMap( m_mapCalculator->getCBFImage(), m_2DView);
        // Suposem que tots els volums reconstruits tenen les mateixes mides, que en principi és correcte
/*        pos[0] = m_aifIndex[0]*m_map2Volume->getSpacing()[0] + m_map2Volume->getOrigin()[0];
        pos[1] = m_aifIndex[1]*m_map2Volume->getSpacing()[1] + m_map2Volume->getOrigin()[1];
        pos[2] = m_aifIndex[2]*m_map2Volume->getSpacing()[2] + m_map2Volume->getOrigin()[2];*/
        break;
    case 2:     //MTT
        m_2DView->getDrawer()->removeAllPrimitives();
        m_2DView->setInput( m_mapCalculator->getMTTVolume() );
        this->createColorMap( m_mapCalculator->getMTTImage(), m_2DView);
        // Suposem que tots els volums reconstruits tenen les mateixes mides, que en principi és correcte
/*        pos[0] = m_aifIndex[0]*m_map1Volume->getSpacing()[0] + m_map1Volume->getOrigin()[0];
        pos[1] = m_aifIndex[1]*m_map1Volume->getSpacing()[1] + m_map1Volume->getOrigin()[1];
        pos[2] = m_aifIndex[2]*m_map1Volume->getSpacing()[2] + m_map1Volume->getOrigin()[2];*/
        break;
    default:
        break;
    }

    //m_aifDrawPoint->setPosition(pos);
    /*std::cout<<"Pos: ["<<pos[0]<<","<<pos[1]<<","<<pos[2]<<"]"<<std::endl;
    std::cout<<"Index: ["<<m_aifIndex[0]<<","<<m_aifIndex[1]<<","<<m_aifIndex[2]<<"]"<<m_aifSlice<<std::endl;
    std::cout<<m_aifDrawPoint<<std::endl;
    std::cout<<m_2DView->getView()<<std::endl;
    */
    //m_2DView->getDrawer()->draw(m_aifDrawPoint, m_2DView->getView(), m_aifSlice);
    //m_2DView->getDrawer()->refresh();
}

void QPerfusionMapReconstructionExtension::computePerfusionMap( )
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_mapCalculator->setDSCVolume(m_DSCVolume);
    m_mapCalculator->run();

    m_meanseries = m_mapCalculator->getMeanDeltaRPerSlice();

/*    itk::MinimumMaximumImageCalculator< DoubleImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< DoubleImageType >::New();
    minmaxCalc->SetImage(m_mapCalculator->getCBVImage());
    minmaxCalc->SetRegion(m_mapCalculator->getCBVImage()->GetRequestedRegion());
    minmaxCalc->Compute();
    m_graphicplot->setMaxY(minmaxCalc->GetMaximum());
    m_graphicplot->setMinY(minmaxCalc->GetMinimum());
    std::cout<<"Max = "<<minmaxCalc->GetMaximum()<<", min = "<<minmaxCalc->GetMinimum()<<std::endl;*/

    QApplication::restoreOverrideCursor();
}


void QPerfusionMapReconstructionExtension::paintMap( )
{
    switch(m_mapViewComboBox->currentIndex())
    {
    case 0:     //CBV
        m_2DView->setInput( m_mapCalculator->getCBVVolume() );
        m_2DView->removeAnnotation( Q2DViewer::ScalarBarAnnotation );
        this->createColorMap(m_mapCalculator->getCBVImage(), m_2DView);
        break;
    case 1:     //CBF
        m_2DView->setInput( m_mapCalculator->getCBFVolume() );
        m_2DView->removeAnnotation(Q2DViewer::ScalarBarAnnotation);
        //m_2DView->resetWindowLevelToDefault();
        this->createColorMap(m_mapCalculator->getCBFImage(), m_2DView);
        break;
    case 2:     //MTT
        m_2DView->setInput( m_mapCalculator->getMTTVolume() );
        m_2DView->removeAnnotation(Q2DViewer::ScalarBarAnnotation);
        //m_2DView->resetWindowLevelToDefault();
        this->createColorMap(m_mapCalculator->getMTTImage(), m_2DView);
        break;
    default:
        break;
    }

    QVector<double> inputAIF = m_mapCalculator->getAIF();

    double minp = inputAIF[0];
    double maxp = inputAIF[0];
    int t, tend = inputAIF.size();

    for (t=0;t<tend;t++)
    {
        if(inputAIF[t]>maxp) maxp = inputAIF[t];
        if(inputAIF[t]<minp) minp = inputAIF[t];
    }

    m_aifplot->setData(inputAIF);



/*
    QTime time;
    int deltaRtime = 0;
    int momentstime = 0;
    int findAiftime = 0;
    int computePerfusiontime = 0;
    time.restart();
    this->computeDeltaR();
    deltaRtime += time.elapsed();
    time.restart();
    this->computeMoments();
    momentstime += time.elapsed();
    time.restart();
    this->findAIF();
    findAiftime += time.elapsed();
    time.restart();
    this->computePerfusion();
    computePerfusiontime += time.elapsed();
    DEBUG_LOG(QString( "TEMPS COMPUTANT DELTAR : %1ms " ).arg( deltaRtime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT MOMENTS : %1ms " ).arg( momentstime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT FINDING AIF : %1ms " ).arg( findAiftime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT PERFUSION TIME : %1ms " ).arg( computePerfusiontime ) );
*/
    m_sliceViewSlider->setMaximum(m_2DView->getInput()->getDimensions()[2] - 1);
}

void QPerfusionMapReconstructionExtension::createColorMap( )
{
    //Pintem el mapa amb colors
    //m_mapMin=100;
    //m_mapMax=155;
    //std::cout<<"Max Map:"<<m_mapMax<<", Min Map:"<<m_mapMin<<std::endl;
    /*mapMin=0;
    mapMax=m_mainVolume->getSeries()->getNumberOfPhases();*/
    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    mapHueLut->SetTableRange( m_mapMin, m_mapMax );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    /*int nvalues=mapHueLut->GetNumberOfTableValues();
    double* tvalue= new double[4];
    for(int i=0;i<((threshold*nvalues)/255);i++)
    {
        tvalue=mapHueLut->GetTableValue(i);
        tvalue[0]=0.0;  //Posem els valors transparents
        tvalue[1]=0.0;  //Posem els valors transparents
        tvalue[2]=0.0;  //Posem els valors transparents
        tvalue[3]=0.0;  //Posem els valors transparents
        mapHueLut->SetTableValue(i, tvalue);
    }*/
    mapHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = mapHueLut->GetTable();
    unsigned char tuple[4] = { 0, 0, 0, 0 };
    table->SetTupleValue( 0, tuple );
    unsigned char tuple2[4] = { 1, 1, 1, 1 };
    table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple2 );

    m_2DView->getWindowLevelMapper()->SetLookupTable( mapHueLut );

    m_2DView->setWindowLevel(1.0, m_mapMin - 1.0);
    //Potser això fa que es recalculi dues vegades??
    //m_2DView->setWindowLevel(m_mapMax - m_mapMin, (m_mapMax + m_mapMin)/2);
}

void QPerfusionMapReconstructionExtension::createColorMap2( )
{
    //Pintem el mapa amb colors
    //m_mapMin=100;
    //m_mapMax=155;
    //std::cout<<"Max Map:"<<m_mapMax<<", Min Map:"<<m_mapMin<<std::endl;
    /*mapMin=0;
    mapMax=m_mainVolume->getSeries()->getNumberOfPhases();*/
    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    mapHueLut->SetTableRange( m_mapMin, m_mapMax );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    /*int nvalues=mapHueLut->GetNumberOfTableValues();
    double* tvalue= new double[4];
    for(int i=0;i<((threshold*nvalues)/255);i++)
    {
        tvalue=mapHueLut->GetTableValue(i);
        tvalue[0]=0.0;  //Posem els valors transparents
        tvalue[1]=0.0;  //Posem els valors transparents
        tvalue[2]=0.0;  //Posem els valors transparents
        tvalue[3]=0.0;  //Posem els valors transparents
        mapHueLut->SetTableValue(i, tvalue);
    }*/
    mapHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = mapHueLut->GetTable();
    unsigned char tuple[4] = { 0, 0, 0, 0 };
    table->SetTupleValue( 0, tuple );
    //unsigned char tuple2[4] = { 1.0, 1.0, 1.0, 1.0 };
    table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple );

    m_2DView->getWindowLevelMapper()->SetLookupTable( mapHueLut );

    m_2DView->setWindowLevel(1.0, m_mapMin - 1.0);
    //Potser això fa que es recalculi dues vegades??
    //m_2DView_4->setWindowLevel(m_mapMax - m_mapMin, (m_mapMax + m_mapMin)/2);
}

void QPerfusionMapReconstructionExtension::createColorMap( double window, double level )
{
    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    mapHueLut->SetTableRange( level - window/2, level + window/2 );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    /*int nvalues=mapHueLut->GetNumberOfTableValues();
    double* tvalue= new double[4];
    for(int i=0;i<((threshold*nvalues)/255);i++)
    {
        tvalue=mapHueLut->GetTableValue(i);
        tvalue[0]=0.0;  //Posem els valors transparents
        tvalue[1]=0.0;  //Posem els valors transparents
        tvalue[2]=0.0;  //Posem els valors transparents
        tvalue[3]=0.0;  //Posem els valors transparents
        mapHueLut->SetTableValue(i, tvalue);
    }*/
    mapHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = mapHueLut->GetTable();
    unsigned char tuple[4] = { 0, 0, 0, 0 };
    table->SetTupleValue( 0, tuple );
    table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple );

    m_2DView->getWindowLevelMapper()->SetLookupTable( mapHueLut );
}

void QPerfusionMapReconstructionExtension::createColorMap(DoubleImageType::Pointer image, Q2DViewer* viewer)
{
    itk::MinimumMaximumImageCalculator< DoubleImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< DoubleImageType >::New();

    minmaxCalc->SetImage(image);
    minmaxCalc->SetRegion(image->GetRequestedRegion());
    minmaxCalc->Compute();
    double minimage = minmaxCalc->GetMinimum();

    static const int nbins = 300;
    double histwidth = minmaxCalc->GetMaximum() - minmaxCalc->GetMinimum();
    double binsize = histwidth / (double) nbins;

    QVector<unsigned int> hist (nbins+1,0);


    typedef itk::ImageRegionIterator<DoubleImageType> DoubleIterator;
    DoubleIterator imIter( image, image->GetBufferedRegion() );
    typedef itk::ImageRegionIterator<BoolImageType> BoolIterator;
    BoolIterator checkIter( m_mapCalculator->getCheckImage(), m_mapCalculator->getCheckImage()->GetBufferedRegion() );
    imIter.GoToBegin();
    checkIter.GoToBegin();
    int cont=0;
    while(!imIter.IsAtEnd())
    {
        if(checkIter.Get())
        {
            hist[(int)(((double)imIter.Get()-minimage)/binsize)]++;
            cont++;
        }
        ++imIter;
        ++checkIter;
    }

    double lowerlimit = -1e6;
    double upperlimit = -1e6;
    double acum = 0;

    int i;
    for(i = 0; i < hist.size();i++)
    {
        acum+=hist[i];
        if((lowerlimit < -1e5) && (acum>0.01*cont))
        {
            lowerlimit = minimage + (double)i*binsize;
        }
        if((upperlimit < -1e5) && (acum>=0.99*cont))
        {
            upperlimit = minimage + (double)i*binsize;
        }
    }

    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    //mapHueLut->SetTableRange( minmaxCalc->GetMinimum(), minmaxCalc->GetMaximum() );
    mapHueLut->SetTableRange( lowerlimit, upperlimit );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    mapHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = mapHueLut->GetTable();
    this->getPerfusionColormapTable(table);

    //unsigned char tuple[4] = { 0, 0, 0, 0 };
    //table->SetTupleValue( 0, tuple );
    //table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple );

    viewer->getWindowLevelMapper()->SetLookupTable( mapHueLut );
    viewer->setWindowLevel(1.0,minmaxCalc->GetMinimum() - 1.0);
}

void QPerfusionMapReconstructionExtension::createColorMap(Volume::ItkImageType::Pointer image, Q2DViewer* viewer)
{
    itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::New();

    minmaxCalc->SetImage(image);
    minmaxCalc->SetRegion(image->GetRequestedRegion());
    minmaxCalc->Compute();
    Volume::ItkImageType::PixelType minimage = minmaxCalc->GetMinimum();

    static const int nbins = 300;
    Volume::ItkImageType::PixelType histwidth = minmaxCalc->GetMaximum() - minmaxCalc->GetMinimum();
    Volume::ItkImageType::PixelType binsize = histwidth / (Volume::ItkImageType::PixelType) nbins;

    QVector<unsigned int> hist (nbins+1,0);


    typedef itk::ImageRegionIterator<Volume::ItkImageType> ImageIterator;
    ImageIterator imIter( image, image->GetBufferedRegion() );
    typedef itk::ImageRegionIterator<BoolImageType> BoolIterator;
    BoolIterator checkIter( m_mapCalculator->getCheckImage(), m_mapCalculator->getCheckImage()->GetBufferedRegion() );
    imIter.GoToBegin();
    checkIter.GoToBegin();
    int cont=0;
    while(!imIter.IsAtEnd())
    {
        if(checkIter.Get())
        {
            hist[(int)(((Volume::ItkImageType::PixelType)imIter.Get()-minimage)/binsize)]++;
            cont++;
        }
        ++imIter;
        ++checkIter;
    }

    double lowerlimit = -1e6;
    double upperlimit = -1e6;
    double acum = 0;

    int i;
    for(i = 0; i < hist.size();i++)
    {
        acum+=hist[i];
        if((lowerlimit < -1e5) && (acum>0.01*cont))
        {
            lowerlimit = minimage + (double)i*binsize;
        }
        if((upperlimit < -1e5) && (acum>=0.99*cont))
        {
            upperlimit = minimage + (double)i*binsize;
        }
    }
    
    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    //mapHueLut->SetTableRange( minmaxCalc->GetMinimum(), minmaxCalc->GetMaximum() );
    mapHueLut->SetTableRange( lowerlimit, upperlimit );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    mapHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = mapHueLut->GetTable();
    this->getPerfusionColormapTable(table);

    //unsigned char tuple[4] = { 0, 0, 0, 0 };
    //table->SetTupleValue( 0, tuple );
    //table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple );

    viewer->getWindowLevelMapper()->SetLookupTable( mapHueLut );
    viewer->setWindowLevel(1.0,minmaxCalc->GetMinimum() - 1.0);
}

void QPerfusionMapReconstructionExtension::getPerfusionColormapTable( vtkUnsignedCharArray * table )
{
    int red[256], green[256], blue[256];

    red[0] = 0;    green[0] = 0;   blue[0] = 0;
    red[1] = 127;    green[1] = 0;   blue[1] = 127;
    red[2] = 126;    green[2] = 0;   blue[2] = 129;
    red[3] = 124;    green[3] = 0;   blue[3] = 131;
    red[4] = 123;    green[4] = 0;   blue[4] = 133;
    red[5] = 122;    green[5] = 0;   blue[5] = 135;
    red[6] = 120;    green[6] = 0;   blue[6] = 137;
    red[7] = 119;    green[7] = 0;   blue[7] = 139;
    red[8] = 118;    green[8] = 0;   blue[8] = 141;
    red[9] = 116;    green[9] = 0;   blue[9] = 143;
    red[10] = 115;    green[10] = 0;   blue[10] = 146;
    red[11] = 113;    green[11] = 0;   blue[11] = 148;
    red[12] = 112;    green[12] = 0;   blue[12] = 150;
    red[13] = 111;    green[13] = 0;   blue[13] = 152;
    red[14] = 109;    green[14] = 0;   blue[14] = 154;
    red[15] = 108;    green[15] = 0;   blue[15] = 156;
    red[16] = 107;    green[16] = 0;   blue[16] = 158;
    red[17] = 105;    green[17] = 0;   blue[17] = 160;
    red[18] = 104;    green[18] = 0;   blue[18] = 162;
    red[19] = 103;    green[19] = 0;   blue[19] = 164;
    red[20] = 101;    green[20] = 0;   blue[20] = 166;
    red[21] = 100;    green[21] = 0;   blue[21] = 168;
    red[22] = 99;    green[22] = 0;   blue[22] = 170;
    red[23] = 97;    green[23] = 0;   blue[23] = 172;
    red[24] = 96;    green[24] = 0;   blue[24] = 174;
    red[25] = 94;    green[25] = 0;   blue[25] = 176;
    red[26] = 93;    green[26] = 0;   blue[26] = 178;
    red[27] = 92;    green[27] = 0;   blue[27] = 180;
    red[28] = 90;    green[28] = 0;   blue[28] = 183;
    red[29] = 89;    green[29] = 0;   blue[29] = 185;
    red[30] = 88;    green[30] = 0;   blue[30] = 187;
    red[31] = 86;    green[31] = 0;   blue[31] = 189;
    red[32] = 85;    green[32] = 0;   blue[32] = 191;
    red[33] = 84;    green[33] = 4;   blue[33] = 193;
    red[34] = 82;    green[34] = 8;   blue[34] = 195;
    red[35] = 81;    green[35] = 12;   blue[35] = 197;
    red[36] = 80;    green[36] = 16;   blue[36] = 199;
    red[37] = 78;    green[37] = 20;   blue[37] = 201;
    red[38] = 77;    green[38] = 24;   blue[38] = 203;
    red[39] = 75;    green[39] = 28;   blue[39] = 205;
    red[40] = 74;    green[40] = 32;   blue[40] = 207;
    red[41] = 73;    green[41] = 36;   blue[41] = 209;
    red[42] = 71;    green[42] = 40;   blue[42] = 211;
    red[43] = 70;    green[43] = 44;   blue[43] = 213;
    red[44] = 69;    green[44] = 48;   blue[44] = 215;
    red[45] = 67;    green[45] = 52;   blue[45] = 217;
    red[46] = 66;    green[46] = 56;   blue[46] = 220;
    red[47] = 65;    green[47] = 60;   blue[47] = 222;
    red[48] = 63;    green[48] = 64;   blue[48] = 224;
    red[49] = 62;    green[49] = 68;   blue[49] = 226;
    red[50] = 61;    green[50] = 72;   blue[50] = 228;
    red[51] = 59;    green[51] = 76;   blue[51] = 230;
    red[52] = 58;    green[52] = 80;   blue[52] = 232;
    red[53] = 56;    green[53] = 85;   blue[53] = 234;
    red[54] = 55;    green[54] = 89;   blue[54] = 236;
    red[55] = 54;    green[55] = 93;   blue[55] = 238;
    red[56] = 52;    green[56] = 97;   blue[56] = 240;
    red[57] = 51;    green[57] = 101;   blue[57] = 242;
    red[58] = 50;    green[58] = 105;   blue[58] = 244;
    red[59] = 48;    green[59] = 109;   blue[59] = 246;
    red[60] = 47;    green[60] = 113;   blue[60] = 248;
    red[61] = 46;    green[61] = 117;   blue[61] = 250;
    red[62] = 44;    green[62] = 121;   blue[62] = 252;
    red[63] = 43;    green[63] = 125;   blue[63] = 255;
    red[64] = 42;    green[64] = 129;   blue[64] = 255;
    red[65] = 40;    green[65] = 133;   blue[65] = 255;
    red[66] = 39;    green[66] = 137;   blue[66] = 255;
    red[67] = 37;    green[67] = 141;   blue[67] = 255;
    red[68] = 36;    green[68] = 145;   blue[68] = 255;
    red[69] = 35;    green[69] = 149;   blue[69] = 255;
    red[70] = 33;    green[70] = 153;   blue[70] = 255;
    red[71] = 32;    green[71] = 157;   blue[71] = 255;
    red[72] = 31;    green[72] = 161;   blue[72] = 255;
    red[73] = 29;    green[73] = 165;   blue[73] = 255;
    red[74] = 28;    green[74] = 170;   blue[74] = 255;
    red[75] = 27;    green[75] = 174;   blue[75] = 255;
    red[76] = 25;    green[76] = 178;   blue[76] = 255;
    red[77] = 24;    green[77] = 182;   blue[77] = 255;
    red[78] = 23;    green[78] = 186;   blue[78] = 255;
    red[79] = 21;    green[79] = 190;   blue[79] = 255;
    red[80] = 20;    green[80] = 194;   blue[80] = 255;
    red[81] = 18;    green[81] = 198;   blue[81] = 255;
    red[82] = 17;    green[82] = 202;   blue[82] = 255;
    red[83] = 16;    green[83] = 206;   blue[83] = 255;
    red[84] = 14;    green[84] = 210;   blue[84] = 255;
    red[85] = 13;    green[85] = 214;   blue[85] = 255;
    red[86] = 12;    green[86] = 218;   blue[86] = 255;
    red[87] = 10;    green[87] = 222;   blue[87] = 255;
    red[88] = 9;    green[88] = 226;   blue[88] = 255;
    red[89] = 8;    green[89] = 230;   blue[89] = 255;
    red[90] = 6;    green[90] = 234;   blue[90] = 255;
    red[91] = 5;    green[91] = 238;   blue[91] = 255;
    red[92] = 4;    green[92] = 242;   blue[92] = 255;
    red[93] = 2;    green[93] = 246;   blue[93] = 255;
    red[94] = 1;    green[94] = 250;   blue[94] = 255;
    red[95] = 0;    green[95] = 255;   blue[95] = 255;
    red[96] = 0;    green[96] = 255;   blue[96] = 255;
    red[97] = 4;    green[97] = 255;   blue[97] = 250;
    red[98] = 8;    green[98] = 255;   blue[98] = 246;
    red[99] = 12;    green[99] = 255;   blue[99] = 242;
    red[100] = 16;    green[100] = 255;   blue[100] = 238;
    red[101] = 20;    green[101] = 255;   blue[101] = 234;
    red[102] = 24;    green[102] = 255;   blue[102] = 230;
    red[103] = 28;    green[103] = 255;   blue[103] = 226;
    red[104] = 32;    green[104] = 255;   blue[104] = 222;
    red[105] = 36;    green[105] = 255;   blue[105] = 218;
    red[106] = 40;    green[106] = 255;   blue[106] = 214;
    red[107] = 44;    green[107] = 255;   blue[107] = 210;
    red[108] = 48;    green[108] = 255;   blue[108] = 206;
    red[109] = 52;    green[109] = 255;   blue[109] = 202;
    red[110] = 56;    green[110] = 255;   blue[110] = 198;
    red[111] = 60;    green[111] = 255;   blue[111] = 194;
    red[112] = 64;    green[112] = 255;   blue[112] = 190;
    red[113] = 68;    green[113] = 255;   blue[113] = 186;
    red[114] = 72;    green[114] = 255;   blue[114] = 182;
    red[115] = 76;    green[115] = 255;   blue[115] = 178;
    red[116] = 80;    green[116] = 255;   blue[116] = 174;
    red[117] = 85;    green[117] = 255;   blue[117] = 170;
    red[118] = 89;    green[118] = 255;   blue[118] = 165;
    red[119] = 93;    green[119] = 255;   blue[119] = 161;
    red[120] = 97;    green[120] = 255;   blue[120] = 157;
    red[121] = 101;    green[121] = 255;   blue[121] = 153;
    red[122] = 105;    green[122] = 255;   blue[122] = 149;
    red[123] = 109;    green[123] = 255;   blue[123] = 145;
    red[124] = 113;    green[124] = 255;   blue[124] = 141;
    red[125] = 117;    green[125] = 255;   blue[125] = 137;
    red[126] = 121;    green[126] = 255;   blue[126] = 133;
    red[127] = 125;    green[127] = 255;   blue[127] = 129;
    red[128] = 129;    green[128] = 255;   blue[128] = 125;
    red[129] = 133;    green[129] = 255;   blue[129] = 121;
    red[130] = 137;    green[130] = 255;   blue[130] = 117;
    red[131] = 141;    green[131] = 255;   blue[131] = 113;
    red[132] = 145;    green[132] = 255;   blue[132] = 109;
    red[133] = 149;    green[133] = 255;   blue[133] = 105;
    red[134] = 153;    green[134] = 255;   blue[134] = 101;
    red[135] = 157;    green[135] = 255;   blue[135] = 97;
    red[136] = 161;    green[136] = 255;   blue[136] = 93;
    red[137] = 165;    green[137] = 255;   blue[137] = 89;
    red[138] = 170;    green[138] = 255;   blue[138] = 85;
    red[139] = 174;    green[139] = 255;   blue[139] = 80;
    red[140] = 178;    green[140] = 255;   blue[140] = 76;
    red[141] = 182;    green[141] = 255;   blue[141] = 72;
    red[142] = 186;    green[142] = 255;   blue[142] = 68;
    red[143] = 190;    green[143] = 255;   blue[143] = 64;
    red[144] = 194;    green[144] = 255;   blue[144] = 60;
    red[145] = 198;    green[145] = 255;   blue[145] = 56;
    red[146] = 202;    green[146] = 255;   blue[146] = 52;
    red[147] = 206;    green[147] = 255;   blue[147] = 48;
    red[148] = 210;    green[148] = 255;   blue[148] = 44;
    red[149] = 214;    green[149] = 255;   blue[149] = 40;
    red[150] = 218;    green[150] = 255;   blue[150] = 36;
    red[151] = 222;    green[151] = 255;   blue[151] = 32;
    red[152] = 226;    green[152] = 255;   blue[152] = 28;
    red[153] = 230;    green[153] = 255;   blue[153] = 24;
    red[154] = 234;    green[154] = 255;   blue[154] = 20;
    red[155] = 238;    green[155] = 255;   blue[155] = 16;
    red[156] = 242;    green[156] = 255;   blue[156] = 12;
    red[157] = 246;    green[157] = 255;   blue[157] = 8;
    red[158] = 250;    green[158] = 255;   blue[158] = 4;
    red[159] = 255;    green[159] = 255;   blue[159] = 0;
    red[160] = 255;    green[160] = 255;   blue[160] = 0;
    red[161] = 255;    green[161] = 252;   blue[161] = 0;
    red[162] = 255;    green[162] = 249;   blue[162] = 0;
    red[163] = 255;    green[163] = 246;   blue[163] = 0;
    red[164] = 255;    green[164] = 244;   blue[164] = 0;
    red[165] = 255;    green[165] = 241;   blue[165] = 0;
    red[166] = 255;    green[166] = 238;   blue[166] = 0;
    red[167] = 255;    green[167] = 236;   blue[167] = 0;
    red[168] = 255;    green[168] = 233;   blue[168] = 0;
    red[169] = 255;    green[169] = 230;   blue[169] = 0;
    red[170] = 255;    green[170] = 228;   blue[170] = 0;
    red[171] = 255;    green[171] = 225;   blue[171] = 0;
    red[172] = 255;    green[172] = 222;   blue[172] = 0;
    red[173] = 255;    green[173] = 220;   blue[173] = 0;
    red[174] = 255;    green[174] = 217;   blue[174] = 0;
    red[175] = 255;    green[175] = 214;   blue[175] = 0;
    red[176] = 255;    green[176] = 212;   blue[176] = 0;
    red[177] = 255;    green[177] = 209;   blue[177] = 0;
    red[178] = 255;    green[178] = 206;   blue[178] = 0;
    red[179] = 255;    green[179] = 204;   blue[179] = 0;
    red[180] = 255;    green[180] = 201;   blue[180] = 0;
    red[181] = 255;    green[181] = 198;   blue[181] = 0;
    red[182] = 255;    green[182] = 195;   blue[182] = 0;
    red[183] = 255;    green[183] = 193;   blue[183] = 0;
    red[184] = 255;    green[184] = 190;   blue[184] = 0;
    red[185] = 255;    green[185] = 187;   blue[185] = 0;
    red[186] = 255;    green[186] = 185;   blue[186] = 0;
    red[187] = 255;    green[187] = 182;   blue[187] = 0;
    red[188] = 255;    green[188] = 179;   blue[188] = 0;
    red[189] = 255;    green[189] = 177;   blue[189] = 0;
    red[190] = 255;    green[190] = 174;   blue[190] = 0;
    red[191] = 255;    green[191] = 171;   blue[191] = 0;
    red[192] = 255;    green[192] = 169;   blue[192] = 0;
    red[193] = 255;    green[193] = 166;   blue[193] = 0;
    red[194] = 255;    green[194] = 163;   blue[194] = 0;
    red[195] = 255;    green[195] = 161;   blue[195] = 0;
    red[196] = 255;    green[196] = 158;   blue[196] = 0;
    red[197] = 255;    green[197] = 155;   blue[197] = 0;
    red[198] = 255;    green[198] = 153;   blue[198] = 0;
    red[199] = 255;    green[199] = 150;   blue[199] = 0;
    red[200] = 255;    green[200] = 147;   blue[200] = 0;
    red[201] = 255;    green[201] = 144;   blue[201] = 0;
    red[202] = 255;    green[202] = 142;   blue[202] = 0;
    red[203] = 255;    green[203] = 139;   blue[203] = 0;
    red[204] = 255;    green[204] = 136;   blue[204] = 0;
    red[205] = 255;    green[205] = 134;   blue[205] = 0;
    red[206] = 255;    green[206] = 131;   blue[206] = 0;
    red[207] = 255;    green[207] = 128;   blue[207] = 0;
    red[208] = 255;    green[208] = 126;   blue[208] = 0;
    red[209] = 255;    green[209] = 123;   blue[209] = 0;
    red[210] = 255;    green[210] = 120;   blue[210] = 0;
    red[211] = 255;    green[211] = 118;   blue[211] = 0;
    red[212] = 255;    green[212] = 115;   blue[212] = 0;
    red[213] = 255;    green[213] = 112;   blue[213] = 0;
    red[214] = 255;    green[214] = 110;   blue[214] = 0;
    red[215] = 255;    green[215] = 107;   blue[215] = 0;
    red[216] = 255;    green[216] = 104;   blue[216] = 0;
    red[217] = 255;    green[217] = 101;   blue[217] = 0;
    red[218] = 255;    green[218] = 99;   blue[218] = 0;
    red[219] = 255;    green[219] = 96;   blue[219] = 0;
    red[220] = 255;    green[220] = 93;   blue[220] = 0;
    red[221] = 255;    green[221] = 91;   blue[221] = 0;
    red[222] = 255;    green[222] = 88;   blue[222] = 0;
    red[223] = 255;    green[223] = 85;   blue[223] = 0;
    red[224] = 255;    green[224] = 83;   blue[224] = 0;
    red[225] = 252;    green[225] = 80;   blue[225] = 0;
    red[226] = 250;    green[226] = 77;   blue[226] = 0;
    red[227] = 248;    green[227] = 75;   blue[227] = 0;
    red[228] = 246;    green[228] = 72;   blue[228] = 0;
    red[229] = 244;    green[229] = 69;   blue[229] = 0;
    red[230] = 242;    green[230] = 67;   blue[230] = 0;
    red[231] = 240;    green[231] = 64;   blue[231] = 0;
    red[232] = 238;    green[232] = 61;   blue[232] = 0;
    red[233] = 236;    green[233] = 59;   blue[233] = 0;
    red[234] = 234;    green[234] = 56;   blue[234] = 0;
    red[235] = 232;    green[235] = 53;   blue[235] = 0;
    red[236] = 230;    green[236] = 50;   blue[236] = 0;
    red[237] = 228;    green[237] = 48;   blue[237] = 0;
    red[238] = 226;    green[238] = 45;   blue[238] = 0;
    red[239] = 224;    green[239] = 42;   blue[239] = 0;
    red[240] = 222;    green[240] = 40;   blue[240] = 0;
    red[241] = 220;    green[241] = 37;   blue[241] = 0;
    red[242] = 217;    green[242] = 34;   blue[242] = 0;
    red[243] = 215;    green[243] = 32;   blue[243] = 0;
    red[244] = 213;    green[244] = 29;   blue[244] = 0;
    red[245] = 211;    green[245] = 26;   blue[245] = 0;
    red[246] = 209;    green[246] = 24;   blue[246] = 0;
    red[247] = 207;    green[247] = 21;   blue[247] = 0;
    red[248] = 205;    green[248] = 18;   blue[248] = 0;
    red[249] = 203;    green[249] = 16;   blue[249] = 0;
    red[250] = 201;    green[250] = 13;   blue[250] = 0;
    red[251] = 199;    green[251] = 10;   blue[251] = 0;
    red[252] = 197;    green[252] = 8;   blue[252] = 0;
    red[253] = 195;    green[253] = 5;   blue[253] = 0;
    red[254] = 193;    green[254] = 2;   blue[254] = 0;
    red[255] = 191;    green[255] = 0;   blue[255] = 0;

    int i;
    unsigned char tuple[4] ;
    for(i=0;i<256;i++)
    {
        tuple[0]=red[i];
        tuple[1]=green[i];
        tuple[2]=blue[i];
        tuple[3]=0;
        table->SetTupleValue( i, tuple );
    }

}

void QPerfusionMapReconstructionExtension::applyFilterMapImage( )
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    typedef itk::CastImageFilter< Volume::ItkImageType, InternalImageType >                     InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType >                      OutputCastingFilterType;

    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType >      CurvatureFlowImageFilterType;

    //Aquesta funció no fa res!!!TODO: fer que faci alguna cosa
    //if(m_map0Volume != 0)
    if(0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Volume::ItkImageType::RegionType region;
        Volume::ItkImageType::IndexType start;
        start[0]=0;
        start[1]=0;
        start[2]=0;
        Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
        size[2]=m_DSCVolume->getNumberOfSlicesPerPhase();
        region.SetSize(size);
        region.SetIndex(start);
        Volume::ItkImageType::Pointer auxImage = Volume::ItkImageType::New();
        auxImage->SetRegions( region );
        auxImage->Allocate();


        InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
        OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
        CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

        //incaster->SetInput( m_map2Volume->getItkData() );
        incaster->SetInput( m_2DView->getInput()->getItkData() );
        smoothing->SetInput( incaster->GetOutput() );
        outcaster->SetInput( smoothing->GetOutput() );

        smoothing->SetNumberOfIterations( 5 );
        smoothing->SetTimeStep( 0.0625 );

        try
        {
            outcaster->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            //std::cerr << "Exception caught !" << std::endl;
            //std::cerr << excep << std::endl;
        }

        typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
        Iterator outIter( outcaster->GetOutput(), outcaster->GetOutput()->GetBufferedRegion() );
        Iterator auxIter( auxImage, auxImage->GetBufferedRegion() );
        outIter.GoToBegin();
        auxIter.GoToBegin();
        unsigned int i,j,k;
        for (k=0;k<size[2];k++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    auxIter.Set(outIter.Get());
                    ++auxIter;
                    ++outIter;
                }
            }
        }
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        //output->setImages( m_Volume->getImages() );
//         std::cout<<"Init Saving Volume"<<std::endl;
//         typedef itk::ImageFileWriter <Volume::ItkImageType> writerType;
//         writerType::Pointer mapWriter3 = writerType::New();
//         mapWriter3->SetFileName("filteredImage.mhd");
//         mapWriter3->SetInput(outcaster->GetOutput() );
//         mapWriter3->Update();
//         writerType::Pointer mapWriter2 = writerType::New();
//         mapWriter2->SetFileName("originalImage.mhd");
//         mapWriter2->SetInput(m_mapVolume->getItkData() );
//         mapWriter2->Update();

        //auxImage = outcaster->GetOutput();
        Volume* mapVolume = new Volume();
        mapVolume->setImages( m_DSCVolume->getPhaseImages(0) );
        try
        {
//           m_mapVolume->setData( outcaster->GetOutput() );
           mapVolume->setData( auxImage );
        }
        catch( itk::ExceptionObject & excep )
        {
            //std::cerr << "Exception caught !" << std::endl;
            //std::cerr << excep << std::endl;
        }
        //m_mapVolume->getVtkData()->Update();
        m_2DView->setInput( mapVolume );
        //m_2DView->removeAnnotation(Q2DViewer::AllAnnotation);
        m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
        //m_2DView->resetWindowLevelToDefault();
        this->createColorMap(auxImage, m_2DView);
        m_2DView->setSlice( m_sliceViewSlider->value() );
        QApplication::restoreOverrideCursor();

    }

}

void QPerfusionMapReconstructionExtension::eventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        paintCursorSignal();
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

void QPerfusionMapReconstructionExtension::paintCursorSignal( )
{
    if(m_mapCalculator->getCBVVolume())    //Si hi ha alguna cosa al volum
    {
        double pos[3];
        
        Volume* cbvMapVolume = m_mapCalculator->getCBVVolume();
        PerfusionMapCalculatorMainThread::DoubleTemporalImageType::Pointer signalImage = m_mapCalculator->getDeltaRImage();

        if( m_2DView->getCurrentCursorImageCoordinate(pos) )
        {
            int index[3];
            index[0] = (int)((pos[0]- cbvMapVolume->getOrigin()[0])/cbvMapVolume->getSpacing()[0]);
            index[1] = (int)((pos[1]- cbvMapVolume->getOrigin()[1])/cbvMapVolume->getSpacing()[1]);
            index[2] = m_2DView->getCurrentSlice();
            DoubleTemporalImageType::IndexType indexTemp;
            indexTemp[1] = index[0];
            indexTemp[2] = index[1];
            indexTemp[3] = index[2];
            int t, tend = m_DSCVolume->getNumberOfPhases();
            QVector<double> signal(tend);
            QPolygonF p;
        
            double minsig = 100000.0;
            double maxsig = -100000.0;
        
            for (t=0;t<tend;t++)
            {
                indexTemp[0] = t;
                signal[t] = signalImage->GetPixel(indexTemp);
                if(signal[t]>maxsig) maxsig = signal[t];
                if(signal[t]<minsig) minsig = signal[t];
           }

            //Empirical values
            m_graphicplot->setMaxY(0.05);
            m_graphicplot->setMinY(-0.01);
            //m_graphicplot->setHold(true);
            m_graphicplot->setData( signal );
            if(m_meanseries.size() > m_2DView->getCurrentSlice())
            {
                m_graphicplot->setData( m_meanseries[m_2DView->getCurrentSlice()], 1 );
                m_graphicplot->setPaintingFeatures(Qt::red, 1.5, 1);
            }
        }
    }    
}

void QPerfusionMapReconstructionExtension::paintMeanSlice( int slice )
{
    if(m_mapCalculator->getCBVVolume())    //Si hi ha alguna cosa al volum
    {
        //std::cout<<"Painting mean series: slice = "<<slice<<", size ="<<m_meanseries.size()<<std::endl;
        if(m_meanseries.size() > slice)
        {
            m_graphicplot->setData( m_meanseries[slice], 1 );
        }
    }    
}

void QPerfusionMapReconstructionExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;
}


void QPerfusionMapReconstructionExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}


void QPerfusionMapReconstructionExtension::contextMenuDSCRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    // map to global
    //QPoint globalPoint = this->mapToGlobal( point );
//     emit showContextMenu( globalPoint );

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QPerfusionMapReconstructionExtension::contextMenuEvent(QContextMenuEvent *event)
{
    PatientBrowserMenu *patientMenu = new PatientBrowserMenu(this);
    patientMenu->setAttribute(Qt::WA_DeleteOnClose);
    patientMenu->setPatient( QApplicationMainWindow::getActiveApplicationMainWindow()->getCurrentPatient() );

    connect(patientMenu, SIGNAL( selectedVolume(Volume *) ), SLOT( setVolume(Volume *) ));

    patientMenu->popup(event->globalPos()); //->globalPos() ?
}

void QPerfusionMapReconstructionExtension::setVolume(Volume *volume)
{
    m_DSCLineEdit->clear();
    m_DSCLineEdit->insert( volume->getImage(0,0)->getParentSeries()->getDescription() );
    m_DSCVolume = volume;
}

bool QPerfusionMapReconstructionExtension::findProbableSeries( )
{
    bool findDSC=false;
    bool findpre=false;
    bool findpost=false;
    foreach( Study *study, m_mainVolume->getPatient()->getStudies() )
    {
        foreach( Series *series, study->getSeries() )
        {
            //DEBUG_LOG(series->getDescription());
            if((series->getDescription().contains("DSC", Qt::CaseInsensitive) || series->getDescription().contains("#PERFUSION", Qt::CaseInsensitive)) && series->getFirstVolume()) // && slicesDSC < series->getFirstVolume()->getDimensions()[3])
            {
                m_DSCLineEdit->clear();
                m_DSCLineEdit->insert(series->getDescription());
                m_DSCVolume = series->getFirstVolume();
                findDSC=true;
                //slicesDSC = series->getFirstVolume()->getDimensions()[3];
            }
        }
    }
    return (findDSC && findpre && findpost);
}


void QPerfusionMapReconstructionExtension::readSettings()
{
    Settings settings;

    settings.restoreGeometry( PerfusionMapReconstructionSettings::HorizontalSplitterGeometry, m_horizontalSplitter );
    settings.restoreGeometry( PerfusionMapReconstructionSettings::VerticalSplitterGeometry, m_verticalSplitter );
}

void QPerfusionMapReconstructionExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry( PerfusionMapReconstructionSettings::HorizontalSplitterGeometry, m_horizontalSplitter );
    settings.saveGeometry( PerfusionMapReconstructionSettings::VerticalSplitterGeometry, m_verticalSplitter );
}

}
