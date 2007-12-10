/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qsegmentationframeworkextension.h"
#include "volume.h"
#include "logging.h"
#include "toolsactionfactory.h"
#include "qwindowlevelcombobox.h"
#include "q2dviewer.h"

#include "connectedthreshold.h"
#include "isolatedconnected.h"
#include "neighborhoodconnected.h"
#include "confidenceconnected.h"
#include "volumcalculator.h"
#include "contourntool.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>

// VTK
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkSplineWidget.h>
#include <vtkPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkTriangleStrip.h>
#include <vtkQuadricDecimation.h>
#include <vtkImageData.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
//#include <vtkContourRepresentation.h>
//#include <vtkContourWidget.h>
//#include <vtkSplineFilter.h>
//#include <vtkContourWidget.h>

// ITK
#include <itkImage.h>

#include <iostream.h>


namespace udg {


QSegmentationFrameworkExtension::QSegmentationFrameworkExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;
    m_maskVolume = new Volume();
    //m_maskVolume = 0;
    m_isSeed=false;
    m_isMask=false;
    m_isCont=false;
    m_isFirst=true;
   // m_seed = new QTableWidget(5, 3, this);
    m_seed->setRowCount(5);
    m_seed->setColumnCount(3);
    m_howManySeeds=0;
    m_outSeed=false;
    m_volumCalculatorX =0;
    m_volumCalculatorZ=0;
    m_volumCalculatorY=0;
    m_method=0;
    m_methodNC=0;
    m_methodIC=0;
    m_methodCC=0;
    //m_contourn=0;
    createActions();
    createConnections();
}

QSegmentationFrameworkExtension::~QSegmentationFrameworkExtension()
{


    if(m_maskVolume != 0)
    {
        delete m_maskVolume;
    }
    if (m_volumCalculatorX!= 0){
    delete m_volumCalculatorX;
    delete m_volumCalculatorZ;
    delete m_volumCalculatorY;
    }
    if(m_method != 0)
    {
        delete m_method;
    }
    if(m_methodIC != 0)
    {
        delete m_methodIC;
    }
    if(m_methodNC != 0)
    {
        delete m_methodNC;
    }
    if(m_methodCC != 0)
    {
        delete m_methodCC;
    }
/*    if(m_contourn != 0)
    {
        delete m_contourn;
    }
*/


}

void QSegmentationFrameworkExtension::createActions()
{
///TOOLS QUE ENS PERMETEN CANVIAR DE VISTES AXIAL; SAGITAL; CORONAL.
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("&Axial View") );
    m_axialViewAction->setShortcut( tr("Ctrl+A") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    m_axialViewToolButton->setDefaultAction( m_axialViewAction );

    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("&Sagital View") );
    m_sagitalViewAction->setShortcut( tr("Ctrl+S") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Saggital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );

    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("&Coronal View") );
    m_coronalViewAction->setShortcut( tr("Ctrl+C") );
    m_coronalViewAction->setStatusTip( tr("Change Current View To Coronal") );
    m_coronalViewAction->setIcon( QIcon(":/images/coronal.png") );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );



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

    // m_contournAction = new ContournTool( m_2DView , m_maskVolume );
    m_contournAction = new QAction( 0 );
    m_contournAction->setText( tr("CONTORN") );
    m_contournAction->setStatusTip( tr("Enable/Disable Contourn tool") );
    m_contournAction->setIcon( QIcon(":/images/distance.png") );
    m_contournAction->setCheckable( true );
   // m_contorn = new ContournTool( m_2DView , m_maskVolume );
    //m_signalMapper->setMapping( m_contournAction , "ContournTool" );
    //connect( m_contournAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    //m_contournAction = new ContournTool( m_2DView , m_maskVolume );
    //m_contournAction = m_actionFactory->getActionFrom( "ContournTool" );
    //m_contournAction = m_contorn;
    m_contournToolButton->setDefaultAction( m_contournAction );
    m_contournToolButton->setEnabled( false );
    //connect( m_contournAction , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );



    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_seedAction );
    m_toolsActionGroup->addAction( m_contournAction );
    //m_toolsActionGroup->addAction( m_editorAction );
    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_seedAction->trigger();


}

