/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dmprviewer.h"

//includes vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkEventQtSlotConnect.h>
#include <QVTKWidget.h>
#include <vtkCamera.h>

// Per crear la bounding box del model
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
// Per el maracdor d'orientació
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkPropAssembly.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkCommand.h>

//includes propis
#include "volume.h"


namespace udg {


class PlanesInteractionCallback : public vtkCommand
{
public:
    static PlanesInteractionCallback *New(){ return new PlanesInteractionCallback; }
    Q3DMPRViewer *m_viewer;    
    virtual void Execute( vtkObject* caller, unsigned long event, void *vtkNotUsed(callData) )
    {    
        // Obtenim l'interactor que el crida
        vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast( caller ); 
        if( m_viewer )
        {
            m_viewer->planeInteraction();
        }
    }

};

Q3DMPRViewer::Q3DMPRViewer( QWidget *parent )
 : QViewer( parent )
{
    
    //Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_ren = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_ren);
    // Creem tres vistes ortogonals utilitzant la classe ImagePlaneWidget
    // 
    m_axialImagePlaneWidget =  vtkImagePlaneWidget::New();
    m_sagitalImagePlaneWidget =  vtkImagePlaneWidget::New();
    m_coronalImagePlaneWidget =  vtkImagePlaneWidget::New();
    
    m_outlineActor = vtkActor::New();

    // interacció
    m_vtkQtConnections = vtkEventQtSlotConnect::New();    
    
    m_axialResliced = new Volume; 
    m_sagitalResliced = new Volume;
    m_coronalResliced = new Volume;
}

Q3DMPRViewer::~Q3DMPRViewer()
{
    m_ren->Delete();
}

void Q3DMPRViewer::setInput( Volume* volume )
{
    m_mainVolume = volume;    
    int *size = m_mainVolume->getVtkData()->GetDimensions();
    
    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------

    // Utilitzem un "cell picker" per interactuar amb les vistes ortogonals.
    vtkCellPicker * picker = vtkCellPicker::New();
    picker->SetTolerance(0.005);

    //Assignem les propietats per defecte
    vtkProperty * ipwProp = vtkProperty::New();
    
    // Els 3 widgets s'utilizen per visualizar el model
    // (mostra imatges en 2D amb 3 orientacions diferents)
    
//     
//     Pla AXIAL
// 
    m_axialImagePlaneWidget->DisplayTextOn();
    
    
    m_axialImagePlaneWidget->SetPicker(picker);
    m_axialImagePlaneWidget->RestrictPlaneToVolumeOn();
    m_axialImagePlaneWidget->SetKeyPressActivationValue('z');
    m_axialImagePlaneWidget->GetPlaneProperty()->SetColor(1, 1, 0);
    m_axialImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    
    m_axialImagePlaneWidget->TextureInterpolateOn();
    
    m_axialImagePlaneWidget->SetResliceInterpolateToLinear();
    
    m_axialImagePlaneWidget->SetInput( m_mainVolume->getVtkData() ); 
    m_axialResliced->setData( m_axialImagePlaneWidget->GetResliceOutput() );
    

// 
//     Pla SAGITAL
// 
    m_sagitalImagePlaneWidget->DisplayTextOn();
    m_sagitalImagePlaneWidget->SetInput( m_mainVolume->getVtkData() );
    m_sagitalResliced->setData( m_sagitalImagePlaneWidget->GetResliceOutput() );
        
    m_sagitalImagePlaneWidget->SetPicker(picker);
    m_sagitalImagePlaneWidget->RestrictPlaneToVolumeOn();
    m_sagitalImagePlaneWidget->SetKeyPressActivationValue('x');
    m_sagitalImagePlaneWidget->GetPlaneProperty()->SetColor(1, 0, 0);
    m_sagitalImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    m_sagitalImagePlaneWidget->TextureInterpolateOn();
    m_sagitalImagePlaneWidget->SetLookupTable(m_axialImagePlaneWidget->GetLookupTable());
    m_sagitalImagePlaneWidget->SetResliceInterpolateToCubic();

// 
//     Pla CORONAL
//     
    m_coronalImagePlaneWidget->DisplayTextOn();
    m_coronalImagePlaneWidget->SetInput( m_mainVolume->getVtkData() );
    m_coronalResliced->setData( m_coronalImagePlaneWidget->GetResliceOutput() );
    
    m_coronalImagePlaneWidget->SetPicker(picker);
    m_coronalImagePlaneWidget->SetKeyPressActivationValue('y');
    m_coronalImagePlaneWidget->GetPlaneProperty()->SetColor(0, 0, 1);
    m_coronalImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    m_coronalImagePlaneWidget->TextureInterpolateOn();
    m_coronalImagePlaneWidget->SetLookupTable(m_axialImagePlaneWidget->GetLookupTable());
    m_coronalImagePlaneWidget->SetResliceInterpolateToCubic();
    
// 
//     INTERACCIÓ
// 
    m_axialImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );    
    m_sagitalImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    m_coronalImagePlaneWidget->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );

    // interacció
    
    PlanesInteractionCallback *planesInteractionCallback = PlanesInteractionCallback::New();
    planesInteractionCallback->m_viewer = this;
    m_axialImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
    m_sagitalImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
    m_coronalImagePlaneWidget->AddObserver( vtkCommand::InteractionEvent , planesInteractionCallback );
    
    // \TODO mirar perquè no funciona
