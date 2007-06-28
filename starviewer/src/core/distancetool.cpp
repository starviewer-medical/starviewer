/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetool.h"
#include "q2dviewer.h"
#include <vtkCommand.h>
#include <distance.h>
#include <vtkRenderer.h>
#include <vtkLine.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <math.h>
#include <QList>

#include "logging.h"


//\TODO treure línies comentades que no tenen cap sentit
namespace udg {

DistanceTool::DistanceTool( Q2DViewer *viewer , QObject * )
: HighlightColor( Qt::darkGreen ), NormalColor( Qt::green ), SelectedColor( Qt::darkGreen )

{
    m_nearestPoint = NOTHINGSELECTED;
    m_state = NONE;
    m_2DViewer = viewer;
    m_selectedAssembly = NULL;
    m_previousHighlightedAssembly = NULL;

    //creació de tots els objectes de la classe
    m_distanceLine = vtkLineSource::New();
    m_lineActor = vtkActor2D::New();
    m_lineMapper = vtkPolyDataMapper2D::New();

    m_lineActor->GetProperty()->SetColor( NormalColor.redF(), NormalColor.greenF(), NormalColor.blueF() );
    m_lineActor->GetProperty()->SetLineWidth( 2 );
    m_distanceLine->SetResolution ( 2 );

    vtkCoordinate *coordinate = vtkCoordinate::New();
    coordinate->SetCoordinateSystemToWorld();
    m_lineMapper->SetTransformCoordinate( coordinate );

    if( m_2DViewer )
    {
        m_currentSlice = m_2DViewer->getSlice();
        m_lastView = m_2DViewer->getView();
    }
    else
    {
        DEBUG_LOG( "El viewer proporcionat és NUL! ");
        m_currentSlice = 0;
        m_lastView = 0;
    }
    //fem les connexions necessàries
    //\TODO aquestes connexions només haurien d'estar actives quan la tool està activa! Aquí és on es veu l'evidència de la necessitat d'una classe que gestioni els objectes que es pinten per pantalla. Això fa que surtin misstges de log que no s'haurien de produir a part de crides a rutines innecessàries
    connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), this , SLOT( drawDistancesOfSlice( int ) ) );
    connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), this , SLOT( unselectDistance() ) );

    coordinate->Delete();
}

DistanceTool::~DistanceTool()
{
    m_distanceLine->Delete();
    m_lineMapper->Delete();
    m_lineActor->Delete();
}

void DistanceTool::unselectDistance()
{
    this->setDistanceColor( m_selectedAssembly, NormalColor );
    m_selectedAssembly = NULL;
    m_state = NONE;
    m_previousHighlightedAssembly = NULL;
    m_nearestPoint = NOTHINGSELECTED;
}

vtkCaptionActor2D* DistanceTool::createACaption2D()
{
    vtkCaptionActor2D *captionActor = vtkCaptionActor2D::New();
    captionActor->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    captionActor->BorderOff();
    captionActor->LeaderOff();
    captionActor->ThreeDimensionalLeaderOff();
    captionActor->GetCaptionTextProperty()->SetColor( NormalColor.redF(), NormalColor.greenF(), NormalColor.blueF() );
    captionActor->SetPadding( 5 );
    captionActor->SetPosition( -1.0 , -1.0 );
    captionActor->SetHeight( 0.05 );
    captionActor->SetWidth( 0.09 );
    captionActor->GetCaptionTextProperty()->ShadowOff();
    captionActor->GetCaptionTextProperty()->ItalicOff();

    return ( captionActor );
}

void DistanceTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
        //click amb el botó esquerre: anotació de distància
        case vtkCommand::LeftButtonPressEvent:

            if ( !m_selectedAssembly )
            {
                if ( m_state == NONE )
                    this->startDistanceAnnotation();
                else if ( m_state == ANNOTATING )
                    this->endDistanceAnnotation();
            }
            else
            {
                if ( m_state == MOVINGPOINT )
                    this->updateSelectedPoint();
                else
                    this->getNearestPointOfSelectedDistance();
            }
            break;

        case vtkCommand::MouseMoveEvent:

            if ( m_state == NONE )
                this->highlightNearestAssembly();
            else if( m_state == ANNOTATING )
                this->doDistanceSimulation();
            else if ( m_state == MOVINGPOINT )
            {
                if ( m_nearestPoint == FIRST )
                    this->updateFirstPoint();
                else if ( m_nearestPoint == SECOND )
                    this->updateSecondPoint();
            }
            break;

        //click amb el botó dret: volem que si l'element clickat és una distància, quedi seleccionat.
        case vtkCommand::RightButtonPressEvent:
                this->selectDistance();
            break;

        case vtkCommand::KeyPressEvent:
                this->answerToKeyEvent();
            break;

        default:
            break;
    }
}

