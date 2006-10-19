/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dorientationmarker.h"

// Per el maracdor d'orientació
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkPropAssembly.h>
#include <vtkAssembly.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkTransform.h>

namespace udg {

Q3DOrientationMarker::Q3DOrientationMarker( vtkRenderWindowInteractor *interactor , QObject *parent)
  : QObject(parent){

  // Extret de http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/GUI/Tcl/ProbeWithSplineWidget.tcl?root=VTK&content-type=text/plain
//  Create a composite orientation marker using
//  vtkAnnotatedCubeActor and vtkAxesActor.
//
    // \TODO se suposa que aquesta orientació és correcta si estem veient el volum en axial. Caldria mirar el sistema de com posar les etiquetes correctament, automàticament a partir per exemple del tag dicom o deixar-ho en mans de la classe que ho faci servir i proporcionar mètodes d'aquesta classe per posar les etiquetes correctament
    vtkAnnotatedCubeActor *cubeActor = vtkAnnotatedCubeActor::New();
    cubeActor->SetXPlusFaceText("L");
    cubeActor->SetXMinusFaceText("R");
    cubeActor->SetYPlusFaceText("P");
    cubeActor->SetYMinusFaceText("A");
    cubeActor->SetZPlusFaceText("S");
    cubeActor->SetZMinusFaceText("I");
    cubeActor->SetXFaceTextRotation( 90 );
    cubeActor->SetYFaceTextRotation( 180 );
    cubeActor->SetZFaceTextRotation( -90 );
    cubeActor->SetFaceTextScale( 0.65 );
    
    vtkProperty *property = cubeActor->GetCubeProperty();
    property->SetColor( 0.5 , 1 , 1 );
    property = cubeActor->GetTextEdgesProperty();
    property->SetLineWidth( 1 );
    property->SetDiffuse( 0 );
    property->SetAmbient( 1 );
    property->SetColor( 0.18 , 0.28 ,  0.23 );
    cubeActor->TextEdgesOn();
    cubeActor->CubeOn();
    cubeActor->FaceTextOn();

    property = cubeActor->GetXPlusFaceProperty();
    property->SetColor( 0 , 0 , 1 );
    property->SetInterpolationToFlat();
    
    property = cubeActor->GetXMinusFaceProperty();
    property->SetColor( 0 , 0 , 1 );
    property->SetInterpolationToFlat();
    
    property = cubeActor->GetYPlusFaceProperty();
    property->SetColor( 0 , 1 , 0 );
    property->SetInterpolationToFlat();
    
    property = cubeActor->GetYMinusFaceProperty();
    property->SetColor( 0 , 1 , 0 );
    property->SetInterpolationToFlat();
    
    property = cubeActor->GetZPlusFaceProperty();
    property->SetColor( 1 , 0 , 0 );
    property->SetInterpolationToFlat();
    
    property = cubeActor->GetZMinusFaceProperty();
    property->SetColor( 1 , 0 , 0 );
    property->SetInterpolationToFlat();
    
    vtkAxesActor *axes = vtkAxesActor::New();
    axes->SetShaftTypeToCylinder();
    // \TODO podríem aplicar una rotació als eixos perquè les anotacions es veiessin en un sistema de mà-esquerra (rai) en comptes de mà-dreta (lps)
    axes->SetXAxisLabelText( qPrintable( tr("l") ) );
    axes->SetYAxisLabelText( qPrintable( tr("p") ) );
    axes->SetZAxisLabelText( qPrintable( tr("s") ) );
    axes->SetTotalLength( 1.5 , 1.5 ,  1.5 );
    
    vtkTextProperty *textProp = vtkTextProperty::New();
    textProp->ItalicOn();
    textProp->ShadowOn();
    textProp->SetFontFamilyToArial();
    axes->GetXAxisCaptionActor2D()->SetCaptionTextProperty( textProp );
    
    vtkTextProperty *textProp2 = vtkTextProperty::New();
    textProp2->ShallowCopy( textProp );
    axes->GetYAxisCaptionActor2D()->SetCaptionTextProperty( textProp2 );
    
    vtkTextProperty *textProp3 = vtkTextProperty::New();
    textProp3->ShallowCopy( textProp );
    axes->GetZAxisCaptionActor2D()->SetCaptionTextProperty( textProp3 );
    
//     Combine the two actors into one with vtkPropAssembly ...
//     
//     vtkPropAssembly *assembly = vtkPropAssembly::New();
    vtkAssembly *assembly = vtkAssembly::New();
    assembly->AddPart ( axes );
    assembly->AddPart ( cubeActor );
//     vtkTransform *t = vtkTransform::New();
//     t->Scale( 0.1 , 0.1 , 0.1 );
//     assembly->SetUserTransform( t );
//     Add the composite marker to the widget.  The widget
//     should be kept in non-interactive mode and the aspect
//     ratio of the render window taken into account explicitly, 
//     since the widget currently does not take this into 
//     account in a multi-renderer environment.
     
    
    m_markerWidget = vtkOrientationMarkerWidget::New();
    m_markerWidget->SetInteractor( interactor );
    m_markerWidget->SetOutlineColor( 0.93 , 0.57 , 0.13 );
    m_markerWidget->SetOrientationMarker( assembly );
    m_markerWidget->SetViewport( 0.0 , 0.0 , 0.15 , 0.3 );
//     m_markerWidget->SetEnabled(1);
    this->setEnabled( true );
    m_markerWidget->InteractiveOff();
}


Q3DOrientationMarker::~Q3DOrientationMarker()
{
}

void Q3DOrientationMarker::setEnabled( bool enable )
{
    m_markerWidget->SetEnabled( enable );
}

void Q3DOrientationMarker::enable()
{
    this->setEnabled( true );
}

void Q3DOrientationMarker::disable()
{
    this->setEnabled( false );
}

} // end namespace udg
