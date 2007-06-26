/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerblackboard.h"

#include "q2dviewer.h"
#include "logging.h"

// VTK
#include <vtkRenderer.h>
#include <vtkCaptionActor2D.h>
#include <vtkCoordinate.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRegularPolygonSource.h>
#include <vtkCoordinate.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
// per fer polyLines "interpolated"
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>

#include <vtkTextActor.h>

namespace udg {

Q2DViewerBlackBoard::Q2DViewerBlackBoard( Q2DViewer *viewer, QObject *parent )
 : QObject(parent)
{
    m_2DViewer = viewer;
    if( m_2DViewer )
    {
        m_lastSlice = m_2DViewer->getSlice();
        m_lastView = m_2DViewer->getView();

        connect( m_2DViewer, SIGNAL( sliceChanged(int) ), this, SLOT( refreshObjects(int) ) );
        connect( m_2DViewer, SIGNAL( rotationFactorChanged(int) ), this, SLOT( refreshRotation(int) ) );
        connect( m_2DViewer, SIGNAL( rotationDegreesChanged(double) ), this, SLOT( refreshRotation(double) ) );
    }
    else
        DEBUG_LOG( "El viewer proporcionat és NUL!" );
}

Q2DViewerBlackBoard::~Q2DViewerBlackBoard()
{
    vtkRenderer* renderer = m_2DViewer->getRenderer();

    foreach(vtkActor2D *actor, m_axialAnnotations)
    {
        renderer->RemoveActor(actor);
        actor->Delete();
    }

    foreach(vtkActor2D *actor, m_sagitalAnnotations)
    {
        renderer->RemoveActor(actor);
        actor->Delete();
    }

    foreach(vtkActor2D *actor, m_coronalAnnotations)
    {
        renderer->RemoveActor(actor);
        //if (actor) actor->Delete();
        actor->Delete();
    }
}

void Q2DViewerBlackBoard::addPoint( double point[2], int slice, int view, QColor color )
{
    // afegim un cercle petit i llestos :D \TODO potser la mida del radi hauria de ser consistent amb la mida del pixel
    this->addCircle( point, 1.0, slice, view, true, color );
}

void Q2DViewerBlackBoard::addTextAnnotation(  double attachPoint[2], double position1[2], double position2[2], const char *text, int slice, int view,int orientation, int justification, QColor color, bool hasBorder, bool hasLeader )
{
    vtkCaptionActor2D *captionActor = vtkCaptionActor2D::New();
    // li donem el text
    captionActor->SetCaption( text );
    captionActor->SetLeader( hasLeader );
    captionActor->SetBorder( hasBorder );
    captionActor->GetCaptionTextProperty()->SetColor( color.redF(), color.greenF(), color.blueF() ); // a nivell de propietat de text
    captionActor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() ); // a nivell d'actor
    // configuració de propietats del text per defecte
    // això és necessari ja que per defecte les coordenades són relatives ( Pos a attachmentPos i Pos2 a Pos ), i nosaltres volem que siguin absolutes
    captionActor->GetPositionCoordinate()->SetReferenceCoordinate( NULL );
    captionActor->GetPosition2Coordinate()->SetReferenceCoordinate( NULL );
    captionActor->SetAttachmentPoint( attachPoint[0], attachPoint[1], .0 );
    // El punt d'anclatge ja està en coordenades de món per defecte, però els punts de la bounding box no
    captionActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    captionActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    captionActor->SetPosition( position1 );
    captionActor->SetPosition2( position2 );

    // justification: 0 = left, 1 = centered, 2 = right
    captionActor->GetCaptionTextProperty()->SetJustification( justification );
    captionActor->GetCaptionTextProperty()->SetVerticalJustificationToCentered();
    // determinem la orientació del text
    double degrees = orientation * 90.0;
    //\TODO sembla que l'orientació se la passa pel forro
    captionActor->GetCaptionTextProperty()->SetOrientation( degrees );
    this->addActor( captionActor, slice, view );

    // opcional
//     vtkGlyphSource2D *arrow = vtkGlyphSource2D::New();
//     arrow->SetGlyphTypeToHookedArrow();
//     captionActor->SetLeaderGlyph( arrow->GetOutput() );
//     captionActor->GetTextActor()->ScaledTextOn();

    // NOTES ADICIONALS
    // El dicom no diu res de com s'ha de renderitzar el texte quan nomé hi ha l'anchor point, així que ho deixarem tal qual i ens quedem tant amples\TODO es podria fer de diverses formes però ara no ens matarem pas, veure apartat C.10.5.1.1 del Sup 33 del DICOM
}

