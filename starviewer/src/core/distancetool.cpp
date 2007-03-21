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
#include <vtkAlgorithmOutput.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor2DCollection.h>
#include <vtkDiskSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkLineSource.h>
#include <vtkActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkPropPicker.h>
#include <math.h>
#include <QList>

#include "logging.h"

namespace udg {

DistanceTool::DistanceTool( Q2DViewer *viewer , QObject *, const char * )
: HighlightColor( Qt::darkGreen ), NormalColor( Qt::green ), SelectedColor( Qt::darkGreen )

{
    m_state = NONE;
    m_2DViewer = viewer;
    m_firstPointLocated = false;
    m_selectedAssembly = NULL;
    m_somePropSelected = false;
    m_nearestProp = NULL;

    m_previousHighlightedAssembly = NULL;

    //creació de tots els objectes de la classe
    m_distanceLine = vtkLineSource::New();
    m_lineActor = vtkActor2D::New();

    m_lineMapper = vtkPolyDataMapper2D::New();

    //assignem propietats als objectes
    m_lineActor->GetProperty()->SetColor( NormalColor.redF(), NormalColor.greenF(), NormalColor.blueF() );
    m_lineActor->GetProperty()->SetLineWidth(2);
    m_distanceLine->SetResolution (2);

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
    connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), this , SLOT( drawDistancesOfSlice( int ) ) );

    coordinate->Delete();
}

