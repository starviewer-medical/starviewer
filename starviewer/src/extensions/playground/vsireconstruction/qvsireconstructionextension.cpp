/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qvsireconstructionextension.h"

#include "toolsactionfactory.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "series.h"
#include "toolmanager.h"
#include "patientbrowsermenu.h"

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


// ITK
#include <itkImageFileWriter.h>


namespace udg {

QVSIReconstructionExtension::QVSIReconstructionExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_DSCVolume(0), m_mapVolume(0), m_mapVolume2(0), m_isLeftButtonPressed(false)
{
    setupUi( this );

    createActions();
    createConnections();
    readSettings();

}

QVSIReconstructionExtension::~QVSIReconstructionExtension()
{
    writeSettings();
}

void QVSIReconstructionExtension::createActions()
{
    /*// creem el tool manager i li assignem les tools. TODO de moment només tenim VoxelInformation, però s'han d'anar afegint la resta
    m_toolManager = new ToolManager(this);
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    QStringList toolsList;
    toolsList << "VoxelInformationTool";
    m_toolManager->setViewerTools( m_2DView, toolsList );
    m_toolManager->setViewerTools( m_2DView_2, toolsList );
    m_toolManager->setViewerTools( m_2DView_4, toolsList );
*/
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

    m_voxelInformationAction = m_actionFactory->getActionFrom( "VoxelInforamtionTool" );
    m_voxelInformationToolButton->setDefaultAction( m_voxelInformationAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView_2, SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView_4, SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_slicingAction->trigger();
    m_2DView->enableTools();
    m_2DView_2->enableTools();
    m_2DView_4->enableTools();

}

void QVSIReconstructionExtension::createConnections()
{
  connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), SLOT( vsiEventHandler(unsigned long) ) );
  connect( m_2DView_2, SIGNAL( eventReceived( unsigned long ) ), SLOT( vsiEventHandler(unsigned long) ) );
  connect( m_2DView_4, SIGNAL( eventReceived( unsigned long ) ), SLOT( vsiEventHandler(unsigned long) ) );
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView_2 , SLOT( setSlice(int) ) );
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView_4 , SLOT( setSlice(int) ) );
  connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
  connect( m_2DView_2, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
  connect( m_chooseDSCPushButton, SIGNAL( clicked() ), SLOT( contextMenuDSCRelease() ) );
  connect( m_chooseSEPrePushButton, SIGNAL( clicked() ), SLOT( contextMenuSEPreRelease() ) );
  connect( m_chooseSEPostPushButton, SIGNAL( clicked() ), SLOT( contextMenuSEPostRelease() ) );
  connect( m_computeVSIPushButton, SIGNAL( clicked() ), SLOT( computeVSI() ) );
  //connect( m_2DView, SIGNAL( windowLevelChanged( double,double ) ), SLOT( createColorMap( double, double ) ) );
}

void QVSIReconstructionExtension::setInput( Volume *input )
{
    m_mainVolume = input;

    //Posem els nivells de dins i fora de la m�cara els valors l�its del w/l per tal que es vegi correcte
    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_insideValue  = (int) wl[0];
    m_outsideValue = (int) (wl[0] - 2.0*wl[1]);

    std::cout<<"Creating map..."<<std::endl;
    //this->computeMTT();
    //this->computeCBV();
    std::cout<<"Creating transfer function..."<<std::endl;
    this->createColorMap( );
    std::cout<<"Rendering map..."<<std::endl;
    m_2DView->render();
    std::cout<<"Rendering image..."<<std::endl;
    m_2DView_2->render();
    std::cout<<"Done!!"<<std::endl;


}

void QVSIReconstructionExtension::computeTTP( )
{

    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_mainVolume->getItkData()->GetBufferedRegion().GetSize();
    size[2]=m_mainVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer mapImage = Volume::ItkImageType::New();
    mapImage->SetRegions( region );
    mapImage->Allocate();
    std::cout<<"Region:"<<region<<std::endl;
    std::cout<<"Size:"<<size<<std::endl;

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
    ConstIterator iter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );

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

    unsigned int kend=m_mainVolume->getSeries()->getNumberOfSlicesPerPhase();
    unsigned int pend=m_mainVolume->getSeries()->getNumberOfPhases();
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
                   mapIter.Set(((int)(mttIter.Get()*255)/m_mainVolume->getSeries()->getNumberOfPhases()));//Ho escalem pq hi hagi tants valors com colors de la LuT
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
    m_mapVolume->setImages( m_mainVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume->setData(mapImage);

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_mapVolume );
    m_2DView->resetView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();

    m_2DView_2->setInput( m_mainVolume );
    m_2DView_2->resetView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    int* ext= m_mapVolume->getWholeExtent();
    std::cout<<"["<<ext[0]<<","<<ext[1]<<";"<<ext[2]<<","<<ext[3]<<";"<<ext[4]<<","<<ext[5]<<"]"<<std::endl;
    std::cout<<"Number of voxels:"<<cont<<std::endl;

}