void Q2DViewerBlackBoard::addTextAnnotation( const char *text, int slice, int view, bool hasBoundingBox, bool hasAttachPoint, double attachPoint[2], double position1[2], double position2[2], int orientation, int justification, QColor color, bool hasBorder, bool hasLeader )
{
    vtkCaptionActor2D *captionActor = vtkCaptionActor2D::New();
    // li donem el text
    captionActor->SetCaption( text );
    captionActor->SetLeader( hasLeader );
    captionActor->SetBorder( hasBorder );
    captionActor->GetCaptionTextProperty()->SetColor( color.redF(), color.greenF(), color.blueF() ); // a nivell de propietat de text
    captionActor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() ); // a nivell d'actor
    // configuració de propietats del text per defecte
    // això és necessari ja que per defecte les coordenades són relatives ( Pos a attachmentPos i Pos2 a Pos ), i nosaltres volem que siguin absolutes


    if( hasBoundingBox )
    {
        captionActor->GetPositionCoordinate()->SetReferenceCoordinate( NULL );
        captionActor->GetPosition2Coordinate()->SetReferenceCoordinate( NULL );
        // El punt d'anclatge ja està en coordenades de món per defecte, però els punts de la bounding box no
        captionActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        captionActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
        captionActor->SetPosition( position1 );
        captionActor->SetPosition2( position2 );
    }
    if( hasAttachPoint )
        captionActor->SetAttachmentPoint( attachPoint[0], attachPoint[1], .0 );

    // justification: 0 = left, 1 = centered, 2 = right
    captionActor->GetCaptionTextProperty()->SetJustification( justification );
    captionActor->GetCaptionTextProperty()->SetVerticalJustificationToCentered();
    // determinem la orientació del text
    double degrees = orientation * 90.0;
    //\TODO sembla que l'orientació se la passa pel forro
    captionActor->GetCaptionTextProperty()->SetOrientation( degrees );
    this->addActor( captionActor, slice, view );
}

void Q2DViewerBlackBoard::addCircle( double center[2], double radius, int slice, int view, bool filled, QColor color )
{
    vtkRegularPolygonSource *circle = vtkRegularPolygonSource::New();
    circle->SetCenter( center[0], center[1], 0.0 );
    circle->SetRadius( radius );
    circle->SetNumberOfSides( 60 ); // com més sides, més línies, per tant el cercle tindria més resolució.
    circle->SetGeneratePolygon( filled );
    this->addActor( this->createActorFromPolyData( circle->GetOutput(), color ), slice, view );
}

void Q2DViewerBlackBoard::addEllipse( double xAxis1[2], double xAxis2[2], double yAxis1[2], double yAxis2[2], int slice, int view, bool filled, QColor color )
{
    double xRadius, yRadius;
    xRadius = fabs( xAxis1[0] - xAxis2[0] ) / 2.0;
    yRadius = fabs( yAxis1[1] - yAxis2[1] ) / 2.0;
    // faltaria indicar el centre que caldria calcular a partir dels quatre punts!

    double intersection[2];
    intersection[0] = ((yAxis2[0] - yAxis1[0]) / 2.0) + yAxis1[0];
    intersection[1] = ((xAxis2[1] - xAxis1[1]) / 2.0) + xAxis1[1];

    vtkPoints *ellipsePoints = vtkPoints::New();
    vtkCellArray *ellipseVertexs = vtkCellArray::New();
    ellipseVertexs->InsertNextCell( 61 );
    for( int i = 0; i < 60 ; i++ )
    {
        double degrees = i*6*vtkMath::DoubleDegreesToRadians();
        ellipsePoints->InsertPoint( i , cos(degrees)*xRadius + intersection[0], sin(degrees)*yRadius + intersection[1], .0 );
        ellipseVertexs->InsertCellPoint( i );
    }
    ellipseVertexs->InsertCellPoint( 0 );
    vtkPolyData *ellipse = vtkPolyData::New();
    ellipse->SetPoints( ellipsePoints );
    if( filled )
        ellipse->SetPolys( ellipseVertexs ); // ho pinta filled
    else
        ellipse->SetLines( ellipseVertexs ); // ho pinta buit

    this->addActor( this->createActorFromPolyData( ellipse, color ), slice, view );
}