void DistanceTool::startDistanceAnnotation()
{
    int xy[2];
    double position[4];

    m_state = ANNOTATING;

    //ens assegurem de que la línia d'informació no és visible
    m_lineActor->VisibilityOff();

    m_2DViewer->getInteractor()->GetEventPosition( xy );
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    m_distanceStartPosition[0] = position[0];
    m_distanceStartPosition[1] = position[1];
    m_distanceStartPosition[2] = position[2];

    //assignem el primer extrem a la línia de distància
    m_distanceLine->SetPoint1( m_distanceStartPosition );

    m_lineMapper->SetInput( m_distanceLine->GetOutput() );
    m_lineActor->SetMapper( m_lineMapper );

    m_2DViewer->getRenderer()->AddActor(m_lineActor);
}

void DistanceTool::doDistanceSimulation()
{
    int xy[2];
    double position[4];

    m_2DViewer->getInteractor()->GetEventPosition (xy);
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    m_distanceCurrentPosition[0] = position[0];
    m_distanceCurrentPosition[1] = position[1];
    m_distanceCurrentPosition[2] = position[2];

    //assignem el segon extrem a la línia de distància
    m_distanceLine->SetPoint2( m_distanceCurrentPosition );
    m_lineActor->VisibilityOn();
    m_2DViewer->getInteractor()->Render();
}

void DistanceTool::endDistanceAnnotation()
{
    //creem la línia final entre els dos punts
    vtkLineSource *m_distanceLineAux = vtkLineSource::New();
    vtkActor2D *m_lineActorAux = vtkActor2D::New();
    vtkPolyDataMapper2D *m_lineMapperAux = vtkPolyDataMapper2D::New();

    //assignem el sistema de coordenades al mapper
    vtkCoordinate *coordinate = vtkCoordinate::New();
    coordinate->SetCoordinateSystemToWorld();
    m_lineMapperAux->SetTransformCoordinate( coordinate );

    m_lineActorAux->GetProperty()->SetColor( NormalColor.redF(), NormalColor.greenF(), NormalColor.blueF() );
    m_lineActorAux->GetProperty()->SetLineWidth( 2 );
    m_distanceLineAux->SetResolution ( 2 );

    m_distanceLineAux->SetPoint1( m_distanceStartPosition );
    m_distanceLineAux->SetPoint2( m_distanceCurrentPosition );

    //calculem la distància que s'ha mesurat
    Distance d(m_distanceStartPosition, m_distanceCurrentPosition);

    m_textDistanceActor = createACaption2D();

    QString str = QString( "%1 mm" ).arg( d.getDistance2D(), 0, 'f',  2);

    //Assignem el text a l'etiqueta de la distància i la situem
    m_textDistanceActor->SetCaption( qPrintable ( str ) );
    m_textDistanceActor->SetAttachmentPoint( m_distanceCurrentPosition );

    m_lineMapperAux->SetInput(m_distanceLineAux->GetOutput());
    m_lineActorAux->SetMapper(m_lineMapperAux);

    //afegim la distància anotada al mapa de distàncies
    vtkPropAssembly *m_actorsAssembly = vtkPropAssembly::New();

    m_actorsAssembly->AddPart( m_lineActorAux );
    m_actorsAssembly->AddPart( m_textDistanceActor );

    AssemblyAndLineObject *assemblyAndLine = new AssemblyAndLineObject(m_actorsAssembly, m_distanceLineAux );

    m_2DViewer->getRenderer()->AddActor( m_actorsAssembly );

    switch ( m_2DViewer->getView() )
    {
    case Q2DViewer::Axial:
        m_distancesOfAxialViewMap.insert( m_2DViewer->getSlice(), assemblyAndLine );
        break;
    case Q2DViewer::Sagittal:
        m_distancesOfSagittalViewMap.insert( m_2DViewer->getSlice(), assemblyAndLine );
        break;
    case Q2DViewer::Coronal:
        m_distancesOfCoronalViewMap.insert( m_2DViewer->getSlice(), assemblyAndLine );
        break;
    default:
        DEBUG_LOG( "El visor no té cap vista assignada encara" );
        break;
    }
    m_2DViewer->getRenderer()->RemoveActor( m_lineActor );
    m_2DViewer->refresh();

    m_textDistanceActor->Delete();
    m_distanceLineAux->Delete();
    m_lineActorAux->Delete();
    m_lineMapperAux->Delete();
    coordinate->Delete();
    m_actorsAssembly->Delete();

    //determinem que estem en l'estat NONE, és a dir, no estem anotant cap distància.
    m_state = NONE;
}

