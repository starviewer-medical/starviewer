/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qglialestimationextension.h"

#include "toolsactionfactory.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "toolmanager.h"
#include "patientbrowsermenu.h"
#include "../diffusionperfusionsegmentation/itkRegistre3DAffine.h"
#include "strokesegmentationmethod.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QSettings>
#include <QMessageBox>
#include <QContextMenuEvent>

// VTK
#include <vtkCommand.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageCast.h>
#include <vtkImageActor.h>
#include <vtkImageThreshold.h>

// ITK
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkCastImageFilter.h>
#include "itkRescaleIntensityImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"


namespace udg {

QGlialEstimationExtension::QGlialEstimationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_T1Volume(0),  m_T1MaskVolume(0), m_perfuVolume(0), m_mapVolume(0), m_FLAIRVolume(0), m_difuVolume(0), m_spectrumVolume(0), m_registeredVolume(0), m_segmentationMethod(0), m_isLeftButtonPressed(false)
{
    setupUi( this );

    createActions();
    createConnections();
    readSettings();

}

QGlialEstimationExtension::~QGlialEstimationExtension()
{

    delete m_toolManager;
    delete m_rotateClockWiseAction;

    writeSettings();
}

void QGlialEstimationExtension::createActions()
{

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_1 , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_2 , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_3 , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_4 , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_5 , SLOT( rotateClockWise() ) );
    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView_6 , SLOT( rotateClockWise() ) );

    // Tools
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->getToolAction("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );
    m_seedToolButton->setDefaultAction( m_toolManager->getToolAction("SeedTool") );

    // Tool d'slicing per teclat
    QAction *slicingKeyboardTool = m_toolManager->getToolAction("SlicingKeyboardTool");
    slicingKeyboardTool->trigger();

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "ScreenShotTool"<<"SeedTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_translateToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    // La tool de sincronització sempre estarà activada, encara que no hi tingui cap visualitzador
    m_toolManager->getToolAction("SynchronizeTool")->setChecked( true );

    // registrem al manager les tools que van amb el viewer principal
    //initializeDefaultTools( m_selectedViewer->getViewer() );

    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool" <<  "SlicingKeyboardTool"<<"SeedTool";

    m_toolManager->setViewerTools( m_2DView_1, toolsList );
    m_toolManager->setViewerTools( m_2DView_2, toolsList );
    m_toolManager->setViewerTools( m_2DView_3, toolsList );
    m_toolManager->setViewerTools( m_2DView_6, toolsList );
    m_toolManager->setViewerTools( m_2DView_4, toolsList );
    m_toolManager->setViewerTools( m_2DView_5, toolsList );

}

void QGlialEstimationExtension::createConnections()
{
    //connect( m_2DView_1, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
    //connect( m_2DView_2, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
    connect( m_chooseT1PushButton, SIGNAL( clicked() ), SLOT( contextMenuT1Release() ) );
    connect( m_choosePerfuPushButton, SIGNAL( clicked() ), SLOT( contextMenuPerfuRelease() ) );
    connect( m_chooseFLAIRPushButton, SIGNAL( clicked() ), SLOT( contextMenuFLAIRRelease() ) );
    connect( m_chooseDifuPushButton, SIGNAL( clicked() ), SLOT( contextMenuDifuRelease() ) );
    connect( m_chooseSpectrumPushButton, SIGNAL( clicked() ), SLOT( contextMenuSpectrumRelease() ) );
    connect( m_filterVSIPushButton, SIGNAL( clicked() ), SLOT( applyFilterMapImage() ) );
    //connect( m_2DView_1, SIGNAL( windowLevelChanged( double,double ) ), SLOT( createColorMap( double, double ) ) );
    connect( m_opacityRegistrationSlider, SIGNAL( valueChanged(int) ), SLOT( setRegistrationOpacity(int) ) );
    connect( m_registrationPushButton, SIGNAL( clicked() ), SLOT( applyRegistration() ) );
    connect( m_computeCBVPushButton, SIGNAL( clicked() ), SLOT( computeCBV() ) );
    connect( m_T1ValueSlider, SIGNAL( valueChanged(int) ), SLOT( viewT1Thresholds(int) ) );
    connect( m_T1MaskOpacitySlider, SIGNAL( valueChanged(int) ), SLOT( setT1MaskOpacity(int) ) );
    connect( m_2DView_1, SIGNAL( seedChanged() ), SLOT( setT1SeedPosition() ) );
    connect( m_T1ApplyPushButton, SIGNAL( clicked() ), SLOT( applyT1Segmentation() ) );
}

void QGlialEstimationExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    if (this->findProbableSeries( ) )
    {
        //std::cout<<"Tot ok!!"<<std::endl;
    }

    if(m_T1Volume != 0)
    {
        itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::New();

        minmaxCalc->SetImage(m_T1Volume->getItkData());
        minmaxCalc->SetRegion(m_T1Volume->getItkData()->GetRequestedRegion());
        minmaxCalc->Compute();

        DEBUG_LOG( QString("ItkMax=%1, ItkMin=%2").arg(minmaxCalc->GetMaximum()).arg(minmaxCalc->GetMinimum()) );

        m_minT1Value = minmaxCalc->GetMinimum();
        m_maxT1Value = minmaxCalc->GetMaximum();
        m_T1ValueSpinBox->setMinimum( m_minT1Value );
        m_T1ValueSpinBox->setMaximum( m_maxT1Value );
        m_T1ValueSlider->setMinimum( m_minT1Value );
        m_T1ValueSlider->setMaximum( m_maxT1Value );

        m_2DView_1->setInput( m_T1Volume );
        m_2DView_1->resetCamera();
        //m_2DView_1->removeAnnotation(Q2DViewer::AllAnnotation);
    }
    if(m_perfuVolume != 0)
    {
        //TODO: Calcular el mapa
        //this->computeCBV();
        //m_2DView_2->setInput( m_mapVolume );
        //m_2DView_2->setInput( m_perfuVolume );
        //m_2DView_2->removeAnnotation(Q2DViewer::AllAnnotation);
    }
    if(m_FLAIRVolume != 0)
    {
        m_2DView_4->setInput( m_FLAIRVolume );
        //m_2DView_4->removeAnnotation(Q2DViewer::AllAnnotation);
    }
    if(m_difuVolume != 0)
    {
        m_2DView_5->setInput( m_difuVolume );
        //m_2DView_5->removeAnnotation(Q2DViewer::AllAnnotation);
    }
    if(m_spectrumVolume != 0)
    {
        m_2DView_6->setInput( m_spectrumVolume );
    }
    //std::cout<<"Done!!"<<std::endl;
}

