/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawer.h"
#include "line.h"
#include "text.h"
#include "point.h"
#include "polygon.h"
#include "drawingprimitive.h"
#include "logging.h"
#include "q2dviewer.h"

//includes vtk
#include <vtkActor2D.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkDiskSource.h>
#include <vtkCaptionActor2D.h>
#include  <vtkTextProperty.h>
#include  <vtkProperty.h>
#include  <vtkTextActor.h>
#include  <vtkPolygon.h>
#include  <vtkPoints.h>
#include  <vtkDataSetMapper.h>
#include  <vtkUnstructuredGrid.h>
#include <vtkRenderer.h>

//includes Qt
#include <QColor>
#include <QList>

/*\TODO els polígons, de moment, es reprepresenten amb la classe vtkPolygon, perquè sembla que és més fàcil que crear manualment el polydata. 
El que no acaba d'agradar és que vtkPolygon s'ha de mapejar amb un vtkDataSetMapper i aquest no és acceptat per un actor 2D, per tant el representa
com un actor 3d en un visor 2d. De moment no dóna problemes però pot ser que calgui canviar-ho.
*/

namespace udg {

Drawer::Drawer( Q2DViewer *m_viewer , QObject *parent ) : QObject( parent ) 
{
    m_2DViewer = m_viewer;
}

Drawer::~Drawer()
{}

void Drawer::drawPoint( Point *point )
{
    //Definim el punt i les seves propietats
    vtkDiskSource *pointSource = vtkDiskSource::New();
    
    //determinem si el punt s'ha de mostrar ple o no
    if ( point->isFilled() )
        pointSource->SetInnerRadius( 0. );
    else
        pointSource->SetInnerRadius( ( point->getWidth() - ( point->getWidth() / 3 ) ) );
    
    
    pointSource->SetOuterRadius( point->getWidth() );
    pointSource->SetRadialResolution( 1 );
    
    //mirem quina forma ha de tenir
    if ( point->isRounded() )
        pointSource->SetCircumferentialResolution( 20 );
    else
        pointSource->SetCircumferentialResolution( 4 );
        
    vtkActor2D *pointSourceActor = vtkActor2D::New();
    vtkPolyDataMapper2D *pointSourceMapper = vtkPolyDataMapper2D::New();
    
    //Assignem el tipus de coordenades seleccionades
    setCoordinateSystem( point->getCoordinatesSystemAsString(), pointSourceActor->GetPositionCoordinate() );
    setCoordinateSystem( point->getCoordinatesSystemAsString(), pointSourceActor->GetActualPositionCoordinate() );
    
    pointSourceMapper->SetInputConnection( pointSource->GetOutputPort() );
    pointSourceActor->SetMapper( pointSourceMapper );

    //Assignem les propietats al punt
    vtkProperty2D *properties = pointSourceActor->GetProperty();
    
    //Assignem color
    QColor pointSourceColor = point->getColor();
    properties->SetColor( pointSourceColor.redF(), pointSourceColor.greenF(), pointSourceColor.blueF() );
           
    //Assignem opacitat del punt
    properties->SetOpacity( point->getOpacity() );
            
    //assignem la posició al punt
    pointSourceActor->SetPosition( point->getPosition() );
    
    //mirem la visibilitat de l'actor
    if ( !point->isVisible() )
        pointSourceActor->VisibilityOff();
    
    m_2DViewer->getRenderer()->AddActor( pointSourceActor );
    m_2DViewer->refresh();
    
    //esborrem els objectes auxiliars 
    pointSourceActor->Delete();
    pointSource->Delete();
    pointSourceMapper->Delete();
}

void Drawer::drawLine( Line *line )
{
    vtkActor2D *lineActor = vtkActor2D::New();
    vtkLineSource *lineSource = vtkLineSource::New();
    vtkPolyDataMapper2D *lineMapper = vtkPolyDataMapper2D::New();
    
    //Assignem el tipus de coordenades seleccionades
    lineMapper->SetTransformCoordinate( getCoordinateSystem( line->getCoordinatesSystemAsString() ) );
    
    lineMapper->SetInputConnection( lineSource->GetOutputPort() );
    lineActor->SetMapper( lineMapper );

    //Assignem les propietats a la línia
    vtkProperty2D *properties = lineActor->GetProperty();
    
    lineSource->SetResolution ( 2 );
    
    //Assignem color
    QColor lineColor = line->getColor();
    properties->SetColor( lineColor.redF(), lineColor.greenF(), lineColor.blueF() );
    
    //Assignem discontinuïtat
    if ( line->isDiscontinuous() )
        properties->SetLineStipplePattern( 2000 );
    else
        properties->SetLineStipplePattern( 65535 );

    //Assignem gruix de la línia        
    properties->SetLineWidth( line->getWidth() );
           
    //Assignem opacitat de la línia  
    properties->SetOpacity( line->getOpacity() );
            
    //assignem els punts a la línia
    lineSource->SetPoint1( line->getFirstPoint() );
    lineSource->SetPoint2( line->getSecondPoint() );
    
    //mirem la visibilitat de l'actor
    if ( !line->isVisible() )
        lineActor->VisibilityOff();
    
    m_2DViewer->getRenderer()->AddActor( lineActor );
    m_2DViewer->refresh();
    
    //esborrem els objectes auxiliars 
    lineActor->Delete();
    lineSource->Delete();
    lineMapper->Delete();
}
    
void Drawer::drawText( Text *text )
{
    vtkCaptionActor2D *textActor = vtkCaptionActor2D::New();
    
    //Assignem el tipus de coordenades seleccionades
    setCoordinateSystem( text->getCoordinatesSystemAsString(), textActor->GetAttachmentPointCoordinate() );
    
    if ( text->isBorderEnabled() )
        textActor->BorderOn();
    else
        textActor->BorderOff();

    //mirem si s'ha d'escalar el text
    if ( text->isTextScaled() )
        textActor->GetTextActor()->ScaledTextOn();
    else
        textActor->GetTextActor()->ScaledTextOff();
    
    //mirem l'opacitat
    textActor->GetCaptionTextProperty()->SetOpacity( text->getOpacity() );
    
    //Assignem color
    QColor textColor = text->getColor();
    textActor->GetCaptionTextProperty()->SetColor( textColor.redF(), textColor.greenF(), textColor.blueF() );
    
    textActor->SetPadding( text->getPadding() );
    
    textActor->SetPosition( -1.0 , -1.0 );
    textActor->SetHeight( text->getHeight() );
    textActor->SetWidth( text->getWidth() );
    
    //deshabilitem la línia que va des del punt de situació al text
    textActor->LeaderOff();
    
    if ( text->hasShadow() )
        textActor->GetCaptionTextProperty()->ShadowOn();
    else
        textActor->GetCaptionTextProperty()->ShadowOff();
    
    if ( text->isItalic() )
        textActor->GetCaptionTextProperty()->ItalicOn();
    else
        textActor->GetCaptionTextProperty()->ItalicOff();
    
    //Assignem el tipus de font al text
    if ( text->getFontFamily() == "Arial" )
        textActor->GetCaptionTextProperty()->SetFontFamilyToArial();
    if ( text->getFontFamily() == "Courier" )
        textActor->GetCaptionTextProperty()->SetFontFamilyToCourier();
    if ( text->getFontFamily() == "Times" )
        textActor->GetCaptionTextProperty()->SetFontFamilyToTimes();
    else
        DEBUG_LOG( "Tipus de font no reconegut a l'intentar crear text!!" );
    
    //Assignem el tamany de la font
    textActor->GetCaptionTextProperty()->SetFontSize( text->getFontSize() );
    
    //Assignem el tipus de justificació horitzontal
    if ( text->getHorizontalJustification() == "Left" )
        textActor->GetCaptionTextProperty()->SetJustificationToLeft();
    if ( text->getHorizontalJustification() == "Centered" )
        textActor->GetCaptionTextProperty()->SetJustificationToCentered();
    if ( text->getHorizontalJustification() == "Right" )
        textActor->GetCaptionTextProperty()->SetJustificationToRight();
    else
    {
        DEBUG_LOG( "Tipus de justificació horitzontal no reconegut a l'intentar crear text!!" );
    }
    
    //Assignem el tipus de justificació vertical
    if ( text->getVerticalJustification() == "Top" )
        textActor->GetCaptionTextProperty()->SetVerticalJustificationToTop();
    if ( text->getVerticalJustification() == "Centered" )
        textActor->GetCaptionTextProperty()->SetVerticalJustificationToCentered();
    if ( text->getVerticalJustification() == "Bottom" )
        textActor->GetCaptionTextProperty()->SetVerticalJustificationToBottom();
    else
    {
        DEBUG_LOG( "Tipus de justificació vertical no reconegut a l'intentar crear text!!" );
    }
    
    //Assignem el text
    textActor->SetCaption( qPrintable ( text->getText() ) );
    
    //Assignem la posició en pantalla
    textActor->SetAttachmentPoint( text->getAttatchmentPoint() );
    
    //mirem la visibilitat de l'actor
    if ( !text->isVisible() )
        textActor->VisibilityOff();
    
    m_2DViewer->getRenderer()->AddActor( textActor );
    m_2DViewer->refresh();
    
    //esborrem els objectes auxiliars 
    textActor->Delete();
}    

void Drawer::drawPolygon( Polygon *polygon )
{
    //primer de tot mirem que el primer punt del polígon i l'últim no estiguin repetits ja que així ho requereix la classe específica de vtk.
    //en aquest cas eliminarem l'últim punt per evitar repetits
    double *firstPoint = polygon->getPoints().first();
    double *lastPoint = polygon->getPoints().last();
    
    if ( ( firstPoint[0] == lastPoint[0] ) && ( firstPoint[1] == lastPoint[1] ) && ( firstPoint[2] == lastPoint[2] ) )
    {
        polygon->getPoints().removeLast();
    }
    
    //afegim els punts del polígon
    vtkPoints *polygonPoints = vtkPoints::New();
    vtkPolygon *aPolygon = vtkPolygon::New();
    
    polygonPoints->SetNumberOfPoints( polygon->getNumberOfPoints() );
    aPolygon->GetPointIds()->SetNumberOfIds( polygon->getNumberOfPoints() );
    
    int i = 0;
    
    foreach (double *point, polygon->getPoints() )
    {
        polygonPoints->InsertPoint( i, point[0], point[1], point[2] );
        aPolygon->GetPointIds()->SetId( i, i );
        i++;
    }
    
    //creem la malla que representarà els punts del polígon
    vtkUnstructuredGrid *aPolygonGrid = vtkUnstructuredGrid::New();
    aPolygonGrid->Allocate( 1, 1 );
    aPolygonGrid->InsertNextCell( aPolygon->GetCellType(), aPolygon->GetPointIds() );
    aPolygonGrid->SetPoints( polygonPoints );
    
    vtkDataSetMapper *aPolygonMapper = vtkDataSetMapper::New();
    aPolygonMapper->SetInput( aPolygonGrid );
    
    vtkActor *aPolygonActor = vtkActor::New();
    aPolygonActor->SetMapper( aPolygonMapper );
    
    //Assignem discontinuïtat
    if ( polygon->isDiscontinuous() )
        aPolygonActor->GetProperty()->SetLineStipplePattern( 2000 );
    else
        aPolygonActor->GetProperty()->SetLineStipplePattern( 65535 );

    //Assignem gruix de la línia        
    aPolygonActor->GetProperty()->SetLineWidth( polygon->getWidth() );
           
    //Assignem opacitat de la línia  
    aPolygonActor->GetProperty()->SetOpacity( polygon->getOpacity() );
    
    //mirem la visibilitat de l'actor
    if ( !polygon->isVisible() )
        aPolygonActor->VisibilityOff();
    
    //Assignem color
    QColor color = polygon->getColor();
    aPolygonActor->GetProperty()->SetDiffuseColor( color.redF(), color.greenF(), color.blueF() );
            
    //Mirem si cal dibuixar el background o no
    if ( !polygon->isBackgroundEnabled() )        
        aPolygonActor->GetProperty()->SetRepresentationToWireframe();
            
    m_2DViewer->getRenderer()->AddActor( aPolygonActor );
    m_2DViewer->refresh();
                    
    polygonPoints->Delete();
    aPolygon->Delete();
    aPolygonGrid->Delete();
    aPolygonMapper->Delete();
    aPolygonActor->Delete();
} 
    
vtkCoordinate *Drawer::getCoordinateSystem( QString coordinateSystem )
{
    vtkCoordinate *coordinates = vtkCoordinate::New();
    
    if ( coordinateSystem == "DISPLAY" )
        coordinates->SetCoordinateSystemToDisplay();
    else if ( coordinateSystem == "NORMALIZED_DISPLAY" )
        coordinates->SetCoordinateSystemToNormalizedDisplay();
    else if ( coordinateSystem == "VIEWPORT" )
        coordinates->SetCoordinateSystemToViewport();
    else if ( coordinateSystem == "NORMALIZED_VIEWPORT" )       
        coordinates->SetCoordinateSystemToNormalizedViewport();
    else if ( coordinateSystem == "VIEW" )   
        coordinates->SetCoordinateSystemToView();
    else if ( coordinateSystem == "WORLD" )   
        coordinates->SetCoordinateSystemToWorld();
    else
        DEBUG_LOG( "Sistema de coordenades no esperat per a mapejar la primitiva!!" );

    return( coordinates );
}
    
void Drawer::setCoordinateSystem( QString coordinateSystem, vtkCoordinate *coordinates )
{
    if ( coordinateSystem == "DISPLAY" )
        coordinates->SetCoordinateSystemToDisplay();
    else if ( coordinateSystem == "NORMALIZED_DISPLAY" )
        coordinates->SetCoordinateSystemToNormalizedDisplay();
    else if ( coordinateSystem == "VIEWPORT" )
        coordinates->SetCoordinateSystemToViewport();
    else if ( coordinateSystem == "NORMALIZED_VIEWPORT" )       
        coordinates->SetCoordinateSystemToNormalizedViewport();
    else if ( coordinateSystem == "VIEW" )   
        coordinates->SetCoordinateSystemToView();
    else if ( coordinateSystem == "WORLD" )   
        coordinates->SetCoordinateSystemToWorld();
    else
        DEBUG_LOG( "Sistema de coordenades no esperat!!" );
}  
  
};  // end namespace udg