DistanceTool::~DistanceTool()
{
    m_distanceLine->Delete();
    m_lineMapper->Delete();
    m_lineActor->Delete();
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
            //anotem distància si no hi ha cap distància seleccionada, altrament
            //el que volem és interactuar amb la distància
            if (!m_somePropSelected)
            {
                if ( !m_firstPointLocated )
                    this->startDistanceAnnotation();
                else
                    this->endDistanceAnnotation();
            }
//                 else
//                 {
//                     if ( m_state == MOVINGPOINT )
//                         this->updateSelectedPoint();
//                     else
//                         this->giveMeTheNearestPoint();
//                 }
            break;

        case vtkCommand::MouseMoveEvent:
            //al moure el mouse, simulem la distància que s'anotarà, dibuixant una línia.
            if( m_state == ANNOTATING )
                this->doDistanceSimulation();
            else if ( m_state == NONE )
                this->highlightNearestAssembly();
            else if ( m_state == MOVINGPOINT )
                this->moveSelectedPoint();
            break;

        //click amb el botó dret: volem que si l'element clickat és una distància, quedi seleccionat.
        case vtkCommand::RightButtonPressEvent:
                this->selectDistance();
            break;

        case vtkCommand::KeyPressEvent:
            answerToKeyEvent();
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

    //deixem constància de que ja hem anotat el primer punt
    m_firstPointLocated = true;

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

    m_2DViewer->getRenderer()->AddActor( m_actorsAssembly );

    switch ( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            m_distancesOfAxialViewMap.insert( m_2DViewer->getSlice(), m_actorsAssembly );
            break;
        case Q2DViewer::Sagittal:
            m_distancesOfSagittalViewMap.insert( m_2DViewer->getSlice(), m_actorsAssembly );
            break;
        case Q2DViewer::Coronal:
            m_distancesOfCoronalViewMap.insert( m_2DViewer->getSlice(), m_actorsAssembly );
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

    //deixem constància de que ja hem anotat la distància.
    m_firstPointLocated = false;
}

Distance DistanceTool::getDistanceFromActor2D( vtkActor2D *actor )
{
    vtkPolyDataMapper2D *auxMap = vtkPolyDataMapper2D::SafeDownCast( actor->GetMapper() );
    vtkLineSource *ls = vtkLineSource::SafeDownCast( auxMap->GetInput()->GetProducerPort()->GetProducer() );

    return ( Distance( ls->GetPoint1(), ls->GetPoint2() ) );
}

void DistanceTool::selectDistance()
{
    vtkPropCollection *propCollection = NULL;

    if( m_selectedAssembly )
    {
        this->setDistanceColor( m_selectedAssembly , Qt::green );
        m_selectedAssembly = NULL;
    }

    if( m_previousHighlightedAssembly )
    {
        m_selectedAssembly = m_previousHighlightedAssembly;
        m_previousHighlightedAssembly = NULL;

        this->setDistanceColor( m_selectedAssembly , SelectedColor );
    }
    m_2DViewer->refresh();
}

void DistanceTool::highlightNearestAssembly()
{
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
    double point[2] = { toWorld[0] , toWorld[1] };

    vtkPropAssembly *assembly = this->getNearestAssembly( point );
    if( assembly )
    {
        this->setDistanceColor( assembly , HighlightColor );
        if( m_previousHighlightedAssembly != assembly &&  m_previousHighlightedAssembly != m_selectedAssembly )
            this->setDistanceColor( m_previousHighlightedAssembly , Qt::green );
        m_previousHighlightedAssembly = assembly;
    }
    else
    {
        if( m_previousHighlightedAssembly && m_previousHighlightedAssembly != m_selectedAssembly )
            this->setDistanceColor( m_previousHighlightedAssembly , Qt::green );
    }
}

void DistanceTool::setDistanceColor( vtkPropAssembly *distance, QColor color )
{
    if( distance )
    {
        vtkActor2D *line;
        vtkCaptionActor2D *caption;
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

vtkPropAssembly *DistanceTool::getNearestAssembly( double point[2] )
{
    QList< vtkPropAssembly * > actorsList;
    switch( m_2DViewer->getView() )
    {
    case Q2DViewer::Axial:
        actorsList = m_distancesOfAxialViewMap.values( m_2DViewer->getSlice() );
    break;

    case Q2DViewer::Sagittal:
        actorsList = m_distancesOfSagittalViewMap.values( m_2DViewer->getSlice() );
    break;

    case Q2DViewer::Coronal:
        actorsList = m_distancesOfCoronalViewMap.values( m_2DViewer->getSlice() );
    break;

    default:
        DEBUG_LOG( "vista del visor 2D no esperada!" );
    break;
    }

    vtkPropAssembly *nearest = NULL;
    double minDistance = VTK_DOUBLE_MAX;
    double *p1, *p2;
    double distance;
    Distance dist;
    vtkActor2D *lineActor = NULL;
    vtkPropCollection *assemblyCollection = NULL;
    // d'aquest punt tan sols modificarem la x i la y. La z queda fixada sempre a 0
    double point3D[3] = { .0, .0, .0 };
    foreach( vtkPropAssembly *candidate, actorsList )
    {
        assemblyCollection = candidate->GetParts();
        lineActor = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );
        dist = this->getDistanceFromActor2D( lineActor );
        p1 = dist.getFirstPoint();
        p1[2] = 0.0;
        p2 = dist.getSecondPoint();
        p2[2] = 0.0;
        point3D[0] = point[0];
        point3D[1] = point[1];
        distance = vtkLine::DistanceToLine( point3D , p1 , p2 );
        if( distance < minDistance && distance < 50.0 )
        {
            minDistance = distance;
            nearest = candidate;
        }
    }
    return nearest;
}

void DistanceTool::moveSelectedPoint()
{
//     m_2DViewer->getCurrentCursorPosition( m_distanceCurrentPosition );
//
//         if (m_pointOfDistance == 1)
//             m_selectedDistance->SetPoint1( m_distanceCurrentPosition );
//         else
//             m_selectedDistance->SetPoint2( m_distanceCurrentPosition );
//
// //         m_selectedDiskActor->SetPosition( m_distanceCurrentPosition );
//
//         m_selectedActor->VisibilityOn();
//
//         m_2DViewer->getInteractor()->Render();
}

void DistanceTool::updateSelectedPoint()
{
//         double clickPosition[3];
//
//         m_state = NONE;
//
//         m_2DViewer->getCurrentCursorPosition(clickPosition);
//
//         Distance d;
//
//         if (m_pointOfDistance == 1)
//             d.setPoints( m_selectedDistance->GetPoint2() , clickPosition );
//         else
//             d.setPoints( m_selectedDistance->GetPoint1() , clickPosition );
//
//         QString str = QString("%1 mm").arg(d.getDistance2D(), 0, 'f', 2);
//
//         //Assignem el text a l'etiqueta de la distància i la situem
//         m_textDistanceActor->SetCaption( qPrintable ( str ));
//         m_textDistanceActor->SetAttachmentPoint( clickPosition );
// //         m_2DViewer->getRenderer()->AddActor(m_selectedActor);
//
//         m_textDistanceActor->VisibilityOn();
//
//         m_2DViewer->getInteractor()->Render();
}

void DistanceTool::drawDistancesOfSlice( int slice )
{
    int i;
    vtkPropCollection *pickCollection;
    vtkProp *auxProp;

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

    vtkPropCollection *distance = NULL;
    QList< vtkPropAssembly *> list;
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

    vtkPropCollection *assemblyCollection = NULL;
    foreach( vtkPropAssembly *assembly, list )
    {
        assembly->VisibilityOff();
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

    assemblyCollection = NULL;
    foreach( vtkPropAssembly *assembly, list )
    {
        assembly->VisibilityOn();
    }

    m_2DViewer->refresh();
    //deixem constància de que no tenim seleccionada cap distància
    m_selectedAssembly = NULL;
}

void DistanceTool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una distància, sempre que hi hagi una distància seleccionada i
    //s'hagi polsat la tecla adequada (tecla espai)

    char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
    int keyInt = (int)keyChar;

    if ( m_selectedAssembly != NULL && keyInt == 127 )
    {
        QMutableMapIterator< int , vtkPropAssembly * > *iterator = NULL;
        switch( m_lastView )
        {
        case Q2DViewer::Axial:
            iterator = new QMutableMapIterator< int , vtkPropAssembly * >( m_distancesOfAxialViewMap );
        break;

        case Q2DViewer::Sagittal:
            iterator = new QMutableMapIterator< int , vtkPropAssembly * >( m_distancesOfSagittalViewMap );
        break;

        case Q2DViewer::Coronal:
            iterator = new QMutableMapIterator< int , vtkPropAssembly * >( m_distancesOfCoronalViewMap );
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
                m_2DViewer->getRenderer()->RemoveActor( m_selectedAssembly );
                if( m_selectedAssembly == m_previousHighlightedAssembly )
                    m_previousHighlightedAssembly = NULL;
                m_selectedAssembly = NULL;
                m_2DViewer->refresh();
            }
        }
    }
}

}