void QGlialEstimationExtension::computeTTP( )
{

    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_perfuVolume->getItkData()->GetBufferedRegion().GetSize();
    size[2]=m_perfuVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer mapImage = Volume::ItkImageType::New();
    mapImage->SetRegions( region );
    mapImage->Allocate();
    //std::cout<<"Region:"<<region<<std::endl;
    //std::cout<<"Size:"<<size<<std::endl;

    Volume::ItkImageType::Pointer maxImage = Volume::ItkImageType::New();
    maxImage->SetRegions( region );
    maxImage->Allocate();

    Volume::ItkImageType::Pointer minImage = Volume::ItkImageType::New();
    minImage->SetRegions( region );
    minImage->Allocate();

    typedef itk::Image<float, 3> DoubleImageType;
    DoubleImageType::Pointer mttImage = DoubleImageType::New();
    mttImage->SetRegions( region );
    mttImage->Allocate();
    DoubleImageType::Pointer summttImage = DoubleImageType::New();
    summttImage->SetRegions( region );
    summttImage->Allocate();


    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator iter( m_perfuVolume->getItkData(), m_perfuVolume->getItkData()->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
    Iterator mapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator initialMapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator maxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator initialMaxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator minIter( minImage, minImage->GetBufferedRegion() );
    Iterator initialMinIter( minImage, minImage->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<DoubleImageType> DoubleIterator;
    DoubleIterator mttIter( mttImage, mttImage->GetBufferedRegion() );
    DoubleIterator initialMttIter( mttImage, mttImage->GetBufferedRegion() );
    DoubleIterator summttIter( summttImage, summttImage->GetBufferedRegion() );
    DoubleIterator initialSumMttIter( summttImage, summttImage->GetBufferedRegion() );

    m_minValue = iter.Get();
    m_maxValue = m_minValue;

    Volume::ItkImageType::PixelType value;
    long cont=0;
    unsigned int i,j,k,p;
    std::ofstream fout("perfu.dat", ios::out);

    unsigned int kend=m_perfuVolume->getSeries()->getNumberOfSlicesPerPhase();
    unsigned int pend=m_perfuVolume->getSeries()->getNumberOfPhases();
    mttIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                mttIter.Set(0.0);
                ++mttIter;
            }
        }
    }

    double mttvalue;
    iter.GoToBegin();
    mapIter.GoToBegin();
    maxIter.GoToBegin();
    minIter.GoToBegin();
    mttIter.GoToBegin();
    summttIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        initialMapIter=mapIter;
        initialMaxIter=maxIter;
        initialMinIter=minIter;
        initialMttIter=mttIter;
        initialSumMttIter=summttIter;
        for (p=0;p<pend;p++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    value = iter.Get();
//                     if((i==50)&&(j==70)&&(k==10))
//                     {
//                         fout<<value<<";"<<mapIter.Get()<<";"<<maxIter.Get()<<";"<<minIter.Get()<<";;";
//                     }

                    if ( value < m_minValue ) { m_minValue = value; }
                    if ( value > m_maxValue ) { m_maxValue = value; }

                    if(p==0)
                    {
                        mapIter.Set(0);
                        maxIter.Set(value);
                        minIter.Set(value);
                        mttIter.Set(0.0);
                        summttIter.Set(value);
                    }
                    else
                    {
                        if(value>maxIter.Get())
                        {
                            maxIter.Set(value);
                        }
                        if(value<minIter.Get())
                        {
                            mapIter.Set(p);
                            minIter.Set(value);
                        }
                        mttvalue=mttIter.Get()+(value*p);
                        mttIter.Set(mttvalue);
                        mttvalue=summttIter.Get();
                        summttIter.Set(mttvalue+value);
                    }
                    cont++;
                    /*if((p==pend-1)&&(k==10))
                    {
                        std::cout<<mttIter.Get()<<"(";
                    }*/
                    if(p==pend-1)
                    {
                        if(summttIter.Get()!=0)
                        {
                            mttvalue=mttIter.Get()/summttIter.Get();
                            mttIter.Set(mttvalue);
                        }
                        else
                        {
                            mttIter.Set(0.0);
                        }
                    }
                    /*if((p==pend-1)&&(k==10))
                    {
                        std::cout<<mttIter.Get()<<")";
                    }*/
                    if((i==50)&&(j==70)&&(k==10))
                    {
                        fout<<value<<";"<<mapIter.Get()<<";"<<maxIter.Get()<<";"<<minIter.Get()<<";"<<mttIter.Get()<<";"<<summttIter.Get()<<std::endl;
                    }
                    ++iter;
                    ++mapIter;
                    ++maxIter;
                    ++minIter;
                    ++mttIter;
                    ++summttIter;
                }
                /*if((p==pend-1)&&(k==10))
                {
                    std::cout<<std::endl;
                }*/
            }
            if(p!=pend-1)
            {
                mapIter=initialMapIter;
                maxIter=initialMaxIter;
                minIter=initialMinIter;
                mttIter=initialMttIter;
                summttIter=initialSumMttIter;
            }
        }
    }

    mapIter.GoToBegin();
    maxIter.GoToBegin();
    minIter.GoToBegin();
    mttIter.GoToBegin();
    m_mapMax=mapIter.Get();
    m_mapMin=mapIter.Get();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                //if(0)
                //if(200>maxIter.Get()||700<minIter.Get()||(maxIter.Get()-minIter.Get())<500)//empirical values!first crit.-> background, second->non-perfused regions
                if(200>maxIter.Get())//empirical values!first crit.-> background
                {
                    mapIter.Set(0);
                }
                else
                {
                   mapIter.Set(((int)(mttIter.Get()*255)/m_perfuVolume->getSeries()->getNumberOfPhases()));//Ho escalem pq hi hagi tants valors com colors de la LuT
                }
                if(mapIter.Get()>m_mapMax)
                {
                    m_mapMax=mapIter.Get();
                }
                if(mapIter.Get()<m_mapMin)
                {
                    m_mapMin=mapIter.Get();
                }
                ++mttIter;
                ++mapIter;
                ++maxIter;
                ++minIter;
            }
        }
    }
    fout.close();
    m_mapVolume = new Volume();
    m_mapVolume->setImages( m_perfuVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume->setData(mapImage);

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView_2->setInput( m_mapVolume );
    m_2DView_2->resetView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    int* ext= m_mapVolume->getWholeExtent();
    //std::cout<<"["<<ext[0]<<","<<ext[1]<<";"<<ext[2]<<","<<ext[3]<<";"<<ext[4]<<","<<ext[5]<<"]"<<std::endl;
    //std::cout<<"Number of voxels:"<<cont<<std::endl;

}