void QVSIReconstructionExtension::createColorMap( )
{
    //Pintem el mapa amb colors
    std::cout<<"Max Map:"<<m_mapMax<<", Min Map:"<<m_mapMin<<std::endl;
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
    int nvalues=mapHueLut->GetNumberOfTableValues();
    std::cout<<"Max:"<<m_maxValue<<", Min:"<<m_minValue<<", NValues:"<<nvalues<<std::endl;
    std::cout<<"Max LuT:"<<m_mapMax<<", Min Lut:"<<m_mapMin<<","<<nvalues<<std::endl;
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

    m_2DView->getWindowLevelMapper()->SetLookupTable( mapHueLut );

    m_2DView->setWindowLevel(1.0, m_mapMin - 1.0);
    //Potser això fa que es recalculi dues vegades??
    //m_2DView->setWindowLevel(m_mapMax - m_mapMin, (m_mapMax + m_mapMin)/2);
}


void QVSIReconstructionExtension::createColorMap2( )
{
    //Pintem el mapa amb colors
    std::cout<<"create ColorMap2"<<std::endl;
    std::cout<<"Max Map:"<<m_maxValue<<", Min Map:"<<m_minValue<<std::endl;
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

    m_2DView_4->getWindowLevelMapper()->SetLookupTable( mapHueLut );

    m_2DView_4->setWindowLevel(1.0, m_mapMin - 1.0);
    //Potser això fa que es recalculi dues vegades??
    //m_2DView_4->setWindowLevel(m_mapMax - m_mapMin, (m_mapMax + m_mapMin)/2);
}

void QVSIReconstructionExtension::createColorMap( double window, double level )
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
    std::cout<<"Max:"<<m_maxValue<<", Min:"<<m_minValue<<", NValues:"<<nvalues<<std::endl;
    std::cout<<"Max LuT:"<<m_mapMax<<", Min Lut:"<<m_mapMin<<","<<nvalues<<std::endl;
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

    m_2DView->getWindowLevelMapper()->SetLookupTable( mapHueLut );
}


void QVSIReconstructionExtension::computeCBV( )
{
    if(!m_DSCVolume)
    {
        m_DSCVolume = m_mainVolume;
    }
    std::cout<<"Init computeCBV"<<std::endl;
    //Paràmetres que definim constants però que potser no ho són
    static const unsigned int Nbaseline = 20; //->Mostres pre-bolus
    static const double TEdyn = 0.025; //->valor extret del pwp d'en Gerard
    static const double bloodvolfraction = 0.06; //->Document VSI
    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    std::cout<<"Init DSC Volume"<<std::endl;
    Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
    std::cout<<"DSC Volume Size: "<<size<<std::endl;
    std::cout<<"End DSC Volume"<<std::endl;
    size[2]=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer mapImage = Volume::ItkImageType::New();
    mapImage->SetRegions( region );
    mapImage->Allocate();
    std::cout<<"Region:"<<region<<std::endl;
    std::cout<<"Size:"<<size<<std::endl;

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
    ConstIterator DSCiter( m_DSCVolume->getItkData(), m_DSCVolume->getItkData()->GetBufferedRegion() );

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

    unsigned int kend=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    unsigned int pend=m_DSCVolume->getSeries()->getNumberOfPhases();
    sbIter.GoToBegin();
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    std::cout<<"Init recorregut: ["<<size[0]<<","<<size[1]<<","<<kend<<"]"<<std::endl;
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

    std::cout<<"End recorregut2: "<<std::endl;
    std::cout<<"Init recorregut: ["<<size[0]<<", "<<size[1]<<", "<<Nbaseline<<" ("<<pend<<"), "<<kend<<"]"<<std::endl;
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

    std::cout<<"Init recorregut max: "<<std::endl;
    std::ofstream fout("DSC.dat", ios::out);
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
                    if((i==51)&&(j==65)&&(k==6))
                    {
                        fout<<value<<std::endl;
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
    fout.close();

    /*maxIter.GoToBegin();
    minIter.GoToBegin();
    mapIter.GoToBegin();
    sbIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>(minIter.Get()+300))
                {
                    //std::cout<<sbIter.Get()<<" -> ("<<maxIter.Get()<<","<<minIter.Get()<<")"<<std::endl;
                    mapIter.Set(220);
                }
                else
                {
                    mapIter.Set(0);
                }
                ++maxIter;
                ++minIter;
                ++mapIter;
                ++sbIter;
            }
        }
    }*/

    std::cout<<"End recorregut3: "<<std::endl;
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
                    if(maxIter.Get()<=700)//Out of the brain-> empirical values!first crit.-> background
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
                            if((k==5)&&(p==pend-1))
                            {
                                //std::cout<<valuerCBV - log(DSCiter.Get()/sbIter.Get())/TEdyn<<" ("<<sbIter.Get()<<")"<<std::endl;
                            }
                            //std::cout<<std::endl<<k<<" "<<p;
                        }
                        else
                        {
                            if((k==5)&&(p==pend-1))
                            {
                                //std::cout<<"+"<<" ("<<sbIter.GetIndex()<<")";
                            }
                        }
                    }
                    /*if(p=pend-1)
                    {
                        if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
                        {
                            //mapIter.Set((int)sbIter.Get());
                            mapIter.Set((int)rCBVIter.Get());
                            std::cout<<"+"<<" ("<<mapIter.Get()<<")";
                        }
                        else
                        {
                            mapIter.Set(0);
                        }
                        //std::cout<<mapIter.Get()<<std::endl;
                        if(mapIter.Get()>m_mapMax)
                        {
                            m_mapMax=mapIter.Get();
                        }

                        if(mapIter.Get()<m_mapMin)
                        {
                            m_mapMin=mapIter.Get();
                        }
                    }*/
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
                if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
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
    std::cout<<"Mean:"<<mean<<std::endl;
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    maxIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
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
    std::cout<<"End recorregut5: "<<std::endl;


    if(m_mapVolume!=0)
    {
        delete m_mapVolume;
    }
    m_mapVolume = new Volume();
    m_mapVolume->setImages( m_mainVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume->setData(mapImage);

    typedef itk::ImageFileWriter <Volume::ItkImageType> writerType;
    writerType::Pointer mapWriter = writerType::New();
    mapWriter->SetFileName("VSImap.mhd");
    mapWriter->SetInput(mapImage);
    mapWriter->Update();
    std::cout<<"End new map "<<std::endl;

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_mapVolume );
    m_2DView->resetView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();

    m_2DView_2->setInput( m_mainVolume );
    m_2DView_2->resetView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    int* ext= m_mapVolume->getWholeExtent();
    std::cout<<"["<<ext[0]<<","<<ext[1]<<";"<<ext[2]<<","<<ext[3]<<";"<<ext[4]<<","<<ext[5]<<"]"<<std::endl;
    std::cout<<"Number of voxels:"<<cont<<std::endl;
    std::cout<<"End computeCBV!!!"<<std::endl;

}

