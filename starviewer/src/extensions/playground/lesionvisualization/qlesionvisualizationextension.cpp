/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qlesionvisualizationextension.h"
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
#include <QFileDialog>

// VTK
#include <vtkRenderer.h>
#include <vtkImageMask.h>
#include <vtkProperty.h>
#include <vtkImageIterator.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMetaImageReader.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActorCollection.h>
#include <vtkPropPicker.h>

namespace udg {

QLesionVisualizationExtension::QLesionVisualizationExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_lesionMaskVolume(0), m_inside3Dviewer(false), m_maxLesion(-1)
{
    setupUi( this );

    // activem el framework de "Old Tools"
    // TODO cal reciclar per les noves tools
    m_2DView->enableOldTools();
    createActions();
    createConnections();
    readSettings();

    picker = vtkPropPicker::New();
    m_cylindersCollection = 0;
    m_lesion3Dviewer->getRenderer()->SetBackground( 0.5, 0.5, 0.5 );
}

QLesionVisualizationExtension::~QLesionVisualizationExtension()
{
    writeSettings();

    if(m_lesionMaskVolume)
        delete m_lesionMaskVolume;

    if ( m_maskLesionSlicesArea )
        delete m_maskLesionSlicesArea;

    if ( m_cylindersCollection )
        m_cylindersCollection->Delete();

    picker->Delete();
}

void QLesionVisualizationExtension::createActions()
{
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

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setOldTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
}

void QLesionVisualizationExtension::createConnections()
{
  connect( m_sliceViewSlider, SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
  connect( m_lesion3Dviewer, SIGNAL( eventReceived( unsigned long ) ), SLOT( eventHandler(unsigned long) ) );
  connect( m_opacitySlider, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
  connect( m_2DView, SIGNAL( volumeChanged(Volume *) ), SLOT( setInput( Volume * ) ) );
  connect( m_2DView, SIGNAL( sliceChanged(int) ), m_sliceViewSlider, SLOT( setValue( int ) ) );
  connect( m_2DView, SIGNAL( sliceChanged(int) ), this, SLOT( markCurrentCylinder( int ) ) );
  connect( m_openMaskPushButton, SIGNAL( clicked() ), SLOT( openMaskVolume() ) );
}

void QLesionVisualizationExtension::setInput( Volume *input )
{
    m_mainVolume = input;

    m_2DView->setInput( m_mainVolume );
    m_2DView->resetView( Q2DViewer::Axial );
    m_2DView->removeAnnotation( Q2DViewer::AllAnnotation );
    m_2DView->resetWindowLevelToDefault();
    m_2DView->setOverlayToBlend();

    int* dim = m_mainVolume->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());
    m_2DView->removeAnnotation( Q2DViewer::AllAnnotation );
    m_2DView->enableAnnotation( Q2DViewer::SliceAnnotation );
    m_numberOfSlices = m_2DView->getMaximumSlice()+1;

    m_2DView->render();
}

void QLesionVisualizationExtension::eventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        mouseEvent();
    break;

    case vtkCommand::LeftButtonPressEvent:
        setSlice();
    break;

    case vtkCommand::LeaveEvent:
        m_inside3Dviewer = false;
        break;

    case vtkCommand::EnterEvent:
        m_inside3Dviewer = true;
        break;

    default:
    break;
    }
}

void QLesionVisualizationExtension::setSlice()
{
    int *eventPosition = m_lesion3Dviewer->getEventPosition();
    if ( picker->PickProp ( eventPosition[0], eventPosition[1], m_lesion3Dviewer->getRenderer() ) )
    {
        vtkActor *actor = vtkActor::SafeDownCast( picker->GetViewProp() );

        bool notFound = true;
        int position;

        for ( int i = 0; i < m_cylindersCollection->GetNumberOfItems()&&notFound; i++)
        {
            vtkActor *current = vtkActor::SafeDownCast( m_cylindersCollection->GetItemAsObject( i ) );

            if ( current == actor )
            {
                notFound = false;
                position = i;
            }
        }
        m_2DView->setSlice( position );
    }
}