void DistanceTool::selectDistance()
{
    if( m_selectedAssembly )
    {
        this->setDistanceColor( m_selectedAssembly , NormalColor );

        if( m_previousHighlightedAssembly )
        {
            if ( m_previousHighlightedAssembly == m_selectedAssembly )
                m_selectedAssembly = NULL;
            else
            {
                m_selectedAssembly = m_previousHighlightedAssembly;
                m_previousHighlightedAssembly = NULL;
            }
        }
    }
    else
    {
        int x, y;
        x = m_2DViewer->getInteractor()->GetEventPosition()[0];
        y = m_2DViewer->getInteractor()->GetEventPosition()[1];
        double toWorld[4];
        m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
        double point[2];

        switch( m_2DViewer->getView() )
        {
            case Q2DViewer::Axial:
                point[0] = toWorld[0];
                point[1] = toWorld[1];
                break;
            case Q2DViewer::Sagittal:
                point[1] = toWorld[1];
                point[2] = toWorld[2];
                break;
            case Q2DViewer::Coronal:
                point[0] = toWorld[0];
                point[2] = toWorld[2];
                break;
            default:
                DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
                break;
        }

        m_selectedAssembly = (this->getNearestAssembly( point ));
        this->setDistanceColor( m_selectedAssembly , SelectedColor );
    }
    m_2DViewer->refresh();
}

void DistanceTool::highlightNearestAssembly()
{
    double point[3] = { .0, .0, .0 };
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            point[0] = toWorld[0];
            point[1] = toWorld[1];
            break;
        case Q2DViewer::Sagittal:
            point[1] = toWorld[1];
            point[2] = toWorld[2];
            break;
        case Q2DViewer::Coronal:
            point[0] = toWorld[0];
            point[2] = toWorld[2];
            break;
        default:
            DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
            break;
    }

    AssemblyAndLineObject *assembly = (this->getNearestAssembly( point ));
    if( assembly )
    {
        this->setDistanceColor( assembly , HighlightColor );
        if( m_previousHighlightedAssembly != assembly &&  m_previousHighlightedAssembly != m_selectedAssembly )
            this->setDistanceColor( m_previousHighlightedAssembly , NormalColor );
        m_previousHighlightedAssembly = assembly;
    }
    else
    {
        if( m_previousHighlightedAssembly && m_previousHighlightedAssembly != m_selectedAssembly )
            this->setDistanceColor( m_previousHighlightedAssembly , NormalColor );
    }
}

void DistanceTool::setDistanceColor( AssemblyAndLineObject *assembly, QColor color )
{
    if( assembly )
    {
        vtkActor2D *line;
        vtkCaptionActor2D *caption;
        vtkPropAssembly *distance;
        distance = assembly->getAssembly();
        vtkPropCollection *assemblyCollection = distance->GetParts();

        line = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );
        caption = vtkCaptionActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 1 ) );
        if ( line != NULL && caption != NULL )
        {
            line->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
            caption->GetCaptionTextProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
        }
    }
    else
        DEBUG_LOG( "Assembly buit!" );
}

