/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dmprviewer.h"

#include "windowlevelpresetstooldata.h"

//includes vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkImagePlaneWidget.h>
#include <vtkInteractorStyle.h>
#include <vtkEventQtSlotConnect.h>
#include <QVTKWidget.h>
#include <vtkCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkLookupTable.h>

// Per crear la bounding box del model
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

// Per el marcador d'orientació
#include "q3dorientationmarker.h"

//includes propis
#include "volume.h"
#include "image.h"
#include "logging.h"

namespace udg {

class PlanesInteractionCallback : public vtkCommand
{
public:
    static PlanesInteractionCallback *New(){ return new PlanesInteractionCallback; }
    Q3DMPRViewer *m_viewer;
    virtual void Execute( vtkObject *vtkNotUsed(caller), unsigned long event, void *vtkNotUsed(callData) )
    {
        static double lastWindowLevel[2] = {0.,0.};
        if( m_viewer )
        {
            switch( event )
            {
            case vtkCommand::StartInteractionEvent:
                m_viewer->getCurrentWindowLevel( lastWindowLevel );
            break;

            case vtkCommand::EndInteractionEvent:

            break;

            case vtkCommand::InteractionEvent:
                // actualitzem únicament si ha canviat el window level
                double wl[2];
                m_viewer->getCurrentWindowLevel( wl );
                if( wl[0] != lastWindowLevel[0] || wl[1] != lastWindowLevel[1] )
                {
                    m_viewer->getWindowLevelData()->setCustomWindowLevel( wl[0], wl[1] );
                }
            break;
            }
            m_viewer->planeInteraction();
        }
    }
};

Q3DMPRViewer::Q3DMPRViewer( QWidget *parent )
 : QViewer( parent )
{
    //Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    this->getInteractorStyle()->SetCurrentRenderer( m_renderer );
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    this->initializePlanes();
    // interacció
    m_vtkQtConnections = vtkEventQtSlotConnect::New();

    m_axialPlaneVisible = true;
    m_sagitalPlaneVisible = true;
    m_coronalPlaneVisible = true;

    m_outlineActor = 0;
    m_orientationMarker = 0;
    m_axialResliced = 0;
    m_sagitalResliced = 0;
    m_coronalResliced = 0;

    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    // despatxa qualsevol event-> tools
    m_vtkQtConnections->Connect( this->getInteractor(),
                                 vtkCommand::AnyEvent,
                                 this,
#ifdef VTK_QT_5_0_SUPPORT
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, vtkCommand*) )
#else
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*) )
#endif
                                 );
    // \TODO fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers( vtkCommand::LeftButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::RightButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::CharEvent );

    this->enableTools();
    this->createActors();
    this->addActors();
}

Q3DMPRViewer::~Q3DMPRViewer()
{
    m_renderer->Delete();
    m_vtkQtConnections->Delete();
    m_axialImagePlaneWidget->Delete();
    m_sagitalImagePlaneWidget->Delete();
    m_coronalImagePlaneWidget->Delete();
    m_outlineActor->Delete();
}

void Q3DMPRViewer::setInput( Volume *volume )
{
    m_mainVolume = volume;
    this->createOutline();
    // li proporcionem les dades als plans
    this->updatePlanesData();
    // ajustem els valors del window Level per defecte
    this->updateWindowLevelData();
    //li donem la orientació per defecte
    this->resetViewToAxial();
    render();
}

void Q3DMPRViewer::createActors()
{
    m_outlineActor = vtkActor::New();
    m_orientationMarker = new Q3DOrientationMarker( this->getInteractor() );
}

void Q3DMPRViewer::addActors()
{
    if( !m_outlineActor )
    {
        DEBUG_LOG("Error! Intentant afegir actors que no s'han creat encara");
    }
    else
    {
        m_renderer->AddViewProp( m_outlineActor );
    }
}