void QLesionVisualizationExtension::mouseEvent()
{
    if ( m_inside3Dviewer && m_cylindersCollection )
    {
        for ( int i = 0; i < m_cylindersCollection->GetNumberOfItems(); i++)
        {
            vtkActor *actor = vtkActor::SafeDownCast( m_cylindersCollection->GetItemAsObject( i ) );

            if ( m_maskLesionSlicesArea[i] == 0 )
                actor->GetProperty()->SetColor( 1. ,1. ,1. );
            else
            {
                actor->GetProperty()->SetColor( 1.-( (double)m_maskLesionSlicesArea[i] / (double)m_maxLesion ), 0., 0. );
            }
        }

        int *eventPosition = m_lesion3Dviewer->getEventPosition();
        if ( picker->PickProp ( eventPosition[0], eventPosition[1], m_lesion3Dviewer->getRenderer() ) )
        {
            vtkActor *actor = vtkActor::SafeDownCast( picker->GetViewProp() );
            actor->GetProperty()->SetColor( 0,0,1 );
        }
        else
        {
            vtkActor *actor = vtkActor::SafeDownCast( m_cylindersCollection->GetItemAsObject( m_2DView->getCurrentSlice() ) );
            actor->GetProperty()->SetColor( 0, 0, 1 );
        }

        m_lesion3Dviewer->refresh();
    }
}

void QLesionVisualizationExtension::markCurrentCylinder( int slice )
{
    if ( m_cylindersCollection )
    {
        for ( int i = 0; i < m_cylindersCollection->GetNumberOfItems(); i++)
        {
            vtkActor *actor = vtkActor::SafeDownCast( m_cylindersCollection->GetItemAsObject( i ) );

            if ( i == slice )
                actor->GetProperty()->SetColor( 0., 0., 1. );
            else
            {
                if ( m_maskLesionSlicesArea[i] == 0 )
                    actor->GetProperty()->SetColor( 1. ,1. ,1. );
                else
                    actor->GetProperty()->SetColor( 1.-( (double)m_maskLesionSlicesArea[i] / (double)m_maxLesion ), 0., 0. );
            }
        }
        m_lesion3Dviewer->refresh();
    }
}

void QLesionVisualizationExtension::setOpacity( int op )
{
    if(m_lesionMaskVolume)
    {
        m_2DView->setOpacityOverlay(((double)op)/100.0);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->refresh();
    }
}

void QLesionVisualizationExtension::openMaskVolume()
{
    bool ok;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open lesion  mask file"), m_defaultOpenDirectory, tr("MetaImage Files (*.mhd)"));
    QApplication::setOverrideCursor( QCursor ( Qt::WaitCursor ) );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).suffix() != "mhd" )
            fileName += ".mhd";

        m_defaultOpenDirectory = QFileInfo( fileName ).absolutePath();

        vtkMetaImageReader *reader = vtkMetaImageReader::New();
        reader->SetFileName(qPrintable( fileName ));

        switch( reader->CanReadFile( qPrintable(fileName) ) )
        {
            case 0: // no és un arxiu mhd :(
                ok = false;
                break;

            case 1: // I think I can read the file but I cannot prove it
            case 2: // I definitely can read the file
            case 3: // I can read the file and I have validated that I am the correct reader for this file
                ok = true;
                break;
        }
        reader->Update();
        m_lesionMaskVolume = new Volume(reader->GetOutput());

        m_2DView->setOpacityOverlay( ((double)m_opacitySlider->value()) / 100.0 );
        m_2DView->setOverlayToBlend();
        m_2DView->setOverlayInput( m_lesionMaskVolume );
        m_2DView->refresh();
        m_opacitySlider->setEnabled( true );
        m_opacitiyLabel->setEnabled( true );
        reader->Delete();
    }

    extractLesionMaskArea();
    createCylinder();
    QApplication::restoreOverrideCursor();
}