void QSegmentationFrameworkExtension::createToolBars()
{
}

void QSegmentationFrameworkExtension::createConnections()
{
    connect(m_ApplyCT,SIGNAL(clicked()), SLOT(applyCT()));
    connect(m_ApplyIC,SIGNAL(clicked()), SLOT(applyIC()));
    connect(m_ApplyCC,SIGNAL(clicked()), SLOT(applyCC()));
    connect(m_ApplyNC,SIGNAL(clicked()), SLOT(applyNC()));

    connect(m_clear,SIGNAL(clicked()), SLOT(clearSelected()));
    connect(m_clearAll,SIGNAL(clicked()), SLOT(clearAll()));

    connect(m_selectOutSeed,SIGNAL(clicked()), SLOT(SelectOutSeed()));
    connect(m_sliceViewSlider, SIGNAL(valueChanged(int)), SLOT(setAreaSlice(int)));
    connect(m_opacity, SIGNAL(valueChanged(int)), SLOT(opacityChanged(int)));
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_2DView , SIGNAL( eventReceived( unsigned long ) ), SLOT( strokeEventHandler(unsigned long) ) );
    connect( m_axialViewAction , SIGNAL( triggered() ), SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ), SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ), SLOT( changeViewToCoronal() ) );
    connect( m_contournAction , SIGNAL( triggered() ), SLOT( calculateContorn() ) );
    connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView, SIGNAL( seedChanged() ), SLOT( setSeedPosition() ) );
    connect( m_2DView, SIGNAL(sliceChanged(int)), m_sliceViewSlider , SLOT (setValue(int)));
    connect(m_reset,SIGNAL(clicked()), SLOT(reset()));
}

void QSegmentationFrameworkExtension::setInput( Volume *input )
{
    m_mainVolume = new Volume();
    m_mainVolume = input;


    m_2DView->setInput( m_mainVolume );

    m_2DView->setView( Q2DViewer::Axial );
    m_2DView->removeAnnotation(Q2DViewer::NoAnnotation);
    m_2DView->resetWindowLevelToDefault();
    m_2DView->setOverlayToBlend();
    m_currentView = Axial;

    int* dim;
    dim = m_mainVolume->getDimensions();
    m_2DView->setSlice((dim[2]-1)/2);
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());
    //m_sliceViewSlider->setValue((dim[2]-1)/2);

    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );

    /*typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
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
    }*/
    /*
    vtkPolyData *poly=vtkPolyData::New();
    vtkPolyData *newpoly=vtkPolyData::New();

    vtkTriangleFilter *tri=vtkTriangleFilter::New();
    vtkSplineWidget *m_spline = vtkSplineWidget::New();
    //vtkTriangleStrip *Strip=vtkTriangleStrip::New();
    vtkQuadricDecimation *qua=vtkQuadricDecimation::New();

    m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DView->getInteractor());
    m_spline->ProjectToPlaneOn();
//    prop->SetPointSize(0.05);
    //prop->SetOpacity(0.2);
  //  prop->SetColor(1,0,0);
    //m_spline->SetHandleProperty(prop);
    m_spline->SetProjectionPosition(m_2DView->getCurrentSlice()-1);
    m_spline->SetProjectionNormal(2);
    m_spline->SetNumberOfHandles(5);
    m_spline->SetHandlePosition(0,54,161,0);
    m_spline->SetHandlePosition(1,127,159,0);
    m_spline->SetHandlePosition(2,140,107,0);
    m_spline->SetHandlePosition(3,111,51,0);
    m_spline->SetHandlePosition(4,38,57,0);
    m_spline->On();
    m_spline->ClosedOn();

    m_spline->GetPolyData(poly);
    tri->SetInput(poly);
//tri->SetInput(Strip);
    tri->Update();
    //qua->SetInput(tri->GetOutput());
    //qua->Update();
    newpoly=tri->GetOutput();
    //int i=newpoly->GetDataObjectType();
    newpoly->BuildLinks();
    int i,j;
    j=newpoly->GetNumberOfVerts();
    i= newpoly->IsTriangle(0,3,2);
    std::cout<<"la i és :"<<i<<std::endl;
    std::cout<<"la j és :"<<j<<std::endl;*/
    //newpoly->SetInteractor(m_2DView->getInteractor());
    m_2DView->render();




}