void Q3DMPRViewer::initializePlanes()
{
    vtkCellPicker *picker = vtkCellPicker::New();
    picker->SetTolerance( 0.005 );

    //Assignem les propietats per defecte
    vtkProperty *ipwProp = vtkProperty::New();

    // Creem tres vistes ortogonals utilitzant la classe ImagePlaneWidget
    //
    m_axialImagePlaneWidget =  vtkImagePlaneWidget::New();
    m_sagitalImagePlaneWidget =  vtkImagePlaneWidget::New();
    m_coronalImagePlaneWidget =  vtkImagePlaneWidget::New();
    // Els 3 widgets s'utilizen per visualizar el model
    // (mostra imatges en 2D amb 3 orientacions diferents)
    //
    //     Pla AXIAL
    //
    m_axialImagePlaneWidget->DisplayTextOn();
    m_axialImagePlaneWidget->SetPicker( picker );
    m_axialImagePlaneWidget->RestrictPlaneToVolumeOn();
    m_axialImagePlaneWidget->SetKeyPressActivationValue('z');
    m_axialImagePlaneWidget->GetPlaneProperty()->SetColor( 1. , 1. , .0 );
    m_axialImagePlaneWidget->SetTexturePlaneProperty( ipwProp );
    m_axialImagePlaneWidget->TextureInterpolateOn();
    m_axialImagePlaneWidget->SetResliceInterpolateToCubic();
    //
    //     Pla SAGITAL
    //
    m_sagitalImagePlaneWidget->DisplayTextOn();
    m_sagitalImagePlaneWidget->SetPicker( picker );
    m_sagitalImagePlaneWidget->RestrictPlaneToVolumeOn();
    m_sagitalImagePlaneWidget->SetKeyPressActivationValue('x');
    m_sagitalImagePlaneWidget->GetPlaneProperty()->SetColor( 1. , .6 , .0 );
    m_sagitalImagePlaneWidget->SetTexturePlaneProperty( ipwProp );
    m_sagitalImagePlaneWidget->TextureInterpolateOn();
    m_sagitalImagePlaneWidget->SetLookupTable( m_axialImagePlaneWidget->GetLookupTable() );
    m_sagitalImagePlaneWidget->SetResliceInterpolateToCubic();
    //
    //     Pla CORONAL
    //
    m_coronalImagePlaneWidget->DisplayTextOn();
    m_coronalImagePlaneWidget->SetPicker( picker );
    m_coronalImagePlaneWidget->SetKeyPressActivationValue('y');
    m_coronalImagePlaneWidget->GetPlaneProperty()->SetColor( .0 , 1. , 1. );
    m_coronalImagePlaneWidget->SetTexturePlaneProperty( ipwProp );
    m_coronalImagePlaneWidget->TextureInterpolateOn();
    m_coronalImagePlaneWidget->SetLookupTable( m_axialImagePlaneWidget->GetLookupTable() );
    m_coronalImagePlaneWidget->SetResliceInterpolateToCubic();
    //
    //     INTERACCIÓ
    //
    m_axialImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    m_sagitalImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    m_coronalImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );

    PlanesInteractionCallback *planesInteractionCallback = PlanesInteractionCallback::New();
    planesInteractionCallback->m_viewer = this;
//     m_axialImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
//     m_sagitalImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
//     m_coronalImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
    m_axialImagePlaneWidget->AddObserver( vtkCommand::AnyEvent , planesInteractionCallback );
    m_sagitalImagePlaneWidget->AddObserver( vtkCommand::AnyEvent , planesInteractionCallback );
    m_coronalImagePlaneWidget->AddObserver( vtkCommand::AnyEvent , planesInteractionCallback );
}