AssemblyAndLineObject* DistanceTool::getNearestAssembly( double point3D[3] )
{
    QList< AssemblyAndLineObject* > actorsList;

    int coordinateToZero;

    switch( m_2DViewer->getView() )
    {
    case Q2DViewer::Axial:
        actorsList = m_distancesOfAxialViewMap.values( m_2DViewer->getSlice() );
        //la coordenada que s'ha de deixar a 0 és la z.
        coordinateToZero = 2;
    break;

    case Q2DViewer::Sagittal:
        actorsList = m_distancesOfSagittalViewMap.values( m_2DViewer->getSlice() );
        //la coordenada que s'ha de deixar a 0 és la x.
        coordinateToZero = 0;
    break;

    case Q2DViewer::Coronal:
        actorsList = m_distancesOfCoronalViewMap.values( m_2DViewer->getSlice() );
        //la coordenada que s'ha de deixar a 0 és la y.
        coordinateToZero = 1;
    break;

    default:
        DEBUG_LOG( "vista del visor 2D no esperada!" );
    break;
    }

    AssemblyAndLineObject *nearest = NULL;
    double minDistance = VTK_DOUBLE_MAX;
    double *p1, *p2;
    double distance;

    foreach( AssemblyAndLineObject *candidate, actorsList )
    {
        p1 = candidate->getLine()->GetPoint1();
        p1[coordinateToZero] = 0.0;
        p2 = candidate->getLine()->GetPoint2();
        p2[coordinateToZero] = 0.0;
        distance = vtkLine::DistanceToLine( point3D , p1 , p2 );

        if( distance < minDistance && distance < 50.0 )
        {
            minDistance = distance;
            nearest = candidate;
        }
    }
    return nearest;
}

void DistanceTool::getNearestPointOfSelectedDistance()
{
    double *vertex1, *vertex2;
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
    double point[3] = { toWorld[0] , toWorld[1], toWorld[2] };

    vertex1 = m_selectedAssembly->getLine()->GetPoint1();
    vertex2 = m_selectedAssembly->getLine()->GetPoint2();

    //creem dues distàncies per mirar quina està més aprop
    Distance d1( vertex1, point );
    Distance d2( vertex2, point );

    if ( d1.getDistance3D() <= d2.getDistance3D() ) //vol dir que el punt1 de la distància està més aprop
    {
        m_nearestPoint = FIRST;
        this->updateFirstPoint();
    }
    else                                            //el punt2 és el més proper
    {
        m_nearestPoint = SECOND;
        this->updateSecondPoint();
    }

    m_state = MOVINGPOINT;
}

void DistanceTool::updateFirstPoint()
{
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4], point[2], point2[2];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            point[0] = toWorld[0];
            point[1] = toWorld[1];
            point2[0] = m_selectedAssembly->getLine()->GetPoint2()[0];
            point2[1] = m_selectedAssembly->getLine()->GetPoint2()[1];
            m_selectedAssembly->getLine()->SetPoint1( point[0], point[1], 0 );
            break;
        case Q2DViewer::Sagittal:
            point[0] = toWorld[1];
            point[1] = toWorld[2];
            point2[0] = m_selectedAssembly->getLine()->GetPoint2()[1];
            point2[1] = m_selectedAssembly->getLine()->GetPoint2()[2];
            m_selectedAssembly->getLine()->SetPoint1( 0, point[0], point[1] );
            break;
        case Q2DViewer::Coronal:
            point[0] = toWorld[0];
            point[1] = toWorld[2];
            point2[0] = m_selectedAssembly->getLine()->GetPoint2()[0];
            point2[1] = m_selectedAssembly->getLine()->GetPoint2()[2];
            m_selectedAssembly->getLine()->SetPoint1( point[0], 0, point[1] );
            break;
        default:
            DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
            break;
    }
    //actualitzem la posició del primer punt de la distància i el contingut de l'etiqueta
    Distance dist( point2, point );

    vtkCaptionActor2D *caption = m_selectedAssembly->getCaption();
    QString str = QString( "%1 mm" ).arg( dist.getDistance2D(), 0, 'f',  2);
    caption->SetCaption( qPrintable ( str ) );

    m_2DViewer->refresh();
}

void DistanceTool::updateSecondPoint()
{
    double toWorld[4], point[2], point1[2];
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );

    vtkCaptionActor2D *caption = m_selectedAssembly->getCaption();

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            point[0] = toWorld[0];
            point[1] = toWorld[1];
            point1[0] = m_selectedAssembly->getLine()->GetPoint1()[0];
            point1[1] = m_selectedAssembly->getLine()->GetPoint1()[1];
            m_selectedAssembly->getLine()->SetPoint2( point[0], point[1], 0 );
            caption->SetAttachmentPoint( point[0], point[1], 0 );
            break;
        case Q2DViewer::Sagittal:
            point[0] = toWorld[1];
            point[1] = toWorld[2];
            point1[0] = m_selectedAssembly->getLine()->GetPoint1()[1];
            point1[1] = m_selectedAssembly->getLine()->GetPoint1()[2];
            m_selectedAssembly->getLine()->SetPoint2( 0, point[0], point[1] );
            caption->SetAttachmentPoint( 0, point[0], point[1] );
            break;
        case Q2DViewer::Coronal:
            point[0] = toWorld[0];
            point[1] = toWorld[2];
            point1[0] = m_selectedAssembly->getLine()->GetPoint1()[0];
            point1[1] = m_selectedAssembly->getLine()->GetPoint1()[2];
            m_selectedAssembly->getLine()->SetPoint2( point[0], 0, point[1] );
            caption->SetAttachmentPoint( point[0], 0, point[1] );
            break;
        default:
            DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
            break;
    }

    Distance dist( point1, point );

    QString str = QString( "%1 mm" ).arg( dist.getDistance2D(), 0, 'f',  2);

    //Assignem el text a l'etiqueta de la distància
    caption->SetCaption( qPrintable ( str ) );

    m_2DViewer->refresh();
}