void QGlialEstimationExtension::createColorMap( )
{
    //Pintem el mapa amb colors
    //std::cout<<"Max Map:"<<m_mapMax<<", Min Map:"<<m_mapMin<<std::endl;
    m_mapMin=0;
    //m_mapMax=500;
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
    int nvalues=mapHueLut->GetNumberOfTableValues();
    //std::cout<<"Max:"<<m_maxValue<<", Min:"<<m_minValue<<", NValues:"<<nvalues<<std::endl;
    //std::cout<<"Max LuT:"<<m_mapMax<<", Min Lut:"<<m_mapMin<<","<<nvalues<<std::endl;
    /*double* tvalue= new double[4];
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

    m_2DView_2->getWindowLevelMapper()->SetLookupTable( mapHueLut );

    m_2DView_2->setWindowLevel(1.0, m_mapMin - 1.0);
    //Potser això fa que es recalculi dues vegades??
    //m_2DView->setWindowLevel(m_mapMax - m_mapMin, (m_mapMax + m_mapMin)/2);
}

void QGlialEstimationExtension::createColorMap( double window, double level )
{
    vtkLookupTable* mapHueLut = vtkLookupTable::New();
    mapHueLut->SetTableRange( level - window/2, level + window/2 );
    mapHueLut->SetHueRange( 0.75, 0.0 );
    mapHueLut->SetSaturationRange( 1.0, 1.0 );
    mapHueLut->SetValueRange( 1.0, 1.0 );
    mapHueLut->SetAlphaRange( 1.0, 1.0 );
    mapHueLut->SetRampToLinear();
    mapHueLut->ForceBuild();    //effective built
    int nvalues=mapHueLut->GetNumberOfTableValues();
    //std::cout<<"Max:"<<m_maxValue<<", Min:"<<m_minValue<<", NValues:"<<nvalues<<std::endl;
    //std::cout<<"Max LuT:"<<m_mapMax<<", Min Lut:"<<m_mapMin<<","<<nvalues<<std::endl;
    /*double* tvalue= new double[4];
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

    m_2DView_2->getWindowLevelMapper()->SetLookupTable( mapHueLut );
}

void QGlialEstimationExtension::computeCBV( )
{
    if(!m_perfuVolume)
    {
        //std::cout<<"No hi ha la perfu definida!!!"<<std::endl;
        return;
    }
    //std::cout<<"Init computeCBV"<<std::endl;
    //Paràmetres que definim constants però que potser no ho són
    static const unsigned int Nbaseline = 5; //->Mostres pre-bolus
    static const double TEdyn = 0.025; //->valor extret del pwp d'en Gerard
    static const double bloodvolfraction = 0.06; //->Document VSI
    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    //std::cout<<"Init Perfu Volume"<<std::endl;
    Volume::ItkImageType::SizeType size = m_perfuVolume->getItkData()->GetBufferedRegion().GetSize();
    //std::cout<<"Perfu Volume Size: "<<size<<std::endl;
    //std::cout<<"End Perfu Volume"<<std::endl;
    size[2]=m_perfuVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer mapImage = Volume::ItkImageType::New();
    mapImage->SetRegions( region );
    mapImage->Allocate();
    //std::cout<<"Region:"<<region<<std::endl;
    //std::cout<<"Size:"<<size<<std::endl;

    Volume::ItkImageType::Pointer maxImage = Volume::ItkImageType::New();
    maxImage->SetRegions( region );
    maxImage->Allocate();

    Volume::ItkImageType::Pointer minImage = Volume::ItkImageType::New();
    minImage->SetRegions( region );
    minImage->Allocate();

    typedef itk::Image<float, 3> DoubleImageType;
    DoubleImageType::Pointer sbImage = DoubleImageType::New();
    sbImage->SetRegions( region );
    sbImage->Allocate();

    DoubleImageType::Pointer rCBVImage = DoubleImageType::New();
    rCBVImage->SetRegions( region );
    rCBVImage->Allocate();

    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator DSCiter( m_perfuVolume->getItkData(), m_perfuVolume->getItkData()->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
    Iterator mapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator initialMapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator maxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator initialMaxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator minIter( minImage, minImage->GetBufferedRegion() );
    Iterator initialMinIter( minImage, minImage->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<DoubleImageType> DoubleIterator;
    DoubleIterator sbIter( sbImage, sbImage->GetBufferedRegion() );
    DoubleIterator initialsbIter( sbImage, sbImage->GetBufferedRegion() );
    DoubleIterator rCBVIter( rCBVImage, sbImage->GetBufferedRegion() );
    DoubleIterator initialrCBVIter( rCBVImage, sbImage->GetBufferedRegion() );

    Volume::ItkImageType::PixelType value;
    double valuesb, valuerCBV;
    long cont=0;
    unsigned int i,j,k,p;

    unsigned int kend=m_perfuVolume->getSeries()->getNumberOfSlicesPerPhase();
    unsigned int pend=m_perfuVolume->getSeries()->getNumberOfPhases();
    sbIter.GoToBegin();
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    //std::cout<<"Init recorregut: ["<<size[0]<<","<<size[1]<<","<<kend<<"]"<<std::endl;
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                sbIter.Set(0.0);
                rCBVIter.Set(0.0);
                mapIter.Set(0);
                ++sbIter;
                ++mapIter;
            }
        }
    }

    //std::cout<<"End recorregut2: "<<std::endl;
    //std::cout<<"Init recorregut: ["<<size[0]<<", "<<size[1]<<", "<<Nbaseline<<" ("<<pend<<"), "<<kend<<"]"<<std::endl;
    DSCiter.GoToBegin();
    sbIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        initialsbIter = sbIter;
        for (p=0;p<pend;p++)
        {
            if(p<Nbaseline)
            {
                for (j=0;j<size[1];j++)
                {
                    for (i=0;i<size[0];i++)
                    {
                        value = DSCiter.Get();
                        valuesb = sbIter.Get();
                        //Calculem la mitjana del Nbaseline valors
                        sbIter.Set(valuesb + ((double)value)/Nbaseline);

                        /*if((k==5)&&(p==(Nbaseline-1))&& value>600)
                        {
                            std::cout<<sbIter.Get()<<" ("<<sbIter.GetIndex()<<")";
                        }*/

                        ++DSCiter;
                        ++sbIter;
                    }
                }
                if(p!=Nbaseline-1)
                {
                    sbIter=initialsbIter;
                }
            }
            else
            {
                //TODO: segur que hi ha alguna manera millor de fer-ho!!!
                //Diria que es pot fer amb iterators with index
                for (j=0;j<size[1];j++)
                {
                    for (i=0;i<size[0];i++)
                    {
                        ++DSCiter;
                    }
                }
            }
        }
    }

    //std::cout<<"Init recorregut max: "<<std::endl;
    DSCiter.GoToBegin();
    maxIter.GoToBegin();
    minIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        initialMaxIter = maxIter;
        initialMinIter = minIter;
        for (p=0;p<pend;p++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    value = DSCiter.Get();

                    if(p==0)
                    {
                        maxIter.Set(value);
                        minIter.Set(value);

                    }
                    else
                    {
                        if(value>maxIter.Get())
                        {
                            maxIter.Set(value);
                        }
                        if(value<minIter.Get())
                        {
                            minIter.Set(value);
                        }
                    }
                    ++DSCiter;
                    ++maxIter;
                    ++minIter;
                }
            }
            if(p!=pend-1)
            {
                maxIter=initialMaxIter;
                minIter=initialMinIter;
            }
        }
    }

    //std::cout<<"End recorregut3: "<<std::endl;
    DSCiter.GoToBegin();
    sbIter.GoToBegin();
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    maxIter.GoToBegin();
    minIter.GoToBegin();
    m_mapMax=0;
    m_mapMin=10000;
    for (k=0;k<kend;k++)
    {
        initialsbIter=sbIter;
        initialrCBVIter=rCBVIter;
        initialMapIter = mapIter;
        initialMaxIter = maxIter;
        initialMinIter = minIter;
        for (p=0;p<pend;p++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    //if(maxIter.Get()>(minIter.Get()+200))//Out of the brain-> empirical values!first crit.-> background
                    if(maxIter.Get()<=200)//Out of the brain-> empirical values!first crit.-> background
                    {
                        //std::cout<<"*";
                        rCBVIter.Set(0.0);
                        mapIter.Set(0);
                    }
                    else
                    {
                        if(sbIter.Get()!=0 && DSCiter.Get()!=0)
                        {
                            valuerCBV=rCBVIter.Get();
                            rCBVIter.Set(valuerCBV - log(DSCiter.Get()/sbIter.Get())/TEdyn);
                        }
                    }
                    ++DSCiter;
                    ++sbIter;
                    ++mapIter;
                    ++maxIter;
                    ++minIter;
                    ++rCBVIter;
                }
            }
            if(p!=pend-1)
            {
                sbIter=initialsbIter;
                mapIter=initialMapIter;
                rCBVIter=initialrCBVIter;
                maxIter=initialMaxIter;
                minIter=initialMinIter;
            }
        }
    }
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    maxIter.GoToBegin();
    double sum=0.0;
    int contCBV=0;
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>200)//Out of the brain-> empirical values!first crit.-> background
                {
                    sum += rCBVIter.Get();
                    contCBV++;
                }
                ++mapIter;
                ++maxIter;
                ++rCBVIter;
            }
        }
    }
    double mean=sum/(bloodvolfraction*(double)contCBV);
    //std::cout<<"Mean:"<<mean<<std::endl;
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    maxIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>200)//Out of the brain-> empirical values!first crit.-> background
                {
                    if(rCBVIter.Get()>0)
                    {
                        //mapIter.Set((int)sbIter.Get());
                        //mapIter.Set((int)(rCBVIter.Get()/mean));
                        mapIter.Set((int)(rCBVIter.Get()));
                        //std::cout<<"+"<<" ("<<rCBVIter.Get()/mean<<")";
                    }
                    else
                    {
                        mapIter.Set(0);
                    }
                }
                else
                {
                    mapIter.Set(0);
                }

                        if(mapIter.Get()>m_mapMax)
                        {
                            m_mapMax=mapIter.Get();
                        }

                        if(mapIter.Get()<m_mapMin)
                        {
                            m_mapMin=mapIter.Get();
                        }
                ++mapIter;
                ++maxIter;
                ++rCBVIter;
            }
        }
    }
    //std::cout<<"End recorregut5: "<<std::endl;


    if(m_mapVolume!=0)
    {
        delete m_mapVolume;
    }
    m_mapVolume = new Volume();
    m_mapVolume->setImages( m_perfuVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume->setData(mapImage);

/*    typedef itk::ImageFileWriter <Volume::ItkImageType> writerType;
    writerType::Pointer mapWriter = writerType::New();
    mapWriter->SetFileName("VSImap.mhd");
    mapWriter->SetInput(mapImage);
    mapWriter->Update();*/
    //std::cout<<"End new map "<<std::endl;

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView_2->setInput( m_mapVolume );
    m_2DView_2->resetView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    this->createColorMap();

    int* ext= m_mapVolume->getWholeExtent();
    //std::cout<<"["<<ext[0]<<","<<ext[1]<<";"<<ext[2]<<","<<ext[3]<<";"<<ext[4]<<","<<ext[5]<<"]"<<std::endl;
    //std::cout<<"Number of voxels:"<<cont<<std::endl;
    //std::cout<<"End computeCBV!!!"<<std::endl;

}