void QSegmentationFrameworkExtension::changeViewToAxial()
{
    m_currentView = Axial;

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_sliceSpinBox->setMinimum( extent[4] );
    m_sliceSpinBox->setMaximum( extent[5] );
    m_sliceViewSlider->setMaximum( extent[5] );
    m_2DView->setViewToAxial();
    m_2DView->setSlice(extent[5]/2);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    m_2DView->render();

}

void QSegmentationFrameworkExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_sliceSpinBox->setMinimum( extent[0] );
    m_sliceSpinBox->setMaximum( extent[1] );
    m_sliceViewSlider->setMaximum( extent[1] );
    m_2DView->setViewToSagittal();
    m_2DView->setSlice(extent[1]/2);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    m_2DView->render();
}

void QSegmentationFrameworkExtension::changeViewToCoronal()
{

    m_currentView = Coronal;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );


    m_sliceSpinBox->setMinimum( extent[2] );
    m_sliceSpinBox->setMaximum( extent[3] );
    m_sliceViewSlider->setMaximum( extent[3] );
    m_2DView->setViewToCoronal();
    m_2DView->setSlice(extent[3]/2);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    m_2DView->render();

}

void QSegmentationFrameworkExtension::setView( ViewType view )
{
    switch( view )
    {
    case Axial:
        changeViewToAxial();
    break;
    case Sagital:
        changeViewToSagital();
    break;
    case Coronal:
        changeViewToCoronal();
    break;
    }
}






void QSegmentationFrameworkExtension::SelectOutSeed()
{

m_outSeed=true;

}

void QSegmentationFrameworkExtension::setSeedPosition( )
{
    double pos[3];
    QString aux;
    m_2DView->getSeedPosition(pos);

    if(m_outSeed){

    m_OutSeedX->clear();
    m_OutSeedY->clear();
    m_OutSeedZ->clear();

    aux = QString("%1").arg(pos[0], 0, 'f', 1);
    m_OutSeedX->insert(aux);
    aux = QString("%1").arg(pos[1], 0, 'f', 1);
    m_OutSeedY->insert(aux);
    aux = QString("%1").arg(pos[2], 0, 'f', 1);
    m_OutSeedZ->insert(aux);


    m_outSeed=false;
    m_ApplyIC->setEnabled(true);
    //m_selectOutSeed->setEnabled(false);

    }else{
        QTableWidgetItem* X = new QTableWidgetItem();
        QTableWidgetItem* Y = new QTableWidgetItem();
        QTableWidgetItem* Z = new QTableWidgetItem();

        ///Li passem els paràmetres
        if (m_howManySeeds<=m_seed->rowCount()){


            aux = QString("%1").arg(pos[0], 0, 'f', 1);
            X->setText(aux);
            X->setTextAlignment(Qt::AlignHCenter);
            m_seed->setItem(m_howManySeeds,0,X);

            aux = QString("%1").arg(pos[1], 0, 'f', 1);
            Y->setText(aux);
            Y->setTextAlignment(Qt::AlignHCenter);
            m_seed->setItem(m_howManySeeds,1,Y);

            aux = QString("%1").arg(pos[2], 0, 'f', 1);
            Z->setText(aux);
            Z->setTextAlignment(Qt::AlignHCenter);
            m_seed->setItem(m_howManySeeds,2,Z);


            m_howManySeeds=m_howManySeeds+1;
            m_selectOutSeed->setEnabled(true);
            m_isSeed=true;
            m_selectOutSeed->setEnabled(true);

        }

        m_isSeed=true;
        if(m_isSeed)
        {
            m_ApplyCT->setEnabled(true);
            m_ApplyCC->setEnabled(true);
            m_ApplyNC->setEnabled(true);
        }
    }
}


