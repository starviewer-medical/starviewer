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
#include "ellipse.h"
#include "drawingprimitive.h"
#include "logging.h"
#include "q2dviewer.h"

//includes vtk
#include <vtkProp.h>
#include <vtkActor2D.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRegularPolygonSource.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkPoints.h>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkRenderer.h>

//includes Qt
#include <QColor>
#include <QList>
#include <QString>

namespace udg {

Drawer::Drawer( Q2DViewer *m_viewer , QObject *parent ) : QObject( parent ) 
{
    m_2DViewer = m_viewer;
    
    //Creem les connexions entre el Q2DViewer i el drawer
    connect( m_2DViewer , SIGNAL( sliceChanged(int) ) , this , SLOT( setCurrentSlice( int ) ) );
    connect( m_2DViewer , SIGNAL( viewChanged(int) ) , this , SLOT( setCurrentView( int ) ) );
}

Drawer::~Drawer()
{}

///\TODO crear struct, maps i afegir

void Drawer::drawPoint( Point *point, int slice, int view )
{
    validateCoordinates( point->getPosition(), view ); 
    
    vtkRegularPolygonSource *circle = vtkRegularPolygonSource::New();
    circle->SetRadius( point->getWidth() );
    
    if ( point->isRounded() )
        circle->SetNumberOfSides( 60 );
    else
        circle->SetNumberOfSides( 4 );
    
    //assignem posició
    circle->SetCenter( point->getPosition()[0], point->getPosition()[1], point->getPosition()[2] );
    
    //mirem si s'a de pintar ple o buit
    circle->SetGeneratePolygon( point->isFilled() );
    
    vtkPolyDataMapper2D *polyMapper = vtkPolyDataMapper2D::New();
    polyMapper->SetInput( circle->GetOutput() ) ;
    
    polyMapper->SetTransformCoordinate( getCoordinateSystem( point->getCoordinatesSystemAsString() ) );
    vtkActor2D *actor = vtkActor2D::New();
    actor->SetMapper( polyMapper );
    
     //Assignem les propietats al punt
    vtkProperty2D *properties = actor->GetProperty();
    
    //Assignem opacitat del punt
    properties->SetOpacity( point->getOpacity() );
    
    //mirem la visibilitat de l'actor
    if ( !point->isVisible() )
        actor->VisibilityOff();
    
     //Assignem color
    QColor pointColor = point->getColor();
    actor->GetProperty()->SetColor( pointColor.redF(), pointColor.greenF(), pointColor.blueF() );
    
    addActorAndRefresh( actor, point, slice, view );
    
    //esborrem els objectes auxiliars 
    actor->Delete();
    circle->Delete();
    polyMapper->Delete();
}

void Drawer::drawLine( Line *line, int slice, int view )
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
    
    addActorAndRefresh( lineActor, line, slice, view );
    
    //esborrem els objectes auxiliars 
    lineActor->Delete();
    lineSource->Delete();
    lineMapper->Delete();
}
    
void Drawer::drawText( Text *text, int slice, int view )
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
    textActor->ThreeDimensionalLeaderOff();
    
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
    else if ( text->getFontFamily() == "Courier" )
        textActor->GetCaptionTextProperty()->SetFontFamilyToCourier();
    else if ( text->getFontFamily() == "Times" )
        textActor->GetCaptionTextProperty()->SetFontFamilyToTimes();
    else
        DEBUG_LOG( "Tipus de font no reconegut a l'intentar crear text!!" );
    
    //Assignem el tamany de la font
    textActor->GetCaptionTextProperty()->SetFontSize( text->getFontSize() );
    
    //Assignem el tipus de justificació horitzontal
    if ( text->getHorizontalJustification() == "Left" )
        textActor->GetCaptionTextProperty()->SetJustificationToLeft();
    else if ( text->getHorizontalJustification() == "Centered" )
        textActor->GetCaptionTextProperty()->SetJustificationToCentered();
    else if ( text->getHorizontalJustification() == "Right" )
        textActor->GetCaptionTextProperty()->SetJustificationToRight();
    else
    {
        DEBUG_LOG( "Tipus de justificació horitzontal no reconegut a l'intentar crear text!!" );
    }
    
    //Assignem el tipus de justificació vertical
    if ( text->getVerticalJustification() == "Top" )
        textActor->GetCaptionTextProperty()->SetVerticalJustificationToTop();
    else if ( text->getVerticalJustification() == "Centered" )
        textActor->GetCaptionTextProperty()->SetVerticalJustificationToCentered();
    else if ( text->getVerticalJustification() == "Bottom" )
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
    
    addActorAndRefresh( textActor, text, slice, view );
    
    //esborrem els objectes auxiliars 
    textActor->Delete();
}    

