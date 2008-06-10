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
    m_cubeActor = vtkAnnotatedCubeActor::New();
    this->setOrientationText( "L","R","P","A","S","I" );
//     m_cubeActor->SetXPlusFaceText("L");
//     m_cubeActor->SetXMinusFaceText("R");
//     m_cubeActor->SetYPlusFaceText("P");
//     m_cubeActor->SetYMinusFaceText("A");
//     m_cubeActor->SetZPlusFaceText("S");
//     m_cubeActor->SetZMinusFaceText("I");
    m_cubeActor->SetXFaceTextRotation( 90 );
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
    vtkPropAssembly *assembly = vtkPropAssembly::New();
//     vtkAssembly *assembly = vtkAssembly::New();
    assembly->AddPart ( m_cubeActor );
    assembly->AddPart ( axes );

//     m_cubeActor->SetOrientation( 10 , 99 , 103 ); // aquest no fa cas
//     axes->SetOrientation( 10 , 87 , 15 ); // aquest fa cas
//     assembly->SetOrientation( 10 , 87 , 15 );

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
    this->setEnabled( true );
    m_markerWidget->InteractiveOn();
}


Q3DOrientationMarker::~Q3DOrientationMarker()
{
}

void Q3DOrientationMarker::setOrientationText( QString right, QString left, QString posterior, QString anterior, QString superior, QString inferior )
{
    m_cubeActor->SetXPlusFaceText( qPrintable(right) );
    m_cubeActor->SetXMinusFaceText( qPrintable(left) );
    m_cubeActor->SetYPlusFaceText( qPrintable(posterior) );
    m_cubeActor->SetYMinusFaceText( qPrintable(anterior) );
    m_cubeActor->SetZPlusFaceText( qPrintable(superior) );
    m_cubeActor->SetZMinusFaceText( qPrintable(inferior) );
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