void QSegmentationFrameworkExtension::applyCT()
{

            double x,y,z;
            QString strX, strY, strZ, strV;
            QTableWidgetItem* X = new QTableWidgetItem();
            QTableWidgetItem* Y = new QTableWidgetItem();
            QTableWidgetItem* Z = new QTableWidgetItem();
            m_method = new ConnectedThreshold();

            /// obtenim el volum a treballar

            m_method->setInputVolume( m_mainVolume );

	    ///Obtenim les llavors de la llista i les afegitm al m�tode


            for (int i=0;i<m_howManySeeds;i++){

                    X = m_seed->item(i,0);
                    Y = m_seed->item(i,1);
                    Z = m_seed->item(i,2);

                    x = ((X->text()).toDouble());
                    y = ((Y->text()).toDouble());
                    z = ((Z->text()).toDouble());


                    m_method->addSeed((int)x,(int)y,(int)z);

            }



            /// li indiquem al m�tode els par�metres necessaris
            m_method->setSmoothingParameters();

            m_method->setConnectedParameters( m_lower->value(),
	    				      m_upper->value()
                                     );
            ///Posem el cursor en espera!
            QApplication::setOverrideCursor(Qt::WaitCursor);

            ///Apliquem el m�tode
            if( m_method->applyMethod() )
            {

                ///Mostrem els resultats
                m_maskVolume=m_method->getSegmentedVolume();
                m_2DView->setOverlayToBlend();
                m_2DView->setOpacityOverlay(0.5);
                m_2DView->setOverlayInput(m_maskVolume);
                m_isMask=true;
                QApplication::restoreOverrideCursor();
                m_contournToolButton->setEnabled( true );
            }
            else
            {
                //std::cerr << " No s'ha pogut aplicar el m�tode de segmentaci� Connected " << std::endl;
            }

    VolumeCalculator();



}

void QSegmentationFrameworkExtension::applyIC()
{

            double x,y,z;
            int x2,y2,z2;
            QString strX, strY, strZ, strV;
            QTableWidgetItem* X = new QTableWidgetItem();
            QTableWidgetItem* Y = new QTableWidgetItem();
            QTableWidgetItem* Z = new QTableWidgetItem();
            m_methodIC = new IsolatedConnected();

            /// obtenim el volum a treballar

            m_methodIC->setInputVolume( m_mainVolume );

	    ///Obtenim les llavors de la llista i les afegitm al m�tode


            for (int i=0;i<m_howManySeeds;i++){

                    X = m_seed->item(i,0);
                    Y = m_seed->item(i,1);
                    Z = m_seed->item(i,2);

                    x = ((X->text()).toDouble());
                    y = ((Y->text()).toDouble());
                    z = ((Z->text()).toDouble());


                    m_methodIC->addSeed((int)x,(int)y,(int)z);

            }



            /// li indiquem al m�tode els par�metres necessaris
            m_methodIC->setSmoothingParameters();
            x2=(m_OutSeedX->text()).toInt();
            y2=(m_OutSeedY->text()).toInt();
            z2=(m_OutSeedZ->text()).toInt();
            /*m_methodIC->addSeed2( (m_OutSeedX->text()).toInt(),
                            (m_OutSeedY->text()).toInt(),
                            (m_OutSeedZ->text()).toInt() );*/
            m_methodIC->addSeed2(x2,y2,z2);

            m_methodIC->setIsolatedParameters( m_lowerIsolated->value() );
            ///Posem el cursor en espera!
            QApplication::setOverrideCursor(Qt::WaitCursor);

            ///Apliquem el m�tode
            if( m_methodIC->applyMethod() )
            {   //std::cout << "Arribem aqu�1?? " << std::endl;

                ///Mostrem els resultats
                m_maskVolume=m_methodIC->getSegmentedVolume();
                m_2DView->setOverlayToBlend();
                m_2DView->setOpacityOverlay(0.5);
                m_2DView->setOverlayInput(m_maskVolume);
                m_isMask=true;
                QApplication::restoreOverrideCursor();
                m_contournToolButton->setEnabled( true );
            }
            else
            {
                //std::cerr << " No s'ha pogut aplicar el m�tode de segmentaci� Isolated " << std::endl;
            }

           VolumeCalculator();


}