void QGlialEstimationExtension::applyFilterMapImage( )
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    typedef itk::CastImageFilter< Volume::ItkImageType, InternalImageType >                     InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType >                      OutputCastingFilterType;

    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType >      CurvatureFlowImageFilterType;

    if(m_mapVolume != 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Volume::ItkImageType::RegionType region;
        Volume::ItkImageType::IndexType start;
        start[0]=0;
        start[1]=0;
        start[2]=0;
        //std::cout<<"Init Filter Volume"<<std::endl;
        Volume::ItkImageType::SizeType size = m_perfuVolume->getItkData()->GetBufferedRegion().GetSize();
        size[2]=m_perfuVolume->getSeries()->getNumberOfSlicesPerPhase();
        //std::cout<<"Perfu size:"<<size<<" // Start:"<<m_perfuVolume->getItkData()->GetBufferedRegion().GetSize()<<std::endl;
        //std::cout<<"Map size:"<<m_mapVolume->getItkData()->GetBufferedRegion().GetSize()<<std::endl;
        region.SetSize(size);
        region.SetIndex(start);
        Volume::ItkImageType::Pointer auxImage = Volume::ItkImageType::New();
        auxImage->SetRegions( region );
        auxImage->Allocate();


        InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
        OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
        CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

        incaster->SetInput( m_mapVolume->getItkData() );
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
            std::cerr << "Exception caught !" << std::endl;
            std::cerr << excep << std::endl;
        }

        typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
        Iterator outIter( outcaster->GetOutput(), outcaster->GetOutput()->GetBufferedRegion() );
        Iterator auxIter( auxImage, auxImage->GetBufferedRegion() );
        outIter.GoToBegin();
        auxIter.GoToBegin();
        m_mapMax=outIter.Get();
        m_mapMin=outIter.Get();
        //std::cout<<"Perfu size:"<<size<<" // Start:"<<outcaster->GetOutput()->GetBufferedRegion().GetSize()<<std::endl;
        //std::cout<<"Map size:"<<auxImage->GetBufferedRegion().GetSize()<<std::endl;
        unsigned int i,j,k;
        for (k=0;k<size[2];k++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    auxIter.Set(outIter.Get());
                    if(m_mapMax<outIter.Get()) m_mapMax=outIter.Get();
                    if(m_mapMin>outIter.Get()) m_mapMin=outIter.Get();
                    ++auxIter;
                    ++outIter;
                }
            }
        }
        if(m_mapVolume!=0)
        {
            delete m_mapVolume;
        }
        //std::cout<<"Init SetData Volume"<<std::endl;
        m_mapVolume = new Volume();
        m_mapVolume->setImages( m_perfuVolume->getPhaseVolume(0)->getImages() );
        //std::cout<<"SetData Volume"<<std::endl;
        try
        {
           m_mapVolume->setData( auxImage );
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught !" << std::endl;
            std::cerr << excep << std::endl;
        }
        //std::cout<<"End SetData Volume"<<std::endl;
        m_2DView_2->setInput( m_mapVolume );
        //std::cout<<"Init Colormap"<<std::endl;
        this->createColorMap( );
        m_2DView_2->setSlice( m_mapVolume->getDimensions()[2]/2 );
        QApplication::restoreOverrideCursor();
        //std::cout<<"End Filter Volume"<<std::endl;

    }

}