void QVSIReconstructionExtension::computeVSI( )
{
    /*if(!m_DSCVolume)
    {
        m_DSCVolume = m_mainVolume;
    }*/
    if(!m_DSCVolume || !m_SEPreVolume || !m_SEPostVolume)
    {
        QMessageBox::information(0,tr("VSI Extension"), tr("Some of the images have not been introduced") );
        return;
    }
    std::cout<<"Init computeVSI"<<std::endl;

    //Paràmetres que definim constants però que potser no ho són
    static const unsigned int Nbaseline = 20; //->Mostres pre-bolus
    static const double TEdyn = 0.025; //->valor extret del pwp d'en Gerard
    static const double bloodvolfraction = 0.06; //->Document VSI
    static const double ADC = 0.8; //->Mostres pre-bolus
    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    std::cout<<"Init DSC Volume"<<std::endl;
    Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
    std::cout<<"DSC Volume Size: "<<size<<std::endl;
    std::cout<<"End DSC Volume"<<std::endl;
    size[2]=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer mapImage = Volume::ItkImageType::New();
    mapImage->SetRegions( region );
    mapImage->Allocate();
    std::cout<<"Region:"<<region<<std::endl;
    std::cout<<"Size:"<<size<<std::endl;

    Volume::ItkImageType::Pointer maxImage = Volume::ItkImageType::New();
    maxImage->SetRegions( region );
    maxImage->Allocate();

    Volume::ItkImageType::Pointer minImage = Volume::ItkImageType::New();
    minImage->SetRegions( region );
    minImage->Allocate();

    Volume::ItkImageType::Pointer mapImage2 = Volume::ItkImageType::New();
    mapImage2->SetRegions( region );
    mapImage2->Allocate();

    typedef itk::Image<float, 3> DoubleImageType;
    DoubleImageType::Pointer sbImage = DoubleImageType::New();
    sbImage->SetRegions( region );
    sbImage->Allocate();

    DoubleImageType::Pointer psiImage = DoubleImageType::New();
    psiImage->SetRegions( region );
    psiImage->Allocate();

    DoubleImageType::Pointer presbImage = DoubleImageType::New();
    presbImage->SetRegions( region );
    presbImage->Allocate();

    DoubleImageType::Pointer postsbImage = DoubleImageType::New();
    postsbImage->SetRegions( region );
    postsbImage->Allocate();

    DoubleImageType::Pointer preGEImage = DoubleImageType::New();
    preGEImage->SetRegions( region );
    preGEImage->Allocate();

    DoubleImageType::Pointer postGEImage = DoubleImageType::New();
    postGEImage->SetRegions( region );
    postGEImage->Allocate();

    DoubleImageType::Pointer RSeMeanImage = DoubleImageType::New();
    RSeMeanImage->SetRegions( region );
    RSeMeanImage->Allocate();

    DoubleImageType::Pointer RGeMeanImage = DoubleImageType::New();
    RGeMeanImage->SetRegions( region );
    RGeMeanImage->Allocate();

    DoubleImageType::Pointer rCBVImage = DoubleImageType::New();
    rCBVImage->SetRegions( region );
    rCBVImage->Allocate();

    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator DSCiter( m_DSCVolume->getItkData(), m_DSCVolume->getItkData()->GetBufferedRegion() );
    ConstIterator preIter( m_SEPreVolume->getItkData(), m_SEPreVolume->getItkData()->GetBufferedRegion() );
    ConstIterator postIter( m_SEPostVolume->getItkData(), m_SEPostVolume->getItkData()->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
    Iterator mapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator initialMapIter( mapImage, mapImage->GetBufferedRegion() );
    Iterator mapIter2( mapImage2, mapImage2->GetBufferedRegion() );
    Iterator initialMapIter2( mapImage2, mapImage2->GetBufferedRegion() );
    Iterator maxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator initialMaxIter( maxImage, maxImage->GetBufferedRegion() );
    Iterator minIter( minImage, minImage->GetBufferedRegion() );
    Iterator initialMinIter( minImage, minImage->GetBufferedRegion() );

    typedef itk::ImageRegionIterator<DoubleImageType> DoubleIterator;
    DoubleIterator sbIter( sbImage, sbImage->GetBufferedRegion() );
    DoubleIterator initialsbIter( sbImage, sbImage->GetBufferedRegion() );
    DoubleIterator presbIter( presbImage, presbImage->GetBufferedRegion() );
    DoubleIterator initialpresbIter( presbImage, presbImage->GetBufferedRegion() );
    DoubleIterator postsbIter( postsbImage, postsbImage->GetBufferedRegion() );
    DoubleIterator initialpostsbIter( postsbImage, postsbImage->GetBufferedRegion() );
    DoubleIterator preGEIter( preGEImage, preGEImage->GetBufferedRegion() );
    DoubleIterator initialpreGEIter( preGEImage, preGEImage->GetBufferedRegion() );
    DoubleIterator postGEIter( postGEImage, postGEImage->GetBufferedRegion() );
    DoubleIterator initialpostGEIter( postGEImage, postGEImage->GetBufferedRegion() );
    DoubleIterator rCBVIter( rCBVImage, sbImage->GetBufferedRegion() );
    DoubleIterator initialrCBVIter( rCBVImage, sbImage->GetBufferedRegion() );
    DoubleIterator rSEMeanIter( RSeMeanImage, RSeMeanImage->GetBufferedRegion() );
    DoubleIterator initialrSEMeanIter( RSeMeanImage, RSeMeanImage->GetBufferedRegion() );
    DoubleIterator rGEMeanIter( RGeMeanImage, RGeMeanImage->GetBufferedRegion() );
    DoubleIterator initialrGEMeanIter( RGeMeanImage, RGeMeanImage->GetBufferedRegion() );
    DoubleIterator psiIter( psiImage, psiImage->GetBufferedRegion() );

    typedef itk::ImageFileWriter <Volume::ItkImageType> writerType;

    Volume::ItkImageType::PixelType value;
    double valuesb, valuerCBV;
    long cont=0;
    unsigned int i,j,k,p;

    unsigned int kend=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    unsigned int pend=m_DSCVolume->getSeries()->getNumberOfPhases();
    sbIter.GoToBegin();
    presbIter.GoToBegin();
    rCBVIter.GoToBegin();
    mapIter.GoToBegin();
    std::cout<<"Init recorregut: ["<<size[0]<<","<<size[1]<<","<<kend<<"]"<<std::endl;
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                sbIter.Set(0.0);
                presbIter.Set(0.0);
                rCBVIter.Set(0.0);
                mapIter.Set(0);
                ++sbIter;
                ++mapIter;
                ++preIter;
                ++rCBVIter;
            }
        }
    }

    std::cout<<"End recorregut2: "<<std::endl;
    std::cout<<"Init recorregut: ["<<size[0]<<", "<<size[1]<<", "<<Nbaseline<<" ("<<pend<<"), "<<kend<<"]"<<std::endl;
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