void QSegmentationFrameworkExtension::applyCC()
{

            double x,y,z;
            QString strX, strY, strZ, strV;
            QTableWidgetItem* X = new QTableWidgetItem();
            QTableWidgetItem* Y = new QTableWidgetItem();
            QTableWidgetItem* Z = new QTableWidgetItem();
            m_methodCC = new ConfidenceConnected();

            /// obtenim el volum a treballar

            m_methodCC->setInputVolume( m_mainVolume );

	    ///Obtenim les llavors de la llista i les afegitm al m�tode


            for (int i=0;i<m_howManySeeds;i++){

                    X = m_seed->item(i,0);
                    Y = m_seed->item(i,1);
                    Z = m_seed->item(i,2);

                    x = ((X->text()).toDouble());
                    y = ((Y->text()).toDouble());
                    z = ((Z->text()).toDouble());


                    m_methodCC->addSeed((int)x,(int)y,(int)z);

            }



            /// li indiquem al m�tode els par�metres necessaris
            m_methodCC->setSmoothingParameters();

            m_methodCC->setConfidenceParameters( m_radius->value(),
	    				       (float)m_multiplier->value(),
					       m_iterations->value()
                                             );
            ///Posem el cursor en espera!
            QApplication::setOverrideCursor(Qt::WaitCursor);

            ///Apliquem el m�tode
            if( m_methodCC->applyMethod() )
            {   //std::cout << "Arribem aqu�1?? " << std::endl;

                ///Mostrem els resultats
                m_maskVolume=m_methodCC->getSegmentedVolume();
                m_2DView->setOverlayToBlend();
                m_2DView->setOpacityOverlay(0.5);
                m_2DView->setOverlayInput(m_maskVolume);
                m_isMask=true;
                QApplication::restoreOverrideCursor();
                m_contournToolButton->setEnabled( true );
            }
            else
            {
                //std::cerr << " No s'ha pogut aplicar el m�tode de segmentaci� Confidence " << std::endl;
            }

          VolumeCalculator();

}


void QSegmentationFrameworkExtension::applyNC()
{

            double x,y,z;
            QString strX, strY, strZ, strV;
            QTableWidgetItem* X = new QTableWidgetItem();
            QTableWidgetItem* Y = new QTableWidgetItem();
            QTableWidgetItem* Z = new QTableWidgetItem();
            m_methodNC = new NeighborhoodConnected();

            /// obtenim el volum a treballar

            m_methodNC->setInputVolume( m_mainVolume );

	    ///Obtenim les llavors de la llista i les afegitm al m�tode


            for (int i=0;i<m_howManySeeds;i++){

                    X = m_seed->item(i,0);
                    Y = m_seed->item(i,1);
                    Z = m_seed->item(i,2);

                    x = ((X->text()).toDouble());
                    y = ((Y->text()).toDouble());
                    z = ((Z->text()).toDouble());


                    m_methodNC->addSeed((int)x,(int)y,(int)z);

            }



            /// li indiquem al m�tode els par�metres necessaris
            m_methodNC->setSmoothingParameters();

            m_methodNC->setNeighborhoodParameters( m_lowerNeig->value(),
	    				         m_upperNeig->value(),
                                                 m_radiusNeig->value()
                                                );
            ///Posem el cursor en espera!
            QApplication::setOverrideCursor(Qt::WaitCursor);

            ///Apliquem el m�tode
            if( m_methodNC->applyMethod() )
            {   //std::cout << "Arribem aqu�1?? " << std::endl;

                ///Mostrem els resultats
                m_maskVolume=m_methodNC->getSegmentedVolume();
                m_2DView->setOverlayToBlend();
                m_2DView->setOpacityOverlay(0.5);
                m_2DView->setOverlayInput(m_maskVolume);
                m_isMask=true;
                QApplication::restoreOverrideCursor();
                m_contournToolButton->setEnabled( true );
            }
            else
            {
                //std::cerr << " No s'ha pogut aplicar el m�tode de segmentaci� Neigborhood " << std::endl;
            }

            VolumeCalculator();

}