void QGlialEstimationExtension::applyRegistration(  )
{
    //std::cout<<"Init Registre!!"<<std::endl;
    QApplication::setOverrideCursor( Qt::WaitCursor );

    //Definició de paràmetres
    const double REGISTRATION_FIXED_STANDARD_DEVIATION = 0.4;
    const double REGISTRATION_MOVING_STANDARD_DEVIATION = 0.4;
    const int REGISTRATION_NUMBER_OF_SPACIAL_SAMPLES = 200;
    const int REGISTRATION_FIXED_VARIANCE = 2;
    const int REGISTRATION_MOVING_VARIANCE = 2;
    const int REGISTRATION_MAXIMUM_STEP = 1;
    const double REGISTRATION_MINIMUM_STEP = 0.001;
    const int REGISTRATION_NUMBER_OF_ITERATIONS = 300;

    Volume* fixedVolume;
    Volume* movingVolume;
    Q2DViewer* fixedViewer;
    Q2DViewer* movingViewer;

    switch( m_fixedImageComboBox->currentIndex() )
    {
    case T1:
        fixedVolume = m_T1Volume;
        //std::cout<<"Fem el T1!!!!"<<std::endl;
        fixedViewer = m_2DView_1;
    break;

    case perfu:
        fixedVolume = m_mapVolume;
        //std::cout<<"Fem perfu!!!!"<<std::endl;
        fixedViewer = m_2DView_2;
    break;

    case FLAIR:
        fixedVolume = m_FLAIRVolume;
        //std::cout<<"Fem el FLAIR!!!!"<<std::endl;
        fixedViewer = m_2DView_4;
    break;

    case difu:
        fixedVolume = m_difuVolume;
        //std::cout<<"Fem la difu!!!!"<<std::endl;
        fixedViewer = m_2DView_5;
    break;

    default:
    break;
    }

    switch( m_movingImageComboBox->currentIndex() )
    {
    case T1:
        movingVolume = m_T1Volume;
        //std::cout<<"Fem el T1!!!!"<<std::endl;
        movingViewer = m_2DView_1;
    break;

    case perfu:
        movingVolume = m_mapVolume;
        //std::cout<<"Fem perfu!!!!"<<std::endl;
        movingViewer = m_2DView_2;
    break;

    case FLAIR:
        movingVolume = m_FLAIRVolume;
        //std::cout<<"Fem el FLAIR!!!!"<<std::endl;
        movingViewer = m_2DView_4;
    break;

    case difu:
        movingVolume = m_difuVolume;
        //std::cout<<"Fem la difu!!!!"<<std::endl;
        movingViewer = m_2DView_5;
    break;

    default:
    break;
    }

    if( !fixedVolume || !movingVolume )
    {
        QApplication::restoreOverrideCursor();
        QString message;
        message = tr( "One or more images are not defined" );
        QMessageBox::warning( this , tr("Starviewer") , message );
        return;
    }

    //En cas que algun volum tingui fases agafem la que acualment s'està representant en el seu viewer
    if ( fixedVolume->getNumberOfPhases() !=  1)
    {
        //std::cout<<"El fixed te fases -> "<< fixedVolume->getNumberOfPhases() <<std::endl;
        //std::cout<<"Agafem la fase -> "<< fixedViewer->getCurrentPhase() <<std::endl;
        fixedVolume = fixedVolume->getPhaseVolume ( fixedViewer->getCurrentPhase() );
    }

    if ( movingVolume->getNumberOfPhases() !=  1)
    {
        //std::cout<<"El moving te fases -> "<< movingVolume->getNumberOfPhases() <<std::endl;
        //std::cout<<"Agafem la fase -> "<< movingViewer->getCurrentPhase() <<std::endl;
        movingVolume = movingVolume->getPhaseVolume ( movingViewer->getCurrentPhase() );
    }

    itkRegistre3DAffine< Volume::ItkImageType, Volume::ItkImageType > registre;
    registre.SetInputImages( fixedVolume->getItkData(), movingVolume->getItkData() );
    registre.SetParamatersMetric( REGISTRATION_FIXED_STANDARD_DEVIATION,
                                  REGISTRATION_MOVING_STANDARD_DEVIATION,
                                  REGISTRATION_NUMBER_OF_SPACIAL_SAMPLES );
    registre.SetParamatersGaussian( REGISTRATION_FIXED_VARIANCE, REGISTRATION_MOVING_VARIANCE );
    registre.SetParamatresOptimizer( REGISTRATION_MAXIMUM_STEP, REGISTRATION_MINIMUM_STEP,
                                     REGISTRATION_NUMBER_OF_ITERATIONS );

    if ( registre.applyMethod() )
    {
        typedef itkRegistre3DAffine< Volume::ItkImageType, Volume::ItkImageType >::TransformType TransformType;
        typedef itkRegistre3DAffine< Volume::ItkImageType, Volume::ItkImageType >::TransformType::InputPointType TransformPointType;
        typedef itk::ResampleImageFilter< Volume::ItkImageType, Volume::ItkImageType > ResampleGrisFilterType;
        typedef itk::RescaleIntensityImageFilter< Volume::ItkImageType, Volume::ItkImageType > RescaleFilterType;

        TransformType::Pointer registerTransform;
        itkRegistre3DAffine< Volume::ItkImageType, Volume::ItkImageType >::OptimizerParametersType finalParameters;
        finalParameters = registre.getFinalParameters();

        if ( !registerTransform ) registerTransform = TransformType::New();
        registerTransform->SetParameters( finalParameters );

        Volume::ItkImageType::SpacingType fixedSpacing = fixedVolume->getItkData()->GetSpacing();
        Volume::ItkImageType::PointType fixedOrigin = fixedVolume->getItkData()->GetOrigin();
        Volume::ItkImageType::SizeType fixedSize = fixedVolume->getItkData()->GetLargestPossibleRegion().GetSize();
        TransformPointType centerFixed;
        centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
        centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;
        centerFixed[2] = fixedOrigin[2] + fixedSpacing[2] * fixedSize[2] / 2.0;

        registerTransform->SetCenter( centerFixed );

        ResampleGrisFilterType::Pointer resample = ResampleGrisFilterType::New();
        resample->SetTransform( registerTransform );
        resample->SetInput( movingVolume->getItkData() );
        resample->SetSize( fixedVolume->getItkData()->GetLargestPossibleRegion().GetSize() );
        resample->SetOutputOrigin( fixedVolume->getItkData()->GetOrigin() );
        resample->SetOutputSpacing( fixedVolume->getItkData()->GetSpacing() );
        resample->SetDefaultPixelValue( 0 );
        resample->Update();

        //Seleccionem el window level de la imatge fixa
        double fixedLevel = fixedViewer->getCurrentColorLevel();
        double fixedWindow = fixedViewer->getCurrentColorWindow();

        RescaleFilterType::Pointer rescalerMoving = RescaleFilterType::New();
        rescalerMoving->SetInput( resample->GetOutput() );
        rescalerMoving->SetOutputMinimum( fixedLevel - fixedWindow/2 );
        rescalerMoving->SetOutputMaximum( fixedLevel + fixedWindow/2 );
        rescalerMoving->Update();

        if ( !m_registeredVolume ) m_registeredVolume = new Volume();

        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_registeredVolume->setImages( fixedVolume->getImages() );
        m_registeredVolume->setData( rescalerMoving->GetOutput() );

        m_2DView_3->setInput( fixedVolume );
        m_2DView_3->setWindowLevel(fixedWindow, fixedLevel);

        m_2DView_3->setOverlayToBlend();
        m_2DView_3->setOpacityOverlay( m_opacityRegistrationSlider->value() / 100.0 );
        m_2DView_3->setOverlayInput( m_registeredVolume );

        m_2DView_3->refresh();
    }
    else
    {
        QMessageBox::warning( this, tr("Registration failed!"), tr("Registration failed!") );
    }

    QApplication::restoreOverrideCursor();
    //std::cout<<"Done Registre!!"<<std::endl;
}