//     this->createOrientationMarker();
    //Pq. quedin ben orientats
    this->resetViewToAxial();
    
    // Indiquem el color de fons
    m_ren->SetBackground(0.4392, 0.5020, 0.5647);

    // creem l'outline
    vtkOutlineFilter *outlineFilter = vtkOutlineFilter::New();
    outlineFilter->SetInput( m_mainVolume->getVtkData() );
    vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
    outlineMapper->SetInput( outlineFilter->GetOutput() );
    m_outlineActor->SetMapper( outlineMapper );
    m_ren->AddActor( m_outlineActor );
                       
    // Executem el Renderer perquè mostri les imatges    
    render();
}

vtkRenderWindowInteractor *Q3DMPRViewer::getInteractor()
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

vtkRenderer *Q3DMPRViewer::getRenderer()
{
    return m_ren;
}

void Q3DMPRViewer::render()
{
    m_ren->Render();
}

void Q3DMPRViewer::resetViewToSagital()
{
    this->resetPlanes();
    this->setCameraOrientation(SAGITAL);
    this->getInteractor()->Render();
}

void Q3DMPRViewer::resetViewToCoronal()
{
    this->resetPlanes();
    this->setCameraOrientation(CORONAL);
    this->getInteractor()->Render();
}

void Q3DMPRViewer::resetViewToAxial()
{
    this->resetPlanes();
    this->setCameraOrientation(AXIAL);
    this->getInteractor()->Render();
}

void Q3DMPRViewer::setSagitalVisibility(bool enable)
{
    if (enable)
        m_sagitalImagePlaneWidget->On();
    else
        m_sagitalImagePlaneWidget->Off();
}

void Q3DMPRViewer::setCoronalVisibility(bool enable)
{
    if (enable)
        m_coronalImagePlaneWidget->On();
    else
        m_coronalImagePlaneWidget->Off();
}

void Q3DMPRViewer::setAxialVisibility(bool enable)
{
    if (enable)
        m_axialImagePlaneWidget->On();
    else
        m_axialImagePlaneWidget->Off();
}

void Q3DMPRViewer::resetPlanes()
{
    int *size = m_mainVolume->getVtkData()->GetDimensions();
    
    m_axialImagePlaneWidget->SetPlaneOrientationToZAxes();
    m_axialImagePlaneWidget->SetSliceIndex(size[2]/2);
    
    m_sagitalImagePlaneWidget->SetPlaneOrientationToXAxes();
    m_sagitalImagePlaneWidget->SetSliceIndex(size[0]/2);
    
    m_coronalImagePlaneWidget->SetPlaneOrientationToYAxes();
    m_coronalImagePlaneWidget->SetSliceIndex(size[1]/2);
    
    m_axialImagePlaneWidget->On();
    m_sagitalImagePlaneWidget->On();
    m_coronalImagePlaneWidget->On();
    
}

void Q3DMPRViewer::setCameraOrientation(int orientation)
{
    vtkCamera *cam = this->getRenderer() ? this->getRenderer()->GetActiveCamera() : NULL;
    if (cam)
    {
        switch (orientation)
        {
        case AXIAL:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(0,0,1); // -1 if medical ?
            cam->SetViewUp(0,-1,0);
            break;
    
        case CORONAL:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(0,-1,0); // 1 if medical ?
            cam->SetViewUp(0,0,1);
            break;
    
        case SAGITAL:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(1,0,0); // -1 if medical ?
            cam->SetViewUp(0,0,1);
            break;
        }
        this->getRenderer()->ResetCamera();
    }
}