void Drawer::drawPolygon( Polygon *polygon, int slice, int view )
{
    //Ens assegurem que el polígon és tancat, és a dir, que el primer punt coincideix amb el primer
    double *firstPoint = polygon->getPoints().first();
    double *lastPoint = polygon->getPoints().last();
    
    if ( ( firstPoint[0] != lastPoint[0] ) || ( firstPoint[1] != lastPoint[1] ) || ( firstPoint[2] != lastPoint[2] ) )
    {
        polygon->addPoint( firstPoint );
    }
    
    vtkPolyData *polydata = vtkPolyData::New();
    vtkPoints *points = vtkPoints::New();
    vtkCellArray *vertexs = vtkCellArray::New(); 
    
    //especifiquem el nombre de vèrtexs que té el polígon
    vertexs->InsertNextCell( polygon->getNumberOfPoints() );
    points->SetNumberOfPoints( polygon->getNumberOfPoints() );
        
    //Calculem els punts de l'el·lipse
    for ( int i = 0; i < polygon->getNumberOfPoints(); i++ )
    {
        points->InsertPoint( i, polygon->getPoints().at( i ) );
        vertexs->InsertCellPoint( i );
    }
    //assignem els punts al polydata
    polydata->SetPoints( points );
    
    if ( polygon->isBackgroundEnabled() )
        polydata->SetPolys( vertexs );
    else
        polydata->SetLines( vertexs );
    
    vtkActor2D *actor = vtkActor2D::New();
    vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::New();   
    
    actor->SetMapper( mapper );
    mapper->SetTransformCoordinate( getCoordinateSystem( polygon->getCoordinatesSystemAsString() ) );
    mapper->SetInput( polydata );

    //Assignem discontinuïtat
    if ( polygon->isDiscontinuous() )
        actor->GetProperty()->SetLineStipplePattern( 2000 );
    else
        actor->GetProperty()->SetLineStipplePattern( 65535 );

    //Assignem gruix de la línia        
    actor->GetProperty()->SetLineWidth( polygon->getWidth() );
           
    //Assignem opacitat de la línia  
    actor->GetProperty()->SetOpacity( polygon->getOpacity() );
    
    //mirem la visibilitat de l'actor
    if ( !polygon->isVisible() )
        actor->VisibilityOff();
    
    //Assignem color
    QColor color = polygon->getColor();
    actor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
            
    addActorAndRefresh( actor, polygon, slice, view );
    
    actor->Delete();
    mapper->Delete();
    points->Delete();
    vertexs->Delete();
    polydata->Delete();
}

void Drawer::addActorAndRefresh( vtkProp *prop, DrawingPrimitive *drawingPrimitive, int slice, int view )
{
    m_2DViewer->getRenderer()->AddActor( prop );
    
    //creem l'objecte PrimitivePropAssociation per introduir al mapa
    PrimitivePropAssociation association;
    association.primitive = drawingPrimitive;
    association.actor = prop;
    
    //introduïm l'associació al mapa corresponent
    addPrimitive( association, slice, view );
    
    m_2DViewer->refresh();          
}
 
void Drawer::validateCoordinates( double coordinates[3], int view )
{
    double aux;
    switch( view )
    {
        case Q2DViewer::Axial:
            //no cal fer res perquè les coordenades ja són les desitjades
            break;
    
        case Q2DViewer::Sagittal:
            aux = coordinates[2];
            coordinates[2] = coordinates[0];
            coordinates[0] = aux;
            break;
        
        case Q2DViewer::Coronal:
            aux = coordinates[1];
            coordinates[1] = coordinates[2];
            coordinates[2] = aux;
            break;
            
        default:
            DEBUG_LOG( "Vista no reconeguda a l'intentar dibuixar una el·lipse!!" );
            return;
            break;
    }
} 
 
void Drawer::drawEllipse( double rectangleCoordinate1[3], double rectangleCoordinate2[3], QColor color, QString behavior, int slice, int view )
{    
    Ellipse *ellipse = new Ellipse( rectangleCoordinate1, rectangleCoordinate2, behavior );
    ellipse->setColor( color );
    drawEllipse( ellipse, slice, view );
}