//                         if((k==5)&&(p==(Nbaseline-1))&& value>600)
//                         {
//                             std::cout<<sbIter.Get()<<" ("<<sbIter.GetIndex()<<")";
//                         }

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

    std::cout<<"Init recorregut max: "<<std::endl;
    std::ofstream fout("DSC.dat", ios::out);
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
//                     if((i==51)&&(j==65)&&(k==6))
//                     {
//                         fout<<value<<std::endl;
//                     }

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
    fout.close();

    /*maxIter.GoToBegin();
    minIter.GoToBegin();
    mapIter.GoToBegin();
    sbIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>(minIter.Get()+300))
                {
                    //std::cout<<sbIter.Get()<<" -> ("<<maxIter.Get()<<","<<minIter.Get()<<")"<<std::endl;
                    mapIter.Set(220);
                }
                else
                {
                    mapIter.Set(0);
                }
                ++maxIter;
                ++minIter;
                ++mapIter;
                ++sbIter;
            }
        }
    }*/

    std::cout<<"End recorregut3: "<<std::endl;
    DSCiter.GoToBegin();
    sbIter.GoToBegin();
    rCBVIter.GoToBegin();
    mapIter2.GoToBegin();
    maxIter.GoToBegin();
    minIter.GoToBegin();
    m_mapMax=0;
    m_mapMin=10000;
    m_maxValue=0;
    m_minValue=0;//Ja sabem que el minim serà 0
    for (k=0;k<kend;k++)
    {
        initialsbIter=sbIter;
        initialrCBVIter=rCBVIter;
        initialMapIter2 = mapIter2;
        initialMaxIter = maxIter;
        initialMinIter = minIter;
        for (p=0;p<pend;p++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    //if(maxIter.Get()>(minIter.Get()+200))//Out of the brain-> empirical values!first crit.-> background
                    if(maxIter.Get()<=700)//Out of the brain-> empirical values!first crit.-> background
                    {
                        //std::cout<<"*";
                        rCBVIter.Set(0.0);
                        mapIter2.Set(0);
                    }
                    else
                    {
                        if(sbIter.Get()!=0 && DSCiter.Get()!=0)
                        {
                            valuerCBV=rCBVIter.Get();
                            rCBVIter.Set(valuerCBV - log(DSCiter.Get()/sbIter.Get())/TEdyn);
                            mapIter2.Set((int)rCBVIter.Get());
                            if(mapIter2.Get()>m_maxValue) m_maxValue=mapIter2.Get();
/*                            if((k==5)&&(p==pend-1))
                            {
                                //std::cout<<valuerCBV - log(DSCiter.Get()/sbIter.Get())/TEdyn<<" ("<<sbIter.Get()<<")"<<std::endl;
                            }*/
                            //std::cout<<std::endl<<k<<" "<<p;
                        }
                        else
                        {
                            rCBVIter.Set(0.0);
                            mapIter2.Set(0);
/*                            if((k==5)&&(p==pend-1))
                            {
                                std::cout<<"+"<<" ("<<sbIter.GetIndex()<<")";
                            }*/
                        }
                    }
                    /*if(p=pend-1)
                    {
                        if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
                        {
                            //mapIter.Set((int)sbIter.Get());
                            mapIter.Set((int)rCBVIter.Get());
                            std::cout<<"+"<<" ("<<mapIter.Get()<<")";
                        }
                        else
                        {
                            mapIter.Set(0);
                        }
                        //std::cout<<mapIter.Get()<<std::endl;
                        if(mapIter.Get()>m_mapMax)
                        {
                            m_mapMax=mapIter.Get();
                        }

                        if(mapIter.Get()<m_mapMin)
                        {
                            m_mapMin=mapIter.Get();
                        }
                    }*/
                    ++DSCiter;
                    ++sbIter;
                    ++mapIter2;
                    ++maxIter;
                    ++minIter;
                    ++rCBVIter;
                }
            }
            if(p!=pend-1)
            {
                sbIter=initialsbIter;
                mapIter2=initialMapIter2;
                rCBVIter=initialrCBVIter;
                maxIter=initialMaxIter;
                minIter=initialMinIter;
            }
        }
    }