void DistanceTool::updateSelectedPoint()
{
    m_nearestPoint = NOTHINGSELECTED;
    m_state = NONE;
}

void DistanceTool::drawDistancesOfSlice( int slice )
{
    // si hem canviat de vista, primer fem invisibles els actors de la última vista a la última llesca
    int viewToClear;
    if( m_lastView != m_2DViewer->getView() )
    {
        // s'ha canviat de vista
        viewToClear = m_lastView;
        m_lastView = m_2DViewer->getView();
    }
    else // continuem a la mateixa vista
        viewToClear = m_2DViewer->getView();

    QList< AssemblyAndLineObject *> list;
    switch( viewToClear )
    {
    case Q2DViewer::Axial:
        list = m_distancesOfAxialViewMap.values( m_currentSlice );
    break;

    case Q2DViewer::Sagittal:
        list = m_distancesOfSagittalViewMap.values( m_currentSlice );
    break;

    case Q2DViewer::Coronal:
        list = m_distancesOfCoronalViewMap.values( m_currentSlice );
    break;

    default:
        DEBUG_LOG( "Valor inesperat" );
    break;
    }

    foreach( AssemblyAndLineObject *assembly, list )
    {
        assembly->getAssembly()->VisibilityOff();
    }

    // actualitzem la llesca
    m_currentSlice = slice;

    // actualitzem la llesca i fem visibles els actors que hi hagi en ella en la vista en la que ens trobem
    switch( m_lastView )
    {
    case Q2DViewer::Axial:
        list = m_distancesOfAxialViewMap.values( m_currentSlice );
    break;

    case Q2DViewer::Sagittal:
        list = m_distancesOfSagittalViewMap.values( m_currentSlice );
    break;

    case Q2DViewer::Coronal:
        list = m_distancesOfCoronalViewMap.values( m_currentSlice );
    break;

    default:
        DEBUG_LOG( "Valor inesperat" );
    break;
    }

    foreach( AssemblyAndLineObject *assembly, list )
    {
        assembly->getAssembly()->VisibilityOn();
    }

    m_2DViewer->refresh();
    //deixem constància de que no tenim seleccionada cap distància
    m_selectedAssembly = NULL;
}

void DistanceTool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una distància, sempre que hi hagi una distància seleccionada i
    //s'hagi polsat la tecla adequada (tecla sup)

    char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
    int keyInt = (int)keyChar;

    if ( m_selectedAssembly != NULL && keyInt == 127 )
    {
        QMutableMapIterator< int , AssemblyAndLineObject* > *iterator = NULL;
        switch( m_lastView )
        {
        case Q2DViewer::Axial:
            iterator = new QMutableMapIterator< int , AssemblyAndLineObject* >( m_distancesOfAxialViewMap );
        break;

        case Q2DViewer::Sagittal:
            iterator = new QMutableMapIterator< int , AssemblyAndLineObject* >( m_distancesOfSagittalViewMap );
        break;

        case Q2DViewer::Coronal:
            iterator = new QMutableMapIterator< int , AssemblyAndLineObject* >( m_distancesOfCoronalViewMap );
        break;

        default:
            DEBUG_LOG( "Valor inesperat" );
        break;
        }
        if ( iterator )
        {
            iterator->toFront();
            if( iterator->findNext( m_selectedAssembly ) )
            {
                iterator->remove();
                m_2DViewer->getRenderer()->RemoveActor( (m_selectedAssembly->getAssembly()) );
                if( m_selectedAssembly == m_previousHighlightedAssembly )
                    m_previousHighlightedAssembly = NULL;

                m_selectedAssembly = NULL;
                m_2DViewer->refresh();
            }
        }
    }
}

}