void Q3DMPRViewer::updatePlanesData()
{
    if( m_mainVolume )
    {
        m_axialImagePlaneWidget->SetInput( m_mainVolume->getVtkData() );
        if( !m_axialResliced )
            m_axialResliced = new Volume( m_axialImagePlaneWidget->GetResliceOutput() );
        else
            m_axialResliced->setData( m_axialImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_axialResliced->setImages( m_mainVolume->getImages() );

        m_sagitalImagePlaneWidget->SetInput( m_mainVolume->getVtkData() );
        if( !m_sagitalResliced )
            m_sagitalResliced = new Volume( m_sagitalImagePlaneWidget->GetResliceOutput() );
        else
            m_sagitalResliced->setData( m_sagitalImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_sagitalResliced->setImages( m_mainVolume->getImages() );

        m_coronalImagePlaneWidget->SetInput( m_mainVolume->getVtkData() );
        if( !m_coronalResliced )
            m_coronalResliced = new Volume( m_coronalImagePlaneWidget->GetResliceOutput() );
        else
            m_coronalResliced->setData( m_coronalImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_coronalResliced->setImages( m_mainVolume->getImages() );
    }
    else
    {
        DEBUG_LOG( "No es poden inicialitzar les dades dels plans. No hi ha dades d'entrada" );
    }
}

void Q3DMPRViewer::createOutline()
{
    if( m_mainVolume )
    {
        // creem l'outline
        vtkOutlineFilter *outlineFilter = vtkOutlineFilter::New();
        outlineFilter->SetInput( m_mainVolume->getVtkData() );
        vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
        outlineMapper->SetInput( outlineFilter->GetOutput() );
        m_outlineActor->SetMapper( outlineMapper );
    }
    else
    {
        DEBUG_LOG( "Intentant crear outline sense haver donat input abans" );
    }
}

vtkRenderer *Q3DMPRViewer::getRenderer()
{
    return m_renderer;
}

void Q3DMPRViewer::render()
{
    // Indiquem el color de fons, blau cel, \TODO això podria anar al inicialitzar-se i prou
    m_renderer->SetBackground( 0.4392, 0.5020, 0.5647 );
    m_renderer->Render();
}

void Q3DMPRViewer::reset()
{
    // \TODO implementar
    this->resetViewToAxial();
    this->setAxialVisibility( true );
    this->setSagitalVisibility( true );
    this->setCoronalVisibility( true );
    this->resetWindowLevelToDefault();
}

void Q3DMPRViewer::resetViewToAxial()
{
    this->resetPlanes();
    this->setCameraOrientation(Axial);
}

void Q3DMPRViewer::resetViewToSagital()
{
    this->resetPlanes();
    this->setCameraOrientation(Sagital);
}

void Q3DMPRViewer::resetViewToCoronal()
{
    this->resetPlanes();
    this->setCameraOrientation(Coronal);
}

void Q3DMPRViewer::setSagitalVisibility(bool enable)
{
    m_sagitalPlaneVisible = enable;
    if (enable)
        m_sagitalImagePlaneWidget->On();
    else
        m_sagitalImagePlaneWidget->Off();
}

void Q3DMPRViewer::setCoronalVisibility(bool enable)
{
    m_coronalPlaneVisible = enable;
    if (enable)
        m_coronalImagePlaneWidget->On();
    else
        m_coronalImagePlaneWidget->Off();
}

void Q3DMPRViewer::setAxialVisibility(bool enable)
{
    m_axialPlaneVisible = enable;
    if (enable)
        m_axialImagePlaneWidget->On();
    else
        m_axialImagePlaneWidget->Off();
}

void Q3DMPRViewer::resetPlanes()
{
    if( m_mainVolume )
    {
        int *size = m_mainVolume->getVtkData()->GetDimensions();

        m_axialImagePlaneWidget->SetPlaneOrientationToZAxes();
        m_axialImagePlaneWidget->SetSliceIndex(size[2]/2);

        m_sagitalImagePlaneWidget->SetPlaneOrientationToXAxes();
        m_sagitalImagePlaneWidget->SetSliceIndex(size[0]/2);

        m_coronalImagePlaneWidget->SetPlaneOrientationToYAxes();
        m_coronalImagePlaneWidget->SetSliceIndex(size[1]/2);

        if( m_axialPlaneVisible )
            m_axialImagePlaneWidget->On();
        if( m_sagitalPlaneVisible )
            m_sagitalImagePlaneWidget->On();
        if( m_coronalPlaneVisible )
            m_coronalImagePlaneWidget->On();
    }
}

void Q3DMPRViewer::setWindowLevel( double window , double level )
{
    if( m_mainVolume )
    {
        // amb un n'hi ha prou ja que cada vtkImagePlaneWidget comparteix la mateixa LUT
        m_axialImagePlaneWidget->SetWindowLevel( window , level );
    }
    else
    {
        DEBUG_LOG("Intentant posar un window level sense donar un input abans");
    }
}

void Q3DMPRViewer::resetWindowLevelToDefault()
{
    if( m_mainVolume )
    {
        m_axialImagePlaneWidget->SetWindowLevel( m_defaultWindow , m_defaultLevel );
    }
    else
    {
        DEBUG_LOG("Intentant posar un window level sense donar un input abans");
    }
}

void Q3DMPRViewer::enableOutline( bool enable )
{
    m_isOutlineEnabled = enable;
    m_outlineActor->SetVisibility( m_isOutlineEnabled );
}

void Q3DMPRViewer::outlineOn()
{
    this->enableOutline( true );
}

void Q3DMPRViewer::outlineOff()
{
    this->enableOutline( false );
}

void Q3DMPRViewer::enableOrientationMarker( bool enable )
{
    m_orientationMarker->setEnabled( enable );
}

void Q3DMPRViewer::orientationMarkerOn()
{
    this->enableOrientationMarker( true );
}

void Q3DMPRViewer::orientationMarkerOff()
{
    this->enableOrientationMarker( false );
}

void Q3DMPRViewer::setVtkLUT( vtkLookupTable *lut )
{
    m_axialImagePlaneWidget->SetLookupTable( lut );
    m_sagitalImagePlaneWidget->SetLookupTable( m_axialImagePlaneWidget->GetLookupTable() );
    m_coronalImagePlaneWidget->SetLookupTable( m_axialImagePlaneWidget->GetLookupTable() );
}

vtkLookupTable *Q3DMPRViewer::getVtkLUT( )
{
    return vtkLookupTable::SafeDownCast( m_axialImagePlaneWidget->GetLookupTable() );
}

void Q3DMPRViewer::getCurrentWindowLevel( double wl[2] )
{
    m_axialImagePlaneWidget->GetWindowLevel( wl );
}

void Q3DMPRViewer::resetView( CameraOrientationType )
{
    // TODO estem obligats a implementar-lo. Caldria aplicar la orientació que se'ns demana
}

void Q3DMPRViewer::planeInteraction()
{
    emit planesHasChanged();
}

Volume *Q3DMPRViewer::getAxialResliceOutput()
{
    return m_axialResliced;
}

Volume *Q3DMPRViewer::getSagitalResliceOutput()
{
    return m_sagitalResliced;
}

Volume *Q3DMPRViewer::getCoronalResliceOutput()
{
    return m_coronalResliced;
}

double *Q3DMPRViewer::getAxialPlaneOrigin()
{
    return m_axialImagePlaneWidget->GetOrigin();
}

double *Q3DMPRViewer::getAxialPlaneNormal()
{
    return m_axialImagePlaneWidget->GetNormal();
}

void Q3DMPRViewer::getAxialPlaneOrigin( double origin[3] )
{
    m_axialImagePlaneWidget->GetOrigin( origin );
}

void Q3DMPRViewer::getAxialPlaneNormal( double normal[3] )
{
    m_axialImagePlaneWidget->GetNormal( normal );
}

double *Q3DMPRViewer::getSagitalPlaneOrigin()
{
    return m_sagitalImagePlaneWidget->GetOrigin();
}

double *Q3DMPRViewer::getSagitalPlaneNormal()
{
    return m_sagitalImagePlaneWidget->GetNormal();
}

void Q3DMPRViewer::getSagitalPlaneOrigin( double origin[3] )
{
    m_sagitalImagePlaneWidget->GetOrigin( origin );
}

void Q3DMPRViewer::getSagitalPlaneNormal( double normal[3] )
{
    m_sagitalImagePlaneWidget->GetNormal( normal );
}

double *Q3DMPRViewer::getCoronalPlaneOrigin()
{
    return m_coronalImagePlaneWidget->GetOrigin();
}

double *Q3DMPRViewer::getCoronalPlaneNormal()
{
    return m_coronalImagePlaneWidget->GetNormal();
}

void Q3DMPRViewer::getCoronalPlaneOrigin( double origin[3] )
{
    m_coronalImagePlaneWidget->GetOrigin( origin );
}

void Q3DMPRViewer::getCoronalPlaneNormal( double normal[3] )
{
    m_coronalImagePlaneWidget->GetNormal( normal );
}

};  // end namespace udg