//     writerType::Pointer mapWriter5 = writerType::New();
//     mapWriter5->SetFileName("rCBVImage.mhd");
//     mapWriter5->SetInput(mapImage);
//     mapWriter5->Update();


    rCBVIter.GoToBegin();
    maxIter.GoToBegin();
    double sum=0.0;
    int contCBV=0;
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
                {
                    sum += rCBVIter.Get();
                    contCBV++;
                }
                ++maxIter;
                ++rCBVIter;
            }
        }
    }
    double mean=sum/(bloodvolfraction*(double)contCBV);
    std::cout<<"Mean: "<<mean<<std::endl;
    rCBVIter.GoToBegin();
    psiIter.GoToBegin();
    maxIter.GoToBegin();
    mapIter.GoToBegin();
    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(maxIter.Get()>700)//Out of the brain-> empirical values!first crit.-> background
                {
                    if(rCBVIter.Get()>0)
                    {
                        //mapIter.Set((int)sbIter.Get());
                        //psiIter.Set((rCBVIter.Get()/mean));
                        psiIter.Set(rCBVIter.Get());
                        mapIter.Set((int)(rCBVIter.Get()));
                        //std::cout<<"+"<<" ("<<psiIter.Get()<<")";
                    }
                    else
                    {
                        psiIter.Set(0);
                        mapIter.Set(0);
                    }
                }
                else
                {
                    psiIter.Set(0);
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
                ++psiIter;
                ++mapIter;
                ++maxIter;
                ++rCBVIter;
            }
        }
    }
//     writerType::Pointer mapWriter4 = writerType::New();
//     mapWriter4->SetFileName("psiImage.mhd");
//     mapWriter4->SetInput(mapImage);
//     mapWriter4->Update();

    std::cout<<"End recorregut5: "<<std::endl;
    //Calculem la mitjana dels valors del pre
    preIter.GoToBegin();
    presbIter.GoToBegin();
    postsbIter.GoToBegin();
    postIter.GoToBegin();
    mapIter.GoToBegin();
    kend=m_SEPreVolume->getSeries()->getNumberOfSlicesPerPhase();
    pend=m_SEPreVolume->getSeries()->getNumberOfPhases();
    std::cout<<"End getSlices: "<<pend<<" "<<Nbaseline<<" "<<m_SEPostVolume->getSeries()->getNumberOfPhases()<<"!!!"<<std::endl;
    for (k=0;k<kend;k++)
    {
        initialMapIter = mapIter;
        initialpresbIter = presbIter;
        initialpostsbIter = postsbIter;
        for (p=0;p<pend;p++)
        {
            for (j=0;j<size[1];j++)
            {
                for (i=0;i<size[0];i++)
                {
                    value = preIter.Get();
                    valuesb = presbIter.Get();
                    //Calculem la mitjana de totes les series valors
                    presbIter.Set(valuesb + ((double)value)/pend);
                    //mapIter.Set((int)presbIter.Get());
                    value = postIter.Get();
                    valuesb = postsbIter.Get();
                    //Calculem la mitjana de totes les series valors
                    postsbIter.Set(valuesb + ((double)value)/pend);
                    mapIter.Set((int)(presbIter.Get()-postsbIter.Get()));
                    ++preIter;
                    ++presbIter;
                    ++postIter;
                    ++postsbIter;
                    ++mapIter;
                }
            }
            if(p!=pend-1)
            {
                mapIter = initialMapIter;
                presbIter=initialpresbIter;
                postsbIter=initialpostsbIter;
            }
        }
    }