void QSegmentationFrameworkExtension::clearAll()
{
QStringList llista;
llista <<"X"<<"Y"<<"Z";

m_seed->clear();
m_seed->setHorizontalHeaderLabels(llista);
m_isSeed=false;
m_howManySeeds=0;
m_ApplyCT->setEnabled(false);
m_ApplyCC->setEnabled(false);
m_ApplyNC->setEnabled(false);
m_ApplyIC->setEnabled(false);



}

void QSegmentationFrameworkExtension::clearSelected()
{
bool trobat =false;
int i=0;

while((!trobat)&&(i<m_howManySeeds)){



         if(m_seed->isItemSelected((m_seed->item(i,0))))
         {

            m_seed->removeRow(i);
            if(m_howManySeeds>m_seed->rowCount())
            {
                m_howManySeeds=m_seed->rowCount();
                m_seed->insertRow(m_seed->rowCount());
            }else{
                m_seed->insertRow(m_seed->rowCount());
                m_howManySeeds=m_howManySeeds-1;
            }

           trobat=true;

    }
    i++;

}



}

void QSegmentationFrameworkExtension::VolumeCalculator()
{

    char* tempchar = new char[20];

    float res;

    /** *********CALCUL DEL VOLUM***********   */


    m_spacing[0] = ((ImageType*)(m_mainVolume->getItkData()))->GetSpacing()[0];//x
    m_spacing[1] = ((ImageType*)(m_mainVolume->getItkData()))->GetSpacing()[1];//y
    m_spacing[2] = ((ImageType*)(m_mainVolume->getItkData()))->GetSpacing()[2];//z


    m_volumCalculatorY = new VolumCalculator((ImageType*)(m_maskVolume->getItkData()),0);
    m_volumCalculatorZ = new VolumCalculator((ImageType*)(m_maskVolume->getItkData()),1);
    m_volumCalculatorX = new VolumCalculator((ImageType*)(m_maskVolume->getItkData()),2);



    sprintf(tempchar,"%.2f",m_volumCalculatorX->GetPixelS()*(m_spacing[0]*m_spacing[1]*m_spacing[2]));//el volum
    m_totalVolume->setText(tr(tempchar));//atencio

        switch( m_currentView )
            {
            case Axial:
                res = m_spacing[1]*m_spacing[2];
                sprintf(tempchar,"%.2f",m_volumCalculatorX->GetPixelSSli(m_sliceViewSlider->value())*res);//l'àrea de la llesca en mm quadrats
            break;
            case Sagital:
                res = m_spacing[0]*m_spacing[2];
	        sprintf(tempchar,"%.2f",m_volumCalculatorY->GetPixelSSli(m_sliceViewSlider->value())*res);
            break;
            case Coronal:
                res = m_spacing[0]*m_spacing[1];
	        sprintf(tempchar,"%.2f",m_volumCalculatorZ->GetPixelSSli(m_sliceViewSlider->value())*res);//el volum de la llesca en mm quadrats
            break;
            }


        m_sliceArea->setText(tr(tempchar));


       /** ***********FI CALCUL VOLUM*****************/




 }

void QSegmentationFrameworkExtension::setAreaSlice(int Sli)
{
    if(m_isMask){
    float res;
     char* tempchar = new char[20];

     switch( m_currentView )
            {
            case Axial:
                res = m_spacing[1]*m_spacing[2];
                sprintf(tempchar,"%.2f",m_volumCalculatorX->GetPixelSSli(Sli)*res);
            break;
            case Sagital:
                res = m_spacing[0]*m_spacing[2];
	        sprintf(tempchar,"%.2f",m_volumCalculatorY->GetPixelSSli(Sli)*res);
            break;
            case Coronal:
                res = m_spacing[0]*m_spacing[1];
	        sprintf(tempchar,"%.2f",m_volumCalculatorZ->GetPixelSSli(Sli)*res);
            break;
            }

        //std::cout<<"res és : "<<res<<std::endl;
        m_sliceArea->setText(tr(tempchar));
        if(m_isCont)
        {
        setSplineLength(m_contorn->getLength());
        }

    }

}