void QGlialEstimationExtension::glialEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        //setPaintCursor();
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

void QGlialEstimationExtension::viewT1Thresholds(int value)
{
    if ( !m_T1MaskVolume )
    {
        m_T1MaskVolume = new Volume();
    }
    else
    {
        delete m_T1MaskVolume;
        m_T1MaskVolume = new Volume();
    }

    vtkImageThreshold * imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_T1Volume->getVtkData() );
    imageThreshold->ThresholdByUpper( value );
    imageThreshold->SetInValue ( m_2DView_1->getCurrentColorLevel() + m_2DView_1->getCurrentColorWindow()/2 );
    imageThreshold->SetOutValue( m_2DView_1->getCurrentColorLevel() - m_2DView_1->getCurrentColorWindow()/2 );
    imageThreshold->Update();

    m_T1MaskVolume->setImages( m_T1Volume->getImages() );
    m_T1MaskVolume->setData( imageThreshold->GetOutput() );

    m_2DView_1->setOverlayToBlend();
    m_2DView_1->setOpacityOverlay( m_T1MaskOpacitySlider->value() / 100.0 );
    //m_2DView_1->setOpacityOverlay( 0.5 );
    m_2DView_1->setOverlayInput( m_T1MaskVolume );

    m_2DView_1->refresh();
    //m_T1OpacityLabel->setEnabled( true );
    //m_T1OpacitySlider->setEnabled( true );

    imageThreshold->Delete();
}