//     writerType::Pointer mapWriter3 = writerType::New();
//     mapWriter3->SetFileName("difsbImage.mhd");
//     mapWriter3->SetInput(mapImage);
//     mapWriter3->Update();

    kend=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    pend=m_DSCVolume->getSeries()->getNumberOfPhases();
    std::cout<<"End recorregut6: "<<std::endl;
    std::cout<<"Init recorregut7: ["<<size[0]<<", "<<size[1]<<", "<<pend - m_SEPostVolume->getSeries()->getNumberOfPhases()<<" ("<<pend<<"), "<<kend<<"]"<<m_SEPreVolume->getSeries()->getNumberOfPhases()<<std::endl;
    DSCiter.GoToBegin();
    preGEIter.GoToBegin();
    postGEIter.GoToBegin();
    mapIter.GoToBegin();
    std::cout<<"End getSlices: "<<pend<<" "<<Nbaseline<<" "<<m_SEPostVolume->getSeries()->getNumberOfPhases()<<"!!!"<<std::endl;
    for (k=0;k<kend;k++)
    {
        initialMapIter = mapIter;
        initialpreGEIter = preGEIter;
        initialpostGEIter = postGEIter;
        for (p=0;p<pend;p++)
        {
            if(p<(unsigned int)m_SEPreVolume->getSeries()->getNumberOfPhases())//per fer les mitjanes entre els mateixos valors que l'SE
            {
                for (j=0;j<size[1];j++)
                {
                    for (i=0;i<size[0];i++)
                    {
                        value = DSCiter.Get();
                        valuesb = preGEIter.Get();
                        //Calculem la mitjana de totes les series valors
                        preGEIter.Set(valuesb + ((double)value)/m_SEPreVolume->getSeries()->getNumberOfPhases());
                        //mapIter.Set((int)presbIter.Get());
                        ++DSCiter;
                        ++preGEIter;
                        ++postGEIter;
                        ++mapIter;
                    }
                }
            }else if(p>=pend-m_SEPreVolume->getSeries()->getNumberOfPhases()){
                for (j=0;j<size[1];j++)
                {
                    for (i=0;i<size[0];i++)
                    {
                        value = DSCiter.Get();
                        valuesb = postGEIter.Get();
                        //Calculem la mitjana de totes les series valors
                        postGEIter.Set(valuesb + ((double)value)/m_SEPreVolume->getSeries()->getNumberOfPhases());
                        mapIter.Set((int)(preGEIter.Get()-postGEIter.Get()));
                        ++DSCiter;
                        ++preGEIter;
                        ++postGEIter;
                        ++mapIter;
                    }
                }

            }else{
                for (j=0;j<size[1];j++)
                {
                    for (i=0;i<size[0];i++)
                    {
                        ++DSCiter;
                        ++preGEIter;
                        ++postGEIter;
                        ++mapIter;
                    }
                }
            }
            if(p!=pend-1)
            {
                mapIter = initialMapIter;
                preGEIter=initialpreGEIter;
                postGEIter=initialpostGEIter;
            }
        }
    }
//     writerType::Pointer mapWriter6 = writerType::New();
//     mapWriter6->SetFileName("difGEImage.mhd");
//     mapWriter6->SetInput(mapImage);
//     mapWriter6->Update();


    postsbIter.GoToBegin();
    presbIter.GoToBegin();
    postGEIter.GoToBegin();
    preGEIter.GoToBegin();
    rSEMeanIter.GoToBegin();
    rGEMeanIter.GoToBegin();
    mapIter.GoToBegin();
    int rsecont=0;
    int rgecont=0;
    double maxGE=-1000;
    double minGE=1000;

    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if( postGEIter.Get()!=0 && preGEIter.Get()!=0){
                    rGEMeanIter.Set(-1.0*log(postGEIter.Get()/preGEIter.Get())/TEdyn);
                    //std::cout<<"("<<postGEIter.Get()<<","<<preGEIter.Get()<<","<<rGEMeanIter.Get()<<")";
                    if(rGEMeanIter.Get()>0) rgecont++;
                    if(rGEMeanIter.Get()>maxGE) maxGE=rGEMeanIter.Get();
                    if(rGEMeanIter.Get()<minGE) minGE=rGEMeanIter.Get();
                }else{
                    rGEMeanIter.Set(0.0);
                }
                /*meanGE=rGEMeanIter.Get();
                rGEMeanIter.Set(meanGE/(double)m_SEPostVolume->getSeries()->getNumberOfPhases());*/

                if( postsbIter.Get()!=0 && presbIter.Get()!=0){
                    rSEMeanIter.Set(-1.0*log(postsbIter.Get()/presbIter.Get())/TEdyn);
                    //std::cout<<"("<<postsbIter.Get()<<","<<presbIter.Get()<<","<<rSEMeanIter.Get()<<")";
                    if(rSEMeanIter.Get()>0) rsecont++;
                }else{
                    rSEMeanIter.Set(0.0);
                }
                /*meanSE=rSEMeanIter.Get();
                rSEMeanIter.Set(meanSE/(double)m_SEPostVolume->getSeries()->getNumberOfPhases());*/
                mapIter.Set((int)rGEMeanIter.Get());
                ++postsbIter;
                ++presbIter;
                ++postGEIter;
                ++preGEIter;
                ++rSEMeanIter;
                ++rGEMeanIter;
                ++mapIter;
            }
        }
    }