void QSegmentationFrameworkExtension::opacityChanged(int Op){

if (m_isMask){

m_2DView->setOpacityOverlay((double)Op/100.0);
m_2DView->setOverlayInput(m_maskVolume);
m_2DView->getInteractor()->Render();

}


}


/*void QSegmentationFrameworkExtension::Contorn( )
{


    float xt=((ImageType*)m_mainVolume->getItkData())->GetSpacing()[1];
    float yt=((ImageType*)m_mainVolume->getItkData())->GetSpacing()[2];

    m_spline = vtkSplineWidget::New();
    m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DView->getInteractor());
    m_spline->ProjectToPlaneOn();
    m_spline->SetNumberOfHandles(3);


    m_splineY = vtkSplineWidget::New();
    m_splineY->SetPriority(1.0);
    m_splineY->SetInteractor(m_2DView->getInteractor());
    m_splineY->ProjectToPlaneOn();
    m_splineY->SetNumberOfHandles(3);



    m_splineZ = vtkSplineWidget::New();
    m_splineZ->SetPriority(1.0);
    m_splineZ->SetInteractor(m_2DView->getInteractor());
    m_splineZ->ProjectToPlaneOn();
    m_splineZ->SetNumberOfHandles(3);


    switch( m_currentView )
            {
            case Axial:
            m_spline->SetProjectionPosition(m_2DView->getCurrentSlice()+1);
            m_spline->SetProjectionNormal(3);
            m_spline->SetHandlePosition(0,58*xt,133*yt,0);
            m_spline->SetHandlePosition(1,162*xt,77*yt,0);
            m_spline->SetHandlePosition(2,57*xt,33*yt,0);
            m_spline->ClosedOn();
            m_spline->On();
            m_splineZ->Off();
            m_splineY->Off();
            break;
            case Sagital:
            m_splineY->SetProjectionPosition(m_2DView->getCurrentSlice()+1);
            m_splineY->SetProjectionNormal(0);
            m_splineY->SetHandlePosition(0,0,58*xt,133*yt);
            m_splineY->SetHandlePosition(1,0,162*xt,77*yt);
            m_splineY->SetHandlePosition(2,0,57*xt,33*yt);
              m_splineY->ClosedOn();
            m_spline->Off();
            m_splineZ->Off();
            m_splineY->On();
            break;
            case Coronal:
            m_splineZ->SetProjectionPosition(m_2DView->getCurrentSlice()-1);
            m_splineZ->SetProjectionNormal(1);
            m_splineZ->SetHandlePosition(0,58*xt,0,133*yt);
            m_splineZ->SetHandlePosition(1,162*xt,0,77*yt);
            m_splineZ->SetHandlePosition(2,57*xt,0,33*yt);
            m_splineZ->ClosedOn();
            m_spline->Off();
            m_splineZ->On();
            m_splineY->Off();

            break;
            }



    m_2DView->getInteractor()->Render();
}
*/

void QSegmentationFrameworkExtension::calculateContorn( )
{
  if(!m_isCont){
  m_contorn = new ContournTool( m_2DView , m_maskVolume );
  connect( m_contorn , SIGNAL( actualspline(double ) ), SLOT( setSplineLength(double) ) );
  connect( m_contorn , SIGNAL( originalspline(double, int ) ), SLOT( setOriginalLength(double, int) ) );
  m_isCont=true;
  }

  m_reset->setEnabled(true);



}

void QSegmentationFrameworkExtension::setSplineLength(double area)
{
  double total=0;
  char* tempchar = new char[20];
  m_newSpline=0;

  if(m_originalSpline<area){

  m_newSpline=area-m_originalSpline;
  total=m_novaArea+m_newSpline;
  sprintf(tempchar,"%.2f",(total));//el volum de la llesca en mm quadrats
  m_newArea->setText(tr(tempchar));
  total=m_nouVolum+m_newSpline;
  sprintf(tempchar,"%.2f",(total));
  m_newVolume->setText(tr(tempchar));
  }
  else{

  m_newSpline=m_originalSpline-area;
  total=m_novaArea-m_newSpline;
  sprintf(tempchar,"%.2f",(total));//el volum de la llesca en mm quadrats
  m_newArea->setText(tr(tempchar));
  total=m_nouVolum-m_newSpline;
  sprintf(tempchar,"%.2f",(total));
  m_newVolume->setText(tr(tempchar));

  }

}