void QLesionVisualizationExtension::extractLesionMaskArea()
{
    m_maskLesionSlicesArea = new int[m_numberOfSlices];
    double lesionVolume = 0.;

    int i;

    for ( i = 0; i < m_numberOfSlices; i++ )
        m_maskLesionSlicesArea[i] = 0;

    int *dims =m_lesionMaskVolume->getVtkData()->GetExtent();
    int *value;

    for ( i = dims[4]; i < dims[5]; i++ )
    {
        for ( int j = dims[0]; j < dims[1]; j++ )
        {
            for ( int k = dims[2]; k < dims[3]; k++ )
            {
                value=(int*)m_lesionMaskVolume->getVtkData()->GetScalarPointer( j, k, i );

                if (*value != 0)
                {
                    m_maskLesionSlicesArea[i]++;
                    lesionVolume++;
                }
            }
        }
    }
    double *spacing = m_mainVolume->getSpacing();

    for (i = 0; i < 3; i++)
        lesionVolume *= spacing[i];

    m_volumeLabel->setText(QString("Lesion Volume (in mm3): %1").arg(lesionVolume, 0, 'f', 2));
}

void QLesionVisualizationExtension::createCylinder()
{
    int i;
    double white[3] = { 1., 1., 1. };
    double selectedColor[3] = { 0., 0., 1.};
    double redLevel;

    m_cylindersCollection = vtkActorCollection::New();
    vtkCamera *camera = m_lesion3Dviewer->getRenderer()->GetActiveCamera();

    double position = -0.32 * (m_numberOfSlices/2);

    for ( i = 0; i < m_numberOfSlices; i++ )
    {
        if ( m_maskLesionSlicesArea[i] > m_maxLesion )
            m_maxLesion = m_maskLesionSlicesArea[i];
    }

    for ( i = 0; i < m_numberOfSlices; i++ )
    {
        vtkCylinderSource *source = vtkCylinderSource::New();
        source->SetResolution( 20 );
        source->SetHeight( 0.3 );
        source->SetRadius( 2 );

        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
        mapper->ImmediateModeRenderingOn();
        mapper->SetInputConnection(source->GetOutputPort());

        vtkActor *actor = vtkActor::New();

        if ( i == 0 ) //la llesca activa del 2DViewer serà la 0, per tant el cilindre 0 el pintem com a seleccionat.
            actor->GetProperty()->SetColor( selectedColor );
        else
        {
            if ( m_maskLesionSlicesArea[i] == 0 )
                actor->GetProperty()->SetColor( white );
            else
            {
                redLevel = (double)m_maskLesionSlicesArea[i] / (double)m_maxLesion;
                actor->GetProperty()->SetColor( 1.-redLevel, 0., 0. );
            }
        }
        actor->SetMapper(mapper);
        source->SetCenter(0,position,0);
        actor->RotateZ( -90. );

        position += 0.32;
        m_lesion3Dviewer->getRenderer()->AddViewProp(actor);

        m_cylindersCollection->AddItem(actor);

        source->Delete();
        mapper->Delete();
        actor->Delete();
    }

//     camera->SetPosition( bounds[0]+bounds[1] / 2, bounds[2]+bounds[2] / 2, bounds[5] + bounds[5] );
//     camera->SetEyeAngle( 2. );
//     camera->SetDistance( 10.7081 );
//     camera->SetFocalPoint( 7.16328, -0.00117, 0. );
    camera->SetPosition( camera->GetPosition()[0], camera->GetPosition()[1], (double)m_numberOfSlices/3.8 );
//     camera->SetViewAngle(90);
//     camera->SetViewUp( 0, 1, 0 );

    m_lesion3Dviewer->render();
}

void QLesionVisualizationExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("StarViewer-App-LesionVisualization");

    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );
    m_defaultOpenDirectory = settings.value( "openDirectory", "." ).toString();
    settings.endGroup();
}

void QLesionVisualizationExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("StarViewer-App-LesionVisualization");

    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );
    settings.setValue("openDirectory", m_defaultOpenDirectory );

    settings.endGroup();
}
}