void Q3DMPRViewer::createOrientationMarker()
{
// Extret de http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/GUI/Tcl/ProbeWithSplineWidget.tcl?root=VTK&content-type=text/plain
//  Create a composite orientation marker using
//  vtkAnnotatedCubeActor and vtkAxesActor.
// 

    m_cubeActor = vtkAnnotatedCubeActor::New();
    m_cubeActor->SetXPlusFaceText("R");
    m_cubeActor->SetXMinusFaceText("L");
    m_cubeActor->SetYPlusFaceText("A");
    m_cubeActor->SetYMinusFaceText("P");
    m_cubeActor->SetZPlusFaceText("I");
    m_cubeActor->SetZMinusFaceText("S");
    m_cubeActor->SetXFaceTextRotation( 180 );
    m_cubeActor->SetYFaceTextRotation( 180 );
    m_cubeActor->SetZFaceTextRotation( -90 );
    m_cubeActor->SetFaceTextScale( 0.65 );
    
    vtkProperty *property = m_cubeActor->GetCubeProperty();
    property->SetColor( 0.5 , 1 , 1 );
    property = m_cubeActor->GetTextEdgesProperty();
    property->SetLineWidth( 1 );
    property->SetDiffuse( 0 );
    property->SetAmbient( 1 );
    property->SetColor( 0.18 , 0.28 ,  0.23 );
    m_cubeActor->TextEdgesOn();
    m_cubeActor->CubeOn();
    m_cubeActor->FaceTextOn();

    property = m_cubeActor->GetXPlusFaceProperty();
    property->SetColor( 0 , 0 , 1 );
    property->SetInterpolationToFlat();
    
    property = m_cubeActor->GetXMinusFaceProperty();
    property->SetColor( 0 , 0 , 1 );
    property->SetInterpolationToFlat();
    
    property = m_cubeActor->GetYPlusFaceProperty();
    property->SetColor( 0 , 1 , 0 );
    property->SetInterpolationToFlat();
    
    property = m_cubeActor->GetYMinusFaceProperty();
    property->SetColor( 0 , 1 , 0 );
    property->SetInterpolationToFlat();
    
    property = m_cubeActor->GetZPlusFaceProperty();
    property->SetColor( 1 , 0 , 0 );
    property->SetInterpolationToFlat();
    
    property = m_cubeActor->GetZMinusFaceProperty();
    property->SetColor( 1 , 0 , 0 );
    property->SetInterpolationToFlat();
    
    vtkAxesActor *axes = vtkAxesActor::New();
    axes->SetShaftTypeToCylinder();
    axes->SetXAxisLabelText("x");
    axes->SetYAxisLabelText("y");
    axes->SetZAxisLabelText("z");
    axes->SetTotalLength( 1.5 , 1.5 ,  1.5 );
    
    vtkTextProperty *textProp = vtkTextProperty::New();
    textProp->ItalicOn();
    textProp->ShadowOn();
    textProp->SetFontFamilyToTimes();
    axes->GetXAxisCaptionActor2D()->SetCaptionTextProperty( textProp );
    
    vtkTextProperty *textProp2 = vtkTextProperty::New();
    textProp2->ShallowCopy( textProp );
    axes->GetYAxisCaptionActor2D()->SetCaptionTextProperty( textProp2 );
    
    vtkTextProperty *textProp3 = vtkTextProperty::New();
    textProp3->ShallowCopy( textProp );
    axes->GetZAxisCaptionActor2D()->SetCaptionTextProperty( textProp3 );
    
//     Combine the two actors into one with vtkPropAssembly ...
//     
    vtkPropAssembly *assembly = vtkPropAssembly::New();
    assembly->AddPart ( axes );
    assembly->AddPart ( m_cubeActor );
    
//     Add the composite marker to the widget.  The widget
//     should be kept in non-interactive mode and the aspect
//     ratio of the render window taken into account explicitly, 
//     since the widget currently does not take this into 
//     account in a multi-renderer environment.
     
    vtkOrientationMarkerWidget *marker = vtkOrientationMarkerWidget::New();
    marker->SetInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    marker->SetOutlineColor( 0.93 , 0.57 , 0.13 );
    marker->SetOrientationMarker( assembly );
    marker->SetViewport( 0.0 , 0.0 , 0.15 , 0.3 );
 
    m_ren->AddActor( m_cubeActor );
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