void Q2DViewerBlackBoard::addPolyline( vtkPoints *points, int slice, int view, bool interpolated, bool filled, QColor color )
{
    if( !points )
    {
        DEBUG_LOG( "Els vtkPoints són NULS!" );
        return;
    }
    // Declarem el polydata que cadascu omplirà a la seva manera
    vtkPolyData *polyData = 0;
    if( interpolated )
    {
        // vtkParametric spline acts as the interpolating engine
        vtkParametricSpline *parametricSpline = vtkParametricSpline::New();
        // si l'últim punt i el primer són el mateix esborrarem l'últim i indicarem que és un spline tancat
        int lastPoint = points->GetNumberOfPoints() - 1;
        if( points->GetPoint(0)[0] == points->GetPoint( lastPoint )[0] && points->GetPoint(0)[1] == points->GetPoint( lastPoint )[1] && points->GetPoint(0)[2] == points->GetPoint( lastPoint )[2] )
        {
            points->SetNumberOfPoints( lastPoint );
            parametricSpline->ClosedOn();
        }
        else
        {
            parametricSpline->ClosedOff();
        }
        parametricSpline->SetPoints( points );
        parametricSpline->ParameterizeByLengthOff();
        // Define the points and line segments representing the spline
        int resolution = 100;

        vtkParametricFunctionSource *parametricFunctionSource = vtkParametricFunctionSource::New();
        parametricFunctionSource->SetParametricFunction( parametricSpline );
        parametricFunctionSource->SetScalarModeToNone();
        parametricFunctionSource->GenerateTextureCoordinatesOff();
        parametricFunctionSource->SetUResolution( resolution );
        parametricFunctionSource->Update();

        if( filled )
        {
            polyData = vtkPolyData::New();
            vtkPoints *splinePoints = parametricFunctionSource->GetOutput()->GetPoints();
            polyData->SetPoints( splinePoints );
            vtkCellArray *splineVertexs = vtkCellArray::New();
            splineVertexs->InsertNextCell( splinePoints->GetNumberOfPoints() );
            for( int i = 0; i < splinePoints->GetNumberOfPoints(); i++ )
                splineVertexs->InsertCellPoint( i );

            polyData->SetPolys( splineVertexs );
        }
        else
        {
            polyData = parametricFunctionSource->GetOutput();
        }
    }
    else
    {
        int n = points->GetNumberOfPoints();
        vtkCellArray *vertexs = vtkCellArray::New();
        vertexs->InsertNextCell( n );
        for( int i = 0; i < n; i++ )
            vertexs->InsertCellPoint( i );
        polyData = vtkPolyData::New();
        polyData->SetPoints( points );
        if( filled )
            polyData->SetPolys( vertexs ); // ho pintarà filled
        else
            polyData->SetLines( vertexs ); // ho pintarà buit
    }
    this->addActor( this->createActorFromPolyData( polyData, color ), slice, view );
}

void Q2DViewerBlackBoard::addKeyImageNoteMark(int slice, int view, QColor color, const QString &description)
{
    // Marca del cercle
    vtkRegularPolygonSource *circle = vtkRegularPolygonSource::New();
    circle->SetCenter( 0.515, 0.01, 0.0 );
    circle->SetRadius( 0.005 );
    circle->SetNumberOfSides( 60 ); // com més sides, més línies, per tant el cercle tindria més resolució.
    circle->SetGeneratePolygon( true );

    vtkPolyDataMapper2D *polyMapper = vtkPolyDataMapper2D::New();
    polyMapper->SetInputConnection( circle->GetOutputPort() ) ;

    vtkCoordinate *coordinateSystem = vtkCoordinate::New();
    coordinateSystem->SetCoordinateSystemToNormalizedDisplay();
    polyMapper->SetTransformCoordinate( coordinateSystem );
    vtkActor2D *circleActor = vtkActor2D::New();
    circleActor->SetMapper( polyMapper );
    circleActor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );

    this->addActor( circleActor, slice, view );

    // Si tenim descprició li afegim
    if (!description.isEmpty())
    {
        vtkTextActor *descriptionActor = vtkTextActor::New();
        descriptionActor->ScaledTextOff();
        descriptionActor->GetTextProperty()->SetFontSize( 18 );
        descriptionActor->GetTextProperty()->BoldOn();

        descriptionActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
        descriptionActor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedDisplay();

        descriptionActor->GetTextProperty()->SetJustificationToLeft();
        descriptionActor->GetTextProperty()->ShadowOn();
        descriptionActor->SetPosition( 0.525 , 0.01 );

        descriptionActor->SetInput( qPrintable(description) );

        this->addActor( descriptionActor, slice, view);
    }
}

Q2DViewerBlackBoard::ActorsListType Q2DViewerBlackBoard::getActorsList( int slice, int view )
{
    ActorsListType list;
    list.clear();
    switch( view )
    {
    case Q2DViewer::Axial:
        list = m_axialAnnotations.values( slice );
    break;

    case Q2DViewer::Sagittal:
        list = m_sagitalAnnotations.values( slice );
    break;

    case Q2DViewer::Coronal:
        list = m_coronalAnnotations.values( slice );
    break;

    default:
        DEBUG_LOG( "Valor no esperat" );
    break;
    }
    return list;
}