void QGlialEstimationExtension::setT1MaskOpacity( int opacity )
{
    m_2DView_1->setOpacityOverlay(((double)opacity)/100.0);
    m_2DView_1->setOverlayInput(m_T1MaskVolume);
    m_2DView_1->refresh();
}

void QGlialEstimationExtension::setT1SeedPosition ( )
{
    m_2DView_1->getSeedPosition( m_seedT1Position );

    m_T1SeedXLineEdit->setText( QString::number( m_seedT1Position[0], 'f', 1 ) );
    m_T1SeedYLineEdit->setText( QString::number( m_seedT1Position[1], 'f', 1 ) );
    m_T1SeedZLineEdit->setText( QString::number( m_seedT1Position[2], 'f', 1 ) );

    m_T1ApplyPushButton->setEnabled( true );
}

void QGlialEstimationExtension::applyT1Segmentation()
{
    if( m_T1Volume->getNumberOfPhases() > 1)
    {
        QString message;
        message = tr( "The number of phases of the T1 Volume is greater than 1" );
        QMessageBox::warning( this , tr("Starviewer") , message );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    if ( !m_segmentationMethod ) m_segmentationMethod = new StrokeSegmentationMethod();

    m_segmentationMethod->setVolume( m_T1Volume );

    if ( !m_T1MaskVolume ) m_T1MaskVolume = new Volume();

    m_segmentationMethod->setMask( m_T1MaskVolume );
    m_segmentationMethod->setInsideMaskValue ( m_2DView_1->getCurrentColorLevel() + m_2DView_1->getCurrentColorWindow()/2 );
    m_segmentationMethod->setOutsideMaskValue( m_2DView_1->getCurrentColorLevel() - m_2DView_1->getCurrentColorWindow()/2 );
    m_segmentationMethod->setHistogramLowerLevel( m_T1ValueSlider->value() );
    m_segmentationMethod->setHistogramUpperLevel( m_maxT1Value );
    m_segmentationMethod->setSeedPosition( m_seedT1Position[0], m_seedT1Position[1], m_seedT1Position[2] );

    m_T1VolumeCont = m_segmentationMethod->applyMethod();
    //Compte Phases!!!!!!!!
    m_T1Cont = (int)(m_T1VolumeCont / (m_T1Volume->getSpacing()[0]*m_T1Volume->getSpacing()[1]*m_T1Volume->getSpacing()[2]));

    m_2DView_1->setOverlayToBlend();
    m_2DView_1->setOpacityOverlay( m_T1MaskOpacitySlider->value() / 100.0 );
    m_2DView_1->setOverlayInput( m_T1MaskVolume );

    m_T1MaskOpacityLabel->setEnabled( true );
    m_T1MaskOpacitySlider->setEnabled( true );

    m_T1VolumeLineEdit->setText( QString::number( m_T1VolumeCont, 'f', 2 ) );
    m_T1VolumeLabel->setEnabled( true );
    m_T1VolumeLineEdit->setEnabled( true );

/*    m_editorAction->trigger();
    m_diffusion2DView->disableTools();
    m_editorAction->setEnabled( true );

    m_paintEditorAction->setEnabled(true);
    m_eraseEditorAction->setEnabled(true);
    m_eraseSliceEditorAction->setEnabled(true);
    m_eraseRegionEditorAction->setEnabled(true);
    m_eraseEditorAction->trigger();
    m_editorTool = QDifuPerfuSegmentationExtension::Erase;
    m_editorSize->setEnabled(true);

    m_lesionViewAction->setEnabled( true );
    m_lesionViewAction->trigger();
    this->viewLesionOverlay();
*/
    m_2DView_1->refresh();

    QApplication::restoreOverrideCursor();
}

void QGlialEstimationExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;
}

void QGlialEstimationExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}