//     writerType::Pointer mapWriter2 = writerType::New();
//     mapWriter2->SetFileName("rGEMeanImage.mhd");
//     mapWriter2->SetInput(mapImage);
//     mapWriter2->Update();

    std::cout<<std::endl<<"End recorregut7: rse="<<rsecont<<" // rge="<<rgecont<<std::endl;
    std::cout<<"End recorregut7: min="<<minGE<<" // max="<<maxGE<<std::endl;
    std::cout<<"Init recorregut8: ["<<size[0]<<", "<<size[1]<<", "<<pend - m_SEPostVolume->getSeries()->getNumberOfPhases()<<" ("<<pend<<"), "<<kend<<"]"<<std::endl;
    psiIter.GoToBegin();
    rSEMeanIter.GoToBegin();
    rGEMeanIter.GoToBegin();
    mapIter.GoToBegin();
    m_mapMax=-1000;
    m_mapMin=1000;
    //m_maxValue=-1000;
    //m_minValue=1000;
    int vsicont=0;
    int vsicont0=0;
    //double mult = 10.0;
    double mult = 100*m_multiplierSpinBox->value();

    for (k=0;k<kend;k++)
    {
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                if(psiIter.Get()!=0)
                {
                    if(rSEMeanIter.Get()>0 && rGEMeanIter.Get()>0 )//&& rGEMeanIter.Get()<rSEMeanIter.Get())
                    {
                        //Compt multiplicat per mult per millor visulaització
                        mapIter.Set(mult*1.736*sqrt((psiIter.Get()/mean)*ADC)*(rGEMeanIter.Get()/pow(rSEMeanIter.Get(),1.5f)));
                        //std::cout<<"+"<<" ("<<mapIter.Get()<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)*(rGEMeanIter.Get()/pow(rSEMeanIter.Get(),1.5))<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)<<" , "<<rGEMeanIter.Get()<<","<<rSEMeanIter.Get()<<","<<pow(rSEMeanIter.Get(),1.5)<<")";
                        if(mapIter.Get()>500){
                            //std::cout<<std::endl<<"-"<<" ("<<mapIter.Get()<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)*(rGEMeanIter.Get()/pow(rSEMeanIter.Get(),1.5))<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)<<" , "<<rGEMeanIter.Get()<<","<<rSEMeanIter.Get()<<","<<pow(rSEMeanIter.Get(),1.5)<<")";
                            mapIter.Set(500);
                        }
                        if(mapIter.Get()<0){
                            //std::cout<<std::endl<<"-"<<" ("<<mapIter.Get()<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)*(rGEMeanIter.Get()/pow(rSEMeanIter.Get(),1.5))<<")"<<" ("<<1.736*sqrt(psiIter.Get()*ADC)<<" , "<<rGEMeanIter.Get()<<","<<rSEMeanIter.Get()<<","<<pow(rSEMeanIter.Get(),1.5)<<")";
                            mapIter.Set(200);
                        }
                        vsicont++;
                    }
                    else
                    {
                        vsicont0++;
                        //posem a petit, però que ho pinti de color
                        mapIter.Set(0.0);
                    }
                }
                else
                {
                    mapIter.Set(0.0);
                }

                if(mapIter.Get()>m_mapMax)
                {
                    m_mapMax=mapIter.Get();
                }

                if(mapIter.Get()<m_mapMin)
                {
                    m_mapMin=mapIter.Get();
                }
                    //if ( value < m_minValue ) { m_minValue = value; }
                    //if ( value > m_maxValue ) { m_maxValue = value; }

                ++psiIter;
                ++mapIter;
                ++rSEMeanIter;
                ++rGEMeanIter;
            }
        }
    }

    std::cout<<"End recorregut 8"<<std::endl;
    std::cout<<"Min:"<<m_mapMin<<" // Max:"<<m_mapMax<<std::endl;
    std::cout<<"Max Value: "<<m_maxValue<<", Min Value: "<<m_minValue<<std::endl;
    std::cout<<"cont: "<<vsicont<<" // cont0: "<<vsicont0<<std::endl;


    if(m_mapVolume!=0)
    {
        delete m_mapVolume;
    }
    m_mapVolume = new Volume();
    m_mapVolume->setImages( m_mainVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume->setData(mapImage);

    if(m_mapVolume2!=0)
    {
        delete m_mapVolume2;
    }
    m_mapVolume2 = new Volume();
    m_mapVolume2->setImages( m_mainVolume->getPhaseVolume(0)->getImages() );
    m_mapVolume2->setData(mapImage2);

/*    writerType::Pointer mapWriter = writerType::New();
    mapWriter->SetFileName("VSImap.mhd");
    mapWriter->SetInput(mapImage);
    mapWriter->Update();*/
    std::cout<<"End new map "<<std::endl;

    // \TODO ara ho fem "a saco" per?s'hauria de millorar
    m_2DView->setInput( m_mapVolume );
    m_2DView->resetView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::AllAnnotation);
    m_2DView->resetWindowLevelToDefault();
    this->createColorMap( );
    std::cout<<"End new map "<<std::endl;
    m_2DView_4->setInput( m_mapVolume2 );
    m_2DView_4->resetView( Q2DViewer::Axial );
    m_2DView_4->removeAnnotation(Q2DViewer::AllAnnotation);
    m_2DView_4->resetWindowLevelToDefault();
    this->createColorMap2( );
    std::cout<<"End new map "<<std::endl;

    m_2DView_2->setInput( m_DSCVolume );
    m_2DView_2->resetView( Q2DViewer::Axial );
    m_2DView_2->removeAnnotation(Q2DViewer::AllAnnotation);
    m_2DView_2->resetWindowLevelToDefault();

    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase()-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase()-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    int* ext= m_mapVolume->getWholeExtent();
    std::cout<<"["<<ext[0]<<","<<ext[1]<<";"<<ext[2]<<","<<ext[3]<<";"<<ext[4]<<","<<ext[5]<<"]"<<std::endl;
    std::cout<<"Number of voxels:"<<cont<<std::endl;
    std::cout<<"End computeVSI!!!"<<std::endl;

}