void Q2DViewerBlackBoard::addActor( vtkActor2D *actor, int slice, int view )
{
    bool ok = true;
    switch( view )
    {
    case Q2DViewer::Axial:
        m_axialAnnotations.insert( slice, actor );
    break;

    case Q2DViewer::Sagittal:
        m_sagitalAnnotations.insert( slice, actor );
    break;

    case Q2DViewer::Coronal:
        m_coronalAnnotations.insert( slice, actor );
    break;

    default:
        DEBUG_LOG( "Valor no esperat" );
        ok = false;
    break;
    }
    if( ok )
    {
        actor->VisibilityOff();
        m_2DViewer->getRenderer()->AddActor( actor );
        this->refreshObjects( m_2DViewer->getSlice() );
    }
}

vtkActor2D *Q2DViewerBlackBoard::createActorFromPolyData( vtkPolyData *polyData, QColor color )
{
    if( !polyData )
    {
        DEBUG_LOG( "El Polydata és NULL!!!" );
        return 0;
    }
    vtkPolyDataMapper2D *polyMapper = vtkPolyDataMapper2D::New();
    polyMapper->SetInput( polyData ) ;

    vtkCoordinate *coordinateSystem = vtkCoordinate::New();
    coordinateSystem->SetCoordinateSystemToWorld();
    polyMapper->SetTransformCoordinate( coordinateSystem );
    vtkActor2D *actor = vtkActor2D::New();
    actor->SetMapper( polyMapper );
    actor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
    return actor;
}

void Q2DViewerBlackBoard::refreshObjects( int slice )
{
    int viewToClear;
    if( m_lastView != m_2DViewer->getView() )
    {
        // s'ha canviat de vista
        viewToClear = m_lastView;
        m_lastView = m_2DViewer->getView();
    }
    else // continuem a la mateixa vista
        viewToClear = m_2DViewer->getView();

    // Primer esborrem els de la llesca anterior, en la corresponent vista
    ActorsListType listToClear = this->getActorsList( m_lastSlice, viewToClear );
    foreach( vtkActor2D *actor, listToClear )
    {
        actor->VisibilityOff();
    }
    // I ara fem visibles els de la nova llesca ( i vista si ha canviat )
    ActorsListType listToView = this->getActorsList( slice, m_lastView );
    foreach( vtkActor2D *actor, listToView )
    {
        actor->VisibilityOn();
    }
    // actualitzem la llesca
    m_lastSlice = slice;
}

void Q2DViewerBlackBoard::refreshRotation( int factor )
{
    ActorsListType list = this->getActorsList( m_lastSlice, m_lastView );
    // per cada caption la rotarem tal com li toca
    vtkCaptionActor2D *caption;
    foreach( vtkActor2D *actor, list )
    {
        if( QString("vtkCaptionActor2D") == actor->GetClassName() )
        {
            caption = vtkCaptionActor2D::SafeDownCast( actor );
            caption->GetCaptionTextProperty()->SetOrientation( caption->GetCaptionTextProperty()->GetOrientation() + factor*90.0 );
        }
    }
}

void Q2DViewerBlackBoard::refreshRotation( double degrees )
{
}

void Q2DViewerBlackBoard::clear()
{
    // recorrem cadscun dels maps i primer retirem l'actor de l'escena i després l'esborrem de la llista
    QMutableMapIterator<int, vtkActor2D *> axialIterator( m_axialAnnotations );
    axialIterator.toFront();
    while( axialIterator.hasNext() )
    {
        axialIterator.next();
        m_2DViewer->getRenderer()->RemoveActor( axialIterator.value() );
        axialIterator.remove();
    }

    QMutableMapIterator<int, vtkActor2D *> sagitalIterator( m_sagitalAnnotations );
    sagitalIterator.toFront();
    while( sagitalIterator.hasNext() )
    {
        sagitalIterator.next();
        m_2DViewer->getRenderer()->RemoveActor( sagitalIterator.value() );
        sagitalIterator.remove();
    }

    QMutableMapIterator<int, vtkActor2D *> coronalIterator( m_coronalAnnotations );
    coronalIterator.toFront();
    while( coronalIterator.hasNext() )
    {
        coronalIterator.next();
        m_2DViewer->getRenderer()->RemoveActor( coronalIterator.value() );
        coronalIterator.remove();
    }
    // refresquem l'escena
    m_2DViewer->refresh();
}

}