void QSegmentationFrameworkExtension::setOriginalLength(double area, int llesca)
{
    ///SI ho executem per primera vegada simplement posem els valors trobats en els resultats
    if(m_isFirst){
    m_newArea->setText(m_sliceArea->text());
    m_newVolume->setText(m_totalVolume->text());
    m_novaArea=m_sliceArea->text().toDouble();
    m_nouVolum=m_totalVolume->text().toDouble();
    m_originalSpline=area;
    m_llescaactual=llesca;
    m_isFirst=false;

    ///Si ho executem per 2ona o + vegades
    }else{


    if (llesca==m_llescaactual){
        if(m_originalSpline==area){
        ///Si és la mateixa llesca i la mateixa spline, el que fem és tronar als valors originals
            m_newArea->setText(m_sliceArea->text());
            m_newVolume->setText(m_totalVolume->text());
            m_novaArea=m_sliceArea->text().toDouble();
            m_nouVolum=m_totalVolume->text().toDouble();
        }else{
        ///Si és la mateixa llesca però no la mateixa spline, mantenim com a originals els valors que
        ///que hem obtingut del manipular el contorn anterior.
        m_originalSpline=area;
        m_novaArea=m_newArea->text().toDouble();
        m_nouVolum=m_newVolume->text().toDouble();

        }
    }
    else{
        ///Si canviem de llesca, només mantenim el valor del nou volum
        m_originalSpline=area;
        m_nouVolum=m_newVolume->text().toDouble();
        m_novaArea=m_sliceArea->text().toDouble();
        m_newArea->setText(m_sliceArea->text());
        m_llescaactual=llesca;

    }



    }


}


void QSegmentationFrameworkExtension::reset(){

m_newArea->setText(m_sliceArea->text());
m_newVolume->setText(m_totalVolume->text());
   // m_originalSpline=area;




}


void QSegmentationFrameworkExtension::Contorn(vtkImageData *segmentat){


    /*vtkContourFilter *skinExtractor = vtkContourFilter::New();
    skinExtractor->SetInput(segmentat);
    skinExtractor->SetNumberOfContours(1);
    skinExtractor->SetValue(0, 255);
    vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(60.0);*/

    /*vtkSplineFilter *spline = vtkSplineFilter::New();
    spline->SetInput(skinExtractor->GetOutput());
    vtkSplineWidget *widget = vtkSplineWidget::New();
    widget->SetInput(spline->GetOutput());*/
   // vtkStripper *skinStripper = vtkStripper::New();
    //skinStripper->SetInputConnection(skinNormals->GetOutputPort());
    //vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
    //vtkContourWidget *conWidget = vtkContourWidget::New();
    //conWidget->Initialize(skinExtractor->GetOutput());
    //vtkPolyData * poly= vtkPolyData::New();
    //widget->GetPolyData(poly);
    //skinMapper->SetInputConnection(skinStripper->GetOutputPort());
    //skinMapper->SetInput(poly);
    //skinMapper->SetInputConnection(skinExtractor->GetOutputPort());
    //vtkSplineWidget *widget = vtkSplineWidget::New();

    //skinMapper->SetInputConnection(skinNormals->GetOutputPort());
    //vtkPolyData * poly= vtkPolyData::New();
    //widget->GetPolyData(poly);

    //poly=skinMapper->GetInput();
    //int i=widget->GetNumberOfHandles();
    //std::cout<<"Tenim tants punts: "<<i<<endl;
    //skinMapper->ScalarVisibilityOff();
    /*vtkActor *skin = vtkActor::New();
    skin->SetMapper(skinMapper);
    skin->GetProperty()->SetDiffuseColor(1, .49, .25);
    skin->GetProperty()->SetSpecular(.3);
    skin->GetProperty()->SetSpecularPower(20);
    skin->VisibilityOn();

    m_2DView->getRenderer()-> AddActor( skin );
    m_2DView->getInteractor()->Render();*/
}





}