void QVSIReconstructionExtension::vsiEventHandler( unsigned long id )
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

void QVSIReconstructionExtension::leftButtonEventHandler( )
{
    m_isLeftButtonPressed = true;
}


void QVSIReconstructionExtension::setLeftButtonOff( )
{
    m_isLeftButtonPressed = false;
}


void QVSIReconstructionExtension::contextMenuDSCRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    // map to global
    //QPoint globalPoint = this->mapToGlobal( point );
//     emit showContextMenu( globalPoint );
    m_imageVSItype = QVSIReconstructionExtension::DSC;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QVSIReconstructionExtension::contextMenuSEPreRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    // map to global
    //QPoint globalPoint = this->mapToGlobal( point );
//     emit showContextMenu( globalPoint );
    m_imageVSItype = QVSIReconstructionExtension::SEPre;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QVSIReconstructionExtension::contextMenuSEPostRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPositionX = 0;//this->getEventPositionX();
    int eventPositionY = 0;//this->getEventPositionY();

    QPoint point = QPoint( eventPositionX, eventPositionY );

    // map to global
    //QPoint globalPoint = this->mapToGlobal( point );
//     emit showContextMenu( globalPoint );
    m_imageVSItype = QVSIReconstructionExtension::SEPost;

    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point) );
}

void QVSIReconstructionExtension::contextMenuEvent(QContextMenuEvent *event)
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

void QVSIReconstructionExtension::setSeries(Series *series)
{
    QString modality = series->getModality();
    if( modality == "KO" || modality == "PR" || modality == "SR" )
    {
        QMessageBox::information( this , tr( "Viewer" ) , tr( "The selected item is not a valid image format" ) );
    }
    else
    {
        switch(m_imageVSItype)
        {
        case DSC:
            m_DSCLineEdit->clear();
            m_DSCLineEdit->insert(series->getDescription());
            m_DSCVolume = series->getFirstVolume();
            break;
        case SEPre:
            m_SEPreLineEdit->clear();
            m_SEPreLineEdit->insert(series->getDescription());
            m_SEPreVolume = series->getFirstVolume();
            break;
        case SEPost:
            m_SEPostLineEdit->clear();
            m_SEPostLineEdit->insert(series->getDescription());
            m_SEPostVolume = series->getFirstVolume();
            break;
        default:
            DEBUG_LOG("No existeix aquest tipus d'imatge!!");
            break;
        }
    }
}


void QVSIReconstructionExtension::readSettings()
{
    QSettings settings("GGG", "StarViewer-App-VSIReconstruction");
    settings.beginGroup("StarViewer-App-VSIReconstruction");

    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );
    m_verticalImageSplitter->restoreState( settings.value("verticalImageSplitter").toByteArray() );
    m_horizontalImageSplitter->restoreState( settings.value("horizontalImageSplitter").toByteArray() );
    settings.endGroup();
}

void QVSIReconstructionExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-VSIReconstruction");
    settings.beginGroup("StarViewer-App-VSIReconstruction");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );
    settings.setValue("horizontalImageSplitter", m_horizontalImageSplitter->saveState() );
    settings.setValue("verticalImageSplitter", m_verticalImageSplitter->saveState() );

    settings.endGroup();
}

}