void QGlialEstimationExtension::contextMenuT1Release()
{
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    m_imageGlialtype = QGlialEstimationExtension::T1;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QGlialEstimationExtension::contextMenuPerfuRelease()
{
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    m_imageGlialtype = QGlialEstimationExtension::perfu;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QGlialEstimationExtension::contextMenuFLAIRRelease()
{
    int eventPositionX = 0;
    int eventPositionY = 0;

    QPoint point = QPoint( eventPositionX, eventPositionY );

    m_imageGlialtype = QGlialEstimationExtension::FLAIR;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QGlialEstimationExtension::contextMenuDifuRelease()
{
    int eventPositionX = 0;
    int eventPositionY = 0;

    QPoint point = QPoint( eventPositionX, eventPositionY );

    m_imageGlialtype = QGlialEstimationExtension::difu;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QGlialEstimationExtension::contextMenuSpectrumRelease()
{
    int eventPositionX = 0;
    int eventPositionY = 0;

    QPoint point = QPoint( eventPositionX, eventPositionY );

    m_imageGlialtype = QGlialEstimationExtension::spectrum;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QGlialEstimationExtension::contextMenuEvent(QContextMenuEvent *event)
{
    //if (m_contextMenuActive)
    //{
        PatientBrowserMenu *patientMenu = new PatientBrowserMenu(this);
        patientMenu->setAttribute(Qt::WA_DeleteOnClose);
        patientMenu->setPatient( QApplicationMainWindow::getActiveApplicationMainWindow()->getCurrentPatient() );

        connect(patientMenu, SIGNAL( selectedSeries(Series*) ), SLOT( setSeries(Series*) ));

        QString seriesUID;
        if( m_mainVolume )
            seriesUID = m_mainVolume->getSeries()->getInstanceUID();
        patientMenu->popup( event->globalPos(), seriesUID  ); //->globalPos() ?

    //}
}

void QGlialEstimationExtension::setSeries(Series *series)
{
    QString modality = series->getModality();
    itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::Pointer minmaxCalc;
    if( modality == "KO" || modality == "PR" || modality == "SR" )
    {
        QMessageBox::information( this , tr( "Viewer" ) , tr( "The selected item is not a valid image format" ) );
    }
    else
    {
        switch(m_imageGlialtype)
        {
        case T1:
            m_T1LineEdit->clear();
            m_T1LineEdit->insert(series->getDescription());
            m_T1Volume = series->getFirstVolume();

            minmaxCalc = itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::New();
            minmaxCalc->SetImage(m_T1Volume->getItkData());
            minmaxCalc->SetRegion(m_T1Volume->getItkData()->GetRequestedRegion());
            minmaxCalc->Compute();
            DEBUG_LOG( QString("ItkMax=%1, ItkMin=%2").arg(minmaxCalc->GetMaximum()).arg(minmaxCalc->GetMinimum()) );
            m_minT1Value = minmaxCalc->GetMinimum();
            m_maxT1Value = minmaxCalc->GetMaximum();
            m_T1ValueSpinBox->setMinimum( m_minT1Value );
            m_T1ValueSpinBox->setMaximum( m_maxT1Value );
            m_T1ValueSlider->setMinimum( m_minT1Value );
            m_T1ValueSlider->setMaximum( m_maxT1Value );

            m_2DView_1->setInput( m_T1Volume );
            break;
        case perfu:
            m_perfuLineEdit->clear();
            m_perfuLineEdit->insert(series->getDescription());
            m_perfuVolume = series->getFirstVolume();
            this->computeCBV();
            m_2DView_2->setInput( m_mapVolume );
            break;
        case FLAIR:
            m_FLAIRLineEdit->clear();
            m_FLAIRLineEdit->insert(series->getDescription());
            m_FLAIRVolume = series->getFirstVolume();
            m_2DView_4->setInput( m_FLAIRVolume );
            break;
        case difu:
            m_difuLineEdit->clear();
            m_difuLineEdit->insert(series->getDescription());
            m_difuVolume = series->getFirstVolume();
            m_2DView_5->setInput( m_difuVolume );
            break;
        case spectrum:
            m_spectrumLineEdit->clear();
            m_spectrumLineEdit->insert(series->getDescription());
            m_spectrumVolume = series->getFirstVolume();
            m_2DView_6->setInput( m_spectrumVolume );
            break;
        default:
            DEBUG_LOG("No existeix aquest tipus d'imatge!!");
            break;
        }
    }
}

bool QGlialEstimationExtension::findProbableSeries( )
{
    //std::cout<<"Nombre d'estudis:" <<QApplicationMainWindow::getActiveApplicationMainWindow()->getCurrentPatient()->getStudies().first()->getSeries().size() <<std::endl;
    bool findT1=false;
    bool findperfu=false;
    bool findFLAIR=false;
    bool finddifu=false;
    bool findspectrum=false;
    foreach( Study *study, m_mainVolume->getPatient()->getStudies() )
    {
        foreach( Series *series, study->getSeries() )
        {
            if(series->getDescription().contains("T1", Qt::CaseInsensitive) && series->getFirstVolume()) // && slicesDSC < series->getFirstVolume()->getDimensions()[3])
            {
                m_T1LineEdit->clear();
                m_T1LineEdit->insert(series->getDescription());
                m_T1Volume = series->getFirstVolume();
                findT1=true;
                //std::cout<<"*";
                //slicesDSC = series->getFirstVolume()->getDimensions()[3];
            }
            if((series->getDescription().contains("PERFUSIO", Qt::CaseInsensitive)||series->getDescription().contains("DSC", Qt::CaseInsensitive)) && series->getFirstVolume()) // && slicespre < series->getFirstVolume()->getDimensions()[3])
            {
                if(series->getFirstVolume()->getNumberOfPhases()>10)
                {
                    m_perfuLineEdit->clear();
                    m_perfuLineEdit->insert(series->getDescription());
                    m_perfuVolume = series->getFirstVolume();
                    findperfu=true;
                    //std::cout<<"+";
                    //slicespre = series->getFirstVolume()->getDimensions()[3];
                }
            }
            if(series->getDescription().contains("FLAIR", Qt::CaseInsensitive) && series->getFirstVolume()) // && slicespost < series->getFirstVolume()->getDimensions()[3])
            {
                m_FLAIRLineEdit->clear();
                m_FLAIRLineEdit->insert(series->getDescription());
                m_FLAIRVolume = series->getFirstVolume();
                findFLAIR=true;
                //std::cout<<"-";
                //slicespost = series->getFirstVolume()->getDimensions()[3];
            }
            //DEBUG_LOG(series->getDescription());
            if(series->getDescription().contains("DIFUSIO", Qt::CaseInsensitive) && series->getFirstVolume()) // && slicesDSC < series->getFirstVolume()->getDimensions()[3])
            {
                m_difuLineEdit->clear();
                m_difuLineEdit->insert(series->getDescription());
                m_difuVolume = series->getFirstVolume();
                finddifu=true;
                //std::cout<<"*";
                //slicesDSC = series->getFirstVolume()->getDimensions()[3];
            }
            //DEBUG_LOG(series->getDescription());
            if(series->getDescription().contains("SPECTR", Qt::CaseInsensitive) && series->getFirstVolume()) // && slicesDSC < series->getFirstVolume()->getDimensions()[3])
            {
                m_spectrumLineEdit->clear();
                m_spectrumLineEdit->insert(series->getDescription());
                m_spectrumVolume = series->getFirstVolume();
                findspectrum=true;
                //std::cout<<"*";
                //slicesDSC = series->getFirstVolume()->getDimensions()[3];
            }
            //std::cout<<series->getDescription().toAscii().data()<<"//"<<series->getProtocolName().toAscii().data()<<"//"<<series->getInstanceUID().toAscii().data()<<"//"<<series->getFirstVolume()<<"//"<<series->getImages().size()<<std::endl;
        }
    }
    return (findT1 && findperfu && findFLAIR && finddifu && findspectrum);
}

void QGlialEstimationExtension::setRegistrationOpacity(int op)
{
    m_2DView_3->setOpacityOverlay(((double)op)/100.0);
    m_2DView_3->setOverlayInput(m_registeredVolume);
    m_2DView_3->refresh();
}

void QGlialEstimationExtension::readSettings()
{
    QSettings settings;
    m_verticalSplitter->restoreState( settings.value("StarViewer-App-GlialEstimation/verticalSplitter").toByteArray() );
}

void QGlialEstimationExtension::writeSettings()
{
    QSettings settings;
    settings.setValue("StarViewer-App-GlialEstimation/verticalSplitter", m_verticalSplitter->saveState() );
}

}