void Drawer::drawEllipse( Ellipse *ellipse, int slice, int view )
{
    double intersection[2], degrees, xAxis1[2], xAxis2[2], yAxis1[2], yAxis2[2], xRadius, yRadius, *topLeft, *bottomRight, *center, *minor, *major;
    int i;
    
    //validem les coordenades segons la vista en la que estem
    topLeft = ellipse->getTopLeftPoint();
    bottomRight = ellipse->getBottomRightPoint();
    
    //validem les coordenades
    validateCoordinates( topLeft, view );
    validateCoordinates( bottomRight, view );
    
    //assignem les noves coordenades corretgides
    ellipse->setTopLeftPoint( topLeft );
    ellipse->setBottomRightPoint( bottomRight );
    
    vtkPolyData *polydata = vtkPolyData::New();
    vtkPoints *points = vtkPoints::New();
    vtkCellArray *vertexs = vtkCellArray::New(); 

    topLeft = ellipse->getTopLeftPoint();
    bottomRight = ellipse->getBottomRightPoint();
    center = ellipse->getCenter();
    minor = ellipse->getMinorRadius();
    major = ellipse->getMajorRadius();
    
    //especifiquem el nombre de vèrtexs que té l'el·lipse
    vertexs->InsertNextCell( 61 );
        
    xAxis1[0] = center[0];
    xAxis1[1] = center[1];
    xAxis2[0] = bottomRight[0];
    xAxis2[1] = center[1];
    yAxis1[0] = center[0];
    yAxis1[1] = center[1];
    yAxis2[0] = center[0];
    yAxis2[1] = bottomRight[1];
    
    //calculem els radis i la intersecció d'aquests, segons tractem una el·lipse o un cercle
    xRadius = fabs( xAxis1[0] - xAxis2[0] );
    
    if ( ellipse->getBehavior() == "Ellipse" )
        yRadius = fabs( yAxis1[1] - yAxis2[1] );
    else
        yRadius = xRadius;
    
    intersection[0] = ((yAxis2[0] - yAxis1[0]) / 2.0) + yAxis1[0];
    intersection[1] = ((xAxis2[1] - xAxis1[1]) / 2.0) + xAxis1[1];
    
    //Calculem els punts de l'el·lipse
    for ( i = 0; i < 60; i++ )
    {
        degrees = i*6*vtkMath::DoubleDegreesToRadians();
        if ( m_2DViewer->getView() == Q2DViewer::Axial )
        {
            points->InsertPoint( i, cos( degrees )*xRadius + intersection[0], sin( degrees )*yRadius + intersection[1], .0 );
        }
        else if ( m_2DViewer->getView() == Q2DViewer::Sagittal )
        {
            points->InsertPoint( i, 0., sin( degrees )*yRadius + intersection[1],cos( degrees )*xRadius + intersection[0] );
        }
        else
        {
            points->InsertPoint( i, sin( degrees )*yRadius + intersection[1], 0., cos( degrees )*xRadius + intersection[0] );
        }
        vertexs->InsertCellPoint( i );
    }
    
    //afegim l'últim punt per tancar la ROI
    vertexs->InsertCellPoint( 0 );
    
    polydata->SetPoints( points );
    
    if ( ellipse->isBackgroundEnabled() )
        polydata->SetPolys( vertexs );
    else
        polydata->SetLines( vertexs );
    
    vtkActor2D *actor = vtkActor2D::New();
    vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::New();   
    
    actor->SetMapper( mapper );
    mapper->SetTransformCoordinate( getCoordinateSystem( ellipse->getCoordinatesSystemAsString() ) );
    mapper->SetInput( polydata );

    //Assignem discontinuïtat
    if ( ellipse->isDiscontinuous() )
        actor->GetProperty()->SetLineStipplePattern( 2000 );
    else
        actor->GetProperty()->SetLineStipplePattern( 65535 );

    //Assignem gruix de la línia        
    actor->GetProperty()->SetLineWidth( ellipse->getWidth() );
           
    //Assignem opacitat de la línia  
    actor->GetProperty()->SetOpacity( ellipse->getOpacity() );
    
    //mirem la visibilitat de l'actor
    if ( !ellipse->isVisible() )
        actor->VisibilityOff();
    
    //Assignem color
    QColor color = ellipse->getColor();
    actor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
            
    addActorAndRefresh( actor, ellipse, slice, view );         
    
    actor->Delete();
    mapper->Delete();
    points->Delete();
    vertexs->Delete();
    polydata->Delete();
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

Drawer::PrimitivesList Drawer::getPrimitivesList( int slice, int view )
{
    PrimitivesList list;
    list.clear();
    
    switch( view )
    {
        case Q2DViewer::Axial: 
            list = m_axialPrimitives.values( slice );
            break;

        case Q2DViewer::Sagittal:
            list = m_sagitalPrimitives.values( slice );
            break;

        case Q2DViewer::Coronal: 
            list = m_coronalPrimitives.values( slice );
            break;

        default:
            DEBUG_LOG( "Valor no esperat" );
            break;
    }
    return list;
}

void Drawer::addPrimitive( PrimitivePropAssociation primitive, int slice, int view )
{
    bool ok = true;
    switch( view )
    {
        case Q2DViewer::Axial: 
            m_axialPrimitives.insert( slice, primitive );
            break;

        case Q2DViewer::Sagittal: 
            m_sagitalPrimitives.insert( slice, primitive );
            break;

        case Q2DViewer::Coronal:    
            m_coronalPrimitives.insert( slice, primitive );
            break;

        default:
            DEBUG_LOG( "Valor no esperat" );
            ok = false;
            break;
    }
    if( ok )
    {
        // si la primitiva l'estem afegint en la llesca i vista actuals serà per defecte visible, altrament no
        if( slice == m_currentSlice && view == m_currentView )
            setVisibility( primitive, true );
        else
            setVisibility( primitive, false );
    }
}

void Drawer::setVisibility( PrimitivePropAssociation primitivePropAssociation, bool visibility )
{
    if( visibility )
    {
        primitivePropAssociation.primitive->visibilityOn();
        primitivePropAssociation.actor->VisibilityOn();
    }
    else
    {
        primitivePropAssociation.primitive->visibilityOff();
        primitivePropAssociation.actor->VisibilityOff();
    }    
}

void Drawer::hidePrimitivesFrom( int slice, int view )
{
    PrimitivesList listToHide = this->getPrimitivesList( slice, view );
    
    foreach( PrimitivePropAssociation primitive, listToHide )
    {
        setVisibility( primitive, false );
    }
    //actualitzem el Q2DViewer
    m_2DViewer->refresh();
}

void Drawer::showPrimitivesFrom( int slice, int view )
{
    PrimitivesList listToShow = this->getPrimitivesList( slice, view );
    
    foreach( PrimitivePropAssociation primitive, listToShow )
    {
        setVisibility( primitive, true );
    }
    //actualitzem el Q2DViewer
    m_2DViewer->refresh();
}

void Drawer::setCurrentSlice( int slice )
{
    // Primer fem insvisibles els de la llesca en la que ens trobàvem fins ara, en la corresponent vista
    hidePrimitivesFrom( m_currentSlice, m_currentView );
    // I ara fem visibles els de la nova llesca, en la corresponent vista
    showPrimitivesFrom( slice, m_currentView );
    // actualitzem la llesca
    m_currentSlice = slice;
}

void Drawer::setCurrentView( int view )
{
    if( m_currentView != view )
    {
        // s'ha canviat de vista, per tant cal netejar la última vista. Quan es faci el set slice de la nova vista, ja es faran visibles els que toquin
        showPrimitivesFrom( m_currentSlice, m_currentView );
        //actualitzem la nova vista
        m_currentView = view;
    }
    //else: continuem a la mateixa vista, per tant no cal fer res
}

void Drawer::removeAllPrimitives()
{
    // recorrem cadscun dels maps, primer retirem l'actor de l'escena i després l'esborrem de la llista
    foreach( PrimitivePropAssociation primitiveAssociation, m_axialPrimitives )
    {
        m_2DViewer->getRenderer()->RemoveActor( primitiveAssociation.actor );
    }
    
    foreach( PrimitivePropAssociation primitiveAssociation, m_sagitalPrimitives )
    {
        m_2DViewer->getRenderer()->RemoveActor( primitiveAssociation.actor );
    }
    
    foreach( PrimitivePropAssociation primitiveAssociation, m_coronalPrimitives )
    {
        m_2DViewer->getRenderer()->RemoveActor( primitiveAssociation.actor );
    }
    
    //Esborrem el contingut dels maps
    m_axialPrimitives.clear();
    m_sagitalPrimitives.clear();
    m_coronalPrimitives.clear();
            
    // refresquem l'escena
    m_2DViewer->refresh();
}
  
};  // end namespace udg
